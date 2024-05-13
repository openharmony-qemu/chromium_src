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

#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"

#include "base/logging.h"
#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/http_body_stream.h"

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

  if (!cef_request->GetUploadStream()) {
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
  const int length = cef_method.length();
  *method = new char[length + 1];
  strcpy((*method), cef_method.c_str());
}

void ArkWeb_ResourceRequest_::GetUrl(char** url) const {
  if (!cef_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr.";
    return;
  }

  std::string cef_url = cef_request->GetURL().ToString();
  const int length = cef_url.length();
  *url = new char[length + 1];
  strcpy((*url), cef_url.c_str());
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

  return cef_request->IsMainFrame();
}

bool ArkWeb_ResourceRequest_::HasUserGesture() const {
  if (!cef_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr.";
    return false;
  }

  return cef_request->HasUserGesture();
}

void ArkWeb_ResourceRequest_::GetReferrer(char** referrer) const {
  if (!cef_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr.";
    return;
  }

  std::string cef_referrer = cef_request->GetReferrerURL().ToString();
  const int length = cef_referrer.length();
  *referrer = new char[length + 1];
  strcpy((*referrer), cef_referrer.c_str());
}

int32_t ArkWeb_ResourceRequest_::GetRequestResourceType() const {
  if (!cef_request) {
    LOG(ERROR) << "scheme_handler resource request is nullptr.";
    return -1;
  }

  return cef_request->GetResourceType();
}

HeaderValue::HeaderValue(const std::string& key, const std::string& value)
    : key(key), value(value) {}

ArkWeb_RequestHeaderList_::ArkWeb_RequestHeaderList_(
    const ArkWeb_ResourceRequest* request) {
  CefRequest::HeaderMap headers;
  if (!request || !request->cef_request) {
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
  if (index >= header_value.size() || index < 0) {
    LOG(ERROR) << "scheme_handler get header index is invalid " << index;
    return;
  }

  std::string cef_key = header_value[index].key;
  int length = cef_key.length();
  *key = new char[length + 1];
  strcpy((*key), cef_key.c_str());

  std::string cef_value = header_value[index].value;
  length = cef_value.length();
  *value = new char[length + 1];
  strcpy((*value), cef_value.c_str());
}

int32_t ArkWeb_RequestHeaderList_::GetSize() const {
  return header_value.size();
}
