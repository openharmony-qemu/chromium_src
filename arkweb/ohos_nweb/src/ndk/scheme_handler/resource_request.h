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

#ifndef OHOS_NWEB_SRC_CAPI_SCHEME_HANDLER_RESOURCE_REQUEST_H
#define OHOS_NWEB_SRC_CAPI_SCHEME_HANDLER_RESOURCE_REQUEST_H

#include <string>

#include "cef/include/cef_request.h"
#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"

struct ArkWeb_ResourceRequest_ {
  ArkWeb_ResourceRequest_(CefRefPtr<CefRequest> request);
  ~ArkWeb_ResourceRequest_();

  void GetHttpBodyStream(ArkWeb_HttpBodyStream** stream) const;
  void GetMethod(char** method) const;
  void GetUrl(char** url) const;
  bool IsRedirect() const;
  bool IsMainFrame() const;
  bool HasUserGesture() const;
  void GetReferrer(char** referrer) const;
  void SetForEts(bool ets) { is_ets_ = ets; }
  int32_t GetRequestResourceType() const;
  void GetFrameUrl(char** frame_url) const;

  CefRefPtr<CefRequest> cef_request;
  CefRefPtr<ArkWeb_HttpBodyStream> http_body_stream;
  void* user_data{nullptr};
  bool is_ets_ = false;
};

struct HeaderValue {
  HeaderValue(const std::string& key, const std::string& value);

  std::string key;
  std::string value;
};

struct ArkWeb_RequestHeaderList_ {
  ArkWeb_RequestHeaderList_(const ArkWeb_ResourceRequest* resourceRequest);
  void GetHeader(int index, char** key, char** value) const;
  int32_t GetSize() const;

  std::vector<HeaderValue> header_value;
};

#endif  // OHOS_NWEB_SRC_CAPI_SCHEME_HANDLER_RESOURCE_REQUEST_H
