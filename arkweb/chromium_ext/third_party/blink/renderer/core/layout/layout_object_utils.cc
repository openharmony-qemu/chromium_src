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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/layout/layout_object_utils.h"
namespace blink {
LayoutObjectUtils::LayoutObjectUtils(LayoutObject* impl) {
  layout_object_ = impl;
}

void LayoutObjectUtils::Trace(Visitor* visitor) const {
  visitor->Trace(layout_object_);
}

#if BUILDFLAG(ARKWEB_MENU)
bool LayoutObjectUtils::ArkWebVisibleToHitTestRequest(
    const HitTestRequest& request) const {
  if (layout_object_->IsImage() && request.OnDoHitTest()) {
    return layout_object_->StyleRef().Visibility() == EVisibility::kVisible;
  }
  return layout_object_->StyleRef().Visibility() == EVisibility::kVisible &&
         (request.IgnorePointerEventsNone() ||
          layout_object_->StyleRef().VisibleToHitTesting());
}

bool LayoutObjectUtils::get_position_mode_ = false;
#endif
}  // namespace blink
