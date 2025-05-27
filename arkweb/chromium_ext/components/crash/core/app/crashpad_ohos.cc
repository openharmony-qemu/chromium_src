// Copyright 2024 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on crashpad_android.cc originally written by
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <dlfcn.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>

#include "arkweb/chromium_ext/base/process/process_handle_posix_ex.h"
#include "base/environment.h"
#include "base/files/file_util.h"
#include "base/files/scoped_file.h"
#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/path_service.h"
#include "base/posix/eintr_wrapper.h"
#include "base/posix/global_descriptors.h"
#include "base/process/process_handle.h"
#include "base/rand_util.h"
#include "base/strings/stringprintf.h"
#include "base/synchronization/lock.h"
#include "build/branding_buildflags.h"
#include "build/build_config.h"
#include "components/crash/core/app/crash_reporter_client.h"
#include "components/crash/core/app/crashpad.h"
#include "content/public/common/content_descriptors.h"
#include "sandbox/linux/services/syscall_wrappers.h"
#include "third_party/crashpad/crashpad/client/annotation.h"
#include "third_party/crashpad/crashpad/client/client_argv_handling.h"
#include "third_party/crashpad/crashpad/client/crashpad_client.h"
#include "third_party/crashpad/crashpad/client/simulate_crash_linux.h"
#include "third_party/crashpad/crashpad/snapshot/sanitized/sanitization_information.h"
#include "third_party/crashpad/crashpad/util/linux/exception_handler_client.h"
#include "third_party/crashpad/crashpad/util/linux/exception_handler_protocol.h"
#include "third_party/crashpad/crashpad/util/linux/exception_information.h"
#include "third_party/crashpad/crashpad/util/linux/proc_info_ohos.h"
#include "third_party/crashpad/crashpad/util/linux/scoped_pr_set_dumpable.h"
#include "third_party/crashpad/crashpad/util/misc/from_pointer_cast.h"
#include "third_party/crashpad/crashpad/util/posix/signals.h"

namespace crashpad {
namespace {

class AllowedMemoryRanges {
 public:
  AllowedMemoryRanges() {
    allowed_memory_ranges_.entries = 0;
    allowed_memory_ranges_.size = 0;
  }

  AllowedMemoryRanges(const AllowedMemoryRanges&) = delete;
  AllowedMemoryRanges& operator=(const AllowedMemoryRanges&) = delete;

  void AddEntry(VMAddress base, VMSize length) {
    SanitizationAllowedMemoryRanges::Range new_entry;
    new_entry.base = base;
    new_entry.length = length;

    base::AutoLock lock(lock_);
    std::vector<SanitizationAllowedMemoryRanges::Range> new_array(array_);
    new_array.push_back(new_entry);
    allowed_memory_ranges_.entries =
        FromPointerCast<VMAddress>(new_array.data());
    allowed_memory_ranges_.size += 1;
    array_ = std::move(new_array);
  }

  SanitizationAllowedMemoryRanges* GetSanitizationAddress() {
    return &allowed_memory_ranges_;
  }

  static AllowedMemoryRanges* Singleton() {
    static base::NoDestructor<AllowedMemoryRanges> singleton;
    return singleton.get();
  }

 private:
  base::Lock lock_;
  SanitizationAllowedMemoryRanges allowed_memory_ranges_;
  std::vector<SanitizationAllowedMemoryRanges::Range> array_;
};

bool SetSanitizationInfo(crash_reporter::CrashReporterClient* client,
                         SanitizationInformation* info) {
  const char* const* allowed_annotations = nullptr;
  void* target_module = nullptr;
  bool sanitize_stacks = false;
  client->GetSanitizationInformation(&allowed_annotations, &target_module,
                                     &sanitize_stacks);
  info->allowed_annotations_address =
      FromPointerCast<VMAddress>(allowed_annotations);
  info->target_module_address = FromPointerCast<VMAddress>(target_module);
  info->allowed_memory_ranges_address = FromPointerCast<VMAddress>(
      AllowedMemoryRanges::Singleton()->GetSanitizationAddress());
  info->sanitize_stacks = sanitize_stacks;
  return allowed_annotations != nullptr || target_module != nullptr ||
         sanitize_stacks;
}

void SetExceptionInformation(siginfo_t* siginfo,
                             ucontext_t* context,
                             ExceptionInformation* info) {
  info->siginfo_address =
      FromPointerCast<decltype(info->siginfo_address)>(siginfo);
  info->context_address =
      FromPointerCast<decltype(info->context_address)>(context);
  info->thread_id = sandbox::sys_gettid();
  ProcInfo proc;
  std::unordered_map<int, int> tid_nstid_map;
  base::ProcessId realPid = base::GetCurrentRealPid();
  if (GetProcStatusByPid(realPid, proc) && proc.ns &&
      GetTidMapByPid(realPid, tid_nstid_map)) {
    LOG(INFO) << "crashpad SetExceptionInformation, crash happened, crash "
                 "process real pid = "
              << realPid << " is in pid namespace = " << proc.ns
              << ", need to convert tid to real pid";
    for (auto it : tid_nstid_map) {
      if (it.second == info->thread_id) {
        info->thread_id = it.first;
        return;
      }
    }
  }
}

void SetClientInformation(ExceptionInformation* exception,
                          SanitizationInformation* sanitization,
                          ExceptionHandlerProtocol::ClientInformation* info) {
  info->exception_information_address =
      FromPointerCast<decltype(info->exception_information_address)>(exception);

  info->sanitization_information_address =
      FromPointerCast<decltype(info->sanitization_information_address)>(
          sanitization);
}

// A signal handler for non-browser processes in the sandbox.
// Sends a message to a crashpad::CrashHandlerHost to handle the crash.
class SandboxedHandler {
 public:
  static SandboxedHandler* Get() {
    static SandboxedHandler* instance = new SandboxedHandler();
    return instance;
  }

  SandboxedHandler(const SandboxedHandler&) = delete;
  SandboxedHandler& operator=(const SandboxedHandler&) = delete;

  bool Initialize(bool dump_at_crash) {
    request_dump_ = dump_at_crash ? 1 : 0;

    SetSanitizationInfo(crash_reporter::GetCrashReporterClient(),
                        &sanitization_);
    server_fd_ = base::GlobalDescriptors::GetInstance()->Get(kCrashDumpSignal);

    bool signal_stack_initialized =
        CrashpadClient::InitializeSignalStackForThread();
    DCHECK(signal_stack_initialized);
    return Signals::InstallCrashHandlers(HandleCrash, SA_ONSTACK,
                                         &old_actions_);
  }

  void HandleCrashNonFatal(int signo, siginfo_t* siginfo, void* context) {
    base::ScopedFD connection;
    server_fd_ = base::GlobalDescriptors::GetInstance()->Get(kCrashDumpSignal);
    if (ConnectToHandler(signo, &connection) == 0) {
      LOG(INFO) << "crashpad SandboxedHandler::HandleCrashNonFatal, connect to "
                   "handler successfully"
                   ", need to request dump";
      ExceptionInformation exception_information;
      SetExceptionInformation(siginfo, static_cast<ucontext_t*>(context),
                              &exception_information);

      ExceptionHandlerProtocol::ClientInformation info;
      SetClientInformation(&exception_information, &sanitization_, &info);

      ScopedPrSetDumpable set_dumpable(/* may_log= */ false);

      ExceptionHandlerClient handler_client(connection.get(), false);
      handler_client.SetCanSetPtracer(false);
      handler_client.RequestCrashDump(info);
    }
  }

 private:
  SandboxedHandler() = default;
  ~SandboxedHandler() = delete;

  int ConnectToHandler(int signo, base::ScopedFD* connection) {
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) != 0) {
      LOG(ERROR)
          << "crashpad ConnectToHandler, create socketpair failed, errno = "
          << errno;
      return errno;
    }
    base::ScopedFD local_connection(fds[0]);
    base::ScopedFD handlers_socket(fds[1]);

    // SELinux may block the handler from setting SO_PASSCRED on this socket.
    // Attempt to set it here, but the handler can still try if this fails.
    int optval = 1;
    socklen_t optlen = sizeof(optval);
    setsockopt(handlers_socket.get(), SOL_SOCKET, SO_PASSCRED, &optval, optlen);

    iovec iov[2];
    iov[0].iov_base = &signo;
    iov[0].iov_len = sizeof(signo);
    iov[1].iov_base = &request_dump_;
    iov[1].iov_len = sizeof(request_dump_);

    msghdr msg;
    msg.msg_name = nullptr;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = std::size(iov);

    char cmsg_buf[CMSG_SPACE(sizeof(int))];
    msg.msg_control = cmsg_buf;
    msg.msg_controllen = sizeof(cmsg_buf);

    cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    *reinterpret_cast<int*>(CMSG_DATA(cmsg)) = handlers_socket.get();

    if (HANDLE_EINTR(sendmsg(server_fd_, &msg, MSG_NOSIGNAL)) < 0) {
      LOG(ERROR) << "crashpad ConnectToHandler, sendmsg failed, errno = "
                 << errno << ", handler fd = " << server_fd_;
      return errno;
    }

    *connection = std::move(local_connection);
    return 0;
  }

  static void HandleCrash(int signo, siginfo_t* siginfo, void* context) {
    LOG(INFO) << "crashpad SandboxedHandler::HandleCrash, received signo = "
              << signo;
    SandboxedHandler* state = Get();
    state->HandleCrashNonFatal(signo, siginfo, context);
    Signals::RestoreHandlerAndReraiseSignalOnReturn(
        siginfo, state->old_actions_.ActionForSignal(signo));
  }

  Signals::OldActions old_actions_ = {};
  SanitizationInformation sanitization_;
  int server_fd_;
  unsigned char request_dump_;
};

}  // namespace
}  // namespace crashpad

namespace crash_reporter {
namespace {
#if defined(__arm__) && defined(__ARM_ARCH_7A__)
#define CURRENT_ABI "armeabi-v7a"
#elif defined(__arm__)
#define CURRENT_ABI "armeabi"
#elif defined(__i386__)
#define CURRENT_ABI "x86"
#elif defined(__mips__)
#define CURRENT_ABI "mips"
#elif defined(__x86_64__)
#define CURRENT_ABI "x86_64"
#elif defined(__aarch64__)
#define CURRENT_ABI "arm64-v8a"
#else
#error "Unsupported target abi"
#endif

void BuildHandlerArgs(CrashReporterClient* crash_reporter_client,
                      base::FilePath* database_path,
                      base::FilePath* metrics_path,
                      std::string* url,
                      std::map<std::string, std::string>* process_annotations,
                      std::vector<std::string>* arguments) {
  crash_reporter_client->GetCrashDumpLocation(database_path);
  crash_reporter_client->GetCrashMetricsLocation(metrics_path);

  *url = std::string();

  std::string product_name;
  std::string product_version;
  std::string channel;
  crash_reporter_client->GetProductNameAndVersion(&product_name,
                                                  &product_version, &channel);
  (*process_annotations)["prod"] = product_name;
  (*process_annotations)["ver"] = product_version;

#if BUILDFLAG(GOOGLE_CHROME_BRANDING)
  // Empty means stable.
  const bool allow_empty_channel = true;
#else
  const bool allow_empty_channel = false;
#endif
  if (allow_empty_channel || !channel.empty()) {
    (*process_annotations)["channel"] = channel;
  }

  (*process_annotations)["plat"] = std::string("OHOS");
}

bool ShouldHandleCrashAndUpdateArguments(bool write_minidump_to_database,
                                         bool write_minidump_to_log,
                                         std::vector<std::string>* arguments) {
  if (!write_minidump_to_database) {
    arguments->push_back("--no-write-minidump-to-database");
  }
  if (write_minidump_to_log) {
    arguments->push_back("--write-minidump-to-log");
  }
  return write_minidump_to_database || write_minidump_to_log;
}

bool GetHandlerPath(base::FilePath* exe_dir, base::FilePath* handler_path) {
  base::FilePath path;
  if (base::PathService::Get(base::DIR_MODULE, exe_dir)) {
    path = exe_dir->Append("arkweb_crashpad_handler");
    if (base::PathExists(path)) {
      *handler_path = path;
      LOG(INFO) << "crashpad GetHandlerPath, handler bin path = "
                << *handler_path;
      return true;
    }
    LOG(INFO) << "crashpad get exe path " << path
              << " failed, try to get handler path from bundle dir";
  } else {
    LOG(INFO) << "crashpad get exe dir failed, try to get bundle dir";
  }

  // There is not any normal way to package native executables in an OHOS hap,
  // rename shared lib
  if (!base::PathService::Get(base::DIR_OHOS_APP_INSTALLATION, exe_dir)) {
    LOG(ERROR) << "crashpad get DIR_OHOS_APP_INSTALLATION failed";
    return false;
  }

#if defined(__arm__)
  std::string platform = "arm";
#elif defined(__x86_64__)
  std::string platform = "x86_64";
#elif defined(__aarch64__)
  std::string platform = "arm64";
#else
  std::string platform = "unsupported";
#endif

  std::string platform_handler_path =
      "nweb/libs/" + platform + "/libarkweb_crashpad_handler.so";
  *handler_path = exe_dir->Append(platform_handler_path);

  if (!base::PathExists(*handler_path)) {
    std::string platform_handler_path_new =
        "arkwebcore/libs/" + platform + "/libarkweb_crashpad_handler.so";
    *handler_path = exe_dir->Append(platform_handler_path_new);
  }

  LOG(INFO) << "crashpad GetHandlerPath, handler bin path = " << *handler_path;
  return true;
}

bool SetLdLibraryPath(const base::FilePath& lib_path) {
#if defined(COMPONENT_BUILD)
  std::string library_path(lib_path.value());

  static constexpr char kLibraryPathVar[] = "LD_LIBRARY_PATH";
  std::unique_ptr<base::Environment> env(base::Environment::Create());
  std::string old_path;
  if (env->GetVar(kLibraryPathVar, &old_path)) {
    library_path.push_back(':');
    library_path.append(old_path);
  }

  if (!env->SetVar(kLibraryPathVar, library_path)) {
    return false;
  }
#endif

  return true;
}

class HandlerStarter {
 public:
  static HandlerStarter* Get() {
    static HandlerStarter* instance = new HandlerStarter();
    return instance;
  }

  HandlerStarter(const HandlerStarter&) = delete;
  HandlerStarter& operator=(const HandlerStarter&) = delete;

  base::FilePath Initialize(bool dump_at_crash) {
    base::FilePath database_path;
    base::FilePath metrics_path;
    std::string url;
    std::map<std::string, std::string> process_annotations;
    std::vector<std::string> arguments;
    BuildHandlerArgs(GetCrashReporterClient(), &database_path, &metrics_path,
                     &url, &process_annotations, &arguments);

    LOG(INFO) << "crashpad HandlerStarter::Initialize, crashpad report "
                 "database path = "
              << database_path << ", metrics path = " << metrics_path
              << ", upload url = " << url;
    base::FilePath exe_dir;
    base::FilePath handler_path;
    if (!GetHandlerPath(&exe_dir, &handler_path)) {
      LOG(ERROR)
          << "crash HandlerStarter::Initialize, get handler bin path failed";
      return database_path;
    }

    if (crashpad::SetSanitizationInfo(GetCrashReporterClient(),
                                      &browser_sanitization_info_)) {
      arguments.push_back(base::StringPrintf("--sanitization-information=%p",
                                             &browser_sanitization_info_));
    }

    std::string browser_ptype;
    if (GetCrashReporterClient()->GetBrowserProcessType(&browser_ptype)) {
      process_annotations["ptype"] = browser_ptype;
    }

    // Don't handle SIGQUIT in the browser process on Android; the system masks
    // this and uses it for generating ART stack traces, and if it gets unmasked
    // (e.g. by a WebView app) we don't want to treat this as a crash.
    GetCrashpadClient().SetUnhandledSignals({SIGQUIT});

    if (!base::PathExists(handler_path)) {
      LOG(ERROR) << "crashpad HandlerStarter::Initialize, handler path is not "
                    "exist, handler path = "
                 << handler_path;
    }

    if (!ShouldHandleCrashAndUpdateArguments(
            dump_at_crash, GetCrashReporterClient()->ShouldWriteMinidumpToLog(),
            &arguments)) {
      return database_path;
    }

    if (!SetLdLibraryPath(exe_dir)) {
      LOG(ERROR)
          << "crash HandlerStarter::Initialize, set lib path failed, path = "
          << exe_dir;
      return database_path;
    }

    bool result = GetCrashpadClient().StartHandlerAtCrash(
        handler_path, database_path, metrics_path, url, process_annotations,
        arguments);
    DCHECK(result);
    return database_path;
  }

  bool StartHandlerForClient(CrashReporterClient* client,
                             int fd,
                             bool write_minidump_to_database) {
    base::FilePath database_path;
    base::FilePath metrics_path;
    std::string url;
    std::map<std::string, std::string> process_annotations;
    std::vector<std::string> arguments;
    BuildHandlerArgs(client, &database_path, &metrics_path, &url,
                     &process_annotations, &arguments);

    LOG(INFO) << "crashpad HandlerStarter::StartHandlerForClient, crashpad "
                 "report database path = "
              << database_path << ", metrics path = " << metrics_path
              << ", upload url = " << url;
    base::FilePath exe_dir;
    base::FilePath handler_path;
    if (!GetHandlerPath(&exe_dir, &handler_path)) {
      LOG(ERROR) << "crash HandlerStarter::StartHandlerForClient, get handler "
                    "bin path failed";
      return false;
    }

    if (!ShouldHandleCrashAndUpdateArguments(write_minidump_to_database,
                                             client->ShouldWriteMinidumpToLog(),
                                             &arguments)) {
      LOG(ERROR) << "crash HandlerStarter::StartHandlerForClient, build "
                    "handler bin args failed";
      return true;
    }

    if (!SetLdLibraryPath(exe_dir)) {
      LOG(ERROR) << "crash HandlerStarter::StartHandlerForClient, set lib path "
                    "failed, path = "
                 << exe_dir;
      return false;
    }

    return GetCrashpadClient().StartHandlerForClient(
        handler_path, database_path, metrics_path, url, process_annotations,
        arguments, fd);
  }

 private:
  HandlerStarter() = default;
  ~HandlerStarter() = delete;

  crashpad::SanitizationInformation browser_sanitization_info_;
};

bool g_is_browser = false;

}  // namespace

// TODO(jperaza): This might be simplified to have both the browser and child
// processes use CRASHPAD_SIMULATE_CRASH() if CrashpadClient allows injecting
// the Chromium specific SandboxedHandler.
NO_SANITIZE("cfi-icall") void DumpWithoutCrashing() {
  if (g_is_browser) {
    CRASHPAD_SIMULATE_CRASH();
  } else {
    siginfo_t siginfo;
    siginfo.si_signo = crashpad::Signals::kSimulatedSigno;
    siginfo.si_errno = 0;
    siginfo.si_code = 0;

    ucontext_t context;
    crashpad::CaptureContext(&context);

    crashpad::SandboxedHandler::Get()->HandleCrashNonFatal(siginfo.si_signo,
                                                           &siginfo, &context);
  }
}

bool GetHandlerSocket(int* fd, pid_t* pid) {
  return crashpad::CrashpadClient::GetHandlerSocket(fd, pid);
}

void AllowMemoryRange(void* begin, size_t length) {
  crashpad::AllowedMemoryRanges::Singleton()->AddEntry(
      crashpad::FromPointerCast<crashpad::VMAddress>(begin),
      static_cast<crashpad::VMSize>(length));
}

namespace internal {

bool StartHandlerForClient(int fd, bool write_minidump_to_database) {
  return HandlerStarter::Get()->StartHandlerForClient(
      GetCrashReporterClient(), fd, write_minidump_to_database);
}

bool PlatformCrashpadInitialization(
    bool initial_client,
    bool browser_process,
    bool embedded_handler,
    const std::string& user_data_dir,
    const base::FilePath& exe_path,
    const std::vector<std::string>& initial_arguments,
    base::FilePath* database_path) {
  DCHECK_EQ(initial_client, browser_process);
  DCHECK(initial_arguments.empty());

  DCHECK(!embedded_handler);
  DCHECK(exe_path.empty());

  g_is_browser = browser_process;

  bool dump_at_crash = true;
  //   base::android::SetJavaExceptionCallback(SetJavaExceptionInfo);

  unsigned int dump_percentage =
      GetCrashReporterClient()->GetCrashDumpPercentage();
  if (dump_percentage < 100 &&
      static_cast<unsigned int>(base::RandInt(0, 99)) >= dump_percentage) {
    dump_at_crash = false;
  }

  if (browser_process) {
    return true;
  }

  LOG(INFO) << "crashpad PlatformCrashpadInitialization, other process init";
  crashpad::SandboxedHandler* handler = crashpad::SandboxedHandler::Get();
  bool result = handler->Initialize(dump_at_crash);
  DCHECK(result);

  *database_path = base::FilePath();
  return true;
}

}  // namespace internal

}  // namespace crash_reporter
