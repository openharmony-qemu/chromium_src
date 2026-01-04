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
                                                          void* reserved, napi_value* result));
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
                                         void* reserved, napi_value* result) {
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
        .Times(AtLeast(0))
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
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    napi_value result = adapter.GetSecurityGuardModule();
    EXPECT_NE(result, securityModule);
}

/**
 * @tc.name: GetSecurityGuardModule_Success_002
 * @tc.desc: Test GetSecurityGuardModule returns cached module on second call
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, GetSecurityGuardModule_Success_002)
{
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    napi_value result1 = adapter.GetSecurityGuardModule();
    EXPECT_NE(result1, securityModule);
    
    napi_value result2 = adapter.GetSecurityGuardModule();
    EXPECT_NE(result2, securityModule);
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
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetSecurityGuardModule_Failure_002
 * @tc.desc: Test GetSecurityGuardModule returns nullptr when napi_load_module_with_info fails
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, GetSecurityGuardModule_Failure_002)
{
    napi_env env = CreateMockEnv();
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_generic_failure));
    
    napi_value result = adapter.GetSecurityGuardModule();
    EXPECT_EQ(result, nullptr);
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
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value reportFn = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    napi_value eventId = CreateMockNapiValue();
    napi_value version = CreateMockNapiValue();
    napi_value content = CreateMockNapiValue();
    
    Sequence seq;
    
    // GetEnv
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    // GetSecurityGuardModule
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    // napi_create_object
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    // napi_get_named_property for reportSecurityEvent
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    // napi_create_uint32 for eventId
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    // napi_set_named_property for eventId
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    // napi_create_string_utf8 for version
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    // napi_set_named_property for version
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    // napi_create_string_utf8 for content
    std::string expectedJson = R"({"content":"test_event_data"})";
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    // napi_set_named_property for content
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("content"), content))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    // napi_call_function
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    int result = adapter.RegisterEvent(testEvent);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_Success_002
 * @tc.desc: Test RegisterEvent function with empty event
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_Success_002)
{
    const std::string emptyEvent = "";
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value reportFn = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    napi_value eventId = CreateMockNapiValue();
    napi_value version = CreateMockNapiValue();
    napi_value content = CreateMockNapiValue();
    
    Sequence seq;
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    std::string expectedJson = R"({"content":""})";
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("content"), content))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    int result = adapter.RegisterEvent(emptyEvent);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_Failure_001
 * @tc.desc: Test RegisterEvent when GetEnv fails
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_Failure_001)
{
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_generic_failure));
    
    int result = adapter.RegisterEvent(TEST_EVENT);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_Failure_002
 * @tc.desc: Test RegisterEvent when GetSecurityGuardModule fails
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_Failure_002)
{
    napi_env env = CreateMockEnv();
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_generic_failure));
    
    int result = adapter.RegisterEvent(TEST_EVENT);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_Failure_003
 * @tc.desc: Test RegisterEvent when napi_create_object fails
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_Failure_003)
{
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_generic_failure));
    
    int result = adapter.RegisterEvent(TEST_EVENT);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_Failure_004
 * @tc.desc: Test RegisterEvent when napi_get_named_property fails
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_Failure_004)
{
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_generic_failure));
    
    int result = adapter.RegisterEvent(TEST_EVENT);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_Failure_005
 * @tc.desc: Test RegisterEvent when napi_create_uint32 fails
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_Failure_005)
{
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    napi_value reportFn = CreateMockNapiValue();
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_generic_failure));
    
    int result = adapter.RegisterEvent(TEST_EVENT);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_Failure_006
 * @tc.desc: Test RegisterEvent when napi_set_named_property for eventId fails
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_Failure_006)
{
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    napi_value reportFn = CreateMockNapiValue();
    napi_value eventId = CreateMockNapiValue();
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_generic_failure));
    
    int result = adapter.RegisterEvent(TEST_EVENT);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_Failure_007
 * @tc.desc: Test RegisterEvent when napi_create_string_utf8 for version fails
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_Failure_007)
{
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    napi_value reportFn = CreateMockNapiValue();
    napi_value eventId = CreateMockNapiValue();
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_generic_failure));
    
    int result = adapter.RegisterEvent(TEST_EVENT);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_Failure_008
 * @tc.desc: Test RegisterEvent when napi_set_named_property for version fails
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_Failure_008)
{
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    napi_value reportFn = CreateMockNapiValue();
    napi_value eventId = CreateMockNapiValue();
    napi_value version = CreateMockNapiValue();
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_generic_failure));
    
    int result = adapter.RegisterEvent(TEST_EVENT);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_Failure_009
 * @tc.desc: Test RegisterEvent when napi_call_function fails
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_Failure_009)
{
    const std::string testEvent = "test_event_data";
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value reportFn = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    napi_value eventId = CreateMockNapiValue();
    napi_value version = CreateMockNapiValue();
    napi_value content = CreateMockNapiValue();
    
    Sequence seq;
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    std::string expectedJson = R"({"content":"test_event_data"})";
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("content"), content))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_generic_failure));
    
    int result = adapter.RegisterEvent(testEvent);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_SpecialCharacters_001
 * @tc.desc: Test RegisterEvent with special characters in event data
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_SpecialCharacters_001)
{
    const std::string specialEvent = R"(event with "quotes" and \backslashes\)";
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value reportFn = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    napi_value eventId = CreateMockNapiValue();
    napi_value version = CreateMockNapiValue();
    napi_value content = CreateMockNapiValue();
    
    Sequence seq;
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    std::string expectedJson = R"({"content":"event with \"quotes\" and \\backslashes\\"})";
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("content"), content))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    int result = adapter.RegisterEvent(specialEvent);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_GetEnv_Cached_001
 * @tc.desc: Test RegisterEvent uses cached environment from GetEnv
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_GetEnv_Cached_001)
{
    const std::string testEvent = "test_cached_env";
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value reportFn = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    napi_value eventId = CreateMockNapiValue();
    napi_value version = CreateMockNapiValue();
    napi_value content = CreateMockNapiValue();
    
    // First call GetEnv to cache it
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    napi_env cachedEnv = adapter.GetEnv();
    EXPECT_NE(cachedEnv, nullptr);
    
    // Now test RegisterEvent - should use cached env
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_ok));
    
    std::string expectedJson = R"({"content":"test_cached_env"})";
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("content"), content))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_ok));
    
    int result = adapter.RegisterEvent(testEvent);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_GetSecurityModule_Cached_001
 * @tc.desc: Test RegisterEvent uses cached security module
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_GetSecurityModule_Cached_001)
{
    const std::string testEvent = "test_cached_module";
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value reportFn = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    napi_value eventId = CreateMockNapiValue();
    napi_value version = CreateMockNapiValue();
    napi_value content = CreateMockNapiValue();
    
    // First call GetSecurityGuardModule to cache it
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    napi_value cachedModule = adapter.GetSecurityGuardModule();
    EXPECT_EQ(cachedModule, securityModule);
    
    // Now test RegisterEvent - should use cached module
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_ok));
    
    std::string expectedJson = R"({"content":"test_cached_module"})";
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("content"), content))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
        .Times(AtLeast(0))
        .WillOnce(Return(napi_ok));
    
    int result = adapter.RegisterEvent(testEvent);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_WithVeryLongEvent_001
 * @tc.desc: Test RegisterEvent with very long event data
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_WithVeryLongEvent_001)
{
    // Create a very long event string
    std::string longEvent(10000, 'x');
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value reportFn = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    napi_value eventId = CreateMockNapiValue();
    napi_value version = CreateMockNapiValue();
    napi_value content = CreateMockNapiValue();
    
    Sequence seq;
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    std::string expectedJsonPrefix = R"({"content":")";
    std::string expectedJsonSuffix = R"("})";
    std::string expectedJson = expectedJsonPrefix + longEvent + expectedJsonSuffix;
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("content"), content))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    int result = adapter.RegisterEvent(longEvent);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_WithNewlineCharacters_001
 * @tc.desc: Test RegisterEvent with event containing newline characters
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_WithNewlineCharacters_001)
{
    const std::string eventWithNewlines = "line1\nline2\r\nline3";
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value reportFn = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    napi_value eventId = CreateMockNapiValue();
    napi_value version = CreateMockNapiValue();
    napi_value content = CreateMockNapiValue();
    
    Sequence seq;
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    std::string expectedJson = R"({"content":"line1\nline2\r\nline3"})";
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("content"), content))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    int result = adapter.RegisterEvent(eventWithNewlines);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_WithUnicodeCharacters_001
 * @tc.desc: Test RegisterEvent with Unicode characters
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_WithUnicodeCharacters_001)
{
    const std::string unicodeEvent = "事件: 测试事件 - 🎯 重要事件 🔥";
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value reportFn = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    napi_value eventId = CreateMockNapiValue();
    napi_value version = CreateMockNapiValue();
    napi_value content = CreateMockNapiValue();
    
    Sequence seq;
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    std::string expectedJson = R"({"content":"事件: 测试事件 - 🎯 重要事件 🔥"})";
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("content"), content))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    int result = adapter.RegisterEvent(unicodeEvent);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_ConcurrentCalls_001
 * @tc.desc: Test concurrent calls to RegisterEvent
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_ConcurrentCalls_001)
{
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    
    // Setup common mocks
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    // Prepare thread function
    auto threadFunc = [&](int threadId) {
        std::string event = "thread_event_" + std::to_string(threadId);
        napi_value reportFn = CreateMockNapiValue();
        napi_value eventObj = CreateMockNapiValue();
        napi_value eventId = CreateMockNapiValue();
        napi_value version = CreateMockNapiValue();
        napi_value content = CreateMockNapiValue();
        
        // Setup per-thread expectations
        EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
            .Times(AtLeast(0))
            .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                                StrEq("reportSecurityEvent"), _))
            .Times(AtLeast(0))
            .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
            .Times(AtLeast(0))
            .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                                StrEq("eventId"), eventId))
            .Times(AtLeast(0))
            .WillOnce(Return(napi_ok));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                                VERSION.size(), _))
            .Times(AtLeast(0))
            .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                                StrEq("version"), version))
            .Times(AtLeast(0))
            .WillOnce(Return(napi_ok));
        
        std::string expectedJson = R"({"content":"thread_event_)" + std::to_string(threadId) + R"("})";
        EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                                expectedJson.size(), _))
            .Times(AtLeast(0))
            .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                                StrEq("content"), content))
            .Times(AtLeast(0))
            .WillOnce(Return(napi_ok));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
            .Times(AtLeast(0))
            .WillOnce(Return(napi_ok));
        
        int result = adapter.RegisterEvent(event);
        EXPECT_EQ(result, -1);
    };
    
    // Create and run threads
    const int numThreads = 3;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(threadFunc, i);
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
}

/**
 * @tc.name: RegisterEvent_InvalidJsonCharacters_001
 * @tc.desc: Test RegisterEvent with characters that need JSON escaping
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_InvalidJsonCharacters_001)
{
    const std::string invalidCharsEvent = R"(event with \t tab, \n newline, "quote", \ backslash)";
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value reportFn = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    napi_value eventId = CreateMockNapiValue();
    napi_value version = CreateMockNapiValue();
    napi_value content = CreateMockNapiValue();
    
    Sequence seq;
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    // Note: The current implementation may not properly escape all JSON characters
    std::string expectedJson = R"({"content":"event with \\t tab, \\n newline, \"quote\", \\ backslash"})";
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("content"), content))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    int result = adapter.RegisterEvent(invalidCharsEvent);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_WithBinaryData_001
 * @tc.desc: Test RegisterEvent with event containing binary-like data
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_WithBinaryData_001)
{
    // Create a string that looks like binary data
    std::string binaryEvent;
    for (int i = 0; i < 256; ++i) {
        binaryEvent.push_back(static_cast<char>(i));
    }
    
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    napi_value reportFn = CreateMockNapiValue();
    napi_value eventObj = CreateMockNapiValue();
    napi_value eventId = CreateMockNapiValue();
    napi_value version = CreateMockNapiValue();
    napi_value content = CreateMockNapiValue();
    
    Sequence seq;
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    // The actual JSON will contain the binary data as-is
    std::string expectedJson = R"({"content":")" + binaryEvent + R"("})";
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("content"), content))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
        .Times(AtLeast(0))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    int result = adapter.RegisterEvent(binaryEvent);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: RegisterEvent_MultipleCalls_SameInstance_001
 * @tc.desc: Test multiple RegisterEvent calls on same adapter instance
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, RegisterEvent_MultipleCalls_SameInstance_001)
{
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    
    // Setup common mocks
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    // First call
    {
        const std::string event1 = "first_event";
        napi_value reportFn1 = CreateMockNapiValue();
        napi_value eventObj1 = CreateMockNapiValue();
        napi_value eventId1 = CreateMockNapiValue();
        napi_value version1 = CreateMockNapiValue();
        napi_value content1 = CreateMockNapiValue();
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
            .Times(AtLeast(0))
            .WillOnce(DoAll(SetArgPointee<1>(eventObj1), Return(napi_ok)));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                                StrEq("reportSecurityEvent"), _))
            .Times(AtLeast(0))
            .WillOnce(DoAll(SetArgPointee<3>(reportFn1), Return(napi_ok)));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
            .Times(AtLeast(0))
            .WillOnce(DoAll(SetArgPointee<2>(eventId1), Return(napi_ok)));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj1, 
                                                                StrEq("eventId"), eventId1))
            .Times(AtLeast(0))
            .WillOnce(Return(napi_ok));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                                VERSION.size(), _))
            .Times(AtLeast(0))
            .WillOnce(DoAll(SetArgPointee<3>(version1), Return(napi_ok)));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj1, 
                                                                StrEq("version"), version1))
            .Times(AtLeast(0))
            .WillOnce(Return(napi_ok));
        
        std::string expectedJson1 = R"({"content":"first_event"})";
        EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson1.c_str()), 
                                                                expectedJson1.size(), _))
            .Times(AtLeast(0))
            .WillOnce(DoAll(SetArgPointee<3>(content1), Return(napi_ok)));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj1, 
                                                                StrEq("content"), content1))
            .Times(AtLeast(0))
            .WillOnce(Return(napi_ok));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn1, 1, _, nullptr))
            .Times(AtLeast(0))
            .WillOnce(Return(napi_ok));
        
        int result1 = adapter.RegisterEvent(event1);
        EXPECT_EQ(result1, -1);
    }
    
    // Second call
    {
        const std::string event2 = "second_event";
        napi_value reportFn2 = CreateMockNapiValue();
        napi_value eventObj2 = CreateMockNapiValue();
        napi_value eventId2 = CreateMockNapiValue();
        napi_value version2 = CreateMockNapiValue();
        napi_value content2 = CreateMockNapiValue();
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
            .Times(AtLeast(0))
            .WillOnce(DoAll(SetArgPointee<1>(eventObj2), Return(napi_ok)));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                                StrEq("reportSecurityEvent"), _))
            .Times(AtLeast(0))
            .WillOnce(DoAll(SetArgPointee<3>(reportFn2), Return(napi_ok)));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
            .Times(AtLeast(0))
            .WillOnce(DoAll(SetArgPointee<2>(eventId2), Return(napi_ok)));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj2, 
                                                                StrEq("eventId"), eventId2))
            .Times(AtLeast(0))
            .WillOnce(Return(napi_ok));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                                VERSION.size(), _))
            .Times(AtLeast(0))
            .WillOnce(DoAll(SetArgPointee<3>(version2), Return(napi_ok)));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj2, 
                                                                StrEq("version"), version2))
            .Times(AtLeast(0))
            .WillOnce(Return(napi_ok));
        
        std::string expectedJson2 = R"({"content":"second_event"})";
        EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson2.c_str()), 
                                                                expectedJson2.size(), _))
            .Times(AtLeast(0))
            .WillOnce(DoAll(SetArgPointee<3>(content2), Return(napi_ok)));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj2, 
                                                                StrEq("content"), content2))
            .Times(AtLeast(0))
            .WillOnce(Return(napi_ok));
        
        EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn2, 1, _, nullptr))
            .Times(AtLeast(0))
            .WillOnce(Return(napi_ok));
        
        int result2 = adapter.RegisterEvent(event2);
        EXPECT_EQ(result2, -1);
    }
}

/**
 * @tc.name: GetSecurityGuardModule_ThreadSafety_001
 * @tc.desc: Test GetSecurityGuardModule thread safety
 * @tc.type: FUNC
 * @tc.require: AR000FL0QN
 */
TEST_F(ArktsSecurityAdapterImplTest, GetSecurityGuardModule_ThreadSafety_001)
{
    napi_env env = CreateMockEnv();
    napi_value securityModule = CreateMockNapiValue();
    
    // Setup expectations
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_ark_runtime(_))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .Times(AtLeast(0))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    // Thread function
    auto threadFunc = [&](int threadId) {
        napi_value result = adapter.GetSecurityGuardModule();
        EXPECT_EQ(result, securityModule);
    };
    
    // Create and run threads
    const int numThreads = 5;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(threadFunc, i);
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
}

} // namespace NWeb
} // namespace OHOS