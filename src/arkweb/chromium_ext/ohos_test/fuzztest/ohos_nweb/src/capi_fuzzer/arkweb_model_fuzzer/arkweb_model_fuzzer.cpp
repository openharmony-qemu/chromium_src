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

#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <vector>

#include "base/logging.h"
#include "arkweb/ohos_nweb/src/capi/arkweb_model.h"
#include "third_party/bounds_checking_function/include/securec.h"

namespace OHOS::NWeb {

void OH_ArkWeb_JavaScript_FuzzTest(FuzzedDataProvider* fdp) {
  std::string webTag = fdp->ConsumeRandomLengthString(64);
  size_t jsCodeSize = fdp->ConsumeIntegralInRange<size_t>(0, 1024);
  std::vector<uint8_t> jsCode = fdp->ConsumeBytes<uint8_t>(jsCodeSize);
  ArkWeb_JavaScriptObject jsObject;
  jsObject.buffer = jsCode.data();
  jsObject.size = jsCode.size();
  jsObject.callback = nullptr;
  jsObject.userData = nullptr;

  std::string permission = fdp->ConsumeRandomLengthString(64);
  const size_t max_methods = 20;
  size_t listSize = fdp->ConsumeIntegralInRange<size_t>(0, max_methods);
  std::vector<ArkWeb_ProxyMethod> methodList;

  for (size_t i = 0; i < listSize; ++i) {
    ArkWeb_ProxyMethod proxyMethod;
    proxyMethod.methodName = strdup(fdp->ConsumeRandomLengthString(64).c_str());
    proxyMethod.callback = nullptr;
    proxyMethod.userData = nullptr;
    methodList.push_back(proxyMethod);
  }

  ArkWeb_ProxyObject proxyObject;
  std::string objNameStr = fdp->ConsumeRandomLengthString(64);
  proxyObject.objName = objNameStr.c_str();
  if (listSize == 0) {
    proxyObject.methodList = nullptr;
    proxyObject.size = 0;
  } else {
    proxyObject.methodList = methodList.data();
    proxyObject.size = listSize;
  }

  OH_ArkWeb_RegisterJavaScriptProxy(webTag.c_str(), &proxyObject);
  OH_ArkWeb_RegisterAsyncJavaScriptProxyEx(webTag.c_str(), &proxyObject, permission.c_str());
  OH_ArkWeb_RunJavaScript(webTag.c_str(), &jsObject);
  OH_ArkWeb_RegisterAsyncJavaScriptProxy(webTag.c_str(), &proxyObject);
  RegisterJavaScriptProxy(webTag.c_str(), &proxyObject, fdp->ConsumeBool(), permission.c_str());
  OH_ArkWeb_DeleteJavaScriptProxy(webTag.c_str(), proxyObject.objName);

  for (auto& method : methodList) {
    free(const_cast<char*>(method.methodName));
    method.methodName = nullptr;
  }
}

void OH_ArkWeb_LifecycleOperations_FuzzTest(FuzzedDataProvider* fdp) {
  std::string webTag = fdp->ConsumeRandomLengthString(64);

  OH_ArkWeb_OnControllerAttached(webTag.c_str(), nullptr, nullptr);
  OH_ArkWeb_Refresh(webTag.c_str());
  OH_ArkWeb_OnPageBegin(webTag.c_str(), nullptr, nullptr);
  OH_ArkWeb_OnPageEnd(webTag.c_str(), nullptr, nullptr);
  OH_ArkWeb_OnScroll(webTag.c_str(), nullptr, nullptr);
  OH_ArkWeb_OnDestroy(webTag.c_str(), nullptr, nullptr);
}

void OH_ArkWeb_WebMessagePorts_FuzzTest(FuzzedDataProvider* fdp) {
  std::string webTag = fdp->ConsumeRandomLengthString(64);
  std::string name = fdp->ConsumeRandomLengthString(64);
  std::string url = fdp->ConsumeRandomLengthString(64);
  size_t size = fdp->ConsumeIntegral<size_t>();
  size_t portCount = 0;

  ArkWeb_WebMessagePortPtr* ports = OH_ArkWeb_CreateWebMessagePorts(webTag.c_str(), &portCount);
  OH_ArkWeb_PostWebMessage(webTag.c_str(), name.c_str(), ports, size, url.c_str());
  OH_ArkWeb_DestroyWebMessagePorts(&ports, portCount);
}

void OH_WebMessage_FuzzTest(FuzzedDataProvider* fdp) {
  std::string webTag = fdp->ConsumeRandomLengthString(64);
  std::string portHandle = fdp->ConsumeRandomLengthString(64);
  size_t dataLength = fdp->ConsumeIntegralInRange<size_t>(0, 1024);
  ArkWeb_WebMessageType webMessageType;
  ArkWeb_WebMessagePort webMessagePort;
  webMessagePort.webTag = const_cast<char*>(webTag.c_str());
  webMessagePort.portHandle = const_cast<char*>(portHandle.c_str());

  ArkWeb_WebMessagePtr messagePtr = OH_WebMessage_CreateWebMessage();

  std::string dataStr = fdp->ConsumeRandomLengthString(dataLength);
  char* data = strdup(dataStr.c_str());
  size_t dataSize = dataStr.size();
  webMessageType = ARKWEB_STRING;
  OH_WebMessage_SetType(messagePtr, webMessageType);
  OH_WebMessage_GetType(messagePtr);
  OH_WebMessage_SetData(messagePtr, data, dataSize + 1);
  OH_WebMessage_GetData(messagePtr, &dataLength);
  OH_WebMessage_PostMessage(&webMessagePort, webTag.c_str(), messagePtr);
  OH_WebMessage_Close(&webMessagePort, webTag.c_str());
  OH_WebMessage_SetMessageEventHandler(&webMessagePort, webTag.c_str(), nullptr, nullptr);
  OH_WebMessage_DestroyWebMessage(&messagePtr);

  free(data);
  data = nullptr;
}

void OH_CookieManager_FuzzTest(FuzzedDataProvider* fdp) {
  std::string url = fdp->ConsumeRandomLengthString(64);
  std::string value = fdp->ConsumeRandomLengthString(64);
  bool incognito = fdp->ConsumeBool();
  bool includeHttpOnly = fdp->ConsumeBool();
  char* cookieValue = nullptr;

  OH_CookieManager_FetchCookieSync(url.c_str(), incognito, includeHttpOnly, &cookieValue);
  free(cookieValue);
  cookieValue = nullptr;
  OH_CookieManager_ConfigCookieSync(url.c_str(), value.c_str(), incognito, includeHttpOnly);
}

void OH_ArkWeb_JavaScriptProxyEx_FuzzTest(FuzzedDataProvider* fdp) {
  std::string webTag = fdp->ConsumeRandomLengthString(64);
  std::string permission = fdp->ConsumeRandomLengthString(64);
  std::string objName = fdp->ConsumeRandomLengthString(64);
  bool isAsync = fdp->ConsumeBool();

  const size_t listSize = 20;
  std::vector<ArkWeb_ProxyMethodWithResult> methodList;

  for(size_t i = 0; i < listSize; ++i) {
    ArkWeb_ProxyMethodWithResult proxyMethod;
    proxyMethod.methodName = strdup(fdp->ConsumeRandomLengthString(64).c_str());
    proxyMethod.callback = nullptr;
    proxyMethod.userData = nullptr;
    methodList.push_back(proxyMethod);
  }

  ArkWeb_ProxyObjectWithResult proxyObject;
  proxyObject.objName = objName.c_str();
  proxyObject.methodList = methodList.data();
  proxyObject.size = listSize;

  OH_ArkWeb_RegisterJavaScriptProxyEx(webTag.c_str(), &proxyObject, permission.c_str());
  RegisterJavaScriptProxyEx(webTag.c_str(), &proxyObject, isAsync, permission.c_str());

  ArkWeb_JavaScriptValueType type = static_cast<ArkWeb_JavaScriptValueType>(
    fdp->ConsumeIntegralInRange<uint32_t>(ARKWEB_JAVASCRIPT_NONE, ARKWEB_JAVASCRIPT_BOOL));

  size_t dataLength = fdp->ConsumeIntegralInRange<size_t>(0, 1024);
  std::string dataStr = fdp->ConsumeRandomLengthString(dataLength);
  char* data = strdup(dataStr.c_str());
  size_t dataSize = dataStr.size();

  ArkWeb_JavaScriptValuePtr javaScriptPtr = OH_JavaScript_CreateJavaScriptValue(type, data, dataSize);

  free(javaScriptPtr);
  javaScriptPtr = nullptr;

  free(data);
  data = nullptr;

  for (auto& method: methodList) {
    free(const_cast<char*>(method.methodName));
    method.methodName = nullptr;
  }
}

void OH_NativeArkWeb_LoadData_FuzzTest(FuzzedDataProvider* fdp) {
  std::string webTag = fdp->ConsumeRandomLengthString(64);
  std::string data = fdp->ConsumeRandomLengthString(64);
  std::string mimeType = fdp->ConsumeRandomLengthString(64);
  std::string encoding = fdp->ConsumeRandomLengthString(64);
  std::string baseUrl = fdp->ConsumeRandomLengthString(64);
  std::string historyUrl = fdp->ConsumeRandomLengthString(64);

  OH_NativeArkWeb_LoadData(webTag.c_str(), data.c_str(), mimeType.c_str(),
      encoding.c_str(), baseUrl.c_str(), historyUrl.c_str());
}
} // OHOS::NWeb

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size < sizeof(int32_t))) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);
  OHOS::NWeb::OH_ArkWeb_JavaScript_FuzzTest(&fdp);
  OHOS::NWeb::OH_ArkWeb_LifecycleOperations_FuzzTest(&fdp);
  OHOS::NWeb::OH_ArkWeb_WebMessagePorts_FuzzTest(&fdp);
  OHOS::NWeb::OH_WebMessage_FuzzTest(&fdp);
  OHOS::NWeb::OH_CookieManager_FuzzTest(&fdp);
  OHOS::NWeb::OH_ArkWeb_JavaScriptProxyEx_FuzzTest(&fdp);
  OHOS::NWeb::OH_NativeArkWeb_LoadData_FuzzTest(&fdp);
  return 0;
}