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

#include <memory>
#include <gtest/gtest.h>
#include "arkweb/build/features/features.h"
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif
#include "arkweb/chromium_ext/content/browser/arkweb_child_process_launcher_helper_utils.h"

using namespace testing;

namespace content {
namespace internal {

class ArkwebChildProcessLauncherHelperUtilsTest : public ::testing::Test {
public:
    void SetUp() override {}
    void TearDown() override {}
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
#endif // BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP)
} // internal
} // content
