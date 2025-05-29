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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCAL_FRAME_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCAL_FRAME_EXT_H_

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCAL_FRAME_H_
#error \
    "must be in include form THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCAL_FRAME_H_"
#endif

#include "third_party/blink/renderer/core/frame/local_frame.h"

namespace blink {

class LocalFrameUtil {
 public:
  static bool SetLayoutAndTextZoomFactorsPage(raw_ptr<LocalFrame> LocalFrameObj,
                                              float& layout_zoom_factor,
                                              float& text_zoom_factor,
                                              Page* page);
  static void SetLayoutAndTextZoomFactorsExt(raw_ptr<LocalFrame> LocalFrameObj,
                                             float& layout_zoom_factor,
                                             float& text_zoom_factor,
                                             bool& layout_zoom_changed,
                                             Page* page);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCAL_FRAME_EXT_H_
