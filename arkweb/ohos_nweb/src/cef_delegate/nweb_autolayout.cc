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

#include "nweb_autolayout.h"

#include <charconv>

#include "base/files/file_util.h"
#include "base/files/memory_mapped_file.h"
#include "base/json/json_writer.h"
#include "base/trace_event/trace_event.h"
#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "arkweb/chromium_ext/url/ohos/log_utils.h"
#include "arkweb/ohos_autolayout/grit/autolayout_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include "libcef/browser/thread_util.h"
#include "hilog/log.h"

namespace OHOS::NWeb {
using namespace ConfigConstants;
std::shared_ptr<NwebAutolayout> NwebAutolayout::GetInstance() {
  static std::shared_ptr<NwebAutolayout> instance = std::make_shared<NwebAutolayout>();
  return instance;
}

NwebAutolayout::NwebAutolayout() {
  Initialize();
}

void NwebAutolayout::Initialize() {
  ScopedTimeLogger timer("Initialize");
  TRACE_EVENT("base", "NwebAutolayout::Initialize");
  auto &adapter = OhosAdapterHelper::GetInstance();
  mAppBundleName_ = adapter.GetSystemPropertiesInstance().GetBundleName();

  std::string ccmConfig = "";
  base::FilePath ccmfile_path = base::FilePath(kCCMConfigPath);
  if (!base::ReadFileToString(ccmfile_path, &ccmConfig)) {
      LOG(WARNING) << "Failed to read Config.json from " << ccmfile_path.MaybeAsASCII();
      mEnable_ = false;
      return;
  }

  LOG(DEBUG) << "get ccmConfig:" << (ccmConfig == "" ? "failed":"successful");
  mJsonRoot = base::JSONReader::Read(ccmConfig);
  if (!mJsonRoot.has_value() || !Parse(mJsonRoot.value())) {
    LOG(INFO) << "Failed to get app config. Disabling feature.";
    mEnable_ = false;
    return;
  }

  mPatternJSSource_ = std::string(mWListEntry_->pattern);
  LoadAutoLayoutFromHap();
  LOG(DEBUG) << "Initialize successfull";
}

static std::string EscapeForJS_TemplateLiteral(const std::string& s) {
    std::stringstream ss;
    for (char c : s) {
        switch (c) {
            case '`':  ss << "\\`";  break;
            case '\\': ss << "\\\\"; break;
            case '$':  ss << "\\$";  break;
            default:   ss << c;     break;
        }
    }
    return ss.str();
}


void NwebAutolayout::CheckCCMandApplyRule(CefRefPtr<CefFrame> frame) {
  ScopedTimeLogger timer("NwebAutolayout::CheckCCMandApplyRule");
  TRACE_EVENT("base", "NwebAutolayout::CheckCCMandApplyRule");
  if (!mEnable_ || mWListEntry_ == nullptr || frame == nullptr) {
    return;
  }
  frame->ExecuteJavaScript(std::string(mWListEntry_->getID), frame->GetURL(), 0);
  frame->ExecuteJavaScript(std::string(mWListEntry_->getPage), frame->GetURL(), 0);
  LOG(DEBUG) << "Add autolayout JavaScript...";
  frame->ExecuteJavaScript(mAutoLayoutJSSource_, frame->GetURL(), 0);

  base::Value::Dict root_dict;
  root_dict.Set(kMinMaskAreaRatioThresholdKey, mCCMConfig_.min_mask_area_ratio_threshold);
  root_dict.Set(kMinContentAreaRatioThresholdKey, mCCMConfig_.min_content_area_ratio_threshold);
  root_dict.Set(kScaleAnimationDurationKey, mCCMConfig_.scale_animation_duration);
  root_dict.Set(kMinDesScaleKey, mCCMConfig_.minScaleFactor);
  root_dict.Set(kNeedCheckIdAndPageKey, base::Value(true));
  std::optional<base::Value::List> list = mWListEntry_->appRuleInfos->Clone();
  root_dict.Set(kAppRuleInfosKey, std::move(*list));

  base::Value::List opacity_list;
  opacity_list.Append(mCCMConfig_.opacity_filter.first);
  opacity_list.Append(mCCMConfig_.opacity_filter.second);
  root_dict.Set(kOpacityFilterKey, base::Value(std::move(opacity_list)));

  base::Value root_value(std::move(root_dict));

  std::string json_string;
  bool success = base::JSONWriter::Write(root_value, &json_string);
  if(success) {
    std::string escaped_json = json_string;
    std::stringstream script;
    script << kAutoLayoutBegin << escaped_json << kAutoLayoutEnd;
    LOG(DEBUG) << "start autolayout JavaScript:"<< script.str();
    frame->ExecuteJavaScript(script.str(), frame->GetURL(), 0);
  }
}

std::string NwebAutolayout::CreateH5AutoLayoutParam(const UrlRuleInfoEntry& url_rule_info) {
  base::Value::Dict root_dict;
  root_dict.Set(kMinMaskAreaRatioThresholdKey, mCCMConfig_.min_mask_area_ratio_threshold);
  root_dict.Set(kMinContentAreaRatioThresholdKey, mCCMConfig_.min_content_area_ratio_threshold);
  root_dict.Set(kScaleAnimationDurationKey, mCCMConfig_.scale_animation_duration);
  root_dict.Set(kMinDesScaleKey, mCCMConfig_.minScaleFactor);
  root_dict.Set(kStrategyKey, url_rule_info.strategy);
  root_dict.Set(kAlphabetIdentificationMinSizeKey, url_rule_info.alphabetIdentificationMinSize);
  root_dict.Set(kAlphabetHeightWidthMinRatioKey, url_rule_info.alphabetHeightWidthMinRatio);
  root_dict.Set(kNeedCheckIdAndPageKey, base::Value(false));

  base::Value::List opacity_list;
  opacity_list.Append(mCCMConfig_.opacity_filter.first);
  opacity_list.Append(mCCMConfig_.opacity_filter.second);
  root_dict.Set(kOpacityFilterKey, base::Value(std::move(opacity_list)));

  base::Value root_value(std::move(root_dict));

  std::string json_string;
  bool success = base::JSONWriter::Write(root_value, &json_string);
  if(!success) {
    LOG(ERROR) << "CreateH5AutoLayoutParam failed, json write error.";
  }
  return json_string;
}

bool NwebAutolayout::Parse(const base::Value& root) {
  base::TimeTicks start_time = base::TimeTicks::Now();
  if (!root.is_dict()) {
    LOG(ERROR) << "Parse Error: Root is not a dictionary.";
    return false;
  }
  const base::Value::Dict& root_dict = root.GetDict();

  if (!ParseToplevelConfig(root_dict)) {
    return false;
  }

  const base::Value::Dict* whitelist_dict = root_dict.FindDict(kWhitelistKey);
  if (!whitelist_dict) {
    LOG(ERROR) << "Parse Error: Missing, empty or invalid type for '"
               << kWhitelistKey << "'.";
    return false;
  }
  return ParseWhitelist(*whitelist_dict);
}

bool ParseConfig(const base::Value::Dict& root_dict, std::string_view key,
                       const RangeLimits& limits, int& output, bool isRequired) {
  if (limits.min > limits.max) {
    LOG(ERROR) << "Parse Error: Invalid range for '" << key << "'.";
    return false;
  }

  auto value_opt = root_dict.FindInt(std::string(key));
  if (!value_opt) {
    if (isRequired) {
      LOG(ERROR) << "Parse Error: '" << key << "' value is missing.";
    }
    return false;
  }

  const int value = *value_opt;
  const bool below_min = limits.inclusive_min ? value < limits.min : value <= limits.min;
  const bool above_max = limits.inclusive_max ? value > limits.max : value >= limits.max;
  if (below_min || above_max) {
    LOG(ERROR) << "Parse Error: '" << key << "' value is invalid or out of range.";
    return false;
  }

  output = value;
  return true;
}

bool ParseOpacityFilter(const base::Value::Dict& root_dict, std::pair<int, int>& opacity_filter) {
  if (kMinOpacityFilter > kMaxOpacityFilter) {
    LOG(ERROR) << "Parse Error: Invalid range for '" << kOpacityFilterKey << "'.";
    return false;
  }

  const base::Value::List* opacity_list = root_dict.FindList(kOpacityFilterKey);
  if (!opacity_list || opacity_list->size() != 2 || !(*opacity_list)[0].is_int() ||
      !(*opacity_list)[1].is_int()) {
    LOG(ERROR) << "Parse Error: Missing, empty or invalid type for '" << kOpacityFilterKey << "'.";
    return false;
  }

  const int first_opacity = (*opacity_list)[0].GetInt();
  const int second_opacity = (*opacity_list)[1].GetInt();
  const bool in_range = first_opacity >= kMinOpacityFilter && first_opacity <= kMaxOpacityFilter &&
                        second_opacity >= kMinOpacityFilter && second_opacity <= kMaxOpacityFilter &&
                        first_opacity <= second_opacity;
  if (!in_range) {
    LOG(ERROR) << "Parse Error: '" << kOpacityFilterKey << "' value is invalid or out of range.";
    return false;
  }

  opacity_filter = {first_opacity, second_opacity};
  return true;
}

bool NwebAutolayout::ParseToplevelConfig(const base::Value::Dict& root_dict) {
  constexpr RangeLimits kMaskAreaThresholdRange{
    kMinMaskAreaRatioThreshold, kMaxMaskAreaRatioThreshold, true, true};
  constexpr RangeLimits kContentAreaThresholdRange{
    kMinContentAreaRatioThreshold, kMaxContentAreaRatioThreshold, false, false};
  constexpr RangeLimits kScaleFactorRange{
    kMinScaleFactor, kMaxScaleFactor, true, true};
  constexpr RangeLimits kScaleAnimationDurationRange{
    kMinScaleAnimationDuration, kMaxScaleAnimationDuration, false, false};
  constexpr RangeLimits kAlphabetIdentificationMinSizeRange{
    kMinAlphabetIdentificationMinSize, kMaxAlphabetIdentificationMinSize, false, true};
  constexpr RangeLimits kAlphabetHeightWidthMinRatioRange{
    kMinAlphabetHeightWidthMinRatio, kMaxAlphabetHeightWidthMinRatio, false, true};

  int min_mask_area_ratio_threshold = 0;
  if (!ParseConfig(root_dict, kMinMaskAreaRatioThresholdKey, kMaskAreaThresholdRange,
                         min_mask_area_ratio_threshold, true)) {
    return false;
  }
  mCCMConfig_.min_mask_area_ratio_threshold = min_mask_area_ratio_threshold;

  int min_content_area_ratio_threshold = 0;
  if (!ParseConfig(root_dict, kMinContentAreaRatioThresholdKey, kContentAreaThresholdRange,
                         min_content_area_ratio_threshold, true)) {
    return false;
  }
  mCCMConfig_.min_content_area_ratio_threshold = min_content_area_ratio_threshold;
  
  int min_scale_factor = 0;
  if (!ParseConfig(root_dict, kMinDesScaleKey, kScaleFactorRange, min_scale_factor, true)) {
    return false;
  }
  mCCMConfig_.minScaleFactor = min_scale_factor;

  int scale_animation_duration = 0;
  if (!ParseConfig(root_dict, kScaleAnimationDurationKey, kScaleAnimationDurationRange,
                         scale_animation_duration, true)) {
    return false;
  }
  mCCMConfig_.scale_animation_duration = scale_animation_duration;

  // alphabet_identification_min_size 和 alphabet_height_width_min_ratio 设置为非必填，兼容旧json格式
  int alphabet_identification_min_size = kInvalidValue;
  ParseConfig(root_dict, kAlphabetIdentificationMinSizeKey, kAlphabetIdentificationMinSizeRange,
                         alphabet_identification_min_size, false);
  mCCMConfig_.alphabet_identification_min_size = alphabet_identification_min_size;

  int alphabet_height_width_min_ratio = kInvalidValue;
  ParseConfig(root_dict, kAlphabetHeightWidthMinRatioKey, kAlphabetHeightWidthMinRatioRange,
                         alphabet_height_width_min_ratio, false);
  mCCMConfig_.alphabet_height_width_min_ratio = alphabet_height_width_min_ratio;

  return ParseOpacityFilter(root_dict, mCCMConfig_.opacity_filter);
}

bool NwebAutolayout::ParseWhitelist(const base::Value::Dict& whitelist_dict) {
  const base::Value::Dict* whitelist_rule = whitelist_dict.FindDict(mAppBundleName_);
  if (!whitelist_rule) {
    return false;
  }
  if (!ParseWhitelistEntry(*whitelist_rule)) {
    LOG(ERROR) << "ParseWhitelistEntry error, invalid data.";
    return false;
  }
  return true;
}

bool NwebAutolayout::ParseWhitelistEntry(const base::Value::Dict& whitelist_dict) {
  std::string json_str;
  if (base::JSONWriter::Write(whitelist_dict, &json_str)) {
    LOG(DEBUG) << "ParseWhitelistEntry whitelist Dict Content: " << json_str;
  } else {
    LOG(ERROR) << "Failed to convert Dict to JSON string.";
  }
  mWListEntry_ = &mCCMConfig_.whitelist;
  if (mCCMConfig_.alphabet_identification_min_size != kInvalidValue &&
      mCCMConfig_.alphabet_height_width_min_ratio != kInvalidValue) {
    mWListEntry_->urlRuleInfos = ParseUrlRuleInfo(whitelist_dict);
  }
  mWListEntry_->appRuleInfos = ParseAppRuleInfo(whitelist_dict, mCCMConfig_.whitelist);
  if (!mWListEntry_->urlRuleInfos.has_value() && !mWListEntry_->appRuleInfos.has_value()) {
    LOG(ERROR) << "Parse Error: Missing, empty or invalid type for urlRuleInfos and appRuleInfos.";
    return false;
  }
  return true;
}

std::optional<base::Value::List> NwebAutolayout::ParseAppRuleInfo(
    const base::Value::Dict& whitelist_dict, WhitelistEntry& current_entry) {
  const std::string* pattern_ptr = whitelist_dict.FindString(kPatternKey);
  const std::string* get_id_ptr = whitelist_dict.FindString(kGetIDKey);
  const std::string* get_page_ptr = whitelist_dict.FindString(kGetPageKey);
  if (!pattern_ptr || pattern_ptr->empty()) {
    LOG(ERROR) << "Parse Error: Missing, empty or invalid type for pattern.";
    return std::nullopt;
  }
  if (!get_id_ptr || get_id_ptr->empty() ) {
    LOG(ERROR) << "Parse Error: Missing, empty or invalid type for getID.";
    return std::nullopt;
  }
  if (!get_page_ptr || get_page_ptr->empty() ) {
    LOG(ERROR) << "Parse Error: Missing, empty or invalid type for getPage.";
    return std::nullopt;
  }
  current_entry.pattern = std::string_view(*pattern_ptr);
  current_entry.getID = std::string_view(*get_id_ptr);
  current_entry.getPage = std::string_view(*get_page_ptr);

  const base::Value::List* app_rules_list =
      whitelist_dict.FindList(kAppRuleInfosKey);
  if (!app_rules_list) {
    LOG(ERROR) << "Parse Error: Missing, empty or invalid type for '"
              << kAppRuleInfosKey << "'.";
    return std::nullopt;
  }

  return app_rules_list->Clone();
}

std::optional<std::vector<UrlRuleInfoEntry>> NwebAutolayout::ParseUrlRuleInfo(
    const base::Value::Dict& whitelist_dict) {
  const base::Value::List* url_rule_list = whitelist_dict.FindList(kUrlRuleInfosKey);
  if (!url_rule_list) {
    return std::nullopt;
  }
  std::vector<UrlRuleInfoEntry> url_rules;
  for (const base::Value& url_rule : *url_rule_list) {
    if (!url_rule.is_dict()) {
      continue;
    }

    const base::Value::Dict& url_rule_dict = url_rule.GetDict();
    const std::string* url_prefix = url_rule_dict.FindString(kUrlPrefixKey);
    std::optional<int> strategy_opt = url_rule_dict.FindInt(kStrategyKey);
    if (!url_prefix || url_prefix->empty() || !strategy_opt.has_value()) {
      LOG(WARNING) << "Parse Error: Missing, empty or invalid type for url_prefix or strategy.";
      continue;
    }
  
    UrlRuleInfoEntry url_rule_info_entry;
    std::unique_ptr<re2::RE2> pattern = std::make_unique<re2::RE2>(std::string(*url_prefix));
    if (!pattern->ok()) {
      LOG(WARNING) << "Skip url_rule_info_entry, invalid urlPrefix pattern.";
      continue;
    }
    url_rule_info_entry.urlPrefixPattern = std::move(pattern);
    url_rule_info_entry.strategy = strategy_opt.value();

    std::optional<int> min_size_opt = url_rule_dict.FindInt(kAlphabetIdentificationMinSizeKey);
    if (min_size_opt.has_value() && min_size_opt.value() > kMinAlphabetIdentificationMinSize &&
        min_size_opt.value() <= kMaxAlphabetIdentificationMinSize) {
      url_rule_info_entry.alphabetIdentificationMinSize = min_size_opt.value();
    } else {
      url_rule_info_entry.alphabetIdentificationMinSize = mCCMConfig_.alphabet_identification_min_size;
    }

    std::optional<int> min_ratio_opt = url_rule_dict.FindInt(kAlphabetHeightWidthMinRatioKey);
    if (min_ratio_opt.has_value() && min_ratio_opt.value() > kMinAlphabetHeightWidthMinRatio &&
        min_ratio_opt.value() <= kMaxAlphabetHeightWidthMinRatio) {
      url_rule_info_entry.alphabetHeightWidthMinRatio = min_ratio_opt.value();
    } else {
      url_rule_info_entry.alphabetHeightWidthMinRatio = mCCMConfig_.alphabet_height_width_min_ratio;
    }

    url_rules.push_back(std::move(url_rule_info_entry));
  }
  return url_rules;
}

class JSResultCallbackImpl : public CefJavaScriptResultCallback {
 public:
  JSResultCallbackImpl(CefRefPtr<CefFrame> frame)
      : frame_(frame){}

  virtual ~JSResultCallbackImpl() {}

  std::string ConvertCefValueToString(CefRefPtr<CefValue> src) {
      std::string dst;
      int type = src->GetType();
      switch (type) {
        case VTYPE_STRING: {
          dst = src->GetString();
          break;
        }
        default: {
          LOG(ERROR) << "OnMessage not support type";
          dst = std::string("");
          break;
        }
      }
      return dst;
  }

  NO_SANITIZE("cfi")
  void OnJavaScriptExeResult(CefRefPtr<CefValue> result) override {
    ScopedTimeLogger perf("OnJavaScriptExeResult");
    std::string data = ConvertCefValueToString(result);
    if (data == "" || data == "false") {
      LOG(DEBUG) << " Pattern result empty, Not target Web Container.";
      return;
    }
    LOG(DEBUG) << " Pattern result:" << data;
    if (data == "true") {
      NwebAutolayout::GetInstance()->CheckCCMandApplyRule(frame_);
    }
  }
  
 private:
  CefRefPtr<CefFrame> frame_;

  IMPLEMENT_REFCOUNTING(JSResultCallbackImpl);
};

void NwebAutolayout::CheckWebContainer(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) {
  ScopedTimeLogger timer("NwebAutolayout::CheckWebContainer");
  TRACE_EVENT("base", "NwebAutolayout::CheckWebContainer");
  if (!mEnable_) {
    LOG(DEBUG) << " AutoLayout Feature disabled. The app is not on the whitelist.";
    return;
  }

  if (browser == nullptr || frame == nullptr || !frame->IsMain()) {
    LOG(ERROR) << "browser or frame invalid.";
    return;
  }

  std::string current_url = frame->GetURL().ToString();
  LOG(DEBUG) << "NwebAutolayout::CheckWebContainer current_url:"
            << url::LogUtils::ConvertUrlWithMask(current_url);

  if (auto url_rule_entry = FindBestMatchRule(current_url)) {
    // H5匹配逻辑
    ApplyH5AutoLayoutStrategy(*url_rule_entry, frame);
  } else if (!mPatternJSSource_.empty()) {
    // 小程序逻辑
    LOG(DEBUG) << "start to check the web container.pattern:" << EscapeForJS_TemplateLiteral(mPatternJSSource_);
    CefRefPtr<JSResultCallbackImpl> JsResultCb = new JSResultCallbackImpl(frame);
    browser->GetHost()->ExecuteJavaScript(EscapeForJS_TemplateLiteral(mPatternJSSource_), JsResultCb, false);
  }
}

void NwebAutolayout::ApplyH5AutoLayoutStrategy(const UrlRuleInfoEntry& url_rule_entry,
                                               CefRefPtr<CefFrame> frame) {
  if (url_rule_entry.strategy == 0) {
    LOG(WARNING) << "AutoLayout quit as strategy is 0.";
    return;
  }
  LOG(DEBUG) << "Add autolayout JavaScript...";
  frame->ExecuteJavaScript(mAutoLayoutJSSource_, frame->GetURL(), 0);
  std::string param_str = CreateH5AutoLayoutParam(url_rule_entry);
  if (static_cast<uint32_t>(url_rule_entry.strategy) &
      static_cast<uint32_t>(AutoLayoutStrategyType::kPopupScale)) {
    std::stringstream script;
    script << kAutoLayoutBegin << param_str << kAutoLayoutEnd;
    LOG(INFO) << "start H5 PopupScale autolayout JavaScript: "<< script.str();
    frame->ExecuteJavaScript(script.str(), frame->GetURL(), 0);
  }
  if (static_cast<uint32_t>(url_rule_entry.strategy) &
      static_cast<uint32_t>(AutoLayoutStrategyType::kAlphabetNavigator)) {
    std::stringstream script;
    script << kAlphabetAutoLayoutBegin << param_str << kAutoLayoutEnd;
    LOG(INFO) << "start H5 AlphabetNavigator autolayout JavaScript: "<< script.str();
    frame->ExecuteJavaScript(script.str(), frame->GetURL(), 0);
  }
}

void NwebAutolayout::LoadAutoLayoutFromHap() {
  ScopedTimeLogger timer("NwebAutolayout::LoadAutoLayoutFromHap");
  TRACE_EVENT("base", "NwebAutolayout::LoadAutoLayoutFromHap");
  const auto& rb = ui::ResourceBundle::GetSharedInstance();
  std::string_view script_data = rb.GetRawDataResource(IDR_AUTOLAYOUT_JS);

  LOG(DEBUG) << "Read Autolayout JS script_data size:" << script_data.size();
  if(script_data.empty()) {
    LOG(ERROR) << "Failed to read Autolayout js failed. Disabling feature.";
    mEnable_ = false;
    return;
  }
  mAutoLayoutJSSource_.assign(script_data.data(), script_data.size());
}

const UrlRuleInfoEntry* NwebAutolayout::FindBestMatchRule(const std::string& current_url) const {
  if (!mWListEntry_ || !mWListEntry_->urlRuleInfos.has_value()) {
    return nullptr;
  }
  ScopedTimeLogger timer("NwebAutolayout::FindBestMatchRule");
  const UrlRuleInfoEntry* match_rule = nullptr;
  size_t best_match_length = 0;
  re2::StringPiece input(current_url);
  for (const UrlRuleInfoEntry& url_rule_entry : mWListEntry_->urlRuleInfos.value()) {
    // 检查URL是否以当前前缀开头
    auto pattern_ptr = url_rule_entry.urlPrefixPattern.get();
    if (!pattern_ptr) {
      continue;
    }
    re2::StringPiece groups[1];
    if (pattern_ptr->Match(input, 0, input.size(), RE2::ANCHOR_START, groups, 1)) {
      size_t current_match_length = groups[0].size();
      if (current_match_length > best_match_length) {
        best_match_length = current_match_length;
        match_rule = &url_rule_entry;
      }
    }
  }
  return match_rule;
}

}  // namespace OHOS::NWeb
