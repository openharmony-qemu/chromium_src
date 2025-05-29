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

#ifndef ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_HILIGHT_ARKWEB_HIGHLIGHT_STYLE_UTILS_H
#define ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_HILIGHT_ARKWEB_HIGHLIGHT_STYLE_UTILS_H

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/graphics/color.h"

namespace blink {
class Document;

class CORE_EXPORT ArkWebHighlightStyleUtils {
  STATIC_ONLY(ArkWebHighlightStyleUtils);

 public:
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  static bool InSelectionDragging(const Document& document);

  // white, alpha channel set to zero
  static constexpr RGBA32 kBackgroundColorInDragging = 0x00FFFFFF;
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
};
}  // namespace blink

#endif  // ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_HILIGHT_ARKWEB_HIGHLIGHT_STYLE_UTILS_H
