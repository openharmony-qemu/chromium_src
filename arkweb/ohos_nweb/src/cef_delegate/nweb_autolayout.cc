#include "nweb_autolayout.h"

#include "hilog/log.h"
#include "base/files/file_util.h"
#include "base/json/json_writer.h"
#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "base/files/memory_mapped_file.h"
#include "libcef/browser/thread_util.h"
#include "base/trace_event/trace_event.h"

namespace OHOS::NWeb {
using namespace ConfigConstants;
std::shared_ptr<NwebAutolayout> NwebAutolayout::GetInstance() {
  struct NWebALMaker : public NwebAutolayout {};
  static std::once_flag once_flag;
  LOG(INFO) << "GetInstance.";
  static std::shared_ptr<NwebAutolayout> instance;
  std::call_once(once_flag, [] { instance = std::make_shared<NWebALMaker>(); });
  return instance;
}

NwebAutolayout::NwebAutolayout() {
  Initialize();
}

void NwebAutolayout::Initialize() {
  ScopedTimeLogger timer("Initialize");
  TRACE_EVENT("base", "NwebAutolayout::Initialize");
  LOG(DEBUG) << "begin to Initialize.";
  auto sv = OhosAdapterHelper::GetInstance().
                                GetSystemPropertiesInstance().
                                GetStringParameter(std::string(kDesScale), "0");
  LOG(DEBUG) << "get kDesScale" << sv;
  auto result = ParseInt(sv);
  if (result && result.value() > 0) {
      mCCMConfig_.minScaleFactor = result.value();
  } else {
      mCCMConfig_.minScaleFactor = 0;
      LOG(ERROR) << "Failed to parse desScale config. Disabling feature.";
      mEnable_ = false;
      return;
  }
  LOG(DEBUG) << "get minScaleFactor:" << mCCMConfig_.minScaleFactor;
  
  mAppBundleName_ = OhosAdapterHelper::GetInstance()
                        .GetSystemPropertiesInstance().GetBundleName();
  if (!mAppBundleName_.length()) {
    LOG(ERROR) << "Failed to get App bundle name. Disabling feature.";
    mEnable_ = false;
    return;
  }
  LOG(DEBUG) << "get mAppBundleName_:" << mAppBundleName_;

  std::string ccmConfig = "";
  base::FilePath ccmfile_path = base::FilePath(std::string(kCCMConfig));
  if (!base::ReadFileToString(ccmfile_path, &ccmConfig)) {
      LOG(WARNING) << "Failed to read MiniAppConfig.json from "
                   << ccmfile_path.MaybeAsASCII();
      mEnable_ = false;
      return;
  }
  LOG(DEBUG) << "get ccmConfig:" << (ccmConfig == ""?"failed":"successful");
  mJsonRoot = base::JSONReader::Read(ccmConfig);
  if (!mJsonRoot.has_value() || !Parse(mJsonRoot.value())) {
    LOG(ERROR) << "Failed to parse CCM config. Disabling feature.";
    mEnable_ = false;
    return;
  }

  if (mCCMConfig_.whitelist.empty()) {
    LOG(INFO) << "checkCCM Error: Whitelist is empty. Disabling feature.";
    mEnable_ = false;
    return;
  }

  auto app_it = mCCMConfig_.whitelist.find(mAppBundleName_);
  if (app_it == mCCMConfig_.whitelist.end()) {
    LOG(ERROR) << "checkCCM Error: Current appBundleName '" << mAppBundleName_
               << "' not in whitelist. Disabling feature.";
    mEnable_ = false;
    return;
  }

  mWListEntry_ = &app_it->second;
  if (mWListEntry_ == nullptr) {
    LOG(ERROR) << "checkCCM Error: Current whitelist is null. Disabling feature.";
    mEnable_ = false;
    return;
  }
  if (mWListEntry_->pattern == "" || mWListEntry_->getID == "" || mWListEntry_->getPage == "") {
    LOG(ERROR) << "checkCCM Error: Current pattern config is null. Disabling feature.";
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
  if (!mEnable_ || mWListEntry_ == nullptr) {
    return;
  }
  LOG(DEBUG) << "register get enviroment param function JavaScript...";
  frame->ExecuteJavaScript(std::string(mWListEntry_->getID), frame->GetURL(), 0);
  frame->ExecuteJavaScript(std::string(mWListEntry_->getPage), frame->GetURL(), 0);
  LOG(DEBUG) << "Add autolayout JavaScript...";
  frame->ExecuteJavaScript(mAutoLayoutJSSource_, frame->GetURL(), 0);

  base::Value::Dict root_dict;
  root_dict.Set(kMinMaskAreaRatioThresholdKey, mCCMConfig_.min_mask_area_ratio_threshold);
  root_dict.Set(kMinContentAreaRatioThresholdKey, mCCMConfig_.min_content_area_ratio_threshold);
  root_dict.Set(kScaleAnimationDurationKey, mCCMConfig_.scale_animation_duration);
  root_dict.Set(kDesScaleKey, mCCMConfig_.minScaleFactor);
  root_dict.Set(kAppRuleInfosKey, mWListEntry_->appRuleInfos);

  base::Value::List opacity_list;
  opacity_list.Append(mCCMConfig_.opacity_filter.first);
  opacity_list.Append(mCCMConfig_.opacity_filter.second);
  root_dict.Set(kOpacityFilterKey, base::Value(std::move(opacity_list)));

  base::Value root_value(std::move(root_dict));

  std::string json_string;
  bool success = base::JSONWriter::Write(root_value, &json_string);
  if(success) {
    std::string escaped_json = EscapeForJS_TemplateLiteral(json_string);
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

bool NwebAutolayout::ParseToplevelConfig(const base::Value::Dict& root_dict) {
  auto min_mask_opt =
      root_dict.FindInt(std::string(kMinMaskAreaRatioThresholdKey));
  if (!min_mask_opt || *min_mask_opt < kMinMaskAreaRatioThreshold ||
      *min_mask_opt > kMaxMaskAreaRatioThreshold) {
    LOG(ERROR) << "Parse Error: '" << kMinMaskAreaRatioThresholdKey
               << "' value is missing, invalid or out of range.";
    return false;
  }
  mCCMConfig_.min_mask_area_ratio_threshold = *min_mask_opt;

  auto min_content_opt =
      root_dict.FindInt(std::string(kMinContentAreaRatioThresholdKey));
  if (!min_content_opt || *min_content_opt <= kMinContentAreaRatioThreshold ||
      *min_content_opt >= kMaxContentAreaRatioThreshold) {
    LOG(ERROR) << "Parse Error: '" << kMinContentAreaRatioThresholdKey
               << "' value is missing, invalid or out of range.";
    return false;
  }
  mCCMConfig_.min_content_area_ratio_threshold = *min_content_opt;

  auto scale_anim_opt =
      root_dict.FindInt(std::string(kScaleAnimationDurationKey));
  if (!scale_anim_opt || *scale_anim_opt <= kMinScaleAnimationDuration ||
      *scale_anim_opt >= kMaxScaleAnimationDuration) {
    LOG(ERROR) << "Parse Error: '" << kScaleAnimationDurationKey
               << "' value is missing, invalid or out of range.";
    return false;
  }
  mCCMConfig_.scale_animation_duration = *scale_anim_opt;

  const base::Value::List* opacity_list = root_dict.FindList(kOpacityFilterKey);
  if (!opacity_list) {
    LOG(ERROR) << "Parse Error: Missing '" << kOpacityFilterKey << "'.";
    return false;
  }
  if (opacity_list->size() != 2 || !(*opacity_list)[0].is_int() ||
      !(*opacity_list)[1].is_int()) {
    return false;
  }
  int first_opacity = (*opacity_list)[0].GetInt(),
      second_opacity = (*opacity_list)[1].GetInt();
  if (first_opacity < kMinOpacityFilter || first_opacity > kMaxOpacityFilter ||
      second_opacity < kMinOpacityFilter ||
      second_opacity > kMaxOpacityFilter || first_opacity > second_opacity) {
    return false;
  }
  mCCMConfig_.opacity_filter = {first_opacity, second_opacity};
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
      return false;
    }
  }
  return true;
}

bool NwebAutolayout::ParseWhitelistEntry(std::string_view app_bundle_name_sv,
    const base::Value::Dict& whitelist_dict) {
  WhitelistEntry& current_entry = mCCMConfig_.whitelist[app_bundle_name_sv];
  const std::string* pattern_ptr = whitelist_dict.FindString(kPatternKey);
  const std::string* get_id_ptr = whitelist_dict.FindString(kGetIDKey);
  const std::string* get_page_ptr = whitelist_dict.FindString(kGetPageKey);

  if (!pattern_ptr || pattern_ptr->empty() ||
       !get_id_ptr || get_id_ptr->empty()||
       !get_page_ptr || get_page_ptr->empty()) {
    LOG(ERROR) << "Parse Error: Missing, empty or invalid type for pattern.";
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
  // base::Value root_value(std::move(app_rules_list->Clone()));

  return base::JSONWriter::Write(*app_rules_list, &current_entry.appRuleInfos );
}

class JSResultCallbackImpl : public CefJavaScriptResultCallback {
 public:
  JSResultCallbackImpl(CefRefPtr<CefFrame> frame)
      : frame_(frame){}

  virtual ~JSResultCallbackImpl() {}

  std::string ConvertCefValueToString(CefRefPtr<CefValue> src) {
      std::string dst;
      int type = src->GetType();
      LOG(DEBUG) << "OnMessage type:" << type;
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
  auto resourceInstance = OhosAdapterHelper::GetInstance().GetResourceAdapter();
  auto autolayoutPath = std::string(kAutoLayoutFileNameHap);
  std::shared_ptr<OhosFileMapper> fileMapper =
      resourceInstance->GetRawFileMapper(autolayoutPath, true);
  if (!fileMapper || fileMapper->GetDataLen() == 0) {
    LOG(ERROR) << "Couldn't filemap AL js file by hap";
    mEnable_ = false;
    return;
  }
  LOG(DEBUG) << "AL js file length: " << fileMapper->GetDataLen()
            << " file fd:" << fileMapper->GetFd();

  std::unique_ptr<base::MemoryMappedFile> mmap =
      std::make_unique<base::MemoryMappedFile>();
  mmap->SetOhosFileMapper(fileMapper);
  if (reinterpret_cast<char*>(mmap->data()) == nullptr || mmap->length() == 0) {
    LOG(ERROR) << "Couldn't mmap AL js file";
    mEnable_ = false;
    return;
  }
  LOG(DEBUG) << "read resource file successfull.";
  mAutoLayoutJSSource_.assign(reinterpret_cast<char*>(mmap->data()), mmap->length());
}

std::optional<int> NwebAutolayout::ParseInt(std::string_view input) {
    if (input.empty()) {
        return std::nullopt;
    }

    int value = -1;
    const char* const start = input.data();
    const char* const end = start + input.size();
    auto result = std::from_chars(start, end, value);
    if (result.ec != std::errc() || result.ptr != end) {
        return std::nullopt;
    }

    return value;
}

}  // namespace OHOS::NWeb
