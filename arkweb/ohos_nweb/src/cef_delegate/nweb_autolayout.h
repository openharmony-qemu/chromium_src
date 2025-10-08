#ifndef NWEB_AUTOLAYOUT_H
#define NWEB_AUTOLAYOUT_H

#include <stdint.h>
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

struct WhitelistEntry {
  std::string_view pattern;
  std::string_view getID;
  std::string_view getPage;
  std::string appRuleInfos;
};

struct ParsedCCMConfig {
  int min_mask_area_ratio_threshold;
  std::pair<int, int> opacity_filter;
  int min_content_area_ratio_threshold;
  int scale_animation_duration;
  int minScaleFactor;
  std::unordered_map<std::string_view, WhitelistEntry> whitelist;
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
constexpr int kMinScaleFactor = 55;
constexpr int kMaxScaleFactor = 100;


constexpr std::string_view kMinMaskAreaRatioThresholdKey = "minMaskAreaRatioThreshold";
constexpr std::string_view kOpacityFilterKey = "opacityFilter";
constexpr std::string_view kMinContentAreaRatioThresholdKey = "minContentAreaRatioThreshold";
constexpr std::string_view kScaleAnimationDurationKey = "scaleAnimationDuration";
constexpr std::string_view kWhitelistKey = "whitelist";
constexpr std::string_view kPatternKey = "pattern";
constexpr std::string_view kGetIDKey = "getID";
constexpr std::string_view kGetPageKey = "getPage";
constexpr std::string_view kAppRuleInfosKey = "appRuleInfos";
constexpr std::string_view kIdKey = "id";
constexpr std::string_view kPgKey = "pg";
constexpr std::string_view kWildcard = "*";
constexpr std::string_view kDesScaleKey = "minScaleFactor";
constexpr std::string_view kDesScale = "const.product.web.minScaleFactor";

constexpr std::string_view kCCMConfig = "/system/variant/phone/base/etc/web/MiniAppConfig.json";

constexpr std::string_view kAutoLayoutFileNameHap = "resources/rawfile/autolayout.js";

constexpr std::string_view kAutoLayoutBegin = "AutoLayout.Main.start(`";
constexpr std::string_view kAutoLayoutEnd = "`);";
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


class NwebAutolayout : public std::enable_shared_from_this<NwebAutolayout> {
 public: 
  NwebAutolayout(const NwebAutolayout&) = delete;
  NwebAutolayout& operator=(const NwebAutolayout&) = delete;
  ~NwebAutolayout() = default;

  static std::shared_ptr<NwebAutolayout> GetInstance();
  void CheckWebContainer(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame);
  void CheckCCMandApplyRule(CefRefPtr<CefFrame> frame);

protected:
  NwebAutolayout();

 private:
  void Initialize();
  bool Parse(const base::Value& root);
  bool ParseToplevelConfig(const base::Value::Dict& root_dict);
  bool ParseWhitelist(const base::Value::Dict& whitelist_list);
  bool ParseWhitelistEntry(std::string_view app_bundle_name_sv, const base::Value::Dict& whitelist_dict);
  void LoadAutoLayoutFromHap();
  std::optional<int> ParseInt(std::string_view input);

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
