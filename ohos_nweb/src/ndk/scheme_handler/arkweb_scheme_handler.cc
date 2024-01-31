/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"

#include "base/logging.h"
#include "ohos_nweb/src/cef_delegate/nweb_application.h"
#include "ohos_nweb/src/cef_delegate/nweb_scheme_handler_factory.h"
#include "ohos_nweb/src/ndk/scheme_handler/post_data_stream.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"
#include "ohos_nweb/src/ndk/scheme_handler/response.h"
#include "ohos_nweb/src/ndk/scheme_handler/scheme_handler.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define ARKWEB_EXPORT __attribute__((visibility("default")))

ARKWEB_EXPORT void OH_ArkWebRequestHeaderList_Create(
    const ArkWeb_ResourceRequest* resource_request,
    ArkWeb_RequestHeaderList** request_header_list) {
  *request_header_list = new ArkWeb_RequestHeaderList_(resource_request);
}

ARKWEB_EXPORT void OH_ArkWebRequestHeaderList_Destroy(
    ArkWeb_RequestHeaderList* request_header_list) {
  if (request_header_list) {
    delete request_header_list;
  } else {
    LOG(ERROR) << "scheme_handler request header list is nullptr";
  }
}

ARKWEB_EXPORT int32_t OH_ArkWebRequestHeaderList_GetSize(
    const ArkWeb_RequestHeaderList* request_header_list) {
  if (!request_header_list) {
    LOG(ERROR) << "scheme_handler request header list is nullptr";
    return -1;
  }

  return request_header_list->GetSize();
}

ARKWEB_EXPORT void OH_ArkWebRequestHeaderList_GetHeader(
    const ArkWeb_RequestHeaderList* request_header_list,
    int32_t index,
    char** key,
    char** value) {
  if (!request_header_list) {
    LOG(ERROR) << "scheme_handler request header list is nullptr";
    return;
  }

  request_header_list->GetHeader(index, key, value);
}

int32_t OH_ArkWebResourceRequest_SetUserData(
    ArkWeb_ResourceRequest* resource_request,
    void* user_data) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler request header list is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }
  resource_request->user_data = user_data;
  return ARKWEB_NET_OK;
}

void* OH_ArkWebResourceRequest_GetUserData(
    const ArkWeb_ResourceRequest* resource_request) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler request header list is nullptr";
    return nullptr;
  }
  return resource_request->user_data;
}

ARKWEB_EXPORT void OH_ArkWebResourceRequest_GetMethod(
    const ArkWeb_ResourceRequest* resource_request,
    char** method) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler request header list is nullptr";
    return;
  }
  resource_request->GetMethod(method);
}

ARKWEB_EXPORT void OH_ArkWebResourceRequest_GetUrl(
    const ArkWeb_ResourceRequest* resource_request,
    char** url) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler request header list is nullptr";
    return;
  }
  resource_request->GetUrl(url);
}

ARKWEB_EXPORT void OH_ArkWebResourceRequest_GetPostData(
    const ArkWeb_ResourceRequest* resource_request,
    ArkWeb_PostDataStream** post_data_stream) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler request header list is nullptr";
    return;
  }
  resource_request->GetPostDataStream(post_data_stream);
}

ARKWEB_EXPORT void OH_ArkWebResourceRequest_DestroyPostData(
    ArkWeb_PostDataStream* post_data_stream) {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post data stream is nullptr";
    return;
  }

  delete post_data_stream;
}

ARKWEB_EXPORT int32_t OH_ArkWebPostDataStream_SetReadCallback(
    ArkWeb_PostDataStream* post_data_stream,
    ArkWeb_PostDataReadCallback readCallback) {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post data stream is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }

  post_data_stream->SetReadCallback(readCallback);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int32_t
OH_ArkWebPostDataStream_SetUserData(ArkWeb_PostDataStream* post_data_stream,
                                    void* user_data) {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post data stream is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }

  post_data_stream->SetUserData(user_data);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void* OH_ArkWebPostDataStream_GetUserData(
    const ArkWeb_PostDataStream* post_data_stream) {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post data stream is nullptr";
    return nullptr;
  }

  return post_data_stream->GetUserData();
}

ARKWEB_EXPORT int32_t
OH_ArkWebPostDataStream_Init(ArkWeb_PostDataStream* post_data_stream,
                             ArkWeb_PostDataStreamInitCallback initCallback) {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post data stream is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }

  post_data_stream->Init(initCallback);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWebPostDataStream_Read(
    const ArkWeb_PostDataStream* post_data_stream,
    uint8_t* buffer,
    int buf_len) {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post data stream is nullptr";
    return;
  }

  post_data_stream->Read(buffer, buf_len);
}

ARKWEB_EXPORT uint64_t
OH_ArkWebPostDataStream_GetSize(const ArkWeb_PostDataStream* post_data_stream) {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post data stream is nullptr";
    return -1;
  }

  return post_data_stream->GetSize();
}

ARKWEB_EXPORT uint64_t OH_ArkWebPostDataStream_GetPosition(
    const ArkWeb_PostDataStream* post_data_stream) {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post data stream is nullptr";
    return -1;
  }

  return post_data_stream->GetPosition();
}

ARKWEB_EXPORT bool OH_ArkWebPostDataStream_IsChunked(
    const ArkWeb_PostDataStream* post_data_stream) {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post data stream is nullptr";
    return false;
  }

  return post_data_stream->IsChunked();
}

ARKWEB_EXPORT bool OH_ArkWebPostDataStream_IsEof(
    const ArkWeb_PostDataStream* post_data_stream) {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post data stream is nullptr";
    return false;
  }

  return post_data_stream->IsEOF();
}

ARKWEB_EXPORT bool OH_ArkWebPostDataStream_IsInMemory(
    const ArkWeb_PostDataStream* post_data_stream) {
  if (!post_data_stream) {
    LOG(ERROR) << "scheme_handler post data stream is nullptr";
    return false;
  }

  return post_data_stream->IsInMemory();
}

ARKWEB_EXPORT void OH_ArkWebResourceRequest_GetReferrer(
    const ArkWeb_ResourceRequest* resource_request,
    char** referrer) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return;
  }

  resource_request->GetReferrer(referrer);
}

ARKWEB_EXPORT void OH_ArkWebResourceRequest_GetRequestHeaders(
    const ArkWeb_ResourceRequest* resource_request,
    ArkWeb_RequestHeaderList** request_header_list) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return;
  }
  *request_header_list = new ArkWeb_RequestHeaderList(resource_request);
}

ARKWEB_EXPORT bool OH_ArkWebResourceRequest_IsRedirect(
    const ArkWeb_ResourceRequest* resource_request) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return false;
  }
  return resource_request->IsRedirect();
}

ARKWEB_EXPORT bool OH_ArkWebResourceRequest_IsMainFrame(
    const ArkWeb_ResourceRequest* resource_request) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return false;
  }
  return resource_request->IsMainFrame();
}

ARKWEB_EXPORT bool OH_ArkWebResourceRequest_HasGesture(
    const ArkWeb_ResourceRequest* resource_request) {
  if (!resource_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr";
    return false;
  }
  return resource_request->HasUserGesture();
}

ARKWEB_EXPORT int32_t OH_ArkWeb_RegisterCustomSchemes(const char* scheme,
                                                      int32_t option) {
  CefRefPtr<OHOS::NWeb::NWebApplication> application =
      OHOS::NWeb::NWebApplication::GetDefault();
  if (!application) {
    LOG(ERROR) << "scheme_handler application is nulltpr.";
    return ARKWEB_NET_INVALID_PARAM;
  }

  application->RegisterCustomSchemes(std::string(scheme), option);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT bool OH_ArkWebServiceWorker_SetSchemeHandler(
    const char* scheme,
    ArkWeb_SchemeHandler* scheme_handler) {
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return false;
  }

  CefRefPtr<OHOS::NWeb::NWebSchemeHandlerFactory> factory =
      OHOS::NWeb::NWebSchemeHandlerFactory::GetOrCreateForScheme(
          std::string(scheme));
  if (!factory) {
    LOG(ERROR) << "scheme_handler factory is nullptr.";
    return false;
  }

  factory->SetSchemeHandlerForSW(scheme_handler);
  return true;
}

ARKWEB_EXPORT bool OH_ArkWeb_SetSchemeHandler(
    const char* scheme,
    const char* web_tag,
    ArkWeb_SchemeHandler* scheme_handler) {
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return false;
  }
  CefRefPtr<OHOS::NWeb::NWebSchemeHandlerFactory> factory =
      OHOS::NWeb::NWebSchemeHandlerFactory::GetOrCreateForScheme(
          std::string(scheme));
  if (!factory) {
    LOG(ERROR) << "scheme_handler factory is nullptr.";
    return false;
  }

  factory->SetSchemeHandler(std::string(web_tag), scheme_handler);
  return true;
}

ARKWEB_EXPORT int32_t OH_ArkWebServiceWorker_ClearSchemeHandlers() {
  OHOS::NWeb::NWebSchemeHandlerFactory::ClearServiceWorkerSchemeHandler();
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int32_t OH_ArkWeb_ClearSchemeHandlers(const char* web_tag) {
  OHOS::NWeb::NWebSchemeHandlerFactory::ClearSchemeHandlers(
      std::string(web_tag));
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWeb_CreateSchemeHandler(
    ArkWeb_SchemeHandler** scheme_handler) {
  *scheme_handler = new ArkWeb_SchemeHandler();
}

ARKWEB_EXPORT void OH_ArkWeb_DestroySchemeHandler(
    ArkWeb_SchemeHandler* scheme_handler) {
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return;
  }

  delete scheme_handler;
}

ARKWEB_EXPORT int32_t
OH_ArkWebSchemeHandler_SetUserData(ArkWeb_SchemeHandler* scheme_handler,
                                   void* user_data) {
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return ARKWEB_NET_INVALID_PARAM;
  }

  if (!user_data) {
    LOG(ERROR) << "scheme_handler set a nullptr.";
    return ARKWEB_NET_INVALID_PARAM;
  }

  scheme_handler->user_data = user_data;
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void* OH_ArkWebSchemeHandler_GetUserData(
    const ArkWeb_SchemeHandler* scheme_handler) {
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return nullptr;
  }

  return scheme_handler->user_data;
}

ARKWEB_EXPORT int32_t OH_ArkWebSchemeHandler_SetOnRequestStart(
    ArkWeb_SchemeHandler* scheme_handler,
    ArkWeb_OnRequestStart on_request_start) {
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return ARKWEB_NET_INVALID_PARAM;
  }

  scheme_handler->on_request_start = on_request_start;
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int32_t
OH_ArkWebSchemeHandler_SetOnRequestStop(ArkWeb_SchemeHandler* scheme_handler,
                                        ArkWeb_OnRequestStop on_request_stop) {
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return ARKWEB_NET_INVALID_PARAM;
  }

  scheme_handler->on_request_stop = on_request_stop;
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWeb_CreateResponse(ArkWeb_Response** response) {
  *response = new ArkWeb_Response();
}

ARKWEB_EXPORT void OH_ArkWeb_DestroyResponse(ArkWeb_Response* response) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }
  delete response;
}

ARKWEB_EXPORT int32_t OH_ArkWebResponse_SetUrl(ArkWeb_Response* response,
                                               const char* url) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }
  response->SetUrl(url);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWebResponse_GetUrl(const ArkWeb_Response* response,
                                            char** url) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }
  response->GetUrl(url);
}

ARKWEB_EXPORT int32_t OH_ArkWebResponse_SetError(ArkWeb_Response* response,
                                                 ArkWeb_NetError error_code) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }
  response->SetError(error_code);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT ArkWeb_NetError
OH_ArkWebResponse_GetError(const ArkWeb_Response* response) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }
  return response->GetError();
}

ARKWEB_EXPORT int32_t OH_ArkWebResponse_SetStatus(ArkWeb_Response* response,
                                                  int status) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }
  response->SetStatus(status);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int OH_ArkWebResponse_GetStatus(const ArkWeb_Response* response) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return -1;
  }
  return response->GetStatus();
}

ARKWEB_EXPORT int32_t OH_ArkWebResponse_SetStatusText(ArkWeb_Response* response,
                                                      const char* status_text) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }
  response->SetStatusText(status_text);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWebResponse_GetStatusText(
    const ArkWeb_Response* response,
    char** status_text) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }
  response->GetStatusText(status_text);
}

ARKWEB_EXPORT int32_t OH_ArkWebResponse_SetMimeType(ArkWeb_Response* response,
                                                    const char* mime_type) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }
  response->SetMimeType(mime_type);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWebResponse_GetMimeType(
    const ArkWeb_Response* response,
    char** mime_type) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }
  response->GetMimeType(mime_type);
}

ARKWEB_EXPORT int32_t OH_ArkWebResponse_SetCharset(ArkWeb_Response* response,
                                                   const char* charset) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }
  response->SetCharset(charset);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWebResponse_GetCharset(const ArkWeb_Response* response,
                                                char** charset) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }
  response->GetCharset(charset);
}

ARKWEB_EXPORT int32_t
OH_ArkWebResponse_SetHeaderByName(ArkWeb_Response* response,
                                  const char* name,
                                  const char* value,
                                  bool overwrite) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }
  response->SetHeaderByName(name, value, overwrite);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWebResponse_GetHeaderByName(
    const ArkWeb_Response* response,
    const char* name,
    char** value) {
  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }
  response->GetHeaderByName(name, value);
}

ARKWEB_EXPORT int32_t OH_ArkWebResourceHandler_DidReceiveResponse(
    const ArkWeb_ResourceHandler* resource_handler,
    const ArkWeb_Response* response) {
  if (!resource_handler) {
    LOG(ERROR) << "scheme_handler response handler is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }

  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }
  resource_handler->DidReceiveResponse(response);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int32_t OH_ArkWebResourceHandler_DidReceiveData(
    const ArkWeb_ResourceHandler* resource_handler,
    const uint8_t* buffer,
    int64_t buf_len) {
  if (!resource_handler) {
    LOG(ERROR) << "scheme_handler response handler is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }

  if (!buffer) {
    LOG(ERROR) << "scheme_handler buffer is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }
  resource_handler->DidReceiveData(buffer, buf_len);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int32_t OH_ArkWebResourceHandler_DidFinish(
    const ArkWeb_ResourceHandler* resource_handler) {
  if (!resource_handler) {
    LOG(ERROR) << "scheme_handler response handler is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }
  resource_handler->DidFinish();
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT int32_t OH_ArkWebResourceHandler_DidFailWithError(
    const ArkWeb_ResourceHandler* resource_handler,
    ArkWeb_NetError error_code) {
  if (!resource_handler) {
    LOG(ERROR) << "scheme_handler response handler is nullptr";
    return ARKWEB_NET_INVALID_PARAM;
  }
  resource_handler->DidFailWithError(error_code);
  return ARKWEB_NET_OK;
}

ARKWEB_EXPORT void OH_ArkWeb_ReleaseString(char* string) {
  if (string) {
    delete string;
  }
}

ARKWEB_EXPORT void OH_ArkWeb_ReleaseByteArray(uint8_t* byte_array) {
  if (byte_array) {
    delete byte_array;
  }
}

#ifdef __cplusplus
}
#endif __cplusplus
