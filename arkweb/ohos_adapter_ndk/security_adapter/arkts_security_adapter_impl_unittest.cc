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
const std::string ArktsSecurityAdapterImplTest::VERSION = "1.0";
const std::string ArktsSecurityAdapterImplTest::TEST_EVENT = "test_security_event";
const std::string ArktsSecurityAdapterImplTest::EMPTY_EVENT = "";
const std::string ArktsSecurityAdapterImplTest::SPECIAL_EVENT = R"(event with "quotes" and \backslashes\)";

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
        .WillOnce(DoAll(SetArgPointee<0>(expectedEnv), Return(napi_ok)));
    
    napi_env result = adapter.GetEnv();
    EXPECT_EQ(result, expectedEnv);
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
        .WillOnce(DoAll(SetArgPointee<0>(expectedEnv), Return(napi_ok)));
    
    napi_env result1 = adapter.GetEnv();
    EXPECT_EQ(result1, expectedEnv);
    
    napi_env result2 = adapter.GetEnv();
    EXPECT_EQ(result2, expectedEnv);
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
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    napi_value result = adapter.GetSecurityGuardModule();
    EXPECT_EQ(result, securityModule);
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
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    napi_value result1 = adapter.GetSecurityGuardModule();
    EXPECT_EQ(result1, securityModule);
    
    napi_value result2 = adapter.GetSecurityGuardModule();
    EXPECT_EQ(result2, securityModule);
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
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
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
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    // GetSecurityGuardModule
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    // napi_create_object
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    // napi_get_named_property for reportSecurityEvent
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    // napi_create_uint32 for eventId
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    // napi_set_named_property for eventId
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    // napi_create_string_utf8 for version
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    // napi_set_named_property for version
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    // napi_create_string_utf8 for content
    std::string expectedJson = R"({"content":"test_event_data"})";
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    // napi_set_named_property for content
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("content"), content))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    // napi_call_function
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    int result = adapter.RegisterEvent(testEvent);
    EXPECT_EQ(result, 0);
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
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    std::string expectedJson = R"({"content":""})";
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("content"), content))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    int result = adapter.RegisterEvent(emptyEvent);
    EXPECT_EQ(result, 0);
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
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
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
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
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
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
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
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
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
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
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
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
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
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
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
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    std::string expectedJson = R"({"content":"test_event_data"})";
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("content"), content))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
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
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<0>(env), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_load_module_with_info(env, StrEq("@ohos.security.securityGuard"), 
                                                                nullptr, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(securityModule), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_object(env, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<1>(eventObj), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_get_named_property(env, securityModule, 
                                                            StrEq("reportSecurityEvent"), _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(reportFn), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_uint32(env, EVENT_ID, _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<2>(eventId), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("eventId"), eventId))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(VERSION.c_str()), 
                                                            VERSION.size(), _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(version), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("version"), version))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    std::string expectedJson = R"({"content":"event with \"quotes\" and \\backslashes\\"})";
    EXPECT_CALL(*g_MockNapiFunctions, napi_create_string_utf8(env, StrEq(expectedJson.c_str()), 
                                                            expectedJson.size(), _))
        .InSequence(seq)
        .WillOnce(DoAll(SetArgPointee<3>(content), Return(napi_ok)));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_set_named_property(env, eventObj, 
                                                            StrEq("content"), content))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    EXPECT_CALL(*g_MockNapiFunctions, napi_call_function(env, securityModule, reportFn, 1, _, nullptr))
        .InSequence(seq)
        .WillOnce(Return(napi_ok));
    
    int result = adapter.RegisterEvent(specialEvent);
    EXPECT_EQ(result, 0);
}

} // namespace NWeb
} // namespace OHOS