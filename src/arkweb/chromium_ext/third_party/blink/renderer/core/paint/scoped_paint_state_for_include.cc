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

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/style/computed_style.h"
#include "third_party/blink/renderer/core/style/computed_style_constants.h"
#endif

namespace blink {

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
bool ScopedBoxContentsPaintState::IsEllipisTextOverFlowInSelection(
    const LayoutBox& box) {
  if (!box.GetDocument().GetSettings() ||
      !box.GetDocument().GetSettings()->IsContextMenuCustomizationEnabled()) {
    return false;
  }
  if ((box.GetSelectionState() == SelectionState::kNone)) {
    return false;
  }
  return box.Style() && box.Style()->TextOverflow() == ETextOverflow::kEllipsis;
}
#endif

}  // namespace blink