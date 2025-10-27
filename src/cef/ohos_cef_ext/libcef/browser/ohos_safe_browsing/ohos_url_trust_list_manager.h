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

#ifndef CEF_LIBCEF_BROWSER_OHOS_SAFE_BROWSING_OHOS_URL_TRUST_LIST_MANAGER_H_
#define CEF_LIBCEF_BROWSER_OHOS_SAFE_BROWSING_OHOS_URL_TRUST_LIST_MANAGER_H_

#include <map>
#include <string>

#include "base/json/json_value_converter.h"
#include "ohos_url_trust_list_interface.h"

namespace ohos_safe_browsing {
static bool ParseString(const base::Value* value, std::string* field) {
  if (!value || !field) {
    return false;
  }
  if (!value->is_string()) {
    *field = "";
  } else {
    *field = value->GetString();
  }
  return true;
}

static bool ParseInt(const base::Value* value, int* field) {
  if (!value || !field) {
    return false;
  }
  if (!value->is_int()) {
    *field = -1;
  } else {
    *field = value->GetInt();
  }
  return true;
}

class UrlTrustRule {
 public:
  std::string scheme;
  std::string host;
  int port = 0;
  std::string path;
  UrlTrustRule() = default;
  ~UrlTrustRule() = default;

  static void RegisterJSONConverter(
      base::JSONValueConverter<UrlTrustRule>* converter) {
    if (!converter) {
      return;
    }
    converter->RegisterCustomValueField<std::string>(
        "scheme", &UrlTrustRule::scheme, &ParseString);
    converter->RegisterCustomValueField<std::string>(
        "host", &UrlTrustRule::host, &ParseString);
    converter->RegisterCustomValueField<int>("port", &UrlTrustRule::port,
                                             &ParseInt);
    converter->RegisterCustomValueField<std::string>(
        "path", &UrlTrustRule::path, &ParseString);
  }
};

class UrlTrustListManager : public UrlTrustListInterface {
 public:
  UrlTrustListManager();
  virtual ~UrlTrustListManager() = default;
  UrlTrustCheckResult CheckUrlTrustList(const GURL& url) override;
  UrlListSetResult SetUrlTrustListWithErrMsg(const std::string& urlTrustList,
                                             std::string& detailErrMsg);

 private:
  std::multimap<std::string, UrlTrustRule> ruleMap_;
};
}  // namespace ohos_safe_browsing
#endif  //  CEF_LIBCEF_BROWSER_OHOS_SAFE_BROWSING_OHOS_URL_TRUST_LIST_MANAGER_H_
