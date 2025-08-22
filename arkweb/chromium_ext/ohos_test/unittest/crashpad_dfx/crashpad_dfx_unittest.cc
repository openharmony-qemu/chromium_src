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
#include <gtest/gtest.h>
#include <sys/mman.h>

#include "arkweb/chromium_ext/third_party/crashpad/crashpad/util/linux/crashpad_dfx.h"

#include "third_party/bounds_checking_function/include/securec.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

extern uid_t g_process_uid;
extern std::string g_bundle_name;
extern std::string g_happen_time;

uid_t g_process_uid = 0;
std::string g_bundle_name = "test_bundle";
std::string g_happen_time = "20250804123456";

namespace crashpad{

class CrashpadDfxTest : public testing :: Test {
public :
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CrashpadDfxTest::SetUpTestCase() {}

void CrashpadDfxTest::TearDownTestCase() {}

void CrashpadDfxTest::SetUp() {}

void CrashpadDfxTest::TearDown() {}

TEST_F(CrashpadDfxTest, CrashpadDfxTest001){
    auto dfxtest = std::make_shared<CrashpadDfx>();
    EXPECT_NE(dfxtest,nullptr);
    const std::string process_type = "browser";
    const std::string happen_time = "20250804123456";
    const std::string bundle_name = "test bundle";
    const std::string error_reason = "test_error";
    dfxtest->ProcessCrashReport(process_type,
                                happen_time,
                                bundle_name,
                                error_reason);
}

TEST_F(CrashpadDfxTest, CrashpadDfxTest002){
    auto dfxtest = std::make_shared<CrashpadDfx>();
    EXPECT_NE(dfxtest,nullptr);

    int32_t process_type = dfxtest->GetProcessTypeByPid(-1);

    EXPECT_EQ(process_type, -1);
}

TEST_F(CrashpadDfxTest,  CrashpadDfxTest003){
    auto dfxtest = std::make_shared<CrashpadDfx>();
    EXPECT_NE(dfxtest,nullptr);
    pid_t current_pid = getpid();

    int32_t process_type = dfxtest->GetProcessTypeByPid(current_pid);
    EXPECT_EQ(process_type, -1);
}

TEST_F(CrashpadDfxTest,  CrashpadDfxTest004){
    auto dfxtest = std::make_shared<CrashpadDfx>();
    EXPECT_NE(dfxtest,nullptr);

    std::string bundle_name = dfxtest->GetProcessBundleName();
    EXPECT_EQ(bundle_name, "bundle-name");
}

TEST_F(CrashpadDfxTest,  CrashpadDfxTest005){
    std::string current_time = CrashpadDfx::GetCurrentTime();
    EXPECT_EQ(current_time.length(),14);
}

TEST_F(CrashpadDfxTest,  CrashpadDfxTest006){
    g_process_uid = 1000;
    const std::string expected = "crashpad-test-bundle_name-1000-unsupported-20250804123456";
    const std::string result = CrashpadDfx::UpdateCrashDumpPathSuffix();
    EXPECT_EQ(result,expected);
}

}//namespace crashpad