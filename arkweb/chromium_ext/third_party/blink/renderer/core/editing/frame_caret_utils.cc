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

#include "third_party/blink/public/web/web_settings.h"
#include "third_party/blink/public/web/web_view.h"
#include "third_party/blink/renderer/core/exported/web_view_impl.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_frame_view.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/page/chrome_client.h"

namespace blink {

FrameCaretUtils::FrameCaretUtils(FrameCaret* frame_caret)
    : frame_caret_(frame_caret) {}

#if BUILDFLAG(ARKWEB_MENU)
bool FrameCaretUtils::GetTouchHandleStateFromWeb(LocalFrameView* frame_view) {
  ChromeClient* client = frame_view->GetChromeClient();
  if (!client || !client->GetWebView() ||
      !client->GetWebView()->GetSettings()) {
    return false;
  }
  auto setting = client->GetWebView()->GetSettings();

  return setting->IsTouchHandleExist();
}
#endif  // BUILDFLAG(ARKWEB_MENU)

}  // namespace blink