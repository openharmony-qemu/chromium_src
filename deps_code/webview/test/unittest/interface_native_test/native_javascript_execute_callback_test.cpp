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

#define private public
#include "base/web/webview/interfaces/native/native_javascript_execute_callback.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace NWeb {

class NativeJavascriptExecuteCallbackTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NativeJavascriptExecuteCallbackTest::SetUpTestCase(void) {}

void NativeJavascriptExecuteCallbackTest::TearDownTestCase(void) {}

void NativeJavascriptExecuteCallbackTest::SetUp(void) {}

void NativeJavascriptExecuteCallbackTest::TearDown(void) {}

void CallbackNative(const char* message) {}

/**
 * @tc.name  : OHNativeJavascriptExecuteCallbackTest_OnReceiveValue_01
 * @tc.desc  : Test OnReceiveValue
 */
HWTEST_F(NativeJavascriptExecuteCallbackTest,
         OHNativeJavascriptExecuteCallbackTest_OnReceiveValue_01, TestSize.Level1) {
    std::function<void(const char*)> callback = nullptr;
    std::shared_ptr<NWebMessage> result = std::make_shared<NWebMessage>(NWebValue::Type::NONE);
    NativeJavaScriptExecuteCallback nativeJSExecuteCallback(callback);
    nativeJSExecuteCallback.OnReceiveValue(result);
}

/**
 * @tc.name  : OHNativeJavascriptExecuteCallbackTest_OnReceiveValue_02
 * @tc.desc  : Test OnReceiveValue
 */
HWTEST_F(NativeJavascriptExecuteCallbackTest,
         OHNativeJavascriptExecuteCallbackTest_OnReceiveValue_02, TestSize.Level1) {
    std::function<void(const char*)> callback = CallbackNative;
    std::shared_ptr<NWebMessage> result = std::make_shared<NWebMessage>(NWebValue::Type::NONE);
    NativeJavaScriptExecuteCallback nativeJSExecuteCallback(callback);
    nativeJSExecuteCallback.OnReceiveValue(result);
}

/**
 * @tc.name  : OHNativeJavascriptExecuteCallbackTest_OnReceiveValue_03
 * @tc.desc  : Test OnReceiveValue
 */
HWTEST_F(NativeJavascriptExecuteCallbackTest,
         OHNativeJavascriptExecuteCallbackTest_OnReceiveValue_03, TestSize.Level1) {
    std::function<void(const char*)> callback = CallbackNative;
    NativeJavaScriptExecuteCallback nativeJSExecuteCallback(callback);
    std::shared_ptr<NWebMessage> result = std::make_shared<NWebMessage>(NWebValue::Type::STRING);
    result->SetString("test");
    nativeJSExecuteCallback.OnReceiveValue(result);
}

} // namespace NWeb
} // namesapce OHOS