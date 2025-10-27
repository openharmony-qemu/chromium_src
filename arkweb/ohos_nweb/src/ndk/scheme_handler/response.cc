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

#include "ohos_nweb/src/ndk/scheme_handler/response.h"

#include "base/logging.h"
#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"
#include "securec.h"

ArkWeb_Response_::ArkWeb_Response_() {
  cef_response = CefResponse::Create();
}

void ArkWeb_Response_::SetUrl(const char* url) {
  if (!cef_response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  cef_response->SetURL(url);
}

void ArkWeb_Response_::GetUrl(char** url) const {
  if (!cef_response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  std::string cef_url = cef_response->GetURL().ToString();
  const uint32_t length = cef_url.length();
  *url = new char[length + 1];
  if (!(*url)) {
    LOG(ERROR) << "GetUrl url is nullptr.";
    return;
  }
  int ret = strcpy_s(*url, length + 1, cef_url.c_str());
  if (ret != EOK) {
    LOG(ERROR) << "GetUrl error, call strcpy_s ret = " << ret;
    delete[] *url;
    *url = nullptr;
  }
}

void ArkWeb_Response_::SetError(ArkWeb_NetError error_code) {
  if (!cef_response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  cef_response->SetError(static_cast<cef_errorcode_t>(error_code));
}

ArkWeb_NetError ArkWeb_Response_::GetError() const {
  if (!cef_response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return ARKWEB_NET_OK;
  }

  return static_cast<ArkWeb_NetError>(cef_response->GetError());
}

void ArkWeb_Response_::SetStatus(int32_t status) {
  if (!cef_response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  cef_response->SetStatus(status);
}

int32_t ArkWeb_Response_::GetStatus() const {
  if (!cef_response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return -1;
  }

  return cef_response->GetStatus();
}

void ArkWeb_Response_::SetStatusText(const char* status_text) {
  if (!cef_response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  cef_response->SetStatusText(status_text);
}

void ArkWeb_Response_::GetStatusText(char** status_text) const {
  if (!cef_response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  std::string cef_status_text = cef_response->GetStatusText().ToString();
  const uint32_t length = cef_status_text.length();
  *status_text = new char[length + 1];
  if (!(*status_text)) {
    LOG(ERROR) << "GetStatusText status_text is nullptr.";
    return;
  }
  int ret = strcpy_s(*status_text, length + 1, cef_status_text.c_str());
  if (ret != EOK) {
    LOG(ERROR) << "GetStatusText error, call strcpy_s ret = " << ret;
    delete[] *status_text;
    *status_text = nullptr;
  }
}

void ArkWeb_Response_::SetMimeType(const char* mime_type) {
  if (!cef_response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  cef_response->SetMimeType(mime_type);
}

void ArkWeb_Response_::GetMimeType(char** mime_type) const {
  if (!cef_response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  std::string cef_mime_type = cef_response->GetMimeType().ToString();
  const uint32_t length = cef_mime_type.length();
  *mime_type = new char[length + 1];
  if (!(*mime_type)) {
    LOG(ERROR) << "GetMimeType mime_type is nullptr.";
    return;
  }
  int ret = strcpy_s(*mime_type, length + 1, cef_mime_type.c_str());
  if (ret != EOK) {
    LOG(ERROR) << "GetMimeType error, call strcpy_s ret = " << ret;
    delete[] *mime_type;
    *mime_type = nullptr;
  }
}

void ArkWeb_Response_::SetCharset(const char* charset) {
  if (!cef_response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  cef_response->SetCharset(charset);
}

void ArkWeb_Response_::GetCharset(char** charset) const {
  if (!cef_response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  std::string cef_charset = cef_response->GetCharset().ToString();
  const uint32_t length = cef_charset.length();
  *charset = new char[length + 1];
  if (!(*charset)) {
    LOG(ERROR) << "GetCharset charset is nullptr.";
    return;
  }
  int ret = strcpy_s(*charset, length + 1, cef_charset.c_str());
  if (ret != EOK) {
    LOG(ERROR) << "GetCharset error, call strcpy_s ret = " << ret;
    delete[] *charset;
    *charset = nullptr;
  }
}

void ArkWeb_Response_::SetHeaderByName(const char* name,
                                       const char* value,
                                       bool overwrite) {
  if (!cef_response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  cef_response->SetHeaderByName(name, value, overwrite);
}

void ArkWeb_Response_::GetHeaderByName(const char* name, char** value) const {
  if (!cef_response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  std::string cef_value = cef_response->GetHeaderByName(name).ToString();
  const uint32_t length = cef_value.length();
  *value = new char[length + 1];
  if (!(*value)) {
    LOG(ERROR) << "GetHeaderByName value is nullptr.";
    return;
  }
  int ret = strcpy_s(*value, length + 1, cef_value.c_str());
  if (ret != EOK) {
    LOG(ERROR) << "GetHeaderByName error, call strcpy_s ret = " << ret;
    delete[] *value;
    *value = nullptr;
  }
}
