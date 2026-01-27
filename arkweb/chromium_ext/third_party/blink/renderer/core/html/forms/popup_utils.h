/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_POPUP_UTILS_H
#define ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_POPUP_UTILS_H

#include "third_party/blink/renderer/platform/wtf/shared_buffer.h"

namespace blink {
class LocalFrame;

class PopupUtils {
 public:
  static void AddAvailRectInWebToData(LocalFrame* frame, SegmentedBuffer& data);
};
}  // namespace blink
#endif