#ifndef OHOS_NWEB_EX_THIRD_PARTY_URL_LOG_UTILS_H_
#define OHOS_NWEB_EX_THIRD_PARTY_URL_LOG_UTILS_H_

#include <string>

#include "base/component_export.h"
#include "base/gtest_prod_util.h"
#include "url/third_party/mozilla/url_parse.h"

namespace url {
class COMPONENT_EXPORT(URL) LogUtils {
 public:
  static bool IsURLSlash(char ch) { return ch == '/' || ch == '\\'; }

  static bool IsSupportScheme(const std::string& url,
                              unsigned int urlLen,
                              unsigned int& colonIndex);
  static void FixupUrlPart(const std::string& text,
                           const url::Component& part,
                           std::string& converted);
  static void ConvertUrlHost(const std::string& url,
                             url::Parsed& parsed,
                             std::string& converted,
                             bool is_for_report = false);
  static void ConvertUrlPath(const std::string& url,
                             const url::Component& part,
                             std::string& converted);
  static void ConvertUrlQuery(const std::string& url,
                              const url::Component& part,
                              std::string& converted);
  static void ConvertUrlRef(const std::string& url,
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

 private:
  FRIEND_TEST_ALL_PREFIXES(LogUtilsTest, IsSupportScheme);
  FRIEND_TEST_ALL_PREFIXES(LogUtilsTest, ConvertUrl);

  static bool IsSupportScheme(const std::string& scheme);
};
}  // namespace url

#endif
