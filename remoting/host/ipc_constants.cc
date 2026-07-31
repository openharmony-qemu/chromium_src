// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "remoting/host/ipc_constants.h"

#include "base/environment.h"
#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/path_service.h"
#include "base/strings/stringprintf.h"
#include "build/build_config.h"
#include "mojo/public/cpp/platform/named_platform_channel.h"
#include "remoting/host/base/username.h"

#if BUILDFLAG(IS_LINUX)
#include "base/nix/xdg_util.h"
#include "remoting/base/file_path_util_linux.h"
#endif

namespace remoting {

namespace {

#if !defined(NDEBUG)
// Use a different IPC name for debug builds so that we can run the host
// directly from out/Debug without interfering with the production host that
// might also be running.
constexpr char kChromotingHostServicesIpcName[] =
    "chromoting.host_services_debug_mojo_ipc";
#else
constexpr char kChromotingHostServicesIpcName[] =
    "chromoting.host_services_mojo_ipc";
#endif

#if BUILDFLAG(IS_MAC)

#if !defined(NDEBUG)
constexpr char kAgentProcessBrokerIpcName[] =
    "chromoting.agent_process_broker_debug_mojo_ipc";
#else
// Must match the `MachServices` key in org.chromium.chromoting.broker.plist.
constexpr char kAgentProcessBrokerIpcName[] =
    "chromoting.agent_process_broker_mojo_ipc";
#endif

#endif

}  // namespace

const base::FilePath::CharType kHostBinaryName[] =
    FILE_PATH_LITERAL("remoting_host");

const base::FilePath::CharType kDesktopBinaryName[] =
    FILE_PATH_LITERAL("remoting_desktop");

const uint64_t kChromotingHostServicesMessagePipeId = 0u;

bool GetInstalledBinaryPath(const base::FilePath::StringType& binary,
                            base::FilePath* full_path) {
  base::FilePath dir_path;
  if (!base::PathService::Get(base::DIR_EXE, &dir_path)) {
    LOG(ERROR) << "Failed to get the executable file name.";
    return false;
  }

  base::FilePath path = dir_path.Append(binary);

#if BUILDFLAG(IS_WIN)
  path = path.ReplaceExtension(FILE_PATH_LITERAL("exe"));
#endif  // BUILDFLAG(IS_WIN)

  *full_path = path;
  return true;
}

const mojo::NamedPlatformChannel::ServerName&
GetChromotingHostServicesServerName() {
  static const base::NoDestructor<mojo::NamedPlatformChannel::ServerName>
      server_name([]() {
#if BUILDFLAG(IS_LINUX)
        // The Linux single-process host runs as the login user, so place the
        // mojo server socket in the user's XDG_RUNTIME_DIR instead of /tmp
        // (which is susceptible to CWE-377). XDG_RUNTIME_DIR is per-user and
        // only readable by the owner, so the username is no longer needed in
        // the socket name.
        auto env = base::Environment::Create();
        return mojo::NamedPlatformChannel::ServerNameFromUTF8(
            base::nix::GetXDGDirectory(env.get(), "XDG_RUNTIME_DIR",
                                       ".config/chrome-remote-desktop")
                .Append(kChromotingHostServicesIpcName)
                .value());
#else
        // On other platforms (Windows and Mac) ServerName is globally unique,
        // so /tmp-style path construction does not apply.
        return mojo::NamedPlatformChannel::ServerNameFromUTF8(
            kChromotingHostServicesIpcName);
#endif
      }());
  return *server_name;
}

#if BUILDFLAG(IS_MAC)

const char kAgentProcessBrokerMessagePipeId[] = "agent-process-broker";

const mojo::NamedPlatformChannel::ServerName&
GetAgentProcessBrokerServerName() {
  static const base::NoDestructor<mojo::NamedPlatformChannel::ServerName>
      server_name(mojo::NamedPlatformChannel::ServerNameFromUTF8(
          kAgentProcessBrokerIpcName));
  return *server_name;
}

#endif

}  // namespace remoting
