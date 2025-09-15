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

#include <securec.h>
#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"

#include "base/logging.h"
#include "cef/libcef/common/request_impl.h"
#include "libcef/common/arkweb_request_impl_ext.h"
#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/http_body_stream.h"
#include "securec.h"

ArkWeb_ResourceRequest_::ArkWeb_ResourceRequest_(CefRefPtr<CefRequest> request)
    : cef_request(request) {
  http_body_stream = new ArkWeb_HttpBodyStream(this);
}

ArkWeb_ResourceRequest_::~ArkWeb_ResourceRequest_() {}

void ArkWeb_ResourceRequest_::GetHttpBodyStream(
    ArkWeb_HttpBodyStream** stream) const {
  if (!cef_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr.";
    return;
  }

  if (!cef_request->AsArkWebRequestExt()->GetUploadStream()) {
    LOG(ERROR) << "scheme_handler resource request donn't have upload stream.";
    *stream = nullptr;
    return;
  }

  http_body_stream->SetForEts(is_ets_);
  *stream = http_body_stream.get();
  // Add ref and decrement at DestroyHttpBodyStream.
  http_body_stream->AddRef();
}

void ArkWeb_ResourceRequest_::GetMethod(char** method) const {
  if (!cef_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr.";
    return;
  }

  std::string cef_method = cef_request->GetMethod().ToString();
  const uint32_t length = cef_method.length();
  *method = new char[length + 1];
  if (!(*method)) {
    LOG(ERROR) << "GetMethod method is nullptr.";
    return;
  }
  int ret = strcpy_s(*method, length + 1, cef_method.c_str());
  if (ret != EOK) {
    LOG(ERROR) << "GetMethod error, call strcpy_s ret = " << ret;
    delete[] *method;
    *method = nullptr;
  }
}

void ArkWeb_ResourceRequest_::GetUrl(char** url) const {
  if (!cef_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr.";
    return;
  }

  std::string cef_url = cef_request->GetURL().ToString();
  const uint32_t length = cef_url.length();
  *url = new char[length + 1];
  if (!(*url)) {
    LOG(ERROR) << "GetUrl url is nullptr.";
    return;
  }
  int ret = strcpy_s(*url, length + 1, cef_url.c_str());
  if (ret != 0) {
    LOG(ERROR) << "GetUrl error, call strcpy_s ret = " << ret;
    delete[] *url;
    *url = nullptr;
  }
}

bool ArkWeb_ResourceRequest_::IsRedirect() const {
  if (!cef_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr.";
    return false;
  }

  // We donn't know which this is a redirect.
  return false;
}

bool ArkWeb_ResourceRequest_::IsMainFrame() const {
  if (!cef_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr.";
    return false;
  }

  return cef_request->AsArkWebRequestExt()->IsMainFrame();
}

bool ArkWeb_ResourceRequest_::HasUserGesture() const {
  if (!cef_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr.";
    return false;
  }

  return cef_request->AsArkWebRequestExt()->HasUserGesture();
}

void ArkWeb_ResourceRequest_::GetReferrer(char** referrer) const {
  if (!cef_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr.";
    return;
  }

  std::string cef_referrer = cef_request->GetReferrerURL().ToString();
  const uint32_t length = cef_referrer.length();
  *referrer = new char[length + 1];
  if (!(*referrer)) {
    LOG(ERROR) << "GetReferrer referrer is nullptr.";
    return;
  }
  int ret = strcpy_s(*referrer, length + 1, cef_referrer.c_str());
  if (ret != 0) {
    LOG(ERROR) << "GetReferrer error, call strcpy_s ret = " << ret;
    delete[] *referrer;
    *referrer = nullptr;
  }
}

int32_t ArkWeb_ResourceRequest_::GetRequestResourceType() const {
  if (!cef_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr.";
    return -1;
  }

  return cef_request->GetResourceType();
}

void ArkWeb_ResourceRequest_::GetFrameUrl(char** frame_url) const {
  if (!cef_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr.";
    return;
  }

  std::string cef_frame_url =
      cef_request->AsArkWebRequestExt()->GetFrameUrl().ToString();
  const uint32_t length = cef_frame_url.length();
  *frame_url = new char[length + 1];
  if (!(*frame_url)) {
    LOG(ERROR) << "GetFrameUrl frame_url is nullptr.";
    return;
  }
  int ret = strcpy_s(*frame_url, length + 1, cef_frame_url.c_str());
  if (ret != 0) {
    LOG(ERROR) << "GetFrameUrl error, call strcpy_s ret = " << ret;
    delete[] *frame_url;
    *frame_url = nullptr;
  }
}

HeaderValue::HeaderValue(const std::string& key, const std::string& value)
    : key(key), value(value) {}

ArkWeb_RequestHeaderList_::ArkWeb_RequestHeaderList_(
    const ArkWeb_ResourceRequest* request) {
  CefRequest::HeaderMap headers;
  if (!request || !request->cef_request) {
    LOG(ERROR) << "ArkWeb_ResourceRequest is null or scheme_handler resource request is nullptr.";
    return;
  }
  request->cef_request->GetHeaderMap(headers);
  CefRequest::HeaderMap::const_iterator it;
  for (it = headers.begin(); it != headers.end(); ++it) {
    HeaderValue item(it->first, it->second);
    header_value.push_back(item);
  }
}

void ArkWeb_RequestHeaderList_::GetHeader(int index,
                                          char** key,
                                          char** value) const {
  if (index >= static_cast<int>(header_value.size()) || index < 0) {
    LOG(ERROR) << "scheme_handler get header index is invalid " << index;
    return;
  }

  std::string cef_key = header_value[index].key;
  uint32_t length = cef_key.length();
  *key = new char[length + 1];
  if (!(*key)) {
    LOG(ERROR) << "GetHeader key is nullptr.";
    return;
  }
  int ret = strcpy_s(*key, length + 1, cef_key.c_str());
  if (ret != 0) {
    LOG(ERROR) << "GetHeader key error, call key strcpy_s ret = " << ret;
    delete[] *key;
    *key = nullptr;
  }

  std::string cef_value = header_value[index].value;
  length = cef_value.length();
  *value = new char[length + 1];
  if (!(*value)) {
    LOG(ERROR) << "GetHeader value is nullptr.";
    return;
  }
  ret = strcpy_s(*value, length + 1, cef_value.c_str());
  if (ret != 0) {
    LOG(ERROR) << "GetHeader value error, call value strcpy_s ret = " << ret;
    delete[] *value;
    *value = nullptr;
  }
}

int32_t ArkWeb_RequestHeaderList_::GetSize() const {
  return header_value.size();
}
