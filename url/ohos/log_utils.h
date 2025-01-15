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
    //Use ConvertUrl to anonymize URLs according to the rules.
    //1.Determine the conditions for anonymization based on the
    //value of the information flow parameter scenario_type.
    //2.For non-information flow pages, only print the scheme
    //and domain name followed by three "*".
    //3.For information flow pages, print the scheme, domain name,
    //and the query parameters to_app, channel, and channelid in
    //the URL, replacing all others with three "*".                        
    static std::string ConvertUrl(const std::string& url,
                                  int32_t scenario_type = 99,
                                  bool should_noise_url = true);
    static std::string ConvertUrlWithMask(const std::string& url);

  private:

    FRIEND_TEST_ALL_PREFIXES(LogUtilsTest, IsSupportScheme);
    FRIEND_TEST_ALL_PREFIXES(LogUtilsTest, ConvertUrl);

    static bool IsSupportScheme(const std::string& scheme);
    static bool IsSupportParam(const std::string& param);
};
}

#endif
