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

#include "third_party/blink/renderer/core/html/html_object_element.h"

namespace blink {

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void HTMLObjectElement::NativeEmbedOverlay(
    const AttributeModificationParams& params) {
  bool new_overlay_infinity = false;
  bool new_overlay = false;
  if (!params.new_value.empty()) {
    String lower_value = params.new_value.LowerASCII();
    if (lower_value == "display:overlay-infinity" ||
      lower_value.Contains("display:overlay-infinity")) {
      new_overlay_infinity = true;
    } else if (lower_value == "display:overlay" ||
      lower_value.Contains("display:overlay")) {
      new_overlay = true;
    }
  }
  Utils()->SetNativeEmbedOverlayInfinity(new_overlay_infinity);
  Utils()->SetNativeEmbedOverlay(new_overlay);
  if (GetLayoutObject()) {
    SetNeedsPluginUpdate(true);
    GetLayoutObject()->SetNeedsLayoutAndFullPaintInvalidation(
        "Arkwebnativestyle changed");
  }
}
#endif

}  // namespace blink