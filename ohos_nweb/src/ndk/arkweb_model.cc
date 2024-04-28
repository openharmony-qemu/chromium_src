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
#include "base/logging.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define ARKWEB_NDK_EXPORT __attribute__((visibility("default")))

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
  RegisterJavaScriptProxy(webTag, proxyObject, false);
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

ARKWEB_NDK_EXPORT void OH_ArkWeb_OnControllerAttached(
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

ARKWEB_NDK_EXPORT void OH_ArkWeb_OnPageBegin(
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

ARKWEB_NDK_EXPORT void OH_ArkWeb_OnPageEnd(const char* webTag,
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

ARKWEB_NDK_EXPORT void OH_ArkWeb_OnDestroy(const char* webTag,
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
  RegisterJavaScriptProxy(webTag, proxyObject, true);
}

void RegisterJavaScriptProxy(
    const char* webTag,
    const ArkWeb_ProxyObject* proxyObject,
    bool isAsync) {
  if (proxyObject == nullptr) {
    LOG(ERROR) << "NativeArkWeb proxy object is nullptr";
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
      proxyObject->objName, methodNameList, std::move(callbackList), isAsync);
  } else {
    LOG(ERROR)
        << "NativeArkWeb RegisterJavaScriptProxy get nweb null: %{public}s"
        << webTag;
  }
}

#ifdef __cplusplus
}
#endif  // __cplusplus