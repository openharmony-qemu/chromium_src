/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohos_nweb/src/capi/arkweb_model.h"

#include <memory>
#include <string>
#include <vector>

#include "arkweb_native_javascript_execute_callback.h"
#include "arkweb_native_object.h"
#include "arkweb_native_web_message_callback.h"
#include "base/logging.h"
#include "nweb_web_message.h"
#include "nweb_value.h"
#include "nweb_value_callback.h"
#include "ohos_nweb/include/nweb_engine.h"
#include "ohos_nweb/include/nweb_errors.h"
#include "cef/libcef/browser/javascript/oh_gin_javascript_bridge_dispatcher_host.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define ARKWEB_NDK_EXPORT __attribute__((visibility("default")))
#define ARK_WEB_NO_SANITIZE __attribute__((no_sanitize("cfi-icall", "cfi")))

ARKWEB_NDK_EXPORT void OH_ArkWeb_RunJavaScript(
    const char* webTag,
    const ArkWeb_JavaScriptObject* javascriptObject) {
  if (javascriptObject == nullptr || javascriptObject->buffer == nullptr) {
    LOG(ERROR) << "NativeArkWeb javascript object or code is nullptr";
    return;
  }

  auto webObjectPtr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(webTag);
  if (!webObjectPtr) {
    LOG(ERROR) << "NativeArkWeb web object pointer is nullptr";
    return;
  }
  if (auto nwebSharedPtr = webObjectPtr->GetWebSharedPtr()) {
    std::string jsCode{reinterpret_cast<const char*>(javascriptObject->buffer),
                       javascriptObject->size};
    auto javaScriptExecuteCallback = [webTag = std::string(webTag),
                                      cb = javascriptObject->callback,
                                      userData = javascriptObject->userData](
                                         const std::vector<uint8_t>& data) {
      ArkWeb_JavaScriptBridgeData jsBridgeData = {data.data(), data.size()};
      if (cb) {
        cb(webTag.c_str(), &jsBridgeData, userData);
      }
    };
    auto callbackImpl =
        std::make_shared<OHOS::NWeb::ArkWebNativeJavaScriptExecuteCallback>(
            std::move(javaScriptExecuteCallback));
    nwebSharedPtr->ExecuteJavaScript(jsCode, callbackImpl, false);
  } else {
    LOG(ERROR) << "NativeArkWeb RunJavaScript get nweb null:" << webTag;
  }
}

ARKWEB_NDK_EXPORT void OH_ArkWeb_RegisterJavaScriptProxy(
    const char* webTag,
    const ArkWeb_ProxyObject* proxyObject) {
  RegisterJavaScriptProxy(webTag, proxyObject, false, "");
}

ARKWEB_NDK_EXPORT void OH_ArkWeb_DeleteJavaScriptProxy(const char* webTag,
                                                       const char* objName) {
  auto webObjectPtr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(webTag);
  if (!webObjectPtr) {
    LOG(ERROR) << "NativeArkWeb web object pointer is nullptr";
    return;
  }

  if (auto nwebSharedPtr = webObjectPtr->GetWebSharedPtr()) {
    nwebSharedPtr->UnRegisterNativeArkJSFunction(objName);
  } else {
    LOG(ERROR)
        << "NativeArkWeb UnregisterJavaScriptProxy get nweb null: %{public}s"
        << webTag;
  }
}

ARKWEB_NDK_EXPORT void OH_ArkWeb_Refresh(const char* webTag) {
  auto webObjectPtr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(webTag);
  if (!webObjectPtr) {
    LOG(ERROR) << "NativeArkWeb web object pointer is nullptr";
    return;
  }
  if (auto nwebSharedPtr = webObjectPtr->GetWebSharedPtr()) {
    nwebSharedPtr->Reload();
  } else {
    LOG(ERROR) << "NativeArkWeb Refresh get nweb null: %{public}s" << webTag;
  }
}

ARK_WEB_NO_SANITIZE ARKWEB_NDK_EXPORT void OH_ArkWeb_OnControllerAttached(
    const char* webTag,
    ArkWeb_OnComponentCallback callback,
    void* userData) {
  if (callback == nullptr) {
    LOG(ERROR) << "NativeArkWeb OnControllerAttached callback is nullptr";
    return;
  }
  auto webObjectPtr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(webTag);
  if (!webObjectPtr) {
    LOG(ERROR) << "NativeArkWeb web object pointer is nullptr";
    return;
  }
  webObjectPtr->SetValidCallback(
      [cb = callback, webTag = std::string(webTag), userData] {
        if (!cb) {
          LOG(ERROR) << "NativeArkWeb OnControllerAttached callback is nullptr";
          return;
        }
        cb(webTag.c_str(), userData);
      });
}

ARK_WEB_NO_SANITIZE ARKWEB_NDK_EXPORT void OH_ArkWeb_OnPageBegin(
    const char* webTag,
    ArkWeb_OnComponentCallback callback,
    void* userData) {
  if (callback == nullptr) {
    LOG(ERROR) << "NativeArkWeb OnPageBegin callback is nullptr";
    return;
  }
  auto webObjectPtr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(webTag);
  if (!webObjectPtr) {
    LOG(ERROR) << "NativeArkWeb web object pointer is nullptr";
    return;
  }
  webObjectPtr->SetLoadStartCallback(
      [cb = callback, webTag = std::string(webTag), userData] {
        if (!cb) {
          LOG(ERROR) << "NativeArkWeb OnPageBegin callback is nullptr";
          return;
        }
        cb(webTag.c_str(), userData);
      });
}

ARK_WEB_NO_SANITIZE ARKWEB_NDK_EXPORT void OH_ArkWeb_OnPageEnd(const char* webTag,
                                           ArkWeb_OnComponentCallback callback,
                                           void* userData) {
  if (callback == nullptr) {
    LOG(ERROR) << "NativeArkWeb OnPageEnd callback is nullptr";
    return;
  }
  auto webObjectPtr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(webTag);
  if (!webObjectPtr) {
    LOG(ERROR) << "NativeArkWeb web object pointer is nullptr";
    return;
  }
  webObjectPtr->SetLoadEndCallback(
      [cb = callback, webTag = std::string(webTag), userData] {
        if (!cb) {
          LOG(ERROR) << "NativeArkWeb OnPageEnd callback is nullptr";
          return;
        }
        cb(webTag.c_str(), userData);
      });
}

ARK_WEB_NO_SANITIZE ARKWEB_NDK_EXPORT void OH_ArkWeb_OnDestroy(const char* webTag,
                                           ArkWeb_OnComponentCallback callback,
                                           void* userData) {
  if (callback == nullptr) {
    LOG(ERROR) << "NativeArkWeb OnDestroy callback is nullptr";
    return;
  }
  auto webObjectPtr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(webTag);
  if (!webObjectPtr) {
    LOG(ERROR) << "NativeArkWeb web object pointer is nullptr";
    return;
  }
  webObjectPtr->SetDestroyCallback(
      [cb = callback, webTag = std::string(webTag), userData] {
        if (!cb) {
          LOG(ERROR) << "NativeArkWeb OnDestroy callback is nullptr";
          return;
        }
        cb(webTag.c_str(), userData);
      });
}

ARKWEB_NDK_EXPORT void OH_ArkWeb_RegisterAsyncJavaScriptProxy(
    const char* webTag,
    const ArkWeb_ProxyObject* proxyObject) {
  RegisterJavaScriptProxy(webTag, proxyObject, true, "");
}

ARK_WEB_NO_SANITIZE void RegisterJavaScriptProxy(
    const char* webTag,
    const ArkWeb_ProxyObject* proxyObject,
    bool isAsync,
    const char* permission) {
  if (proxyObject == nullptr) {
    LOG(ERROR) << "NativeArkWeb proxy object is nullptr";
    return;
  }

  if (proxyObject->objName == nullptr) {
    LOG(ERROR) << "NativeArkWeb proxy object name is nullptr";
    return;
  }

  auto webObjectPtr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(webTag);
  if (!webObjectPtr) {
    LOG(ERROR) << "NativeArkWeb object pointer is nullptr";
    return;
  }

  if (auto nwebSharedPtr = webObjectPtr->GetWebSharedPtr()) {
    int32_t size = proxyObject->size;
    std::vector<std::function<char*(std::vector<std::vector<uint8_t>>&,
                                    std::vector<size_t>&)>>
        callbackList(size);
    const ArkWeb_ProxyMethod* methodList = proxyObject->methodList;
    if (methodList == nullptr) {
      LOG(ERROR) << "NativeArkWeb method list is nullptr";
      return;
    }

    std::vector<std::string> methodNameList(size);
    for (int32_t i = 0; i < size; i++) {
      auto methodNameObject = methodList[i];
      methodNameList[i] = methodNameObject.methodName;
      auto proxyCallback = [cb = methodNameObject.callback,
                            webTag = std::string(webTag),
                            userData = methodNameObject.userData](
                               std::vector<std::vector<uint8_t>>& dataList,
                               std::vector<size_t>& dataSize) -> char* {
        if (cb) {
          size_t size = dataList.size();
          std::vector<ArkWeb_JavaScriptBridgeData> dataVector(size);
          for (size_t i = 0; i < size; i++) {
            ArkWeb_JavaScriptBridgeData data = {.buffer = dataList[i].data(),
                                                .size = dataSize[i]};
            dataVector[i] = data;
          }
          cb(webTag.c_str(), dataVector.data(), size, userData);
        }
        return nullptr;
      };
      callbackList[i] = std::move(proxyCallback);
    }

    nwebSharedPtr->RegisterNativeArkJSFunction(
      proxyObject->objName, methodNameList, std::move(callbackList), isAsync, permission);
  } else {
    LOG(ERROR)
        << "NativeArkWeb RegisterJavaScriptProxy get nweb null: %{public}s"
        << webTag;
  }
}

ARKWEB_NDK_EXPORT void OH_ArkWeb_RegisterJavaScriptProxyEx(const char* webTag,
                                         const ArkWeb_ProxyObject* proxyObject,
                                         const char* permission) {
  RegisterJavaScriptProxy(webTag,
                          proxyObject, false,
                          permission);
}

ARKWEB_NDK_EXPORT ArkWeb_WebMessagePortPtr* OH_ArkWeb_CreateWebMessagePorts(
    const char* webTag,
    size_t* size) {
  if (!webTag || !size) {
    LOG(ERROR) << "NativeArkWeb CreateWebMessagePorts nullptr error";
    return nullptr;
  }

  if (std::string(webTag).empty()) {
    LOG(ERROR) << "NativeArkWeb CreateWebMessagePorts web tag empty";
    *size = 0;
    return nullptr;
  }

  auto webObjectPtr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(webTag);
  if (!webObjectPtr) {
    LOG(ERROR) << "NativeArkWeb object pointer is nullptr";
    *size = 0;
    return nullptr;
  }

  auto nwebSharedPtr = webObjectPtr->GetWebSharedPtr();
  if (!nwebSharedPtr) {
    LOG(ERROR) << "NativeArkWeb CreateWebMessagePorts get nweb null: %{public}s"
               << webTag;
    *size = 0;
    return nullptr;
  }

  std::vector<std::string> ports = nwebSharedPtr->CreateWebMessagePorts();
  if (ports.size() == 0) {
    LOG(ERROR) << "NativeArkWeb CreateWebMessagePorts failed";
    *size = 0;
    return nullptr;
  }

  ArkWeb_WebMessagePortPtr* wPorts =
      new (std::nothrow) ArkWeb_WebMessagePortPtr[ports.size()];
  if (!wPorts) {
    LOG(ERROR) << "NativeArkWeb CreateWebMessagePorts malloc failed";
    *size = 0;
    return nullptr;
  }
  for (unsigned int i = 0; i < ports.size(); i++) {
    wPorts[i] = new (std::nothrow) ArkWeb_WebMessagePort();
    if (!wPorts[i]) {
      LOG(ERROR) << "NativeArkWeb CreateWebMessagePorts malloc failed";
      nwebSharedPtr->ClosePort(std::string(ports[0]));
      OH_ArkWeb_DestroyWebMessagePorts(&wPorts, ports.size());
      *size = 0;
      return nullptr;
    }

    char* tag = new (std::nothrow) char[std::string(webTag).size() + 1];
    if (!tag) {
      LOG(ERROR) << "NativeArkWeb CreateWebMessagePorts malloc failed";
      nwebSharedPtr->ClosePort(std::string(ports[0]));
      OH_ArkWeb_DestroyWebMessagePorts(&wPorts, ports.size());
      *size = 0;
      return nullptr;
    }
    memcpy(tag, (char*)webTag, std::string(webTag).size() + 1);
    wPorts[i]->webTag = tag;

    char* portHandle = new (std::nothrow) char[ports[i].size() + 1];
    if (!portHandle) {
      LOG(ERROR) << "NativeArkWeb CreateWebMessagePorts malloc failed";
      nwebSharedPtr->ClosePort(std::string(ports[0]));
      OH_ArkWeb_DestroyWebMessagePorts(&wPorts, ports.size());
      *size = 0;
      return nullptr;
    }
    memcpy(portHandle, (char*)(ports[i].c_str()), ports[i].size() + 1);
    wPorts[i]->portHandle = portHandle;
  }
  *size = ports.size();
  return wPorts;
}

ARKWEB_NDK_EXPORT void OH_ArkWeb_DestroyWebMessagePorts(
    ArkWeb_WebMessagePortPtr** port,
    size_t size) {
  if (!port || !(*port)) {
    LOG(ERROR) << "NativeArkWeb DestroyWebMessagePorts nullptr error";
    return;
  }
  for (unsigned int i = 0; i < size; i++) {
    if ((*port)[i] && (*port)[i]->webTag) {
      delete[] (*port)[i]->webTag;
      (*port)[i]->webTag = nullptr;
    }
    if ((*port)[i] && (*port)[i]->portHandle) {
      delete[] (*port)[i]->portHandle;
      (*port)[i]->portHandle = nullptr;
    }
    if ((*port)[i]) {
      delete (*port)[i];
      (*port)[i] = nullptr;
    }
  }
  delete[] (*port);
  *port = nullptr;
}

ARKWEB_NDK_EXPORT ArkWeb_ErrorCode OH_ArkWeb_PostWebMessage(
    const char* webTag,
    const char* name,
    ArkWeb_WebMessagePortPtr* webMessagePorts,
    size_t size,
    const char* url) {
  if (!webTag || !webMessagePorts || !name || !url) {
    LOG(ERROR) << "NativeArkWeb PostWebMessage nullptr error";
    return ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM;
  }

  if (std::string(webTag).empty()) {
    LOG(ERROR) << "NativeArkWeb PostWebMessage web tag empty";
    return ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM;
  }

  if (size < 1) {
    LOG(ERROR) << "NativeArkWeb PostWebMessage number error";
    return ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM;
  }

  auto webObjectPtr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(webTag);
  if (!webObjectPtr) {
    LOG(ERROR) << "NativeArkWeb object pointer is nullptr";
    return ArkWeb_ErrorCode::ARKWEB_INIT_ERROR;
  }

  auto nwebSharedPtr = webObjectPtr->GetWebSharedPtr();
  if (!nwebSharedPtr) {
    LOG(ERROR) << "NativeArkWeb PostWebMessage get nweb null: %{public}s"
               << webTag;
    return ArkWeb_ErrorCode::ARKWEB_INIT_ERROR;
  }

  std::vector<std::string> ports;
  for (unsigned int i = 0; i < size; i++) {
    if (webMessagePorts[i]->portHandle) {
      ports.push_back(std::string(webMessagePorts[i]->portHandle));
    }
  }
  nwebSharedPtr->PostWebMessage(std::string(name), ports,
                                std::string(url));
  return ArkWeb_ErrorCode::ARKWEB_SUCCESS;
}

ARKWEB_NDK_EXPORT ArkWeb_ErrorCode OH_WebMessage_PostMessage(
    const ArkWeb_WebMessagePortPtr webMessagePort,
    const char* webTag,
    const ArkWeb_WebMessagePtr message) {
  if (!webTag || !webMessagePort || !message) {
    LOG(ERROR) << "NativeArkWeb PostMessage nullptr error";
    return ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM;
  }

  if (std::string(webTag).empty()) {
    LOG(ERROR) << "NativeArkWeb PostMessage web tag empty";
    return ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM;
  }

  auto webObjectPtr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(webTag);
  if (!webObjectPtr) {
    LOG(ERROR) << "NativeArkWeb object pointer is nullptr";
    return ArkWeb_ErrorCode::ARKWEB_INIT_ERROR;
  }
  auto nwebSharedPtr = webObjectPtr->GetWebSharedPtr();
  if (!nwebSharedPtr) {
    LOG(ERROR) << "NativeArkWeb PostMessage get nweb null: %{public}s"
               << webTag;
    return ArkWeb_ErrorCode::ARKWEB_INIT_ERROR;
  }

  auto webMsg = std::make_shared<OHOS::NWeb::NWebMessage>(
      OHOS::NWeb::NWebValue::Type::NONE);
  if (!(webMessagePort->portHandle) ||
      std::string(webMessagePort->portHandle).empty()) {
    LOG(ERROR) << "can't post message, message port empty";
    return ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM;
  }
  if (!(message->data)) {
    LOG(ERROR) << "can't post message, message data empty";
    return ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM;
  }

  if (message->webMessageType == ArkWeb_WebMessageType::ARKWEB_STRING) {
    webMsg->SetType(OHOS::NWeb::NWebValue::Type::STRING);
    webMsg->SetString(std::string(((char*)message->data)));
    nwebSharedPtr->PostPortMessage(std::string(webMessagePort->portHandle),
                                    webMsg);
  } else if (message->webMessageType ==
              ArkWeb_WebMessageType::ARKWEB_BUFFER) {
    webMsg->SetType(OHOS::NWeb::NWebValue::Type::BINARY);
    std::vector<uint8_t> vecData(
        (uint8_t*)(message->data),
        (uint8_t*)(message->data) + message->dataLength);
    webMsg->SetBinary(vecData);
    nwebSharedPtr->PostPortMessage(std::string(webMessagePort->portHandle),
                                    webMsg);
  } else {
    LOG(ERROR) << "NativeArkWeb PostMessage, not support data type";
    return ArkWeb_ErrorCode::ARKWEB_INVALID_PARAM;
  }
  return ArkWeb_ErrorCode::ARKWEB_SUCCESS;
}

ARKWEB_NDK_EXPORT void OH_WebMessage_Close(
    const ArkWeb_WebMessagePortPtr webMessagePort,
    const char* webTag) {
  if (!webTag || !webMessagePort) {
    LOG(ERROR) << "NativeArkWeb Close nullptr error";
    return;
  }

  if (std::string(webTag).empty()) {
    LOG(ERROR) << "NativeArkWeb Close web tag empty";
    return;
  }

  if (!(webMessagePort->portHandle)) {
    LOG(ERROR) << "NativeArkWeb Close nullptr error";
    return;
  }

  auto webObjectPtr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(webTag);
  if (!webObjectPtr) {
    LOG(ERROR) << "NativeArkWeb object pointer is nullptr";
    return;
  }
  auto nwebSharedPtr = webObjectPtr->GetWebSharedPtr();
  if (!nwebSharedPtr) {
    LOG(ERROR) << "NativeArkWeb Close get nweb null: %{public}s"
               << webTag;
    return;
  }

  nwebSharedPtr->ClosePort(std::string(webMessagePort->portHandle));
}

ARKWEB_NDK_EXPORT void OH_WebMessage_SetMessageEventHandler(
    const ArkWeb_WebMessagePortPtr webMessagePort,
    const char* webTag,
    ArkWeb_OnMessageEventHandler messageEventHandler,
    void* userData) {
  if (!webTag || !webMessagePort || !messageEventHandler) {
    LOG(ERROR) << "NativeArkWeb SetMessageEventHandler nullptr error";
    return;
  }

  if (std::string(webTag).empty()) {
    LOG(ERROR) << "NativeArkWeb SetMessageEventHandler web tag empty";
    return;
  }

  if (!(webMessagePort->portHandle)) {
    LOG(ERROR) << "NativeArkWeb SetMessageEventHandler, nullptr error";
    return;
  }

  auto webObjectPtr =
      OHOS::NWeb::ArkWebNativeObject::GetWebInstanceByWebTag(webTag);
  if (!webObjectPtr) {
    LOG(ERROR) << "NativeArkWeb web object pointer is nullptr";
    return;
  }

  auto nwebSharedPtr = webObjectPtr->GetWebSharedPtr();
  if (!nwebSharedPtr) {
    LOG(ERROR) << "NativeArkWeb SetMessageEventHandler get nweb null: %{public}s"
               << webTag;
    return;
  }

  auto webMessageCallback =
      [webTag = std::string(webTag), port = webMessagePort,
        cb = messageEventHandler,
        uData = userData](const ArkWeb_WebMessagePtr message) {
        if (cb) {
          LOG(DEBUG) << "NativeArkWeb SetMessageEventHandler, run callback";
          cb(webTag.c_str(), port, message, uData);
        }
      };
  auto callbackImpl =
      std::make_shared<OHOS::NWeb::ArkWebNativeWebMessageCallback>(
          std::move(webMessageCallback));
  nwebSharedPtr->SetPortMessageCallback(
      std::string(webMessagePort->portHandle), callbackImpl);
}

ARKWEB_NDK_EXPORT ArkWeb_WebMessagePtr OH_WebMessage_CreateWebMessage() {
  ArkWeb_WebMessagePtr message = new (std::nothrow) ArkWeb_WebMessage();
  if (!message) {
    LOG(ERROR) << "NativeArkWeb CreateWebMessage malloc failed";
    return nullptr;
  }

  return message;
}

ARKWEB_NDK_EXPORT void OH_WebMessage_DestroyWebMessage(ArkWeb_WebMessagePtr* message) {
  if (!message || !(*message)) {
    LOG(ERROR) << "NativeArkWeb DestroyWebMessage nullptr error";
    return;
  }

  if ((*message)->data) {
    delete[] (char*)((*message)->data);
    (*message)->data = nullptr;
  }

  delete (*message);
  *message = nullptr;
}

ARKWEB_NDK_EXPORT void OH_WebMessage_SetType(ArkWeb_WebMessagePtr message,
                           ArkWeb_WebMessageType type) {
  if (!message) {
    LOG(ERROR) << "NativeArkWeb SetType nullptr error";
    return;
  }

  message->webMessageType = type;
}

ARKWEB_NDK_EXPORT ArkWeb_WebMessageType OH_WebMessage_GetType(ArkWeb_WebMessagePtr message) {
  if (!message) {
    LOG(ERROR) << "NativeArkWeb GetType nullptr error";
    return ArkWeb_WebMessageType::ARKWEB_NONE;
  }

  return message->webMessageType;
}

ARKWEB_NDK_EXPORT void OH_WebMessage_SetData(ArkWeb_WebMessagePtr message,
                           void* data,
                           size_t dataLength) {
  if (!message || !data) {
    LOG(ERROR) << "NativeArkWeb SetData nullptr error";
    return;
  }

  if (dataLength == 0) {
    LOG(ERROR) << "NativeArkWeb SetData data size error";
    return;
  }

  char* destination = new (std::nothrow) char[dataLength];

  if (!destination) {
    LOG(ERROR) << "NativeArkWeb SetData malloc failed";
    return;
  }

  memcpy(destination, (char*)data, dataLength);
  message->data = (void*)destination;
  message->dataLength = dataLength;
}

ARKWEB_NDK_EXPORT void* OH_WebMessage_GetData(ArkWeb_WebMessagePtr message, size_t* dataLength) {
  if (!message || !dataLength) {
    LOG(ERROR) << "NativeArkWeb GetData nullptr error";
    *dataLength = 0;
    return nullptr;
  }

  *dataLength = message->dataLength;
  return message->data;
}

ARKWEB_NDK_EXPORT ArkWeb_ErrorCode OH_CookieManager_FetchCookieSync(
        const char* url, bool incognito,bool includeHttpOnly, char** cookie_value) {
  auto cookie_manager = OHOS::NWeb::NWebEngine::GetInstance()->GetCookieManager();
  if (!cookie_manager) {
    LOG(ERROR) << "cookie manager is nullptr";
    return ARKWEB_ERROR_UNKNOWN;
  }

  bool is_valid = true;
  std::string cookie_content =
      cookie_manager->ReturnCookieWithHttpOnly(std::string(url), is_valid, incognito, includeHttpOnly);
  if (cookie_value == nullptr) {
    return ARKWEB_INVALID_PARAM;
  }

  *cookie_value = new char[cookie_content.length() + 1];
  strcpy((*cookie_value), cookie_content.c_str());
  if (cookie_content == "" && !is_valid) {
    return ARKWEB_INVALID_URL; 
  }

  return ARKWEB_SUCCESS;
}

ARKWEB_NDK_EXPORT ArkWeb_ErrorCode OH_CookieManager_ConfigCookieSync(
        const char* url, const char* value, bool incognito, bool includeHttpOnly) {
  auto cookie_manager = OHOS::NWeb::NWebEngine::GetInstance()->GetCookieManager();
  if (!cookie_manager) {
    LOG(ERROR) << "cookie manager is nullptr";
    return ARKWEB_ERROR_UNKNOWN;
  }

  int result = cookie_manager->SetCookieWithHttpOnly(std::string(url), std::string(value), incognito, includeHttpOnly);
  if (result == OHOS::NWeb::NWebErrNo::NWEB_INVALID_URL) {
    return ARKWEB_INVALID_URL;
  } else if (result == OHOS::NWeb::NWebErrNo::NWEB_INVALID_COOKIE_VALUE) {
    return ARKWEB_INVALID_COOKIE_VALUE;
  }

  return ARKWEB_SUCCESS;
}

ARKWEB_NDK_EXPORT bool OH_CookieManager_ExistCookies(bool incognito) {
  auto cookie_manager = OHOS::NWeb::NWebEngine::GetInstance()->GetCookieManager();
  if (!cookie_manager) {
    LOG(ERROR) << "cookie manager is nullptr";
    return false;
  }

  return cookie_manager->ExistCookies(incognito);
}

ARKWEB_NDK_EXPORT void OH_CookieManager_ClearAllCookiesSync(bool incognito) {
  auto cookie_manager = OHOS::NWeb::NWebEngine::GetInstance()->GetCookieManager();
  if (!cookie_manager) {
    LOG(ERROR) << "cookie manager is nullptr";
    return;
  }

  cookie_manager->DeleteCookieEntirely(nullptr, incognito);
}

ARKWEB_NDK_EXPORT void OH_CookieManager_ClearSessionCookiesSync() {
  auto cookie_manager = OHOS::NWeb::NWebEngine::GetInstance()->GetCookieManager();
  if (!cookie_manager) {
    LOG(ERROR) << "cookie manager is nullptr";
    return;
  }

  cookie_manager->DeleteSessionCookies(nullptr);
}

ARKWEB_NDK_EXPORT const char* OH_ArkWeb_GetLastJavascriptProxyCallingFrameUrl() {
  return NWEB::OhGinJavascriptBridgeDispatcherHost::GetLastCallingFrameUrlTLS();
}

#ifdef __cplusplus
}
#endif  // __cplusplus
