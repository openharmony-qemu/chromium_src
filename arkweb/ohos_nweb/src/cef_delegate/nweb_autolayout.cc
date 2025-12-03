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

#include "hilog/log.h"
#include "base/files/file_util.h"
#include "base/json/json_writer.h"
#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "base/files/memory_mapped_file.h"
#include "libcef/browser/thread_util.h"
#include "base/trace_event/trace_event.h"
#include "arkweb/ohos_autolayout/grit/autolayout_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include <charconv>

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

  std::string ccmConfig = "";
  auto configPath = adapter.GetSystemPropertiesInstance().GetStringParameter(std::string(kConfigPath),"");
  base::FilePath ccmfile_path = base::FilePath(configPath);
  if (!base::ReadFileToString(ccmfile_path, &ccmConfig)) {
      LOG(WARNING) << "Failed to read Config.json from " << ccmfile_path.MaybeAsASCII();
      mEnable_ = false;
      return;
  }

  LOG(DEBUG) << "get ccmConfig:" << (ccmConfig == "" ? "failed":"successful");
  mJsonRoot = base::JSONReader::Read(ccmConfig);
  if (!mJsonRoot.has_value() || !Parse(mJsonRoot.value()) || mCCMConfig_.whitelist.empty()) {
    LOG(ERROR) << "Failed to parse CCM config. Disabling feature.";
    mEnable_ = false;
    return;
  }

  mAppBundleName_ = adapter.GetSystemPropertiesInstance().GetBundleName();
  auto app_it = mCCMConfig_.whitelist.find(mAppBundleName_);
  if (app_it == mCCMConfig_.whitelist.end()) {
    LOG(ERROR) << "checkCCM Error:app not in config. Disabling feature.";
    mEnable_ = false;
    return;
  }

  mWListEntry_ = &app_it->second;
  if (mWListEntry_ == nullptr) {
    LOG(ERROR) << "checkCCM Error:config info is null. Disabling feature.";
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
                       const RangeLimits& limits, int& output) {
  if (limits.min > limits.max) {
    LOG(ERROR) << "Parse Error: Invalid range for '" << key << "'.";
    return false;
  }

  auto value_opt = root_dict.FindInt(std::string(key));
  if (!value_opt) {
    LOG(ERROR) << "Parse Error: '" << key << "' value is missing.";
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

  int min_mask_area_ratio_threshold = 0;
  if (!ParseConfig(root_dict, kMinMaskAreaRatioThresholdKey, kMaskAreaThresholdRange,
                         min_mask_area_ratio_threshold)) {
    return false;
  }
  mCCMConfig_.min_mask_area_ratio_threshold = min_mask_area_ratio_threshold;

  int min_content_area_ratio_threshold = 0;
  if (!ParseConfig(root_dict, kMinContentAreaRatioThresholdKey, kContentAreaThresholdRange,
                         min_content_area_ratio_threshold)) {
    return false;
  }
  mCCMConfig_.min_content_area_ratio_threshold = min_content_area_ratio_threshold;
  
  int min_scale_factor = 0;
  if (!ParseConfig(root_dict, kMinDesScaleKey, kScaleFactorRange, min_scale_factor)) {
    return false;
  }
  mCCMConfig_.minScaleFactor = min_scale_factor;

  int scale_animation_duration = 0;
  if (!ParseConfig(root_dict, kScaleAnimationDurationKey, kScaleAnimationDurationRange,
                         scale_animation_duration)) {
    return false;
  }
  mCCMConfig_.scale_animation_duration = scale_animation_duration;

  if (!ParseOpacityFilter(root_dict, mCCMConfig_.opacity_filter)) {
    return false;
  }
  
  return true;
}

bool NwebAutolayout::ParseWhitelist(const base::Value::Dict& whitelist_dict) {
  mCCMConfig_.whitelist.clear();
  for (auto it = whitelist_dict.begin(); it != whitelist_dict.end(); ++it) {
    const std::string& appName =it->first;
    const base::Value& entry_value = it->second;
    if (!entry_value.is_dict()) {
      LOG(ERROR) << "Parse Error: Item in 'whitelist' is not a dictionary.";
      return false;
    }
    if (!ParseWhitelistEntry(appName, entry_value.GetDict())) {
      LOG(ERROR) << "ParseWhitelistEntry error, BundleName:"<< appName;
      return false;
    }
  }
  return true;
}

bool NwebAutolayout::ParseWhitelistEntry(std::string_view app_bundle_name_sv,
    const base::Value::Dict& whitelist_dict) {
  LOG(DEBUG) << "ParseWhitelistEntry for APP:" << app_bundle_name_sv;
  std::string json_str;
  if (base::JSONWriter::Write(whitelist_dict, &json_str)) {
    LOG(DEBUG) << "ParseWhitelistEntry whitelist Dict Content: " << json_str;
  } else {
    LOG(ERROR) << "Failed to convert Dict to JSON string.";
  }
  WhitelistEntry& current_entry = mCCMConfig_.whitelist[app_bundle_name_sv];
  const std::string* pattern_ptr = whitelist_dict.FindString(kPatternKey);
  const std::string* get_id_ptr = whitelist_dict.FindString(kGetIDKey);
  const std::string* get_page_ptr = whitelist_dict.FindString(kGetPageKey);
  if (!pattern_ptr || pattern_ptr->empty()) {
    LOG(ERROR) << "Parse Error: Missing, empty or invalid type for pattern.";
    return false;
  }
  if (!get_id_ptr || get_id_ptr->empty() ) {
    LOG(ERROR) << "Parse Error: Missing, empty or invalid type for getID.";
    return false;
  }
  if (!get_page_ptr || get_page_ptr->empty() ) {
    LOG(ERROR) << "Parse Error: Missing, empty or invalid type for getPage.";
    return false;
  }
  current_entry.pattern = std::string_view(*pattern_ptr);
  current_entry.getID = std::string_view(*get_id_ptr);
  current_entry.getPage = std::string_view(*get_page_ptr);

  const base::Value::List* app_rules_list =
      whitelist_dict.FindList(kAppRuleInfosKey);
  if (!app_rules_list) {
    LOG(ERROR) << "Parse Error: Missing, empty or invalid type for '"
               << kAppRuleInfosKey << "'.";
    return false;
  }
  current_entry.appRuleInfos = app_rules_list->Clone();
  return true;
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
    LOG(DEBUG) << " AutoLayout Feature disabled.";
    return;
  }
  if (browser != nullptr && frame != nullptr && frame->IsMain()) {
    LOG(DEBUG) << "start to check the web container.pattern:" << EscapeForJS_TemplateLiteral(mPatternJSSource_);
    CefRefPtr<JSResultCallbackImpl> JsResultCb = new JSResultCallbackImpl(frame);
    browser->GetHost()->ExecuteJavaScript(EscapeForJS_TemplateLiteral(mPatternJSSource_), JsResultCb, false);
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

}  // namespace OHOS::NWeb
