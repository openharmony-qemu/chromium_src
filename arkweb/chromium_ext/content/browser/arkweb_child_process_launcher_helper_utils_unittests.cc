/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <signal.h>
#include <sys/wait.h>
#include <memory>
#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/content/browser/arkweb_child_process_launcher_helper_utils.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/process/process.h"
#include "base/threading/platform_thread.h"
#include "content/browser/child_process_launcher.h"
#include "content/browser/child_process_launcher_helper.h"
#include "content/public/common/sandboxed_process_launcher_delegate.h"
#include "content/public/test/browser_task_environment.h"
#include "mojo/core/embedder/embedder.h"
#include "mojo/public/cpp/system/invitation.h"
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif

using namespace testing;

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
class MockAafwkAppMgrClientAdapter
    : public OHOS::NWeb::AafwkAppMgrClientAdapter {
 public:
  MOCK_METHOD(int,GetRenderProcessTerminationStatus,(pid_t, int&),(override));
  MOCK_METHOD(int,StartRenderProcess,(const std::string&, int32_t, int32_t, int32_t, pid_t&),(override));
  MOCK_METHOD(void,AttachRenderProcess,(std::shared_ptr<OHOS::NWeb::AafwkRenderSchedulerHostAdapter>),(override));
  MOCK_METHOD(int,StartChildProcess,(const std::string&,int32_t,int32_t,int32_t,pid_t&,const std::string&),(override));
  MOCK_METHOD(void,SaveBrowserConnect,(std::shared_ptr<OHOS::NWeb::AafwkBrowserHostAdapter>),(override));
  MOCK_METHOD(bool, IsRenderProcessByUid, (int), (override));
};
#endif

const int TEST_PID = 12345;
const char* PROCESS_TYPES[] = {"renderer", "gpu-process", "test_process"};
const char* CMD_SWITCH[] = {"type", "for-test", "test-switch"};

class MockChildProcessLauncherHelper {
 public:
  using Process = base::Process;

  MockChildProcessLauncherHelper() {
    auto command_line =
        std::make_unique<base::CommandLine>(base::CommandLine::NO_PROGRAM);
    command_line->AppendSwitchASCII(CMD_SWITCH[0], PROCESS_TYPES[0]);

    base::WeakPtr<content::ChildProcessLauncher> empty_launcher;
    real_helper_ =
        std::make_unique<content::internal::ChildProcessLauncherHelper>(
            0, std::move(command_line), nullptr, empty_launcher, false,
#if BUILDFLAG(IS_ANDROID)
            false,
#endif
            mojo::OutgoingInvitation(), mojo::ProcessErrorCallback(), nullptr,
            base::UnsafeSharedMemoryRegion(),
            base::ReadOnlySharedMemoryRegion());

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
    real_helper_->app_mgr_client_adapter_ = nullptr;
#endif
  }

  ~MockChildProcessLauncherHelper() = default;

  content::internal::ChildProcessLauncherHelper* GetRealHelper() {
    return real_helper_.get();
  }

  std::string GetProcessType() const { return PROCESS_TYPES[2]; }

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
  void SetAppMgrClientAdapter(
      std::unique_ptr<OHOS::NWeb::AafwkAppMgrClientAdapter> adapter) {
    real_helper_->app_mgr_client_adapter_ = std::move(adapter);
  }
#endif

 private:
  std::unique_ptr<content::internal::ChildProcessLauncherHelper> real_helper_;
};

namespace content {
namespace internal {

class ArkwebChildProcessLauncherHelperUtilsTest : public ::testing::Test {
 private:
  content::BrowserTaskEnvironment task_environment_;

 protected:
  ArkwebChildProcessLauncherHelperUtilsTest()
      : task_environment_(content::BrowserTaskEnvironment::IO_MAINLOOP) {}

  void SetUp() override {
    mojo::core::Init();
    helper_ = std::make_unique<MockChildProcessLauncherHelper>();
    utils_ = std::make_unique<
        content::internal::ArkwebChildProcessLauncherHelperUtils>(
        helper_->GetRealHelper());
  }

  void TearDown() override {
    utils_.reset();
    helper_.reset();
    base::RunLoop().RunUntilIdle();
    mojo::core::ShutDown();
  }

  std::unique_ptr<MockChildProcessLauncherHelper> helper_;
  std::unique_ptr<content::internal::ArkwebChildProcessLauncherHelperUtils>
      utils_;
  content::ChildProcessTerminationInfo termination_info_;
  pid_t test_pid_ = TEST_PID;
};

const char* LOG_INFOS[] = {
    "Initiate a request to AMS to create a child process",
    "start render process error",
    "report render process create event success",
    "render pid: 12345",
    "GetTermination get app manager client adapter failed",
    "GetTermination get render process termination status success, invalid status = -1",
    "GetTermination known_dead pid: "};
const char* TEST_ARGS[] = {"test_arg1", "test_arg2", "test-value",
                           "arg with spaces", "--special=chars#&%"};
const char* TERMINAL_REASONS[] = {
  "process normal termination",
  "process abnormal termination",
  "process crashed",
  "process was killed",
  "process out of memory",
  "process exit unknown"};
const int FILE_DESCRIPTORS[] = {
  0,    // FD_STDIN
  1,    // FD_STDOUT
  2,    // FD_STDERR
  10,   // FD_TEST_10
  11,   // FD_TEST_11
  12,   // FD_TEST_12
  100,  // FD_TEST_100
  101,  // FD_TEST_101
  102,  // FD_TEST_102
  -1 };
const int ERROR_CODES[] = {
    -1,  // ERROR_CODE_NEG_ONE
    0    // ERROR_CODE_ZERO
};
const int SPECIAL_STATUS_VALUES[] = {
    -1,   // STATUS_VALUE_NEG_ONE
    0,    // STATUS_VALUE_ZERO
    999,  // STATUS_VALUE_999
    100,  // STATUS_VALUE_100
    1000  // STATUS_VALUE_1000
};
const int SIGNAL_CONSTANTS[] = {
    0x7f,  // SIGNAL_MASK
    0x80   // SIGNAL_FLAG
};

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetProcessStatusByExitCode001) {
    int status = 6;
    bool known_dead = false;
    base::TerminationStatus temp = base::TERMINATION_STATUS_PROCESS_CRASHED;
    auto result = ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(status, known_dead);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetProcessStatusByExitCode002) {
    int status = 7;
    bool known_dead = false;
    base::TerminationStatus temp = base::TERMINATION_STATUS_PROCESS_CRASHED;
    auto result = ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(status, known_dead);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetProcessStatusByExitCode003) {
    int status = 8;
    bool known_dead = false;
    base::TerminationStatus temp = base::TERMINATION_STATUS_PROCESS_CRASHED;
    auto result = ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(status, known_dead);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetProcessStatusByExitCode004) {
    int status = 4;
    bool known_dead = false;
    base::TerminationStatus temp = base::TERMINATION_STATUS_PROCESS_CRASHED;
    auto result = ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(status, known_dead);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetProcessStatusByExitCode005) {
    int status = 11;
    bool known_dead = false;
    base::TerminationStatus temp = base::TERMINATION_STATUS_PROCESS_CRASHED;
    auto result = ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(status, known_dead);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetProcessStatusByExitCode006) {
    int status = 5;
    bool known_dead = false;
    base::TerminationStatus temp = base::TERMINATION_STATUS_PROCESS_CRASHED;
    auto result = ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(status, known_dead);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetProcessStatusByExitCode007) {
    int status = 31;
    bool known_dead = false;
    base::TerminationStatus temp = base::TERMINATION_STATUS_PROCESS_CRASHED;
    auto result = ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(status, known_dead);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetProcessStatusByExitCode008) {
    int status = 9;
    bool known_dead = false;
    base::TerminationStatus temp = base::TERMINATION_STATUS_NORMAL_TERMINATION;
    auto result = ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(status, known_dead);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetProcessStatusByExitCode009) {
    int status = 9;
    bool known_dead = true;
    base::TerminationStatus temp = base::TERMINATION_STATUS_PROCESS_WAS_KILLED;
#if BUILDFLAG(IS_CHROMEOS_ASH) || BUILDFLAG(IS_CHROMEOS_LACROS)
    temp = base::TERMINATION_STATUS_PROCESS_WAS_KILLED_BY_OOM;
#endif
    auto result = ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(status, known_dead);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetProcessStatusByExitCode010) {
    int status = 2;
    bool known_dead = false;
    base::TerminationStatus temp = base::TERMINATION_STATUS_PROCESS_WAS_KILLED;
    auto result = ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(status, known_dead);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetProcessStatusByExitCode011) {
    int status = 15;
    bool known_dead = false;
    base::TerminationStatus temp = base::TERMINATION_STATUS_PROCESS_WAS_KILLED;
    auto result = ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(status, known_dead);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetProcessStatusByExitCode012) {
    int status = 1;
    bool known_dead = false;
    base::TerminationStatus temp = base::TERMINATION_STATUS_NORMAL_TERMINATION;
    auto result = ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(status, known_dead);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetProcessStatusByExitCode013) {
    int status = 1 << 8;
    bool known_dead = true;
    base::TerminationStatus temp = base::TERMINATION_STATUS_ABNORMAL_TERMINATION;
    auto result = ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(status, known_dead);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest,GetProcessStatusByExitCode014) {
  std::vector<int> crash_signals = {SIGABRT, SIGBUS,  SIGFPE, SIGILL,SIGSEGV, SIGTRAP, SIGSYS};
  for (int signal : crash_signals) {
    int test_status = (signal & SIGNAL_CONSTANTS[0]) | SIGNAL_CONSTANTS[1];
    base::TerminationStatus result =
        ArkwebChildProcessLauncherHelperUtils::GetProcessStatusByExitCode(test_status, false);
    EXPECT_EQ(result, base::TERMINATION_STATUS_PROCESS_CRASHED);
  }
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetExitReasonByTerminationStatus001) {
    base::TerminationStatus status = base::TERMINATION_STATUS_NORMAL_TERMINATION;
    std::string temp = "process normal termination";
    auto result = ArkwebChildProcessLauncherHelperUtils::GetExitReasonByTerminationStatus(status);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetExitReasonByTerminationStatus002) {
    base::TerminationStatus status = base::TERMINATION_STATUS_ABNORMAL_TERMINATION;
    std::string temp = "process abnormal termination";
    auto result = ArkwebChildProcessLauncherHelperUtils::GetExitReasonByTerminationStatus(status);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetExitReasonByTerminationStatus003) {
    base::TerminationStatus status = base::TERMINATION_STATUS_PROCESS_CRASHED;
    std::string temp = "process crashed";
    auto result = ArkwebChildProcessLauncherHelperUtils::GetExitReasonByTerminationStatus(status);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetExitReasonByTerminationStatus004) {
    base::TerminationStatus status = base::TERMINATION_STATUS_PROCESS_WAS_KILLED;
    std::string temp = "process was killed";
    auto result = ArkwebChildProcessLauncherHelperUtils::GetExitReasonByTerminationStatus(status);
    EXPECT_EQ(result, temp);
}

#if BUILDFLAG(IS_CHROMEOS_ASH) || BUILDFLAG(IS_CHROMEOS_LACROS)
TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetExitReasonByTerminationStatus005) {
    base::TerminationStatus status = base::TERMINATION_STATUS_PROCESS_WAS_KILLED_BY_OOM;
    std::string temp = "process out of memory";
    auto result = ArkwebChildProcessLauncherHelperUtils::GetExitReasonByTerminationStatus(status);
    EXPECT_EQ(result, temp);
}
#endif // BUILDFLAG(IS_CHROMEOS_ASH) || BUILDFLAG(IS_CHROMEOS_LACROS)

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetExitReasonByTerminationStatus006) {
    base::TerminationStatus status = static_cast<base::TerminationStatus>(999);
    std::string temp = "process exit unknown";
    auto result = ArkwebChildProcessLauncherHelperUtils::GetExitReasonByTerminationStatus(status);
    EXPECT_EQ(result, temp);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetExitReasonByTerminationStatus007) {
  EXPECT_EQ(utils_->GetExitReasonByTerminationStatus(base::TERMINATION_STATUS_LAUNCH_FAILED),
      TERMINAL_REASONS[5]);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetExitReasonByTerminationStatus008) {
  std::map<base::TerminationStatus, std::string> status_reason_map = {
      {base::TERMINATION_STATUS_NORMAL_TERMINATION, TERMINAL_REASONS[0]},
      {base::TERMINATION_STATUS_ABNORMAL_TERMINATION, TERMINAL_REASONS[1]},
      {base::TERMINATION_STATUS_PROCESS_CRASHED, TERMINAL_REASONS[2]},
      {base::TERMINATION_STATUS_PROCESS_WAS_KILLED, TERMINAL_REASONS[3]},
#if BUILDFLAG(IS_CHROMEOS_ASH) || BUILDFLAG(IS_CHROMEOS_LACROS)
      {base::TERMINATION_STATUS_PROCESS_WAS_KILLED_BY_OOM, TERMINAL_REASONS[4]}
#endif
  };

  for (const auto& status_reason : status_reason_map) {
    EXPECT_EQ(utils_->GetExitReasonByTerminationStatus(status_reason.first),
        status_reason.second);
  }
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetExitReasonByTerminationStatus009) {
  base::TerminationStatus status = base::TERMINATION_STATUS_PROCESS_CRASHED;
  std::string reason1 = utils_->GetExitReasonByTerminationStatus(status);
  std::string reason2 = utils_->GetExitReasonByTerminationStatus(status);
  EXPECT_EQ(reason1, reason2);
  EXPECT_EQ(reason1, TERMINAL_REASONS[2]);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetExitReasonByTerminationStatus010) {
  base::TerminationStatus status = static_cast<base::TerminationStatus>(0);
  EXPECT_EQ(utils_->GetExitReasonByTerminationStatus(status), TERMINAL_REASONS[0]);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetTerminationInfoArkweb_KnownDeadTrue_AdapterCallFailed) {
  auto mock_adapter = std::make_unique<MockAafwkAppMgrClientAdapter>();
  EXPECT_CALL(*mock_adapter, GetRenderProcessTerminationStatus(test_pid_, testing::_))
      .WillOnce(testing::Return(-1));
  helper_->SetAppMgrClientAdapter(std::move(mock_adapter));

  content::ChildProcessTerminationInfo info = {};
  EXPECT_NO_FATAL_FAILURE(utils_->GetTerminationInfoArkweb(base::Process(test_pid_), true, info));
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetTerminationInfoArkweb_KnownDeadTrue_InvalidStatus) {
  auto mock_adapter = std::make_unique<MockAafwkAppMgrClientAdapter>();
  EXPECT_CALL(*mock_adapter, GetRenderProcessTerminationStatus(test_pid_, testing::_))
      .WillOnce([](pid_t pid, int& status) {
        status = ERROR_CODES[0];
        return 0;
      });
  helper_->SetAppMgrClientAdapter(std::move(mock_adapter));
  content::ChildProcessTerminationInfo info = {};

  testing::internal::CaptureStderr();
  utils_->GetTerminationInfoArkweb(base::Process(test_pid_), true, info);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(log_output.find(LOG_INFOS[5]) != std::string::npos);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetTerminationInfoArkweb_KnownDeadTrue_NormalExit) {
  auto mock_adapter = std::make_unique<MockAafwkAppMgrClientAdapter>();
  EXPECT_CALL(*mock_adapter, GetRenderProcessTerminationStatus(test_pid_, testing::_))
      .WillOnce([](pid_t pid, int& status) {
        status = ERROR_CODES[1];
        return 0;
      });
  helper_->SetAppMgrClientAdapter(std::move(mock_adapter));
  content::ChildProcessTerminationInfo info = {};

  testing::internal::CaptureStderr();
  utils_->GetTerminationInfoArkweb(base::Process(test_pid_), true, info);
  std::string log_output = testing::internal::GetCapturedStderr();

  EXPECT_EQ(info.status, base::TERMINATION_STATUS_NORMAL_TERMINATION);
  EXPECT_TRUE(log_output.find(LOG_INFOS[6] + std::to_string(test_pid_)) != std::string::npos);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetTerminationInfoArkweb_KnownDeadTrue_ProcessCrashed) {
  auto mock_adapter = std::make_unique<MockAafwkAppMgrClientAdapter>();
  EXPECT_CALL(*mock_adapter, GetRenderProcessTerminationStatus(test_pid_, testing::_))
      .WillOnce([](pid_t pid, int& status) {
        status = (SIGSEGV & SIGNAL_CONSTANTS[0]) | SIGNAL_CONSTANTS[1];
        return 0;
      });
  helper_->SetAppMgrClientAdapter(std::move(mock_adapter));
  content::ChildProcessTerminationInfo info = {};

  testing::internal::CaptureStderr();
  utils_->GetTerminationInfoArkweb(base::Process(test_pid_), true, info);
  std::string log_output = testing::internal::GetCapturedStderr();

  EXPECT_EQ(info.status, base::TERMINATION_STATUS_PROCESS_CRASHED);
  EXPECT_TRUE(log_output.find(LOG_INFOS[6] + std::to_string(test_pid_)) != std::string::npos);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, LaunchChildProcess_AdapterNull) {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(CMD_SWITCH[1])) {
    base::CommandLine::ForCurrentProcess()->RemoveSwitch(CMD_SWITCH[1]);
  }

  helper_->SetAppMgrClientAdapter(nullptr);

  base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
  command_line.AppendSwitchASCII(CMD_SWITCH[0], PROCESS_TYPES[0]);

  base::LaunchOptions options;
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[0], 0));
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[1], 1));
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[2], 2));

  ChildProcessLauncherHelper::Process process;
  testing::internal::CaptureStderr();
  EXPECT_NO_FATAL_FAILURE(
      utils_->LaunchChildProcess(&command_line, &options, process));
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(log_output.find(LOG_INFOS[0]) != std::string::npos);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, LaunchChildProcess_StartChildProcessFailed) {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(CMD_SWITCH[1])) {
    base::CommandLine::ForCurrentProcess()->RemoveSwitch(CMD_SWITCH[1]);
  }

  auto mock_adapter = std::make_unique<MockAafwkAppMgrClientAdapter>();
  EXPECT_CALL(*mock_adapter,
      StartChildProcess(testing::_, testing::_, testing::_, testing::_,
          testing::_, testing::_))
      .WillOnce([](const std::string&, int32_t, int32_t, int32_t, pid_t& pid,
                   const std::string&) {
        pid = 0;
        return -1;
      });

  helper_->SetAppMgrClientAdapter(std::move(mock_adapter));

  base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
  command_line.AppendSwitchASCII(CMD_SWITCH[0], PROCESS_TYPES[0]);
  command_line.AppendArg(TEST_ARGS[0]);

  base::LaunchOptions options;
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[3], 0));
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[4], 1));
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[5], 2));

  ChildProcessLauncherHelper::Process process;
  testing::internal::CaptureStderr();
  EXPECT_NO_FATAL_FAILURE(utils_->LaunchChildProcess(&command_line, &options, process));
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(log_output.find(LOG_INFOS[1]) != std::string::npos);
  EXPECT_TRUE(log_output.find("ret=-1") != std::string::npos);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, LaunchChildProcess_Success) {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(CMD_SWITCH[1])) {
    base::CommandLine::ForCurrentProcess()->RemoveSwitch(CMD_SWITCH[1]);
  }

  auto mock_adapter = std::make_unique<MockAafwkAppMgrClientAdapter>();
  EXPECT_CALL(*mock_adapter, StartChildProcess(testing::_, testing::_, testing::_, testing::_,
      testing::_, testing::_))
      .WillOnce([](const std::string&, int32_t, int32_t, int32_t, pid_t& pid, const std::string&) {
        pid = TEST_PID;
        return 0;
      });

  helper_->SetAppMgrClientAdapter(std::move(mock_adapter));

  base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
  command_line.AppendSwitchASCII(CMD_SWITCH[0], PROCESS_TYPES[0]);
  command_line.AppendArg(TEST_ARGS[0]);
  command_line.AppendArg(TEST_ARGS[1]);

  base::LaunchOptions options;
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[3], 0));
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[4], 1));
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[5], 2));

  ChildProcessLauncherHelper::Process process;
  testing::internal::CaptureStderr();
  EXPECT_NO_FATAL_FAILURE(utils_->LaunchChildProcess(&command_line, &options, process));
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(log_output.find(LOG_INFOS[2]) != std::string::npos);
  EXPECT_TRUE(log_output.find(LOG_INFOS[3]) != std::string::npos);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, LaunchChildProcess_CommandLineArgs) {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(CMD_SWITCH[1])) {
    base::CommandLine::ForCurrentProcess()->RemoveSwitch(CMD_SWITCH[1]);
  }

  auto mock_adapter = std::make_unique<MockAafwkAppMgrClientAdapter>();
  EXPECT_CALL(*mock_adapter, IsRenderProcessByUid(20300002))
      .WillOnce(testing::Return(true));
  EXPECT_CALL(*mock_adapter, IsRenderProcessByUid(100000))
      .WillOnce(testing::Return(false));
  std::string captured_argv;
  EXPECT_CALL(*mock_adapter, StartChildProcess(testing::_, testing::_, testing::_, testing::_,
                                testing::_, testing::_))
      .WillOnce([&](const std::string& argv, int32_t, int32_t, int32_t,
          pid_t& pid, const std::string&) {
        captured_argv = argv;
        pid = TEST_PID;
        return 0;
      });

  helper_->SetAppMgrClientAdapter(std::move(mock_adapter));

  base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
  command_line.AppendSwitchASCII(CMD_SWITCH[0], PROCESS_TYPES[0]);
  command_line.AppendSwitchASCII(CMD_SWITCH[2], TEST_ARGS[2]);
  command_line.AppendArg(TEST_ARGS[0]);
  command_line.AppendArg(TEST_ARGS[3]);
  command_line.AppendArg(TEST_ARGS[4]);

  base::LaunchOptions options;
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[3], 0));
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[4], 1));
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[5], 2));

  ChildProcessLauncherHelper::Process process;
  EXPECT_NO_FATAL_FAILURE(utils_->LaunchChildProcess(&command_line, &options, process));
  EXPECT_FALSE(captured_argv.empty());
  EXPECT_TRUE(captured_argv.find(CMD_SWITCH[0]) != std::string::npos);
  EXPECT_TRUE(captured_argv.find(PROCESS_TYPES[0]) != std::string::npos);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, LaunchChildProcess_FileDescriptors) {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(CMD_SWITCH[1])) {
    base::CommandLine::ForCurrentProcess()->RemoveSwitch(CMD_SWITCH[1]);
  }

  auto mock_adapter = std::make_unique<MockAafwkAppMgrClientAdapter>();
  int32_t captured_shared_fd = FILE_DESCRIPTORS[9];
  int32_t captured_ipc_fd = FILE_DESCRIPTORS[9];
  int32_t captured_crash_signal_fd = FILE_DESCRIPTORS[9];

  EXPECT_CALL(*mock_adapter, StartChildProcess(testing::_, testing::_, testing::_, testing::_,
      testing::_, testing::_))
      .WillOnce([&](const std::string&, int32_t ipc_fd, int32_t shared_fd,
          int32_t crash_signal_fd, pid_t& pid, const std::string&) {
        captured_ipc_fd = ipc_fd;
        captured_shared_fd = shared_fd;
        captured_crash_signal_fd = crash_signal_fd;
        pid = TEST_PID;
        return 0;
      });

  helper_->SetAppMgrClientAdapter(std::move(mock_adapter));

  base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
  command_line.AppendSwitchASCII(CMD_SWITCH[0], PROCESS_TYPES[0]);

  base::LaunchOptions options;
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[6], 0));
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[7], 1));
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[8], 2));

  ChildProcessLauncherHelper::Process process;
  EXPECT_NO_FATAL_FAILURE(utils_->LaunchChildProcess(&command_line, &options, process));
  EXPECT_EQ(captured_shared_fd, FILE_DESCRIPTORS[6]);
  EXPECT_EQ(captured_ipc_fd, FILE_DESCRIPTORS[7]);
  EXPECT_EQ(captured_crash_signal_fd, FILE_DESCRIPTORS[8]);
}
#endif  // BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetTerminationInfoArkweb_KnownDeadTrue_AdapterNull) {
  content::ChildProcessTerminationInfo info = {};
  testing::internal::CaptureStderr();
  utils_->GetTerminationInfoArkweb(base::Process(test_pid_), true, info);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(log_output.find(LOG_INFOS[4]) != std::string::npos);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetTerminationInfoArkweb_KnownDeadFalse) {
  content::ChildProcessTerminationInfo info = {};
  utils_->GetTerminationInfoArkweb(base::Process(test_pid_), false, info);
  EXPECT_EQ(info.status, base::TERMINATION_STATUS_STILL_RUNNING);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, GetTerminationInfoArkweb_EmptyProcess) {
  base::Process empty_process;
  content::ChildProcessTerminationInfo info = {};
  EXPECT_NO_FATAL_FAILURE({ utils_->GetTerminationInfoArkweb(empty_process, false, info); });
  EXPECT_EQ(info.status, base::TERMINATION_STATUS_STILL_RUNNING);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, LaunchChildProcess_ForTest) {
  base::CommandLine::ForCurrentProcess()->AppendSwitch(CMD_SWITCH[1]);

  base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
  command_line.AppendSwitchASCII(CMD_SWITCH[0], PROCESS_TYPES[0]);
  command_line.AppendSwitch(CMD_SWITCH[2]);

  base::LaunchOptions options;
  ChildProcessLauncherHelper::Process process;

  EXPECT_NO_FATAL_FAILURE(utils_->LaunchChildProcess(&command_line, &options, process));
  SUCCEED();

  base::CommandLine::ForCurrentProcess()->RemoveSwitch(CMD_SWITCH[1]);
}

TEST_F(ArkwebChildProcessLauncherHelperUtilsTest, LaunchChildProcess_GPUProcessType) {
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP) && \
    BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(CMD_SWITCH[1])) {
    base::CommandLine::ForCurrentProcess()->RemoveSwitch(CMD_SWITCH[1]);
  }
  auto mock_adapter = std::make_unique<MockAafwkAppMgrClientAdapter>();
  EXPECT_CALL(*mock_adapter, SaveBrowserConnect(testing::_))
      .Times(testing::AtMost(1));
  EXPECT_CALL(*mock_adapter,
      StartChildProcess(testing::_, testing::_, testing::_, testing::_,
          testing::_, testing::_))
      .WillOnce([](const std::string& argv, int32_t ipc_fd, int32_t shared_fd,
                   int32_t crash_signal_fd, pid_t& pid,
                   const std::string& process_type) {
        pid = TEST_PID;
        return 0;
      });
  helper_->SetAppMgrClientAdapter(std::move(mock_adapter));
  base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
  command_line.AppendSwitchASCII(CMD_SWITCH[0], PROCESS_TYPES[1]);
  base::LaunchOptions options;
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[3], 0));
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[4], 1));
  options.fds_to_remap.push_back(std::make_pair(FILE_DESCRIPTORS[5], 2));
  ChildProcessLauncherHelper::Process process;

  EXPECT_NO_FATAL_FAILURE(utils_->LaunchChildProcess(&command_line, &options, process));
#endif
}
}  // namespace internal
}  // namespace content
