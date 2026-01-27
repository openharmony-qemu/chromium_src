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
 * @since 16
 */
struct ArkWeb_JavaScriptValue {
  /** The data type carried in the ArkWeb_WebMessage. */
  ArkWeb_JavaScriptValueType type;

  /** The data in ArkWeb_WebMessage. */
  void* data;

  /** The data length. */
  size_t dataLength;
};

/**
 * @brief Defines the blankless information.
 *
 * @since 20
 */
typedef struct {
  /** The errCode of the blankless. */
  ArkWeb_BlanklessErrorCode errCode;

  /** The estimated similarity of the history snapshots. */
  double similarity;

  /** The loadingTime of the history loading. */
  int32_t loadingTime;
} ArkWeb_BlanklessInfo;

/**
 * @brief Defines the blankless loading callback information.
 *
 * @since 23
 */
typedef struct {
  /**
   * The key value that uniquely identifies the current page. It is the same as the key value of the
   * OH_NativeArkWeb_SetBlanklessLoadingParams API.
   */
  const char* key;
  /** The state of blankless loading progress. */
  ArkWeb_BlanklessLoadingState state;
  /** Indicates the time when the frame insertion succeeds or the frame insertion is removed. */
  int64_t timestamp;
  /** Indicates the reason why the frame insertion failed. */
  const char* reason;
} ArkWeb_BlanklessLoadingInfo;

/**
 * @brief Defines the callback of blankless.
 * @param blanklessLoadingInfo {@Link ArkWeb_BlanklessLoadingInfo}.
 * It will carries some interpolated frame information.
 * @since 23
 */
typedef void (*OH_ArkWeb_OnBlanklessLoadingCallback)(const ArkWeb_BlanklessLoadingInfo* blanklessLoadingInfo);

/**
 * @brief Defines the blankless loading parameter.
 *
 * @since 23
 */
typedef struct {
  /**
   * Whether to enable frame interpolation. The value true indicates to enable
   * frame interpolation, and the value false indicates the opposite.
   */
  bool enable = false;
  /**
   * The duration time of frame interpolation. The unit is ms and the value
   * must between 200 and 2000. Additionally, a default value of 0 can be specified. This means
   * that the duration of frame interpolation will be determined by the system.
   */
  int32_t duration = 0;
  /**
   * Indicates the time when the historical frame interpolation expires. It is
   * UTC time. When the system exceeds this time, it will trigger cleaning.
   * The maximum validity period is 30 days, this represents the current UTC time plus 30 days.
   * It represents using the system's default cleaning time when passing 0 and the default value is 0.
   */
  int64_t expirationTime = 0;
  /**
   * Called after the frame insertion successful, failed, or removed.
   * For details, see {@Link OH_ArkWeb_OnBlanklessLoadingCallback}.
   */
  OH_ArkWeb_OnBlanklessLoadingCallback callback = nullptr;
} ArkWeb_BlanklessLoadingParam;

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

void OH_ArkWeb_RegisterAsyncThreadJavaScriptProxy(
    const char* webTag,
    const ArkWeb_ProxyObjectWithResult* proxyObject,
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

ArkWeb_BlanklessInfo OH_NativeArkWeb_GetBlanklessInfoWithKey(const char* webTag, const char* key);

ArkWeb_BlanklessErrorCode OH_NativeArkWeb_SetBlanklessLoadingWithKey(const char* webTag,
                                                                     const char* key,
                                                                     bool isStarted);

ArkWeb_BlanklessErrorCode OH_NativeArkWeb_SetBlanklessLoadingParams(const char* webTag,
                                                                    const char* key,
                                                                    const ArkWeb_BlanklessLoadingParam& param);

#ifdef __cplusplus
}
#endif
#endif  // ARKWEB_MODEL_Hap
