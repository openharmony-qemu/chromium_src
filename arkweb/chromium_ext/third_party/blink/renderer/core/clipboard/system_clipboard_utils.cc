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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/clipboard/system_clipboard_utils.h"

#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/settings.h"

namespace blink {
SystemClipboardUtils::SystemClipboardUtils(SystemClipboard* system_clipboard,
                                           LocalFrame* frame)
    : system_clipboard_(system_clipboard),
      frame_(frame) {}

SystemClipboardUtils::~SystemClipboardUtils() {}

#if BUILDFLAG(ARKWEB_COPY_OPTION)
bool SystemClipboardUtils::IsCopyAllowed() {
  if (frame_->GetSettings()->GetCopyOption() ==
      blink::mojom::CopyOptionMode::NONE) {
    return false;
  }
  return true;
}
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)
}  // namespace blink
