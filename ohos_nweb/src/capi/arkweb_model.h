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

#ifndef ARKWEB_MODEL_H
#define ARKWEB_MODEL_H
#include "arkweb_type.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Defines the ArkWeb_WebMessagePort that represent a HTML5 message port.
 *
 * @since 12
 */
struct ArkWeb_WebMessagePort {
  /** The web tag. */
  char* webTag;
  /** The channel port. */
  char* portHandle;
};

/**
 * @brief Defines the ArkWeb_WebMessage.
 *
 * @since 12
 */
struct ArkWeb_WebMessage {
  /** The data type carried in the ArkWeb_WebMessage. */
  ArkWeb_WebMessageType webMessageType;

  /** The data in ArkWeb_WebMessage. */
  void* data;

  /** The data length. */
  size_t dataLength;
};

/**
 * @brief Defines the ArkWeb_WebMessage.
 *
 * @since 14
 */
struct ArkWeb_JavaScriptValue {
  /** The data type carried in the ArkWeb_WebMessage. */
  ArkWeb_JavaScriptValueType type;

  /** The data in ArkWeb_WebMessage. */
  void* data;

  /** The data length. */
  size_t dataLength;
};

void OH_ArkWeb_RunJavaScript(const char* webTag,
                             const ArkWeb_JavaScriptObject* javascriptObject);
void OH_ArkWeb_RegisterJavaScriptProxy(const char* webTag,
                                       const ArkWeb_ProxyObject* proxyObject);
void OH_ArkWeb_DeleteJavaScriptProxy(const char* webTag, const char* objName);
void OH_ArkWeb_Refresh(const char* webTag);
void OH_ArkWeb_OnControllerAttached(const char* webTag,
                                    ArkWeb_OnComponentCallback callback,
                                    void* userData);
void OH_ArkWeb_OnPageBegin(const char* webTag,
                           ArkWeb_OnComponentCallback callback,
                           void* userData);
void OH_ArkWeb_OnPageEnd(const char* webTag,
                         ArkWeb_OnComponentCallback callback,
                         void* userData);
void OH_ArkWeb_OnDestroy(const char* webTag,
                         ArkWeb_OnComponentCallback callback,
                         void* userData);
bool OH_ArkWeb_OnScroll(const char* webTag,
                        ArkWeb_OnScrollCallback callback,
                        void* userData);
void OH_ArkWeb_RegisterAsyncJavaScriptProxy(
    const char* webTag,
    const ArkWeb_ProxyObject* proxyObject);
void RegisterJavaScriptProxy(const char* webTag,
                             const ArkWeb_ProxyObject* proxyObject,
                             bool isAsync,
                             const char* permission);
ArkWeb_WebMessagePortPtr* OH_ArkWeb_CreateWebMessagePorts(const char* webTag,
                                                          size_t* size);
void OH_ArkWeb_DestroyWebMessagePorts(ArkWeb_WebMessagePortPtr** ports,
                                      size_t size);
ArkWeb_ErrorCode OH_ArkWeb_PostWebMessage(
    const char* webTag,
    const char* name,
    ArkWeb_WebMessagePortPtr* webMessagePorts,
    size_t size,
    const char* url);

ArkWeb_ErrorCode OH_WebMessage_PostMessage(
    const ArkWeb_WebMessagePortPtr webMessagePort,
    const char* webTag,
    const ArkWeb_WebMessagePtr message);
void OH_WebMessage_Close(const ArkWeb_WebMessagePortPtr webMessagePort,
                         const char* webTag);
void OH_WebMessage_SetMessageEventHandler(
    const ArkWeb_WebMessagePortPtr webMessagePort,
    const char* webTag,
    ArkWeb_OnMessageEventHandler messageEventHandler,
    void* userData);

ArkWeb_WebMessagePtr OH_WebMessage_CreateWebMessage();

void OH_WebMessage_DestroyWebMessage(ArkWeb_WebMessagePtr* message);

void OH_WebMessage_SetType(ArkWeb_WebMessagePtr message,
                           ArkWeb_WebMessageType type);

ArkWeb_WebMessageType OH_WebMessage_GetType(ArkWeb_WebMessagePtr message);

void OH_WebMessage_SetData(ArkWeb_WebMessagePtr message,
                           void* data,
                           size_t dataLength);

void* OH_WebMessage_GetData(ArkWeb_WebMessagePtr message, size_t* dataLength);

ArkWeb_ErrorCode OH_CookieManager_FetchCookieSync(const char* url,
                                                  bool incognito,
                                                  bool includeHttpOnly,
                                                  char** cookieValue);

ArkWeb_ErrorCode OH_CookieManager_ConfigCookieSync(const char* url,
                                                   const char* cookieValue,
                                                   bool incognito,
                                                   bool includeHttpOnly);

bool OH_CookieManager_ExistCookies(bool incognito);

void OH_CookieManager_ClearAllCookiesSync(bool incognito);

void OH_CookieManager_ClearSessionCookiesSync();

const char* OH_ArkWeb_GetLastJavascriptProxyCallingFrameUrl();

void OH_ArkWeb_RegisterJavaScriptProxyEx(
    const char* webTag,
    const ArkWeb_ProxyObjectWithResult* proxyObject,
    const char* permission);

void OH_ArkWeb_RegisterAsyncJavaScriptProxyEx(
    const char* webTag,
    const ArkWeb_ProxyObject* proxyObject,
    const char* permission);

void RegisterJavaScriptProxyEx(const char* webTag,
                               const ArkWeb_ProxyObjectWithResult* proxyObject,
                               bool isAsync,
                               const char* permission);

ArkWeb_JavaScriptValuePtr OH_JavaScript_CreateJavaScriptValue(
    ArkWeb_JavaScriptValueType type,
    void* data,
    size_t dataLength);

ArkWeb_ErrorCode OH_NativeArkWeb_LoadData(const char* webTag,
                                          const char* data,
                                          const char* mimeType,
                                          const char* encoding,
                                          const char* baseUrl,
                                          const char* historyUrl);
#ifdef __cplusplus
}
#endif
#endif  // ARKWEB_MODEL_Hap
