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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_FRAME_CARET_UTILS_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_FRAME_CARET_UTILS_EXT_H_

namespace blink {
class FrameCaret;
class LocalFrameView;

class CORE_EXPORT FrameCaretUtils {
 public:
  FrameCaretUtils(FrameCaret* frame_caret);

#if BUILDFLAG(ARKWEB_MENU)
  bool GetTouchHandleStateFromWeb(LocalFrameView* frame_view);
#endif  // BUILDFLAG(ARKWEB_MENU)

 private:
  FrameCaret* frame_caret_ = nullptr;
};

}  // namespace blink

#endif // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_FRAME_CARET_UTILS_EXT_H_
