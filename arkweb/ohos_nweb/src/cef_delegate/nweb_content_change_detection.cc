/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "nweb_content_change_detection.h"

#include "base/logging.h"
#include "base/strings/string_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "ohos_resources/content_change_detection/grit/content_change_detection_resources.h"

namespace OHOS::NWeb {

constexpr std::string_view kStartTemplate = "(function(window) { window.viewportFontMonitor.start($1, $2); })(window);";
constexpr std::string_view kStopTemplate = "(function(window) { window.viewportFontMonitor.stop(); })(window);";

NWebContentChangeDetection::NWebContentChangeDetection(
    base::WeakPtr<NWebDelegateInterface> nweb_delegate)
    : nweb_delegate_(nweb_delegate) {}

void NWebContentChangeDetection::SetContentChangeDetectionEnable(bool enable) {
    LOG(INFO) << "NWebContentChangeDetection::SetContentChangeDetectionEnable: " << enable;
  enable_ = enable;
  if (!nweb_delegate_) {
      LOG(ERROR) << "SetContentChangeDetectionEnable, nweb_delegate_ is nullptr";
      return;
  }

  std::function<void(void)> callback = nullptr;
  if (enable) {
    callback = [weak = weak_factory_.GetWeakPtr()]() {
      if (!weak) {
        LOG(ERROR) << "weak ptr is nullptr";
        return;
      }
      weak->StartDetection();
    };
    StartDetection();
  } else {
    StopDetection();
  }
  nweb_delegate_->RegisterOnLoadStartedCbForContentChange(std::move(callback));
}

void NWebContentChangeDetection::SetContentChangeDetectionConfig(
    int32_t min_report_time, float text_content_ratio) {
  LOG(INFO) << "NWebContentChangeDetection::SetContentChangeDetectionConfig, min_report_time: "
            << min_report_time << ", text_content_ratio: " << text_content_ratio;
  min_report_time_ = min_report_time;
  text_content_ratio_ = text_content_ratio;
}

void NWebContentChangeDetection::StartDetection() {
  if (!enable_ || !nweb_delegate_) {
    return;
  }
  LOG(INFO) << "NWebContentChangeDetection::StartDetection";
  const auto& rb = ui::ResourceBundle::GetSharedInstance();
  auto script_data = rb.GetRawDataResource(IDR_START_CONTENT_CHANGE_DETECTION_JS);
  if (script_data.empty()) {
    LOG(ERROR) << "get script_data failed";
    return;
  }
  std::string start_js = base::ReplaceStringPlaceholders(kStartTemplate,
      {std::to_string(text_content_ratio_), std::to_string(min_report_time_)}, nullptr);

  std::string final_js = std::string(script_data) + start_js;
  nweb_delegate_->ExecuteJavaScript(final_js);
}

void NWebContentChangeDetection::StopDetection() {
  if (enable_ || !nweb_delegate_) {
    return;
  }
  LOG(INFO) << "NWebContentChangeDetection::StopDetection";
  std::string final_js = std::string(kStopTemplate);
  nweb_delegate_->ExecuteJavaScript(final_js);
}

} // namespace OHOS::NWeb