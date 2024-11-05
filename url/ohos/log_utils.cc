#include "log_utils.h"

#include <memory>
#include "build/build_config.h"
#include "base/strings/string_util.h"
#include "url/url_constants.h"

namespace url {
const char kReplaceStr[] = "***";
const char kSchemeSeparator[] = "//";
const char kSchemeSeparatorForReport[] = "://";
// static
bool LogUtils::IsSupportScheme(const std::string& url,
                               unsigned int urlLen,
                               unsigned int& colonIndex) {
  while (colonIndex < urlLen && url[colonIndex] != ':') {
    colonIndex++;
  }

  if (colonIndex == urlLen) {
    return false;
  }

  return IsSupportScheme(url.substr(0, colonIndex));
}

// static
bool LogUtils::IsSupportScheme(const std::string& scheme) {
  if (base::EqualsCaseInsensitiveASCII(scheme, url::kHttpScheme) ||
      base::EqualsCaseInsensitiveASCII(scheme, url::kHttpsScheme)) {
    return true;
  }
  return false;
}

// static
void LogUtils::FixupUrlPart(const std::string& text,
                            const url::Component& part,
                            std::string& converted) {
  if (!part.is_valid())
    return;

  converted.append(text, part.begin, part.len);
}

// static
void LogUtils::ConvertUrlHost(const std::string& url,
                              url::Parsed& parsed,
                              std::string& converted,
                              bool is_for_report) {
  FixupUrlPart(url, parsed.scheme, converted);
  if (!is_for_report) {
    converted.append(url::kSchemeSeparator);
  } else {
    converted.append(url::kSchemeSeparatorForReport);
  }

  if (parsed.username.is_valid()) {
    converted.append(kReplaceStr);
    if (parsed.password.is_valid()) {
      converted.append(":");
      converted.append(kReplaceStr);
    }
    converted.append("@");
  }

  FixupUrlPart(url, parsed.host, converted);

  if (parsed.port.is_valid()) {
    converted.append(":");
    FixupUrlPart(url, parsed.port, converted);
  }
  converted.append("/***");
  return;
}

// staic
std::string LogUtils::ConvertUrl(const std::string& url,
                                 bool should_noise_url) {
  if (!should_noise_url)
    return url;

  if (url.length() == 0) {
    return url;
  }

  unsigned int colonIndex = 0;
  std::string converted;

  if (!IsSupportScheme(url, url.length(), colonIndex)) {
    if (colonIndex < url.length() - 1)
      return url.substr(0, colonIndex + 1);
    else
      return url;
  }

  url::Parsed parsed;
  url::ParseStandardURL(url.data(), url.length(), &parsed);

  ConvertUrlHost(url, parsed, converted, true);

  return converted;
}

}