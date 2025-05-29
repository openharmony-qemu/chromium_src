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

#ifndef CONTENT_BROWSER_RENDERER_HOST_TEXT_INPUT_MANAGER_UTILS_H__
#define CONTENT_BROWSER_RENDERER_HOST_TEXT_INPUT_MANAGER_UTILS_H__

#include "content/browser/renderer_host/text_input_manager.h"

namespace content {
class CONTENT_EXPORT TextInputManagerUtils {
 public:
  TextInputManager* text_input_manager_;
  TextInputManagerUtils(TextInputManager* impl);

#if BUILDFLAG(ARKWEB_MENU)
  void ArkWebUpdateSelectionMap(
      RenderWidgetHostViewBase* view,
      const ui::mojom::TextInputState& text_input_state) const;
#endif
};
}  // namespace content

#endif  // CONTENT_BROWSER_RENDERER_HOST_TEXT_INPUT_MANAGER_UTILS_H__
