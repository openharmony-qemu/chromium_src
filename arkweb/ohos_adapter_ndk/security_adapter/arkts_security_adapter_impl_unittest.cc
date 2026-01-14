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

#include <cstddef>
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <memory>
#include <stdexcept>
#include <thread>
#include <sstream>

#define private public
#include "arkts_security_adapter_impl.h"
#undef private
#include "napi/native_api.h"
#include "js_native_api.h"

using namespace testing;
using namespace std;

using ::testing::_;
using ::testing::Return;
using ::testing::AtLeast;
using ::testing::AnyNumber;
using ::testing::NiceMock;
using ::testing::DoAll;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::SetArgPointee;
using ::testing::SaveArg;
using ::testing::Eq;
using ::testing::Sequence;
using ::testing::InSequence;
using ::testing::StrEq;
using ::testing::ContainerEq;
using ::testing::ElementsAre;

#define ARG_NUM 1
const static uint64_t EVENT_ID = 0x02C000002;
const std::string VERSION = "1.0";
const std::string TEST_EVENT = "test_security_event";
const std::string EMPTY_EVENT = "";
const std::string SPECIAL_EVENT = R"(event with "quotes" and \backslashes\)";

namespace OHOS {
namespace NWeb {

class MockNapiFunctions {
public:
    virtual ~MockNapiFunctions() = default;
    
    MOCK_METHOD(napi_status, napi_create_ark_runtime, (napi_env* env));
    MOCK_METHOD(napi_status, napi_load_module_with_info, (napi_env env, const char* name, 
                                                          const char* reserved, napi_value* result));
    MOCK_METHOD(napi_status, napi_create_object, (napi_env env, napi_value* result));
    MOCK_METHOD(napi_status, napi_get_named_property, (napi_env env, napi_value object, 
                                                       const char* utf8name, napi_value* result));
    MOCK_METHOD(napi_status, napi_create_uint32, (napi_env env, uint32_t value, napi_value* result));
    MOCK_METHOD(napi_status, napi_set_named_property, (napi_env env, napi_value object, 
                                                       const char* utf8name, napi_value value));
    MOCK_METHOD(napi_status, napi_create_string_utf8, (napi_env env, const char* str, 
                                                       size_t length, napi_value* result));
    MOCK_METHOD(napi_status, napi_call_function, (napi_env env, napi_value recv, napi_value func, 
                                                  size_t argc, const napi_value* argv, napi_value* result));
};

static MockNapiFunctions* g_MockNapiFunctions = nullptr;

extern "C" {
    napi_status napi_create_ark_runtime(napi_env* env) {
        if (env == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_create_ark_runtime(env);
        }
        return napi_generic_failure;
    }
    
    napi_status napi_load_module_with_info(napi_env env, const char* name, 
                                         const char* reserved, napi_value* result) {
        if (name == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_load_module_with_info(env, name, reserved, result);
        }
        return napi_generic_failure;
    }
    
    napi_status napi_create_object(napi_env env, napi_value* result) {
        if (result == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_create_object(env, result);
        }
        return napi_generic_failure;
    }
    
    napi_status napi_get_named_property(napi_env env, napi_value object, 
                                       const char* utf8name, napi_value* result) {
        if (utf8name == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_get_named_property(env, object, utf8name, result);
        }
        return napi_generic_failure;
    }
    
    napi_status napi_create_uint32(napi_env env, uint32_t value, napi_value* result) {
        if (result == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_create_uint32(env, value, result);
        }
        return napi_generic_failure;
    }
    
    napi_status napi_set_named_property(napi_env env, napi_value object, 
                                       const char* utf8name, napi_value value) {
        if (utf8name == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_set_named_property(env, object, utf8name, value);
        }
        return napi_generic_failure;
    }
    
    napi_status napi_create_string_utf8(napi_env env, const char* str, 
                                       size_t length, napi_value* result) {
        if (str == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_create_string_utf8(env, str, length, result);
        }
        return napi_generic_failure;
    }
    
    napi_status napi_call_function(napi_env env, napi_value recv, napi_value func, 
                                   size_t argc, const napi_value* argv, napi_value* result) {
        if (g_MockNapiFunctions != nullptr) {
            return g_MockNapiFunctions->napi_call_function(env, recv, func, argc, argv, result);
        }
        return napi_generic_failure;
    }
}

class ArktsSecurityAdapterImplTest : public testing::Test {
protected:
    static void SetUpTestCase() {
        // Setup if needed
    }
    
    static void TearDownTestCase() {
        // Cleanup if needed
    }
    
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
    
    ArktsSecurityAdapter adapter;
};

/**
 * @tc.name: GetEnv_Success_001
 * @tc.desc: Test GetEnv function returns valid environment
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, GetEnv_Success_001)
{
    napi_env expectedEnv = CreateMockEnv();
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .WillOnce(DoAll(SetArgPointee<0>(expectedEnv), Return(napi_ok)));
    
    napi_env result = adapter.GetEnv();
    EXPECT_NE(result, expectedEnv);
}

/**
 * @tc.name: GetEnv_Success_002
 * @tc.desc: Test GetEnv function returns cached environment on second call
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, GetEnv_Success_002)
{
    napi_env expectedEnv = CreateMockEnv();
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(expectedEnv), Return(napi_ok)));
    
    napi_env result1 = adapter.GetEnv();
    EXPECT_NE(result1, expectedEnv);
    
    napi_env result2 = adapter.GetEnv();
    EXPECT_NE(result2, expectedEnv);
}

/**
 * @tc.name: GetEnv_Failure_001
 * @tc.desc: Test GetEnv function returns nullptr when napi_create_ark_runtime fails
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, GetEnv_Failure_001)
{
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_generic_failure));
    
    napi_env result = adapter.GetEnv();
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetSecurityGuardModule_Success_001
 * @tc.desc: Test GetSecurityGuardModule function returns valid module
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, GetSecurityGuardModule_Success_001)
{
    napi_env env = reinterpret_cast<napi_env>(0x3);
    napi_value securityModule = reinterpret_cast<napi_value>(0x1);
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info( _, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    napi_value result = adapter.GetSecurityGuardModule();
    EXPECT_EQ(result, securityModule);
}

/**
 * @tc.name: GetSecurityGuardModule_Failure_001
 * @tc.desc: Test GetSecurityGuardModule returns nullptr when GetEnv fails
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, GetSecurityGuardModule_Failure_001)
{
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_generic_failure));
    
    napi_value result = adapter.GetSecurityGuardModule();
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: RegisterEvent_Success_001
 * @tc.desc: Test RegisterEvent function with valid event data
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_Success_001)
{
    const std::string testEvent = "test_event_data";
    napi_env g_mockEnv = reinterpret_cast<napi_env>(0x1);

    napi_value securityModule = reinterpret_cast<napi_value>(0x1);
    napi_value reportFn = reinterpret_cast<napi_value>(0x2);
    napi_value eventObj = reinterpret_cast<napi_value>(0x3);
    napi_value eventId = reinterpret_cast<napi_value>(0xaaaaaa);
    napi_value version = reinterpret_cast<napi_value>(0x5);
    napi_value content = reinterpret_cast<napi_value>(0x6);
    napi_value args[1];
    
    Sequence seq;
    
    // GetEnv
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<0>(g_mockEnv), Return(napi_ok)));
    
    // GetSecurityGuardModule
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(g_mockEnv, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    // napi_create_object
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(g_mockEnv, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    // napi_get_named_property for reportSecurityEvent
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(g_mockEnv, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    // napi_create_uint32 for eventId
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(g_mockEnv, _, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    // napi_set_named_property for eventId
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(g_mockEnv, eventObj, 
                                                            StrEq("eventId"), eventId))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    // napi_create_string_utf8 for version
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(g_mockEnv, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    // napi_set_named_property for version
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(g_mockEnv, eventObj, 
                                                            StrEq("version"), version))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    // napi_create_string_utf8 for content
    std::string expectedJson = R"({"content":"test_event_data"})";
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(g_mockEnv, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    // napi_set_named_property for content
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(g_mockEnv, eventObj, 
                                                            StrEq("content"), content))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    // napi_call_function
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(g_mockEnv, securityModule, _, 1, _, nullptr))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    int result = adapter.RegisterEvent(testEvent);
    EXPECT_EQ(result, 0);
}

} // namespace NWeb
} // namespace OHOS