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

#include "third_party/blink/renderer/core/frame/settings.h"

namespace blink {

TEST_F(SystemClipboardTest, IsCopyAllowed) {
  local_frame().GetSettings()->SetCopyOption(
      blink::mojom::CopyOptionMode::NONE);
  EXPECT_EQ(local_frame().GetSettings()->GetCopyOption(),
            blink::mojom::CopyOptionMode::NONE);
  system_clipboard().WritePlainText(String("plain_text"));
  local_frame().GetSettings()->SetCopyOption(
      blink::mojom::CopyOptionMode::IN_APP);
  EXPECT_EQ(local_frame().GetSettings()->GetCopyOption(),
            blink::mojom::CopyOptionMode::IN_APP);
  system_clipboard().WritePlainText(String("plain_text"));
}

}  // namespace blink