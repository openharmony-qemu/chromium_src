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
#ifndef NWEB_AUTOLAYOUT_H
#define NWEB_AUTOLAYOUT_H

#include <cstdint>
#include <vector>
#include <optional>
#include <map>
#include <variant>
#include <algorithm>
#include <unordered_set> 
#include <unordered_map>
#include <utility>
#include <memory>
#include <mutex>
#include <functional>
#include "cef/include/cef_browser.h"
#include "nweb_delegate.h"
#include "web/arkweb_type.h"
#include "base/json/json_value_converter.h"
#include "base/json/json_reader.h"
#include "base/values.h"
#include "base/time/time.h"

namespace OHOS::NWeb {
enum class AutoLayoutStrategyType : uint32_t {
  kPopupScale = 1,
  kAlphabetNavigator = 1 << 1
};

struct UrlRuleInfoEntry {
  std::string urlPrefix;
  int strategy;
  int alphabetIdentificationMinSize;
  int alphabetHeightWidthMinRatio;
};

struct WhitelistEntry {
  std::string_view pattern;
  std::string_view getID;
  std::string_view getPage;
  std::optional<base::Value::List> appRuleInfos;
  std::optional<std::vector<UrlRuleInfoEntry>> urlRuleInfos;
};

struct ParsedCCMConfig {
  int min_mask_area_ratio_threshold;
  std::pair<int, int> opacity_filter;
  int min_content_area_ratio_threshold;
  int scale_animation_duration;
  int minScaleFactor;
  int alphabet_identification_min_size;
  int alphabet_height_width_min_ratio;
  std::unordered_map<std::string_view, WhitelistEntry> whitelist;
};

struct RangeLimits {
  int min;
  int max;
  bool inclusive_min;
  bool inclusive_max;
};

namespace ConfigConstants {
constexpr int kMinMaskAreaRatioThreshold = 50;
constexpr int kMaxMaskAreaRatioThreshold = 100;
constexpr int kMinOpacityFilter = 0;
constexpr int kMaxOpacityFilter = 100;
constexpr int kMinContentAreaRatioThreshold = 10;
constexpr int kMaxContentAreaRatioThreshold = 100;
constexpr int kMinScaleAnimationDuration = 50;
constexpr int kMaxScaleAnimationDuration = 400;
constexpr int kMinScaleFactor = 30;
constexpr int kMaxScaleFactor = 100;
constexpr int kMinAlphabetIdentificationMinSize = 0;
constexpr int kMaxAlphabetIdentificationMinSize = 26;
constexpr int kMinAlphabetHeightWidthMinRatio = 0;
constexpr int kMaxAlphabetHeightWidthMinRatio = 30;

constexpr std::string_view kCCMConfigPath = "/sys_prod/etc/web/WebAutoLayoutConfig.json";
constexpr std::string_view kMinMaskAreaRatioThresholdKey = "minMaskAreaRatioThreshold";
constexpr std::string_view kOpacityFilterKey = "opacityFilter";
constexpr std::string_view kMinContentAreaRatioThresholdKey = "minContentAreaRatioThreshold";
constexpr std::string_view kScaleAnimationDurationKey = "scaleAnimationDuration";
constexpr std::string_view kWhitelistKey = "whitelist";
constexpr std::string_view kPatternKey = "pattern";
constexpr std::string_view kGetIDKey = "GetID";
constexpr std::string_view kGetPageKey = "GetPage";
constexpr std::string_view kAppRuleInfosKey = "appRuleInfos";
constexpr std::string_view kIdKey = "id";
constexpr std::string_view kPgKey = "pg";
constexpr std::string_view kWildcard = "*";
constexpr std::string_view kMinDesScaleKey = "minScaleFactor";
constexpr std::string_view kConfigPath = "const.product.web.alconfig";

constexpr std::string_view kAutoLayoutBegin = "AutoLayout.Main.start(`";
constexpr std::string_view kAutoLayoutEnd = "`);";

constexpr std::string_view kUrlRuleInfosKey = "urlRuleInfos";
constexpr std::string_view kUrlPrefixKey = "urlPrefix";
constexpr std::string_view kStrategyKey = "strategy";
constexpr std::string_view kAlphabetIdentificationMinSizeKey = "alphabetIdentificationMinSize";
constexpr std::string_view kAlphabetHeightWidthMinRatioKey = "alphabetHeightWidthMinRatio";
constexpr std::string_view kNeedCheckIdAndPageKey = "needCheckIdAndPage";
}

class ScopedTimeLogger {
public:
    explicit ScopedTimeLogger(const std::string& name)
        : name_(name), start_time_(base::TimeTicks::Now()) {}

    ~ScopedTimeLogger() {
        base::TimeDelta elapsed_time = base::TimeTicks::Now() - start_time_;
        LOG(DEBUG) << "[" << name_ << "] took "
                  << elapsed_time.InMillisecondsF() << " ms.";
    }

private:
    std::string name_;
    base::TimeTicks start_time_;
};


class NwebAutolayout {
 public: 
  NwebAutolayout(const NwebAutolayout&) = delete;
  NwebAutolayout& operator=(const NwebAutolayout&) = delete;
  ~NwebAutolayout() = default;

  static std::shared_ptr<NwebAutolayout> GetInstance();
  void CheckWebContainer(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame);
  void CheckCCMandApplyRule(CefRefPtr<CefFrame> frame);

  NwebAutolayout();

 protected:
  void Initialize();
  bool Parse(const base::Value& root);
  bool ParseToplevelConfig(const base::Value::Dict& root_dict);
  bool ParseWhitelist(const base::Value::Dict& whitelist_list);
  bool ParseWhitelistEntry(std::string_view app_bundle_name_sv, const base::Value::Dict& whitelist_dict);
  std::optional<base::Value::List> ParseAppRuleInfo(
      const base::Value::Dict& whitelist_dict, WhitelistEntry& current_entry);
  std::optional<std::vector<UrlRuleInfoEntry>> ParseUrlRuleInfo(const base::Value::Dict& whitelist_dict);
  void LoadAutoLayoutFromHap();
  std::optional<int> ParseInt(std::string_view input);

  std::string CreateH5AutoLayoutParam(const UrlRuleInfoEntry& urlRuleInfo);
  void ApplyH5AutoLayoutStrategy(const UrlRuleInfoEntry& url_rule_entry, CefRefPtr<CefFrame> frame);
  const UrlRuleInfoEntry* FindBestMatchRule(const std::string& current_url) const;

  ParsedCCMConfig mCCMConfig_;
  std::string mAppBundleName_;
  WhitelistEntry* mWListEntry_;
  bool mEnable_ = true;
  std::string mAutoLayoutJSSource_;
  std::string mPatternJSSource_;

  std::optional<base::Value> mJsonRoot;
};

}  // namespace OHOS::NWeb
#endif
