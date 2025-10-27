/* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/content/browser/renderer_host/text_input_manager_utils.h"

namespace content {
TextInputManagerUtils::TextInputManagerUtils(TextInputManager* impl) {
  this->text_input_manager_ = impl;
}
#if BUILDFLAG(ARKWEB_MENU)
void TextInputManagerUtils::ArkWebUpdateSelectionMap(
    RenderWidgetHostViewBase* view,
    const ui::mojom::TextInputState& text_input_state) const {
  if (text_input_manager_->text_selection_map_.find(view) !=
          text_input_manager_->text_selection_map_.end() &&
      text_input_state.value.has_value() &&
      text_input_manager_->text_selection_map_[view].text().empty()) {
    LOG(INFO) << "update text_selection_map_ text value";
    text_input_manager_->text_selection_map_[view].SetSelection(
        text_input_state.value.value(),
        text_input_manager_->text_selection_map_[view].offset(),
        text_input_manager_->text_selection_map_[view].range());
  }
}
#endif
}  // namespace content
