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

#include <memory>
#include <string>
#include <vector>

#include "arkweb_native_javascript_execute_callback.h"
#include "arkweb_native_web_message_callback.h"
#include "base/logging.h"
#include "cef/ohos_cef_ext/libcef/browser/javascript/oh_gin_javascript_bridge_dispatcher_host.h"
#include "ohos_nweb/include/nweb_engine.h"
#define private public  // access private member
#include "arkweb/ohos_nweb/src/cef_delegate/nweb_cookie_manager_delegate.h"
#include "arkweb/ohos_nweb/src/nweb_cookie_manager_impl.h"
#undef private
#include "ohos_nweb/src/capi/arkweb_model.h"
#include "ohos_nweb/src/ndk/arkweb_native_object.h"

using ::testing::_;
using ::testing::IsNull;
using ::testing::Return;
std::function<
    std::shared_ptr<OHOS::NWeb::NWebValue>(std::vector<std::vector<uint8_t>>&,
                                           std::vector<size_t>&)>
CreateProxyCallback(ArkWeb_OnJavaScriptProxyCallbackWithResult callback,
                    const std::string& tag,
                    void* data);
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
bool CreateWebMessagePortsInternal(const char* webTag,
                                   size_t* size,
                                   std::vector<std::string>& ports,
                                   ArkWeb_WebMessagePortPtr** wPortsResult);

#ifdef __cplusplus
}
#endif  // __cplusplus

class ArkWebModelTest : public testing::Test {};

class CallbackResult {
 public:
  void Reset() { called_ = false; }
  void Set() { called_ = true; }
  bool Result() { return called_; }

 private:
  bool called_ = false;
} g_callback_called;

void TestArkWebOnJavaScriptCallback(const char* webTag,
                                    const ArkWeb_JavaScriptBridgeData* data,
                                    void* userData) {
  g_callback_called.Set();
}
void TestArkWebOnJavaScriptProxyCallback(
    const char* webTag,
    const ArkWeb_JavaScriptBridgeData* dataArray,
    size_t arraySize,
    void* userData) {
  g_callback_called.Set();
}

ArkWeb_JavaScriptValuePtr MockCallback(const char* webTag,
                                       const ArkWeb_JavaScriptBridgeData* data,
                                       size_t size,
                                       void* userData) {
  if (userData == nullptr) {
    return nullptr;
  } else if (userData == reinterpret_cast<void*>(1)) {
    ArkWeb_JavaScriptValuePtr result = new ArkWeb_JavaScriptValue({});
    result->type = ArkWeb_JavaScriptValueType::ARKWEB_JAVASCRIPT_BOOL;
    result->data = new bool(true);
    return result;
  } else if (userData == reinterpret_cast<void*>(2)) {
    ArkWeb_JavaScriptValuePtr result = new ArkWeb_JavaScriptValue({});
    result->type = ArkWeb_JavaScriptValueType::ARKWEB_JAVASCRIPT_STRING;
    result->data = new char[6]{'H', 'e', 'l', 'l', 'o', '\0'};
    return result;
  } else if (userData == reinterpret_cast<void*>(3)) {
    ArkWeb_JavaScriptValuePtr result = new ArkWeb_JavaScriptValue({});
    result->type = ArkWeb_JavaScriptValueType::ARKWEB_JAVASCRIPT_NONE;
    result->data = nullptr;
    return result;
  }
  return nullptr;
}

TEST_F(ArkWebModelTest, CreateProxyCallback_001) {
  auto callback = CreateProxyCallback(nullptr, "testTag", nullptr);
  std::vector<std::vector<uint8_t>> dataList;
  std::vector<size_t> dataSize;
  EXPECT_EQ(callback(dataList, dataSize), nullptr);
}

TEST_F(ArkWebModelTest, CreateProxyCallback_002) {
  auto callback = CreateProxyCallback(MockCallback, "testTag", nullptr);
  std::vector<std::vector<uint8_t>> dataList;
  std::vector<size_t> dataSize;
  EXPECT_EQ(callback(dataList, dataSize), nullptr);
}

TEST_F(ArkWebModelTest, CreateProxyCallback_003) {
  auto callback = CreateProxyCallback(MockCallback, "testTag", (void*)1);
  std::vector<std::vector<uint8_t>> dataList;
  std::vector<size_t> dataSize;
  auto result = callback(dataList, dataSize);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(result->GetType(), OHOS::NWeb::NWebValue::Type::BOOLEAN);
  EXPECT_EQ(result->GetBoolean(), true);
}

TEST_F(ArkWebModelTest, CreateProxyCallback_004) {
  auto callback = CreateProxyCallback(MockCallback, "testTag", (void*)2);
  std::vector<std::vector<uint8_t>> dataList;
  std::vector<size_t> dataSize;
  auto result = callback(dataList, dataSize);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(result->GetType(), OHOS::NWeb::NWebValue::Type::STRING);
  EXPECT_EQ(result->GetString(), std::string("Hello"));
}

TEST_F(ArkWebModelTest, CreateProxyCallback_005) {
  auto callback = CreateProxyCallback(MockCallback, "invalidTag", (void*)3);
  std::vector<std::vector<uint8_t>> dataList;
  std::vector<size_t> dataSize;
  auto result = callback(dataList, dataSize);
  ASSERT_NE(result, nullptr);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RunJavaScript_001) {
  EXPECT_NO_FATAL_FAILURE(OH_ArkWeb_RunJavaScript(nullptr, nullptr));
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RunJavaScript_002) {
  ArkWeb_JavaScriptObject obj = {};
  OH_ArkWeb_RunJavaScript(nullptr, &obj);
  EXPECT_EQ(obj.buffer, nullptr);
  EXPECT_EQ(obj.callback, nullptr);
  EXPECT_EQ(obj.userData, nullptr);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RunJavaScript_003) {
  const uint8_t* buf = reinterpret_cast<const uint8_t*>("12345");
  ArkWeb_JavaScriptObject obj = {
      .buffer = buf,
      .size = strlen(reinterpret_cast<const char*>(buf)),
      .callback = nullptr,
      .userData = nullptr};
  OH_ArkWeb_RunJavaScript(nullptr, &obj);
  EXPECT_EQ(obj.callback, nullptr);
  EXPECT_EQ(obj.userData, nullptr);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RunJavaScript_004) {
  const uint8_t* buf = reinterpret_cast<const uint8_t*>("123456");
  g_callback_called.Reset();
  ArkWeb_JavaScriptObject obj = {
      .buffer = buf,
      .size = strlen(reinterpret_cast<const char*>(buf)),
      .callback = TestArkWebOnJavaScriptCallback,
      .userData = nullptr};
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  OH_ArkWeb_RunJavaScript("webTag", &obj);
  EXPECT_FALSE(g_callback_called.Result());
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RunJavaScript_005) {
  const uint8_t* buf = reinterpret_cast<const uint8_t*>("123456");
  g_callback_called.Reset();
  ArkWeb_JavaScriptObject obj = {
      .buffer = buf,
      .size = strlen(reinterpret_cast<const char*>(buf)),
      .callback = TestArkWebOnJavaScriptCallback,
      .userData = nullptr};
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  OH_ArkWeb_RunJavaScript("webTag", &obj);
  EXPECT_FALSE(g_callback_called.Result());

  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RunJavaScript_006) {
  ArkWeb_JavaScriptObject obj = {};
  OH_ArkWeb_RunJavaScript("webTag", &obj);
  EXPECT_EQ(obj.buffer, nullptr);
  EXPECT_EQ(obj.callback, nullptr);
  EXPECT_EQ(obj.userData, nullptr);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RunJavaScript_007) {
  const uint8_t* buf = reinterpret_cast<const uint8_t*>("12345");
  ArkWeb_JavaScriptObject obj = {
      .buffer = buf,
      .size = strlen(reinterpret_cast<const char*>(buf)),
      .callback = nullptr,
      .userData = nullptr};
  OH_ArkWeb_RunJavaScript("webTag", &obj);
  EXPECT_EQ(obj.callback, nullptr);
  EXPECT_EQ(obj.userData, nullptr);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RegisterJavaScriptProxy_001) {
  ArkWeb_ProxyMethod method = {.methodName = "methodName",
                               .callback = TestArkWebOnJavaScriptProxyCallback,
                               .userData = nullptr};
  ArkWeb_ProxyMethod methodListPtr[] = {method};
  ArkWeb_ProxyObject obj = {
      .objName = "objName", .methodList = methodListPtr, .size = 1};
  OH_ArkWeb_RegisterJavaScriptProxy("webTag", &obj);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_DeleteJavaScriptProxy_001) {
  const char* web_tag = nullptr;
  OH_ArkWeb_DeleteJavaScriptProxy(web_tag, nullptr);
  EXPECT_EQ(OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(web_tag),
            nullptr);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_DeleteJavaScriptProxy_002) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");

  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  OH_ArkWeb_DeleteJavaScriptProxy("webTag", nullptr);
  EXPECT_NE(web_obj_ptr, nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_ArkWeb_DeleteJavaScriptProxy_003) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  OH_ArkWeb_DeleteJavaScriptProxy("webTag", nullptr);
  EXPECT_NE(web_obj_ptr->GetWebSharedPtr(), nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_ArkWeb_Refresh_001) {
  const char* web_tag = nullptr;
  OH_ArkWeb_Refresh(web_tag);
  EXPECT_EQ(OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(web_tag),
            nullptr);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_Refresh_002) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  OH_ArkWeb_Refresh("webTag");
  EXPECT_NE(web_obj_ptr, nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_ArkWeb_Refresh_003) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  OH_ArkWeb_DeleteJavaScriptProxy("webTag", nullptr);
  OH_ArkWeb_Refresh("webTag");
  EXPECT_NE(web_obj_ptr->GetWebSharedPtr(), nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_ArkWeb_OnControllerAttached_001) {
  OH_ArkWeb_OnControllerAttached("webTag", nullptr, nullptr);
  const char* web_tag = nullptr;
  OH_ArkWeb_OnControllerAttached(web_tag, [](const char*, void*) {}, nullptr);
  EXPECT_EQ(OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(web_tag),
            nullptr);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_OnControllerAttached_002) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  OH_ArkWeb_OnControllerAttached("webTag", [](const char*, void*) {}, nullptr);
  EXPECT_NE(web_obj_ptr, nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_ArkWeb_OnPageBegin_001) {
  OH_ArkWeb_OnPageBegin("webTag", nullptr, nullptr);
  const char* web_tag = nullptr;
  OH_ArkWeb_OnPageBegin(web_tag, [](const char*, void*) {}, nullptr);
  EXPECT_EQ(OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(web_tag),
            nullptr);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_OnPageBegin_002) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  OH_ArkWeb_OnPageBegin("webTag", [](const char*, void*) {}, nullptr);
  EXPECT_NE(web_obj_ptr, nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_ArkWeb_OnPageEnd_001) {
  OH_ArkWeb_OnPageEnd("webTag", nullptr, nullptr);
  const char* web_tag = nullptr;
  OH_ArkWeb_OnPageEnd(web_tag, [](const char*, void*) {}, nullptr);
  EXPECT_EQ(OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(web_tag),
            nullptr);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_OnPageEnd_002) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  OH_ArkWeb_OnPageEnd("webTag", [](const char*, void*) {}, nullptr);
  EXPECT_NE(OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag"),
            nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

void Test_OH_ArkWeb_OnDestroy_MockCallback(const char* webTag, void* userData) {
}

TEST_F(ArkWebModelTest, OH_ArkWeb_OnDestroy_001) {
  OH_ArkWeb_OnDestroy(nullptr, nullptr, nullptr);
  const char* webTag = nullptr;
  void* userData = nullptr;
  OH_ArkWeb_OnDestroy(webTag, Test_OH_ArkWeb_OnDestroy_MockCallback, userData);
  EXPECT_EQ(OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(webTag),
            nullptr);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_OnDestroy_002) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  const char* webTag = "webTag";
  void* userData = nullptr;
  OH_ArkWeb_OnDestroy(webTag, Test_OH_ArkWeb_OnDestroy_MockCallback, userData);
  EXPECT_NE(OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(webTag),
            nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

void Test_OH_ArkWeb_OnScroll_MockCallback(const char* webTag,
                                          void* userData,
                                          double x,
                                          double y) {}

TEST_F(ArkWebModelTest, OH_ArkWeb_OnScroll_001) {
  EXPECT_FALSE(OH_ArkWeb_OnScroll("someTag", nullptr, nullptr));
}

TEST_F(ArkWebModelTest, OH_ArkWeb_OnScroll_002) {
  EXPECT_FALSE(OH_ArkWeb_OnScroll(
      "invalidTag", Test_OH_ArkWeb_OnScroll_MockCallback, nullptr));
}

TEST_F(ArkWebModelTest, OH_ArkWeb_OnScroll_003) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  EXPECT_TRUE(OH_ArkWeb_OnScroll("webTag", Test_OH_ArkWeb_OnScroll_MockCallback,
                                 nullptr));
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, RegisterJavaScriptProxyTest_001) {
  OH_ArkWeb_RegisterAsyncJavaScriptProxy(nullptr, nullptr);
  RegisterJavaScriptProxy("webTag", nullptr, true, "permission");
  ArkWeb_ProxyObject proxyObject = {nullptr, nullptr, 1};
  RegisterJavaScriptProxy("webTag", &proxyObject, true, "permission");
  EXPECT_EQ(proxyObject.objName, nullptr);
}

TEST_F(ArkWebModelTest, RegisterJavaScriptProxyTest_002) {
  ArkWeb_ProxyObject proxyObject = {"objName", nullptr, 1};
  RegisterJavaScriptProxy("webTag", &proxyObject, true, "permission");
}

TEST_F(ArkWebModelTest, RegisterJavaScriptProxyTest_003) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  ArkWeb_ProxyMethod method = {"methodName", nullptr, nullptr};
  ArkWeb_ProxyObject proxyObject = {"objName", &method, 1};
  RegisterJavaScriptProxy("webTag", &proxyObject, true, "permission");
  EXPECT_EQ(OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(nullptr),
            nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, RegisterJavaScriptProxyTest_004) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  ArkWeb_ProxyObject proxyObject = {"objName", nullptr, 1};
  RegisterJavaScriptProxy("webTag", &proxyObject, true, "permission");
  EXPECT_NE(OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag"),
            nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, RegisterJavaScriptProxyTest_005) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  ArkWeb_ProxyMethod method = {.methodName = "methodName",
                               .callback = TestArkWebOnJavaScriptProxyCallback,
                               .userData = nullptr};
  ArkWeb_ProxyMethod methodListPtr[] = {method};
  ArkWeb_ProxyObject obj = {
      .objName = "objName", .methodList = methodListPtr, .size = 1};

  RegisterJavaScriptProxy("webTag", &obj, true, "permission");
  EXPECT_NE(OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag"),
            nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, RegisterJavaScriptProxyTest_006) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  ArkWeb_ProxyMethod method = {.methodName = "methodName",
                               .callback = TestArkWebOnJavaScriptProxyCallback,
                               .userData = nullptr};
  ArkWeb_ProxyMethod methodListPtr[] = {method};
  ArkWeb_ProxyObject obj = {
      .objName = "objName", .methodList = methodListPtr, .size = 1};
  RegisterJavaScriptProxy("webTag", &obj, true, nullptr);
  EXPECT_NE(OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag"),
            nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, CreateWebMessagePortsInternal_001) {
  size_t size = 0;
  std::vector<std::string> ports;
  ArkWeb_WebMessagePortPtr* wPortsResult = nullptr;
  EXPECT_FALSE(
      CreateWebMessagePortsInternal(nullptr, &size, ports, &wPortsResult));
  EXPECT_FALSE(
      CreateWebMessagePortsInternal("validTag", nullptr, ports, &wPortsResult));
  EXPECT_FALSE(
      CreateWebMessagePortsInternal(nullptr, nullptr, ports, &wPortsResult));
}

TEST_F(ArkWebModelTest, CreateWebMessagePortsInternal_002) {
  size_t size = 0;
  std::vector<std::string> ports;
  ArkWeb_WebMessagePortPtr* wPortsResult = nullptr;
  EXPECT_FALSE(CreateWebMessagePortsInternal("", &size, ports, &wPortsResult));
}

TEST_F(ArkWebModelTest, CreateWebMessagePortsInternal_003) {
  size_t size = 0;
  std::vector<std::string> ports;
  ArkWeb_WebMessagePortPtr* wPortsResult = nullptr;
  EXPECT_FALSE(
      CreateWebMessagePortsInternal("webTag", &size, ports, &wPortsResult));
}

TEST_F(ArkWebModelTest, CreateWebMessagePortsInternal_004) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  size_t size = 0;
  std::vector<std::string> ports;
  ArkWeb_WebMessagePortPtr* wPortsResult = nullptr;
  EXPECT_FALSE(
      CreateWebMessagePortsInternal("webTag", &size, ports, &wPortsResult));
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}
class MockNWebImpl : public OHOS::NWeb::NWebImpl {
 public:
  explicit MockNWebImpl(uint32_t id) : NWebImpl(id) {}
  MOCK_METHOD(std::vector<std::string>, CreateWebMessagePorts, (), (override));
};

TEST_F(ArkWebModelTest, CreateWebMessagePortsInternal_005) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<MockNWebImpl> mock_nweb = std::make_shared<MockNWebImpl>(1);
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl = std::static_pointer_cast<OHOS::NWeb::NWebImpl>(mock_nweb);
  EXPECT_CALL(*mock_nweb, CreateWebMessagePorts()).WillOnce(Return(std::vector<std::string>()));

  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  size_t size = 0;
  std::vector<std::string> ports;
  ArkWeb_WebMessagePortPtr* wPortsResult = nullptr;
  EXPECT_FALSE(
      CreateWebMessagePortsInternal("webTag", &size, ports, &wPortsResult));
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, CreateWebMessagePortsInternal_006) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<MockNWebImpl> mock_nweb = std::make_shared<MockNWebImpl>(1);
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl = std::static_pointer_cast<OHOS::NWeb::NWebImpl>(mock_nweb);
  EXPECT_CALL(*mock_nweb, CreateWebMessagePorts()).WillOnce(Return(std::vector<std::string>{"webTag6"}));

  nweb_impl->AddNWebToMap(6, nweb_impl);
  web_obj->BindWebTagToWebInstance(6, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  size_t size = 0;
  std::vector<std::string> ports;
  ArkWeb_WebMessagePortPtr* wPortsResult = nullptr;
  EXPECT_TRUE(
      CreateWebMessagePortsInternal("webTag", &size, ports, &wPortsResult));
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
std::shared_ptr<OHOS::NWeb::NWeb> GetNWebSharedPtrFromTag(const char* webTag);
#ifdef __cplusplus
}
#endif  // __cplusplus

TEST_F(ArkWebModelTest, GetNWebSharedPtrFromTag_001) {
  EXPECT_EQ(GetNWebSharedPtrFromTag(nullptr), nullptr);
}

TEST_F(ArkWebModelTest, GetNWebSharedPtrFromTag_002) {
  EXPECT_EQ(GetNWebSharedPtrFromTag("webTag"), nullptr);
}

TEST_F(ArkWebModelTest, GetNWebSharedPtrFromTag_003) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  EXPECT_EQ(GetNWebSharedPtrFromTag("webTag"), nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, GetNWebSharedPtrFromTag_004) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  EXPECT_NE(GetNWebSharedPtrFromTag("webTag"), nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_ArkWeb_CreateWebMessagePorts_001) {
  size_t size = 0;
  const char* webTag = "webTag";
  ArkWeb_WebMessagePortPtr* result =
      OH_ArkWeb_CreateWebMessagePorts(webTag, &size);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_CreateWebMessagePorts_002) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<MockNWebImpl> mock_nweb = std::make_shared<MockNWebImpl>(1);
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl = std::static_pointer_cast<OHOS::NWeb::NWebImpl>(mock_nweb);
  EXPECT_CALL(*mock_nweb, CreateWebMessagePorts()).WillOnce(Return(std::vector<std::string>{"webTag6"}));
  nweb_impl->AddNWebToMap(6, nweb_impl);
  web_obj->BindWebTagToWebInstance(6, "webTag6");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag6");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  size_t size = 0;
  const char* webTag = "webTag6";
  ArkWeb_WebMessagePortPtr* result =
      OH_ArkWeb_CreateWebMessagePorts(webTag, &size);
  EXPECT_NE(result, nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_ArkWeb_CreateWebMessagePorts_003) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag6");
  std::shared_ptr<MockNWebImpl> mock_nweb = std::make_shared<MockNWebImpl>(1);
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl = std::static_pointer_cast<OHOS::NWeb::NWebImpl>(mock_nweb);
  EXPECT_CALL(*mock_nweb, CreateWebMessagePorts()).WillOnce(Return(std::vector<std::string>{"webTag6"}));
  nweb_impl->AddNWebToMap(6, nweb_impl);
  web_obj->BindWebTagToWebInstance(6, "webTag6");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag6");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  size_t size = 0;
  const char* webTag = "webTag6";
  ArkWeb_WebMessagePortPtr* result =
      OH_ArkWeb_CreateWebMessagePorts(webTag, &size);
  EXPECT_NE(result, nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_ArkWeb_DestroyWebMessagePorts_001) {
  size_t size = 2;
  ArkWeb_WebMessagePortPtr* ports = new ArkWeb_WebMessagePortPtr[size];
  for (size_t i = 0; i < size; ++i) {
    ports[i] = new ArkWeb_WebMessagePort;
    ports[i]->webTag = new char[10];
    ports[i]->portHandle = new char[10];
  }
  OH_ArkWeb_DestroyWebMessagePorts(&ports, size);
  EXPECT_EQ(ports, nullptr);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_DestroyWebMessagePorts_002) {
  size_t size = 0;
  ArkWeb_WebMessagePortPtr* ports = nullptr;
  OH_ArkWeb_DestroyWebMessagePorts(&ports, size);
  EXPECT_EQ(ports, nullptr);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_DestroyWebMessagePorts_003) {
  size_t size = 2;
  ArkWeb_WebMessagePortPtr** port = static_cast<ArkWeb_WebMessagePortPtr**>(
      malloc(size * sizeof(ArkWeb_WebMessagePortPtr*)));
  for (size_t i = 0; i < size; ++i) {
    port[i] = nullptr;
  }
  OH_ArkWeb_DestroyWebMessagePorts(port, size);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_PostWebMessage_001) {
  ArkWeb_WebMessagePortPtr ports[1];
  EXPECT_EQ(OH_ArkWeb_PostWebMessage(nullptr, nullptr, nullptr, 1, nullptr),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  EXPECT_EQ(OH_ArkWeb_PostWebMessage(nullptr, nullptr, nullptr, 1, "url"),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  EXPECT_EQ(OH_ArkWeb_PostWebMessage(nullptr, nullptr, ports, 1, nullptr),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  EXPECT_EQ(OH_ArkWeb_PostWebMessage(nullptr, nullptr, ports, 1, "url"),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  EXPECT_EQ(OH_ArkWeb_PostWebMessage(nullptr, "name", nullptr, 1, nullptr),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  EXPECT_EQ(OH_ArkWeb_PostWebMessage(nullptr, "name", nullptr, 1, "url"),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  EXPECT_EQ(OH_ArkWeb_PostWebMessage(nullptr, "name", ports, 1, nullptr),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  EXPECT_EQ(OH_ArkWeb_PostWebMessage(nullptr, "name", ports, 1, "url"),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  EXPECT_EQ(OH_ArkWeb_PostWebMessage("", nullptr, nullptr, 1, nullptr),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  EXPECT_EQ(OH_ArkWeb_PostWebMessage("", nullptr, nullptr, 1, "url"),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  EXPECT_EQ(OH_ArkWeb_PostWebMessage("", nullptr, ports, 1, nullptr),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  EXPECT_EQ(OH_ArkWeb_PostWebMessage("", nullptr, ports, 1, "url"),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  EXPECT_EQ(OH_ArkWeb_PostWebMessage("", "name", nullptr, 1, nullptr),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  EXPECT_EQ(OH_ArkWeb_PostWebMessage("", "name", nullptr, 1, "url"),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  EXPECT_EQ(OH_ArkWeb_PostWebMessage("", "name", ports, 1, nullptr),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_PostWebMessage_002) {
  ArkWeb_WebMessagePortPtr ports[1];
  EXPECT_EQ(OH_ArkWeb_PostWebMessage("", "name", ports, 1, "url"),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_PostWebMessage_003) {
  ArkWeb_WebMessagePortPtr ports[1];
  EXPECT_EQ(OH_ArkWeb_PostWebMessage("validTag", "name", ports, 0, "url"),
            ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_PostWebMessage_004) {
  ArkWeb_WebMessagePortPtr ports[1];
  EXPECT_EQ(OH_ArkWeb_PostWebMessage("webTag", "name", ports, 1, "url"),
            ArkWeb_ErrorCode::ARKWEB_INIT_ERROR);
}

TEST_F(ArkWebModelTest, OH_ArkWeb_PostWebMessage_005) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  ArkWeb_WebMessagePortPtr ports[1];
  ports[0] = new ArkWeb_WebMessagePort();
  ports[0]->portHandle = nullptr;
  EXPECT_EQ(OH_ArkWeb_PostWebMessage("webTag", "name", ports, 1, "url"),
            ArkWeb_ErrorCode::ARKWEB_INIT_ERROR);
  delete ports[0];
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_ArkWeb_PostWebMessage_006) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  ArkWeb_WebMessagePortPtr ports[1];
  ports[0] = new ArkWeb_WebMessagePort();
  ports[0]->portHandle = nullptr;
  EXPECT_EQ(OH_ArkWeb_PostWebMessage("webTag", "name", ports, 1, "url"),
            ArkWeb_ErrorCode::ARKWEB_SUCCESS);
  delete ports[0];
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_ArkWeb_PostWebMessage_007) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  ArkWeb_WebMessagePortPtr ports[1];
  ports[0] = new ArkWeb_WebMessagePort();
  ports[0]->portHandle = (char*)"portHandle";
  EXPECT_EQ(OH_ArkWeb_PostWebMessage("webTag", "name", ports, 1, "url"),
            ArkWeb_ErrorCode::ARKWEB_SUCCESS);
  delete ports[0];
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_WebMessage_PostMessage_001) {
  auto port = std::make_unique<ArkWeb_WebMessagePort>();
  auto message = std::make_unique<ArkWeb_WebMessage>();
  ArkWeb_ErrorCode result =
      OH_WebMessage_PostMessage(nullptr, nullptr, nullptr);
  result = OH_WebMessage_PostMessage(nullptr, nullptr, message.get());
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  result = OH_WebMessage_PostMessage(nullptr, "webTag", nullptr);
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  result = OH_WebMessage_PostMessage(nullptr, "webTag", message.get());
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  result = OH_WebMessage_PostMessage(port.get(), nullptr, nullptr);
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  result = OH_WebMessage_PostMessage(port.get(), nullptr, message.get());
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  result = OH_WebMessage_PostMessage(port.get(), "webTag", nullptr);
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWebModelTest, OH_WebMessage_PostMessage_002) {
  auto port = std::make_unique<ArkWeb_WebMessagePort>();
  auto message = std::make_unique<ArkWeb_WebMessage>();
  ArkWeb_ErrorCode result =
      OH_WebMessage_PostMessage(port.get(), "", message.get());
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWebModelTest, OH_WebMessage_PostMessage_003) {
  auto port = std::make_unique<ArkWeb_WebMessagePort>();
  auto message = std::make_unique<ArkWeb_WebMessage>();
  ArkWeb_ErrorCode result =
      OH_WebMessage_PostMessage(port.get(), "invalid_tag", message.get());
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INIT_ERROR);
}

TEST_F(ArkWebModelTest, OH_WebMessage_PostMessage_004) {
  auto port = std::make_unique<ArkWeb_WebMessagePort>();
  auto message = std::make_unique<ArkWeb_WebMessage>();
  ArkWeb_ErrorCode result =
      OH_WebMessage_PostMessage(port.get(), "webTag", message.get());
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INIT_ERROR);
}

TEST_F(ArkWebModelTest, OH_WebMessage_PostMessage_005) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  auto port = std::make_unique<ArkWeb_WebMessagePort>();
  auto message = std::make_unique<ArkWeb_WebMessage>();
  ArkWeb_ErrorCode result =
      OH_WebMessage_PostMessage(port.get(), "webTag", message.get());
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INIT_ERROR);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_WebMessage_PostMessage_006) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  auto port = std::make_unique<ArkWeb_WebMessagePort>();
  auto message = std::make_unique<ArkWeb_WebMessage>();
  port->portHandle = (char*)"";
  ArkWeb_ErrorCode result =
      OH_WebMessage_PostMessage(port.get(), "webTag", message.get());
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_WebMessage_PostMessage_007) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  auto port = std::make_unique<ArkWeb_WebMessagePort>();
  auto message = std::make_unique<ArkWeb_WebMessage>();
  port->portHandle = (char*)"portHandle";
  message->webMessageType = ArkWeb_WebMessageType::ARKWEB_STRING;
  ArkWeb_ErrorCode result =
      OH_WebMessage_PostMessage(port.get(), "webTag", message.get());
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_WebMessage_PostMessage_008) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  auto port = std::make_unique<ArkWeb_WebMessagePort>();
  auto message = std::make_unique<ArkWeb_WebMessage>();
  port->portHandle = (char*)"portHandle";
  message->data = new char[10];
  ArkWeb_ErrorCode result =
      OH_WebMessage_PostMessage(port.get(), "webTag", message.get());
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_WebMessage_PostMessage_009) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  auto port = std::make_unique<ArkWeb_WebMessagePort>();
  auto message = std::make_unique<ArkWeb_WebMessage>();
  port->portHandle = (char*)"portHandle";
  auto data_ptr = std::make_unique<char[]>(10);
  message->data = data_ptr.get();
  message->webMessageType = ArkWeb_WebMessageType::ARKWEB_STRING;
  ArkWeb_ErrorCode result =
      OH_WebMessage_PostMessage(port.get(), "webTag", message.get());
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_SUCCESS);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_WebMessage_PostMessage_010) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  auto port = std::make_unique<ArkWeb_WebMessagePort>();
  auto message = std::make_unique<ArkWeb_WebMessage>();
  port->portHandle = (char*)"portHandle";
  auto data_ptr = std::make_unique<char[]>(10);
  message->data = data_ptr.get();
  message->webMessageType = ArkWeb_WebMessageType::ARKWEB_BUFFER;
  ArkWeb_ErrorCode result =
      OH_WebMessage_PostMessage(port.get(), "webTag", message.get());
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_SUCCESS);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_WebMessage_Close_001) {
  EXPECT_NO_FATAL_FAILURE(OH_WebMessage_Close(nullptr, nullptr));
}

TEST_F(ArkWebModelTest, OH_WebMessage_Close_002) {
  ArkWeb_WebMessagePort port;
  const char* webTag = "";
  EXPECT_NO_FATAL_FAILURE(OH_WebMessage_Close(&port, webTag));
}

TEST_F(ArkWebModelTest, OH_WebMessage_Close_003) {
  ArkWeb_WebMessagePort port;
  port.portHandle = nullptr;
  OH_WebMessage_Close(&port, "validTag");
  EXPECT_NO_FATAL_FAILURE(OH_WebMessage_Close(&port, "validTag"));
}

TEST_F(ArkWebModelTest, OH_WebMessage_Close_004) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  ArkWeb_WebMessagePort port;
  port.portHandle = (char*)"validHandle";
  OH_WebMessage_Close(&port, "webTag");
  EXPECT_EQ(OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag"),
            nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_WebMessage_Close_005) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  ArkWeb_WebMessagePort port;
  port.portHandle = (char*)"validHandle";
  OH_WebMessage_Close(&port, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  ASSERT_NE(web_obj_ptr, nullptr);
  EXPECT_EQ(web_obj_ptr->GetWebSharedPtr(), nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_WebMessage_Close_006) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  ASSERT_NE(web_obj_ptr, nullptr);
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  EXPECT_NE(OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag"),
            nullptr);
  EXPECT_NE(web_obj_ptr->GetWebSharedPtr(), nullptr);
  ArkWeb_WebMessagePort port;
  port.portHandle = (char*)"validHandle";
  OH_WebMessage_Close(&port, "webTag");
  EXPECT_NE(web_obj_ptr->GetWebSharedPtr(), nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_WebMessage_SetMessageEventHandler_001) {
  auto port = std::make_unique<ArkWeb_WebMessagePort>();
  const char* webTag = "";
  ArkWeb_OnMessageEventHandler messageEventHandler =
      [](const char*, ArkWeb_WebMessagePortPtr, ArkWeb_WebMessagePtr, void*) {};
  void* userData = nullptr;
  OH_WebMessage_SetMessageEventHandler(nullptr, nullptr, nullptr, userData);
  OH_WebMessage_SetMessageEventHandler(nullptr, nullptr, messageEventHandler,
                                       userData);
  OH_WebMessage_SetMessageEventHandler(nullptr, webTag, nullptr, userData);
  OH_WebMessage_SetMessageEventHandler(port.get(), nullptr, nullptr, userData);
  OH_WebMessage_SetMessageEventHandler(port.get(), nullptr, messageEventHandler,
                                       userData);
  OH_WebMessage_SetMessageEventHandler(port.get(), webTag, nullptr, userData);
  OH_WebMessage_SetMessageEventHandler(port.get(), webTag, messageEventHandler,
                                       userData);
  EXPECT_TRUE(std::string(webTag).empty());
}

TEST_F(ArkWebModelTest, OH_WebMessage_SetMessageEventHandler_002) {
  auto port = std::make_unique<ArkWeb_WebMessagePort>();
  port->portHandle = nullptr;
  const char* webTag = "validTag";
  ArkWeb_OnMessageEventHandler messageEventHandler =
      [](const char*, ArkWeb_WebMessagePortPtr, ArkWeb_WebMessagePtr, void*) {};
  void* userData = nullptr;
  EXPECT_NO_FATAL_FAILURE(OH_WebMessage_SetMessageEventHandler(
      port.get(), webTag, messageEventHandler, userData));
}

TEST_F(ArkWebModelTest, OH_WebMessage_SetMessageEventHandler_003) {
  auto port = std::make_unique<ArkWeb_WebMessagePort>();
  port->portHandle = (char*)"validHandle";
  const char* webTag = "validTag";
  ArkWeb_OnMessageEventHandler messageEventHandler =
      [](const char*, ArkWeb_WebMessagePortPtr, ArkWeb_WebMessagePtr, void*) {};
  void* userData = nullptr;
  OH_WebMessage_SetMessageEventHandler(port.get(), webTag, messageEventHandler,
                                       userData);
}

TEST_F(ArkWebModelTest, OH_WebMessage_SetMessageEventHandler_004) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  auto port = std::make_unique<ArkWeb_WebMessagePort>();
  port->portHandle = (char*)"validHandle";
  const char* webTag = "webTag";
  ArkWeb_OnMessageEventHandler messageEventHandler =
      [](const char*, ArkWeb_WebMessagePortPtr, ArkWeb_WebMessagePtr, void*) {};
  void* userData = nullptr;
  OH_WebMessage_SetMessageEventHandler(port.get(), webTag, messageEventHandler,
                                       userData);
  EXPECT_NE(web_obj_ptr, nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_WebMessage_SetMessageEventHandler_005) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  ASSERT_NE(web_obj_ptr, nullptr);
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  auto port = std::make_unique<ArkWeb_WebMessagePort>();
  port->portHandle = (char*)"validHandle";
  const char* webTag = "webTag";
  ArkWeb_OnMessageEventHandler messageEventHandler =
      [](const char*, ArkWeb_WebMessagePortPtr, ArkWeb_WebMessagePtr, void*) {};
  void* userData = nullptr;
  OH_WebMessage_SetMessageEventHandler(port.get(), webTag, messageEventHandler,
                                       userData);
  EXPECT_NE(web_obj_ptr->GetWebSharedPtr(), nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_WebMessage_CreateWebMessage_001) {
  ArkWeb_WebMessagePtr message = OH_WebMessage_CreateWebMessage();
  EXPECT_NE(message, nullptr);
  if (message) {
    delete message;
  }
}

TEST_F(ArkWebModelTest, OH_WebMessage_DestroyWebMessage_001) {
  OH_WebMessage_DestroyWebMessage(nullptr);
  ArkWeb_WebMessagePtr message = nullptr;
  OH_WebMessage_DestroyWebMessage(&message);
  EXPECT_EQ(message, nullptr);
}

TEST_F(ArkWebModelTest, OH_WebMessage_DestroyWebMessage_002) {
  auto message = new ArkWeb_WebMessage();
  auto data_ptr = new char[10];
  message->data = data_ptr;
  OH_WebMessage_DestroyWebMessage(&message);
  EXPECT_EQ(message, nullptr);
}

TEST_F(ArkWebModelTest, OH_WebMessage_SetType_001) {
  ArkWeb_WebMessage message;
  OH_WebMessage_SetType(&message, ArkWeb_WebMessageType::ARKWEB_STRING);
  EXPECT_EQ(message.webMessageType, ArkWeb_WebMessageType::ARKWEB_STRING);
}

TEST_F(ArkWebModelTest, OH_WebMessage_GetType_001) {
  OH_WebMessage_SetType(nullptr, ArkWeb_WebMessageType::ARKWEB_STRING);
  auto result = OH_WebMessage_GetType(nullptr);
  EXPECT_EQ(result, ArkWeb_WebMessageType::ARKWEB_NONE);
}

TEST_F(ArkWebModelTest, OH_WebMessage_GetType_002) {
  ArkWeb_WebMessage message;
  message.webMessageType = ArkWeb_WebMessageType::ARKWEB_STRING;
  auto result = OH_WebMessage_GetType(&message);
  EXPECT_EQ(result, ArkWeb_WebMessageType::ARKWEB_STRING);
}

TEST_F(ArkWebModelTest, OH_WebMessage_SetData_001) {
  ArkWeb_WebMessagePtr message = nullptr;
  void* data = nullptr;
  size_t dataLength = 10;
  EXPECT_NO_FATAL_FAILURE(OH_WebMessage_SetData(message, data, dataLength));
}

TEST_F(ArkWebModelTest, OH_WebMessage_SetData_002) {
  ArkWeb_WebMessagePtr message = nullptr;
  void* data = malloc(10);
  size_t dataLength = 10;
  EXPECT_NO_FATAL_FAILURE(OH_WebMessage_SetData(message, data, dataLength));
  free(data);
}

TEST_F(ArkWebModelTest, OH_WebMessage_SetData_003) {
  ArkWeb_WebMessagePtr message = new ArkWeb_WebMessage;
  void* data = nullptr;
  size_t dataLength = 10;
  EXPECT_NO_FATAL_FAILURE(OH_WebMessage_SetData(message, data, dataLength));
  delete message;
}

TEST_F(ArkWebModelTest, OH_WebMessage_SetData_004) {
  ArkWeb_WebMessagePtr message = new ArkWeb_WebMessage;
  void* data = malloc(10);
  size_t dataLength = 0;
  EXPECT_NO_FATAL_FAILURE(OH_WebMessage_SetData(message, data, dataLength));
  free(data);
  delete message;
}

TEST_F(ArkWebModelTest, OH_WebMessage_SetData_005) {
  ArkWeb_WebMessagePtr message = new ArkWeb_WebMessage;
  void* data = malloc(10);
  size_t dataLength = 10;
  EXPECT_NO_FATAL_FAILURE(OH_WebMessage_SetData(message, data, dataLength));
  free(data);
  delete message;
}

TEST_F(ArkWebModelTest, OH_WebMessage_SetData_006) {
  ArkWeb_WebMessagePtr message = new ArkWeb_WebMessage;
  void* data = malloc(10);
  size_t dataLength = 10;
  EXPECT_NO_FATAL_FAILURE(OH_WebMessage_SetData(message, data, dataLength));
  free(data);
  delete message;
}

TEST_F(ArkWebModelTest, OH_WebMessage_SetData_008) {
  ArkWeb_WebMessagePtr message = nullptr;
  void* data = nullptr;
  size_t dataLength = 4294967295;
  EXPECT_NO_FATAL_FAILURE(OH_WebMessage_SetData(message, data, dataLength));
}

TEST_F(ArkWebModelTest, OH_WebMessage_GetData_001) {
  ArkWeb_WebMessage message;
  char testData[] = "test data";
  message.data = testData;
  message.dataLength = sizeof(testData);
  size_t dataLength = 0;
  void* result = OH_WebMessage_GetData(&message, &dataLength);
  EXPECT_NE(result, nullptr);
}

TEST_F(ArkWebModelTest, OH_WebMessage_GetData_002) {
  size_t dataLength = 10;
  void* result = OH_WebMessage_GetData(nullptr, &dataLength);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ArkWebModelTest, OH_CookieManager_FetchCookieSync_001) {
  auto cookie_manager = nullptr;
  char* cookie_value = nullptr;
  EXPECT_EQ(
      OH_CookieManager_FetchCookieSync("test", false, false, &cookie_value),
      ARKWEB_SUCCESS);
}

class MockNWebCookieManagerDelegate
    : public OHOS::NWeb::NWebCookieManagerDelegate {
 public:
  std::string ReturnCookieWithHttpOnly(const std::string& url,
                                       bool& is_valid,
                                       bool incognito_mode,
                                       bool includeHttpOnly) override {
    is_valid = false;
    return "";
  }
  int SetCookieWithHttpOnly(const std::string& url,
                            const std::string& value,
                            bool incognito_mode,
                            bool includeHttpOnly) override {
    if (url.empty()) {
      return OHOS::NWeb::NWEB_INVALID_COOKIE_VALUE;
    }
    if (incognito_mode) {
      return OHOS::NWeb::NWEB_INVALID_URL;
    } else {
      return OHOS::NWeb::NWEB_OK;
    }
  }
};

TEST_F(ArkWebModelTest, OH_CookieManager_FetchCookieSync_002) {
  auto cookie_manager =
      std::static_pointer_cast<OHOS::NWeb::NWebCookieManagerImpl>(
          OHOS::NWeb::NWebEngine::GetInstance()->GetCookieManager());
  cookie_manager->delegate_ = std::make_shared<MockNWebCookieManagerDelegate>();
  EXPECT_EQ(OH_CookieManager_FetchCookieSync("http://example.com", false, false,
                                             nullptr),
            ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWebModelTest, OH_CookieManager_FetchCookieSync_003) {
  auto cookie_manager =
      std::static_pointer_cast<OHOS::NWeb::NWebCookieManagerImpl>(
          OHOS::NWeb::NWebEngine::GetInstance()->GetCookieManager());
  cookie_manager->delegate_ = std::make_shared<MockNWebCookieManagerDelegate>();
  char* cookie_value = nullptr;
  EXPECT_EQ(OH_CookieManager_FetchCookieSync("http://example.com", false, false,
                                             &cookie_value),
            ARKWEB_INVALID_URL);
}

TEST_F(ArkWebModelTest, OH_CookieManager_ConfigCookieSync_001) {
  auto cookie_manager =
      std::static_pointer_cast<OHOS::NWeb::NWebCookieManagerImpl>(
          OHOS::NWeb::NWebEngine::GetInstance()->GetCookieManager());
  cookie_manager->delegate_ = std::make_shared<MockNWebCookieManagerDelegate>();
  EXPECT_EQ(OH_CookieManager_ConfigCookieSync("", "cookie=value", false, false),
            ARKWEB_INVALID_COOKIE_VALUE);
}

TEST_F(ArkWebModelTest, OH_CookieManager_ConfigCookieSync_002) {
  auto cookie_manager =
      std::static_pointer_cast<OHOS::NWeb::NWebCookieManagerImpl>(
          OHOS::NWeb::NWebEngine::GetInstance()->GetCookieManager());
  cookie_manager->delegate_ = std::make_shared<MockNWebCookieManagerDelegate>();
  EXPECT_EQ(OH_CookieManager_ConfigCookieSync("invalid_url", "cookie=value",
                                              false, false),
            ARKWEB_SUCCESS);
}

TEST_F(ArkWebModelTest, OH_CookieManager_ConfigCookieSync_003) {
  auto cookie_manager =
      std::static_pointer_cast<OHOS::NWeb::NWebCookieManagerImpl>(
          OHOS::NWeb::NWebEngine::GetInstance()->GetCookieManager());
  cookie_manager->delegate_ = std::make_shared<MockNWebCookieManagerDelegate>();
  EXPECT_EQ(OH_CookieManager_ConfigCookieSync("invalid_url", "cookie=value",
                                              true, false),
            ARKWEB_INVALID_URL);
}

TEST_F(ArkWebModelTest, OH_CookieManager_ExistCookies_001) {
  EXPECT_FALSE(OH_CookieManager_ExistCookies(false));
}

TEST_F(ArkWebModelTest, OH_ArkWeb_GetLastJavascriptProxyCallingFrameUrl_001) {
  OH_CookieManager_ClearAllCookiesSync(false);
  const char* result = OH_ArkWeb_GetLastJavascriptProxyCallingFrameUrl();
  EXPECT_STREQ(result, "");
}

TEST_F(ArkWebModelTest, RegisterJavaScriptProxyEx_002) {
  OH_CookieManager_ClearSessionCookiesSync();
  RegisterJavaScriptProxyEx(nullptr, nullptr, false, nullptr);
  ArkWeb_ProxyObjectWithResult proxyObject = {nullptr, nullptr, 0};
  ;
  RegisterJavaScriptProxyEx("webTag", &proxyObject, false, nullptr);
  EXPECT_EQ(proxyObject.objName, nullptr);
}

TEST_F(ArkWebModelTest, RegisterJavaScriptProxyEx_003) {
  ArkWeb_ProxyObjectWithResult proxyObject = {"objName", nullptr, 0};
  RegisterJavaScriptProxyEx("webTag", &proxyObject, false, nullptr);
  EXPECT_EQ(OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag"),
            nullptr);
}

TEST_F(ArkWebModelTest, RegisterJavaScriptProxyEx_004) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  ArkWeb_ProxyObjectWithResult proxyObject = {"objName", nullptr, 0};
  RegisterJavaScriptProxyEx("webTag", &proxyObject, false, nullptr);
  EXPECT_EQ(web_obj_ptr->GetWebSharedPtr(), nullptr);
}

TEST_F(ArkWebModelTest, RegisterJavaScriptProxyEx_005) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  ArkWeb_ProxyObjectWithResult proxyObject = {"objName", nullptr, 0};
  RegisterJavaScriptProxyEx("webTag", &proxyObject, false, nullptr);
  EXPECT_EQ(proxyObject.methodList, nullptr);
  EXPECT_NE(web_obj_ptr->GetWebSharedPtr(), nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, RegisterJavaScriptProxyEx_006) {
  ArkWeb_ProxyMethodWithResult method = {"methodName", nullptr, nullptr};
  ArkWeb_ProxyObjectWithResult proxyObject = {"objName", &method, 0};
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  RegisterJavaScriptProxyEx("webTag", &proxyObject, false, nullptr);
  EXPECT_NE(proxyObject.methodList, nullptr);
  EXPECT_NE(web_obj_ptr->GetWebSharedPtr(), nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, RegisterJavaScriptProxyEx_007) {
  ArkWeb_ProxyMethodWithResult method = {"methodName", nullptr, nullptr};
  ArkWeb_ProxyObjectWithResult proxyObject = {"objName", &method, 0};
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  RegisterJavaScriptProxyEx("webTag", &proxyObject, false, "permission");
  EXPECT_NE(proxyObject.methodList, nullptr);
  EXPECT_NE(web_obj_ptr->GetWebSharedPtr(), nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, RegisterJavaScriptProxyEx_008) {
  ArkWeb_ProxyMethodWithResult method = {"methodName", nullptr, nullptr};
  ArkWeb_ProxyObjectWithResult proxyObject = {"objName", &method, 1};
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  RegisterJavaScriptProxyEx("webTag", &proxyObject, false, "permission");
  EXPECT_NE(proxyObject.methodList, nullptr);
  EXPECT_NE(web_obj_ptr->GetWebSharedPtr(), nullptr);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RegisterJavaScriptProxyEx_001) {
  const char* webTag = "testTag";
  ArkWeb_ProxyObjectWithResult proxyObject;
  const char* permission = "testPermission";
  EXPECT_NO_FATAL_FAILURE(
      OH_ArkWeb_RegisterJavaScriptProxyEx(webTag, &proxyObject, permission));
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RegisterJavaScriptProxyEx_002) {
  const char* webTag = nullptr;
  ArkWeb_ProxyObjectWithResult proxyObject;
  const char* permission = "testPermission";
  EXPECT_NO_FATAL_FAILURE(
      OH_ArkWeb_RegisterJavaScriptProxyEx(webTag, &proxyObject, permission));
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RegisterJavaScriptProxyEx_003) {
  const char* webTag = "testTag";
  const ArkWeb_ProxyObjectWithResult* proxyObject = nullptr;
  const char* permission = "testPermission";
  EXPECT_NO_FATAL_FAILURE(
      OH_ArkWeb_RegisterJavaScriptProxyEx(webTag, proxyObject, permission));
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RegisterJavaScriptProxyEx_004) {
  const char* webTag = nullptr;
  const ArkWeb_ProxyObjectWithResult* proxyObject = nullptr;
  const char* permission = "testPermission";
  EXPECT_NO_FATAL_FAILURE(
      OH_ArkWeb_RegisterJavaScriptProxyEx(webTag, proxyObject, permission));
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RegisterAsyncJavaScriptProxyEx_001) {
  const char* webTag = "validWebTag";
  ArkWeb_ProxyObject proxyObject;
  const char* permission = "validPermission";
  EXPECT_NO_FATAL_FAILURE(OH_ArkWeb_RegisterAsyncJavaScriptProxyEx(
      webTag, &proxyObject, permission));
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RegisterAsyncJavaScriptProxyEx_002) {
  const char* webTag = nullptr;
  ArkWeb_ProxyObject proxyObject;
  const char* permission = "validPermission";
  EXPECT_NO_FATAL_FAILURE(OH_ArkWeb_RegisterAsyncJavaScriptProxyEx(
      webTag, &proxyObject, permission));
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RegisterAsyncJavaScriptProxyEx_003) {
  const char* webTag = "validWebTag";
  const ArkWeb_ProxyObject* proxyObject = nullptr;
  const char* permission = "validPermission";
  EXPECT_NO_FATAL_FAILURE(OH_ArkWeb_RegisterAsyncJavaScriptProxyEx(
      webTag, proxyObject, permission));
}

TEST_F(ArkWebModelTest, OH_ArkWeb_RegisterAsyncJavaScriptProxyEx_004) {
  const char* webTag = "validWebTag";
  ArkWeb_ProxyObject proxyObject;
  const char* permission = nullptr;
  EXPECT_NO_FATAL_FAILURE(OH_ArkWeb_RegisterAsyncJavaScriptProxyEx(
      webTag, &proxyObject, permission));
}

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
void OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy(
    const char* webTag,
    const ArkWeb_ProxyObjectWithResult* proxyObject,
    const char* permission);
#ifdef __cplusplus
}
#endif  // __cplusplus

TEST_F(ArkWebModelTest,
       OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy_001) {
  const char* webTag = nullptr;
  const ArkWeb_ProxyObjectWithResult* proxyObject = nullptr;
  const char* permission = nullptr;
  EXPECT_NO_FATAL_FAILURE(OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy(
      webTag, proxyObject, permission));
}

TEST_F(ArkWebModelTest,
       OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy_002) {
  EXPECT_NO_FATAL_FAILURE(OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy(
      "webTag", nullptr, nullptr));
}

TEST_F(ArkWebModelTest,
       OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy_003) {
  ArkWeb_ProxyObjectWithResult proxyObject = {nullptr, nullptr, 0};
  EXPECT_NO_FATAL_FAILURE(OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy(
      "webTag", &proxyObject, nullptr));
}

TEST_F(ArkWebModelTest,
       OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy_004) {
  ArkWeb_ProxyObjectWithResult proxyObject = {"objName", nullptr, 0};
  EXPECT_NO_FATAL_FAILURE(OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy(
      "webTag", &proxyObject, nullptr));
}

TEST_F(ArkWebModelTest,
       OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy_005) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  ArkWeb_ProxyObjectWithResult proxyObject = {"objName", nullptr, 0};
  EXPECT_NO_FATAL_FAILURE(OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy(
      "webTag", &proxyObject, nullptr));
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest,
       OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy_006) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  ArkWeb_ProxyMethodWithResult method = {"methodName", nullptr, nullptr};
  ArkWeb_ProxyObjectWithResult proxyObject = {"objName", &method, 0};
  EXPECT_NO_FATAL_FAILURE(OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy(
      "webTag", &proxyObject, "permission"));
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest,
       OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy_007) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  ArkWeb_ProxyObjectWithResult proxyObject = {"objName", nullptr, 0};
  EXPECT_NO_FATAL_FAILURE(OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy(
      "webTag", &proxyObject, "permission"));
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest,
       OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy_008) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  ArkWeb_ProxyMethodWithResult method = {"methodName", nullptr, nullptr};
  ArkWeb_ProxyObjectWithResult proxyObject = {"objName", &method, 0};
  EXPECT_NO_FATAL_FAILURE(OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy(
      "webTag", &proxyObject, nullptr));
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_JavaScript_CreateJavaScriptValue_001) {
  void* data = nullptr;
  size_t dataLength = 10;
  ArkWeb_JavaScriptValuePtr result = OH_JavaScript_CreateJavaScriptValue(
      ArkWeb_JavaScriptValueType::ARKWEB_JAVASCRIPT_BOOL, data, dataLength);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ArkWebModelTest, OH_JavaScript_CreateJavaScriptValue_002) {
  int data = 123;
  size_t dataLength = 0;
  ArkWeb_JavaScriptValuePtr result = OH_JavaScript_CreateJavaScriptValue(
      ArkWeb_JavaScriptValueType::ARKWEB_JAVASCRIPT_BOOL, &data, dataLength);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ArkWebModelTest, OH_JavaScript_CreateJavaScriptValue_003) {
  int data = 123;
  size_t dataLength = sizeof(data);
  ArkWeb_JavaScriptValuePtr result = OH_JavaScript_CreateJavaScriptValue(
      ArkWeb_JavaScriptValueType::ARKWEB_JAVASCRIPT_NONE, &data, dataLength);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ArkWebModelTest, OH_JavaScript_CreateJavaScriptValue_004) {
  int data = 123;
  size_t dataLength = sizeof(data);
  ArkWeb_JavaScriptValuePtr result = OH_JavaScript_CreateJavaScriptValue(
      ArkWeb_JavaScriptValueType::ARKWEB_JAVASCRIPT_BOOL, &data, dataLength);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ArkWebModelTest, OH_JavaScript_CreateJavaScriptValue_005) {
  bool data = true;
  size_t dataLength = sizeof(data);
  ArkWeb_JavaScriptValuePtr result = OH_JavaScript_CreateJavaScriptValue(
      ArkWeb_JavaScriptValueType::ARKWEB_JAVASCRIPT_BOOL, &data, dataLength);
  EXPECT_NE(result, nullptr);
  EXPECT_EQ(result->type, ArkWeb_JavaScriptValueType::ARKWEB_JAVASCRIPT_BOOL);
  EXPECT_EQ(result->dataLength, dataLength);
  EXPECT_EQ(*reinterpret_cast<bool*>(result->data), data);
  delete[] reinterpret_cast<char*>(result->data);
  delete result;
}

TEST_F(ArkWebModelTest, OH_JavaScript_CreateJavaScriptValue_006) {
  bool data = true;
  size_t dataLength = sizeof(data);
  // Assuming memcpy_s fails when destination is nullptr
  ArkWeb_JavaScriptValuePtr result = OH_JavaScript_CreateJavaScriptValue(
      ArkWeb_JavaScriptValueType::ARKWEB_JAVASCRIPT_BOOL, &data, dataLength);
  EXPECT_NE(result, nullptr);
}

TEST_F(ArkWebModelTest, OH_NativeArkWeb_LoadData_001) {
  ArkWeb_ErrorCode result =
      OH_NativeArkWeb_LoadData("validTag", "data", "text/html", "UTF-8",
                               "http://example.com", "http://example.com");
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INIT_ERROR);
}

TEST_F(ArkWebModelTest, OH_NativeArkWeb_LoadData_002) {
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  ArkWeb_ErrorCode result = OH_NativeArkWeb_LoadData(
      "webTag", "data", "text/html", "UTF-8", "", "");
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INIT_ERROR);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_NativeArkWeb_LoadData_003) {
  const char* webTag = "webTag";
  const char* key = "validKey";
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  ArkWeb_ErrorCode result = OH_NativeArkWeb_LoadData(
      "webTag", "data", "text/html", "UTF-8", "", "");
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_SUCCESS);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_NativeArkWeb_LoadData_004) {
  const char* webTag = "webTag";
  const char* key = "validKey";
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  ArkWeb_ErrorCode result = OH_NativeArkWeb_LoadData(
      "webTag", "data", "text/html", "UTF-8", nullptr, "");
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_SUCCESS);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_NativeArkWeb_LoadData_005) {
  const char* webTag = "webTag";
  const char* key = "validKey";
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  ArkWeb_ErrorCode result = OH_NativeArkWeb_LoadData(
      "webTag", "data", "text/html", "UTF-8", nullptr, nullptr);
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_SUCCESS);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_NativeArkWeb_LoadData_006) {
  const char* webTag = "webTag";
  const char* key = "validKey";
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  ArkWeb_ErrorCode result = OH_NativeArkWeb_LoadData(
      "webTag", "data", "text/html", "UTF-8", "", "http://example.com");
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_SUCCESS);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_NativeArkWeb_LoadData_007) {
  ArkWeb_ErrorCode result =
      OH_NativeArkWeb_LoadData("validTag", nullptr, nullptr, nullptr,
                               "http://example.com", "http://example.com");
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  result = OH_NativeArkWeb_LoadData("validTag", nullptr, nullptr, "UTF-8",
                               "http://example.com", "http://example.com");
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  result = OH_NativeArkWeb_LoadData("validTag", nullptr, "text/html", nullptr,
                               "http://example.com", "http://example.com");
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  result = OH_NativeArkWeb_LoadData("validTag", nullptr, "text/html", "UTF-8",
                               "http://example.com", "http://example.com");
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  result = OH_NativeArkWeb_LoadData("validTag", "data", nullptr, nullptr,
                               "http://example.com", "http://example.com");
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  result = OH_NativeArkWeb_LoadData("validTag", "data", nullptr, "UTF-8",
                               "http://example.com", "http://example.com");
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
  result = OH_NativeArkWeb_LoadData("validTag", "data", "text/html", nullptr,
                               "http://example.com", "http://example.com");
  EXPECT_EQ(result, ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWebModelTest, OH_NativeArkWeb_GetBlanklessInfoWithKey_001) {
  const char* webTag = "invalidWebTag";
  const char* key = "validKey";
  ArkWeb_BlanklessInfo result =
      OH_NativeArkWeb_GetBlanklessInfoWithKey(webTag, key);
  EXPECT_EQ(
      result.errCode,
      ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_CONTROLLER_NOT_INITED);
}

TEST_F(ArkWebModelTest, OH_NativeArkWeb_GetBlanklessInfoWithKey_002) {
  const char* webTag = "webTag";
  const char* key = "validKey";
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  ArkWeb_BlanklessInfo result =
      OH_NativeArkWeb_GetBlanklessInfoWithKey(webTag, key);
  EXPECT_EQ(
      result.errCode,
      ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_CONTROLLER_NOT_INITED);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_NativeArkWeb_GetBlanklessInfoWithKey_003) {
  const char* webTag = "webTag";
  const char* key = "validKey";
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  ArkWeb_BlanklessInfo result =
      OH_NativeArkWeb_GetBlanklessInfoWithKey(webTag, key);
  EXPECT_EQ(result.errCode,
            ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_SUCCESS);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_NativeArkWeb_GetBlanklessInfoWithKey_004) {
  const char* webTag = "webTag";
  const char* key = "validKey";
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  ArkWeb_BlanklessInfo result =
      OH_NativeArkWeb_GetBlanklessInfoWithKey(webTag, nullptr);
  EXPECT_EQ(result.errCode,
            ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_INVALID_ARGS);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_NativeArkWeb_SetBlanklessLoadingWithKey_001) {
  const char* webTag = "webTag";
  const char* key = "webTag";
  bool isStarted = true;
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  auto result =
      OH_NativeArkWeb_SetBlanklessLoadingWithKey(webTag, key, isStarted);
  EXPECT_EQ(result,
            ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_SIGNIFICANT_CHANGE);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_NativeArkWeb_SetBlanklessLoadingWithKey_002) {
  const char* webTag = "webTag";
  const char* key = "webTag";
  bool isStarted = false;
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  auto result =
      OH_NativeArkWeb_SetBlanklessLoadingWithKey(webTag, key, isStarted);
  EXPECT_EQ(result,
            ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_SIGNIFICANT_CHANGE);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_NativeArkWeb_SetBlanklessLoadingWithKey_003) {
  const char* webTag = nullptr;
  const char* key = "validKey";
  bool isStarted = true;
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto web_obj_ptr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag("webTag");
  web_obj_ptr->SetWebWeakPtr(nweb_impl);
  auto result =
      OH_NativeArkWeb_SetBlanklessLoadingWithKey(webTag, key, isStarted);
  EXPECT_EQ(
      result,
      ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_CONTROLLER_NOT_INITED);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}

TEST_F(ArkWebModelTest, OH_NativeArkWeb_SetBlanklessLoadingWithKey_004) {
  const char* webTag = "webTag";
  const char* key = "webTag";
  bool isStarted = true;
  auto web_obj = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("webTag");
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl =
      std::make_shared<OHOS::NWeb::NWebImpl>(1);
  nweb_impl->AddNWebToMap(1, nweb_impl);
  web_obj->BindWebTagToWebInstance(1, "webTag");
  auto result =
      OH_NativeArkWeb_SetBlanklessLoadingWithKey(webTag, key, isStarted);
  EXPECT_EQ(
      result,
      ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_CONTROLLER_NOT_INITED);
  web_obj->SetDestroyCallback([]() {});
  web_obj->FireDestroyCallback();
}