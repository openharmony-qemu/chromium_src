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

#ifndef OHOS_NWEB_SRC_CAPI_SCHEME_HANDLER_RESPONSE_H_
#define OHOS_NWEB_SRC_CAPI_SCHEME_HANDLER_RESPONSE_H_

#include <map>
#include <string>

#include "cef/include/cef_response.h"
#include "ohos_nweb/src/capi/arkweb_net_error_list.h"

struct ArkWeb_Response_ {
  ArkWeb_Response_();
  ~ArkWeb_Response_() = default;

  void SetUrl(const char* url);
  void GetUrl(char** url) const;

  void SetError(ArkWeb_NetError error_code);
  ArkWeb_NetError GetError() const;

  void SetStatus(int32_t status);
  int32_t GetStatus() const;

  void SetStatusText(const char* status_text);
  void GetStatusText(char** status_text) const;

  void SetMimeType(const char* mime_type);
  void GetMimeType(char** mime_type) const;

  void SetCharset(const char* charset);
  void GetCharset(char** charset) const;

  void SetHeaderByName(const char* name, const char* value, bool overwrite);
  void GetHeaderByName(const char* name, char** value) const;

  CefRefPtr<CefResponse> cef_response;
};

#endif  // OHOS_NWEB_SRC_CAPI_SCHEME_HANDLER_RESPONSE_H_
