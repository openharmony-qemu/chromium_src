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
#include "ohos_url_trust_list_manager.h"

#include "base/json/json_reader.h"
#include "base/values.h"
#include "content/public/browser/web_contents.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "arkweb/chromium_ext/url/ohos/log_utils.h"

namespace ohos_safe_browsing {
static constexpr int MAX_PATH_SIZE = 0x10000;

char UrlTrustListInterface::interfaceKey;

UrlTrustListManager::UrlTrustListManager() {}

static bool FormatUrlRule(UrlTrustRule& urlRule, std::string& err) {
  std::string scheme = urlRule.scheme.empty() ? "http" : urlRule.scheme;
  std::string path = urlRule.path.empty() ? "" : urlRule.path;
  std::string port = urlRule.port > 0 ? ":" + std::to_string(urlRule.port) : "";

  std::string combine = scheme + "://" + urlRule.host + port + "/" + path;
  GURL gurlRule(combine);
  if (!gurlRule.is_valid()) {
    LOG(ERROR) << "parse: url format is invalid, combine url is "
               << url::LogUtils::ConvertUrlWithMask(combine);
    err = "url format is invalid, combine url is " + combine;
    return false;
  }
  LOG(DEBUG) << "parse: combine url is " << url::LogUtils::ConvertUrlWithMask(combine);
  if (gurlRule.host().empty()) {
    LOG(ERROR) << "parse: format url host is invalid.";
    err = "format url host is invalid";
    return false;
  }
  urlRule.host = gurlRule.host();
  if (!path.empty()) {
    if (gurlRule.path().empty()) {
      LOG(ERROR) << "parse: format url path is invalid";
      err = "format url path is invalid";
      return false;
    }
    urlRule.path = gurlRule.path();
  }
  return true;
}

static bool CheckUrlRuleValid(UrlTrustRule& urlRule, std::string& err) {
  if (!urlRule.scheme.empty()) {
    if (urlRule.scheme != "http" && urlRule.scheme != "https") {
      LOG(ERROR) << "parse: host "
                 << url::LogUtils::ConvertUrlWithMask(urlRule.host)<< " scheme is invalid.";
      err = "host " + urlRule.host + " scheme is invalid";
      return false;
    }
  }

  if (urlRule.host.empty()) {
    LOG(ERROR) << "parse: empty host.";
    err = "empty host";
    return false;
  }
  if (urlRule.port <= -1 || urlRule.port >= 65536) {
    LOG(ERROR) << "parse: host "
               << url::LogUtils::ConvertUrlWithMask(urlRule.host) << " port is invalid";
    err = "host " + urlRule.host + " port is invalid";
    return false;
  }
  if (urlRule.path.size() > MAX_PATH_SIZE) {
    LOG(ERROR) << "parse: host "
               << url::LogUtils::ConvertUrlWithMask(urlRule.host) << " path len too long.";
    err = "host " + urlRule.host + " path len too long";
    return false;
  }
  if (!FormatUrlRule(urlRule, err)) {
    return false;
  }
  LOG(DEBUG) << "parse: url host "
             << url::LogUtils::ConvertUrlWithMask(urlRule.host) << " path " << urlRule.path;
  return true;
}

UrlListSetResult UrlTrustListManager::SetUrlTrustListWithErrMsg(
    const std::string& urlTrustList,
    std::string& detailErrMsg) {
  if (urlTrustList.empty()) {
    LOG(INFO) << "parse: list is empty, disable url trust list.";
    ruleMap_.clear();
    return UrlListSetResult::SET_OK;
  }
  absl::optional<base::Value> jsonParsed = base::JSONReader::Read(urlTrustList);
  if (!jsonParsed || !jsonParsed->is_dict()) {
    LOG(ERROR) << "parse: json format failed.";
    detailErrMsg = "json format failed";
    return UrlListSetResult::PARAM_ERROR;
  }
  base::Value::List* list = jsonParsed->GetDict().FindList("UrlPermissionList");
  if (!list) {
    LOG(ERROR) << "parse: can not find UrlPermissionList.";
    detailErrMsg = "can not find UrlPermissionList";
    return UrlListSetResult::PARAM_ERROR;
  }

  if (list->size() == 0) {
    LOG(INFO) << "parse: list is empty, disable url trust list.";
    ruleMap_.clear();
    return UrlListSetResult::SET_OK;
  }

  int32_t ruleId = 1;
  std::string ruleErr;
  std::multimap<std::string, UrlTrustRule> map;
  for (const auto& ruleJson : *list) {
    UrlTrustRule rule;
    base::JSONValueConverter<UrlTrustRule> converter;
    converter.Convert(ruleJson, &rule);
    if (!CheckUrlRuleValid(rule, ruleErr)) {
      detailErrMsg =
          "rule " + std::to_string(ruleId) + " check error, " + ruleErr;
      return UrlListSetResult::PARAM_ERROR;
    }

    map.insert(std::make_pair(rule.host, rule));
    ruleId++;
  }
  ruleMap_ = map;
  return UrlListSetResult::SET_OK;
}

UrlTrustCheckResult UrlTrustListManager::CheckUrlTrustList(const GURL& url) {
  if (ruleMap_.size() == 0) {
    return UrlTrustCheckResult::RESULT_ALLOW;
  }
  if (!url.SchemeIsHTTPOrHTTPS()) {
    return UrlTrustCheckResult::RESULT_ALLOW;
  }

  auto range = ruleMap_.equal_range(url.host());
  const std::string& path = url.path();
  for (auto itr = range.first; itr != range.second; ++itr) {
    auto& rule = itr->second;
    if (!rule.scheme.empty() && (rule.scheme != url.scheme())) {
      continue;
    }
    if (rule.port > 0 && (rule.port != url.EffectiveIntPort())) {
      continue;
    }
    if (!rule.path.empty()) {
      if (path.find(rule.path) != 0) {
        continue;
      }
      size_t next = rule.path.size();
      if (next < path.size() && !rule.path.ends_with('/') &&
          path[next] != '/') {
        continue;
      }
    }
    return UrlTrustCheckResult::RESULT_ALLOW;
  }
  LOG(ERROR) << "Deny url.";
  LOG(DEBUG) << "Url detail: scheme:" << url.scheme() << ",host:" << url.host()
             << ",port:" << url.EffectiveIntPort() << ",path:" << url.path();
  return UrlTrustCheckResult::RESULT_DENY;
}
}  // namespace ohos_safe_browsing
