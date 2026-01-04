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

#include "nweb_highlight_specified_content.h"

#include "base/logging.h"
#include "base/strings/string_util.h"
#include "ohos_resources/highlight_specified_content/grit/highlight_specified_content_resources.h"
#include "ui/base/resource/resource_bundle.h"

namespace OHOS::NWeb {

constexpr std::string_view kStopTemplate =
    "(function(window) { delete window.AgentHighlightUtils; delete "
    "window.__highlightSpecifiedContentLoaded; })(window);";

NWebHighlightSpecifiedContent::NWebHighlightSpecifiedContent(
    base::WeakPtr<NWebDelegate> nweb_delegate)
    : nweb_delegate_(nweb_delegate), weak_factory_(this) {}

void NWebHighlightSpecifiedContent::SetHighlightSpecifiedContentEnable(
    bool enable) {
  LOG(INFO)
      << "NWebHighlightSpecifiedContent::SetHighlightSpecifiedContentEnable: "
      << enable;
  enable_ = enable;
  if (!nweb_delegate_) {
    LOG(ERROR)
        << "SetHighlightSpecifiedContentEnable, nweb_delegate_ is nullptr";
    return;
  }

  std::function<void(void)> callback = nullptr;
  if (enable) {
    callback = [weak = weak_factory_.GetWeakPtr()]() {
      if (!weak) {
        LOG(ERROR) << "weak ptr is nullptr";
        return;
      }
      weak->RegisterHighlight();
    };
    RegisterHighlight();
    nweb_delegate_->RegisterOnLoadStartedCbForHighlightContent(
        std::move(callback));
  } else {
    RemoveHighlightFunctions();
    nweb_delegate_->RegisterOnLoadStartedCbForHighlightContent(nullptr);
  }
}

void NWebHighlightSpecifiedContent::RegisterHighlight() {
  if (!enable_ || !nweb_delegate_) {
    return;
  }
  LOG(INFO) << "NWebHighlightSpecifiedContent::RegisterHighlight";
  const auto& rb = ui::ResourceBundle::GetSharedInstance();

  LOG(INFO) << "NWebHighlightSpecifiedContent::start run highlight extractor";
  std::string_view highlight_script =
      rb.GetRawDataResource(IDR_HIGHLIGHT_SPECIFIED_CONTENT_JS);
  if (highlight_script.empty()) {
    LOG(ERROR) << "highlight extractor script failed";
    return;
  }
  std::string highlight_js(highlight_script);
  nweb_delegate_->ExecuteJavaScript(highlight_js);
}

void NWebHighlightSpecifiedContent::RemoveHighlightFunctions() {
  if (enable_ || !nweb_delegate_) {
    return;
  }
  LOG(INFO) << "NWebHighlightSpecifiedContent::RemoveHighlightFunctions";
  std::string final_js = std::string(kStopTemplate);
  nweb_delegate_->ExecuteJavaScript(final_js);
}

}  // namespace OHOS::NWeb