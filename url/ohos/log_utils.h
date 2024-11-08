#ifndef OHOS_NWEB_EX_THIRD_PARTY_URL_LOG_UTILS_H_
#define OHOS_NWEB_EX_THIRD_PARTY_URL_LOG_UTILS_H_

#include <string>
#include "base/component_export.h"
#include "base/gtest_prod_util.h"
#include "url/third_party/mozilla/url_parse.h"

namespace url {
class COMPONENT_EXPORT(URL) LogUtils {
  public:
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
    static std::string ConvertUrl(const std::string& url,
                                  bool should_noise_url = true);
    
  private:

    FRIEND_TEST_ALL_PREFIXES(LogUtilsTest, IsSupportScheme);
    FRIEND_TEST_ALL_PREFIXES(LogUtilsTest, ConvertUrl);

    static bool IsSupportScheme(const std::string& scheme);
};
}

#endif