#include "log_utils.h"

#include <memory>
#include "build/build_config.h"
#include "base/strings/string_util.h"
#include "url/url_constants.h"

namespace url {
const char kReplaceStr[] = "***";
const char kSchemeSeparator[] = "//";
const char kSchemeSeparatorForReport[] = "://";
const char kSchemeNotParseOne[] = "about:blank";
const char kSchemeNotParseTwo[] = "hwweb://newtab";
int32_t FEEDSPAGE_TYPE = 5;
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

// static
void LogUtils::ConvertUrlPath(const std::string& url,
                              const url::Component& part,
                              std::string& converted) 
{
  if (!part.is_valid())
    return;

  int scanIndex = part.begin;
  if (IsURLSlash(url[scanIndex])) {
    converted.append("/");
  }
  converted.append(kReplaceStr);
  return;
}

// static
bool LogUtils::IsSupportParam(const std::string& param) {
  if (base::EqualsCaseInsensitiveASCII(param, url::kToAppParam) ||
      base::EqualsCaseInsensitiveASCII(param, url::kChannelParam) ||
      base::EqualsCaseInsensitiveASCII(param, url::kChannelidParam)) {
    return true;
  }
  return false;
}

// static
void LogUtils::ConvertUrlQuery(const std::string& url,
                               const url::Component& part,
                               std::string& converted) 
{
  if (!part.is_valid())
    return;

  converted.append("?");

  int i = 0;
  int appendStart = part.begin;
  int appendLast = part.begin;
  bool saveFlag = true;
  bool replaceFlag = false;
  for (i = part.begin; i < part.end(); i++) {
    std::string temp_str = "";
    if (url[i] == '=') {
      temp_str.append(url, appendStart, i - appendStart);
      if (IsSupportParam(temp_str)) {
        converted.append(url, appendStart, i - appendStart + 1);
        appendLast = i + 1;
        appendStart = i + 1;
        saveFlag = true;
        replaceFlag = false;
      } else {
        converted.append(kReplaceStr);
        saveFlag = false;
        replaceFlag = false;
        appendStart = part.end();
      }
      continue;
    }

    if (url[i] == '&') {
      if (saveFlag) {
        converted.append(url, appendLast, i - appendLast);
        saveFlag = false;
        replaceFlag = true;
      }
      converted.append("&");
      appendStart = i + 1;
    }
  }
  if (replaceFlag) {
    converted.append(kReplaceStr);
  } else if (appendStart <= part.end()) {
    converted.append(url, appendStart, part.end() - appendStart + 1);
  }
}

// static
void LogUtils::ConvertUrlRef(const std::string& url,
                             const url::Component& part,
                             std::string& converted) {
  if (!part.is_valid())
    return;

  converted.append("#");
  converted.append(kReplaceStr);
}

// staic
std::string LogUtils::ConvertUrl(const std::string& url,
                                 int32_t scenario_type,
                                 bool should_noise_url) 
{
  if (!should_noise_url)
    return url;

  if (url.length() == 0) {
    return url;
  }

  if (url.find(kSchemeNotParseOne) == 0 || url.find(kSchemeNotParseTwo) == 0) {
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
  if (scenario_type == FEEDSPAGE_TYPE) {
    ConvertUrlPath(url, parsed.path, converted);
    ConvertUrlQuery(url, parsed.query, converted);
    ConvertUrlRef(url, parsed.ref, converted);
  } else {
    converted.append("/***");
  }
  return converted;
}

//static
std::string LogUtils::ConvertUrlWithMask(const std::string& url) 
{
  if (url.find(kSchemeNotParseOne) == 0 || url.find(kSchemeNotParseTwo) == 0) {
    return url;
  }

  unsigned int colonIndex = 0;
  std::string converted;

  if (!IsSupportScheme(url, url.length(), colonIndex)) {
    if (colonIndex < url.length() - 1)
      return url.substr(0, colonIndex);
    else
      return kReplaceStr;
  }
  
  converted.append(url.substr(0, colonIndex));
  converted.append("//***");
  return converted;
}

}
