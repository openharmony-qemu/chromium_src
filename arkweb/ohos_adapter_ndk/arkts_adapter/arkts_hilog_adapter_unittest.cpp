/*
* Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "arkts_hilog_adapter.h"
#include "securec.h"
#include "../mock_ndk_api/include/mock_napi_api.h"

using namespace testing;
namespace OHOS {
namespace NWeb{
class ArktsHilogAdapterTest : public testing::Test {
protected:    
    void SetUp() override {
        g_MockNapiFunctions = new MockNapiFunctions();
    }
    
    void TearDown() override {
        delete g_MockNapiFunctions;
        g_MockNapiFunctions = nullptr;
    }
    
    napi_value CreateMockNapiValue() {
        static int counter = 0;
        return reinterpret_cast<napi_value>(++counter);
    }
    
    napi_env CreateMockEnv() {
        static int counter = 0;
        return reinterpret_cast<napi_env>(++counter);
    }
};

TEST_F(ArktsHilogAdapterTest, LogInternal_NoEnv) {
    napi_env env = ArktsHilogAdapter::GetEnv();
    EXPECT_EQ(env, nullptr);
    int result = ArktsHilogAdapter::LogInternal(LogLevel::DEBUG, "%s", "Debug message");
    EXPECT_EQ(result, -1);
}

TEST_F(ArktsHilogAdapterTest, LogInternal_CreateEnv) {
    napi_env expectedEnv = CreateMockEnv();
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(expectedEnv), Return(napi_ok)));
    napi_env env = ArktsHilogAdapter::GetEnv();
    EXPECT_NE(env, nullptr);
}

TEST_F(ArktsHilogAdapterTest, LogInternal_Level) {
    napi_env expectedEnv = CreateMockEnv();
    const char* name = "@ohos.hilog";
    const char* module_info = nullptr;

    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .WillRepeatedly(DoAll(SetArgPointee<0>(expectedEnv), Return(napi_ok)));

    EXPECT_CALL(*g_MockNapiFunctions, napi_open_handle_scope(_, _))
        .WillOnce(Invoke([&](napi_env env, napi_handle_scope* result) {
            *result = nullptr;
            return napi_ok;
        }));
    int result = ArktsHilogAdapter::LogInternal(LogLevel::DEBUG, "%s" , "Debug message");
    EXPECT_EQ(result, -1);

    EXPECT_CALL(*g_MockNapiFunctions, napi_open_handle_scope(_, _))
        .WillRepeatedly(Invoke([&](napi_env env, napi_handle_scope* result) {
            *result = reinterpret_cast<napi_handle_scope>(0x123456);
            return napi_ok;
        }));
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(_, _, _, _))
        .WillOnce(Invoke([&](napi_env env, const char* path, const char* module_info, napi_value* result) {
            *result = nullptr;
            return napi_ok;
        }));
    result = ArktsHilogAdapter::LogInternal(LogLevel::DEBUG, "%s" , "Debug message");
    EXPECT_EQ(result, -1);

    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(_, _, _, _))
        .WillRepeatedly(Invoke([&](napi_env env, const char* path, const char* module_info, napi_value* result) {
            *result = reinterpret_cast<napi_value>(0x123456);
            return napi_ok;
        }));

    result = ArktsHilogAdapter::LogInternal(LogLevel::DEBUG, "%s" , "Debug message");
    EXPECT_EQ(result, 0);
    result = ArktsHilogAdapter::LogInternal(LogLevel::INFO, "%s" , "Info message");
    EXPECT_EQ(result, 0);
    result = ArktsHilogAdapter::LogInternal(LogLevel::WARN, "%s" , "Warning message");
    EXPECT_EQ(result, 0);
    result = ArktsHilogAdapter::LogInternal(LogLevel::ERROR, "%s" , "Error message");
    EXPECT_EQ(result, 0);
    result = ArktsHilogAdapter::LogInternal(LogLevel::FATAL, "%s" , "Fatal message");
    EXPECT_EQ(result, 0);
    result = ArktsHilogAdapter::LogInternal(static_cast<LogLevel>(10), "%s" , "Test message");
    EXPECT_EQ(result, -1);
}
} // namespace NWeb
} // namespace OHOS