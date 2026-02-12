/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARKWEB_NWEB_EX_THIRD_PARTY_URL_LOG_UTILS_H_
#define ARKWEB_NWEB_EX_THIRD_PARTY_URL_LOG_UTILS_H_

#include <string>

#include "base/component_export.h"
#include "base/gtest_prod_util.h"
#include "url/third_party/mozilla/url_parse.h"

namespace url {
class COMPONENT_EXPORT(URL) LogUtils {
 public:
  static bool IsURLSlash(char ch) { return ch == '/' || ch == '\\'; }

  static bool IsSupportScheme(const std::string_view url,
                              unsigned int urlLen,
                              unsigned int& colonIndex);
  static void FixupUrlPart(const std::string_view text,
                           const url::Component& part,
                           std::string& converted);
  static void ConvertUrlHost(const std::string_view url,
                             url::Parsed& parsed,
                             std::string& converted,
                             bool is_for_report = false);
  static void ConvertUrlPath(const std::string_view url,
                             const url::Component& part,
                             std::string& converted);
  static void ConvertUrlQuery(const std::string_view url,
                              const url::Component& part,
                              std::string& converted);
  static void ConvertUrlRef(const std::string_view url,
                            const url::Component& part,
                            std::string& converted);
  // Rules for URL anonymization using ConvertUrl.
  // The decision to anonymize is based on the value of the feed flow parameter `scenario_type`.
  // For non feed flow pages, only the scheme, domain, and three asterisks (***) will be printed.
  // For feed flow pages, the scheme, domain, and the following query parameters from the URL will be printed:
  // `to_app`, `channel`, and `channelid`. All other parts will be replaced with three asterisks (***).
  static std::string ConvertUrl(const std::string& url,
                                int32_t scenario_type = 99,
                                bool should_noise_url = true);
  static std::string ConvertUrlWithMask(const std::string& url);
  static std::string ConvertPathWithMask(const std::string& file_path);
  static std::string ConvertUrlParamWithMask(const std::string& url);

 private:
  FRIEND_TEST_ALL_PREFIXES(LogUtilsTest, IsSupportScheme);
  FRIEND_TEST_ALL_PREFIXES(LogUtilsTest, ConvertUrl);

  static bool IsSupportScheme(const std::string_view scheme);
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  static bool IsSupportParam(const std::string& param);
#endif
  static std::string MaskHost(const std::string_view host);
};
}  // namespace url

#endif
