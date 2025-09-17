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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCAL_FRAME_VIEW_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCAL_FRAME_VIEW_UTILS_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/frame/local_frame_view.h"

namespace blink {
class LocalFrameView;
class LocalFrameViewUtils {
 public:
  LocalFrameViewUtils(LocalFrameView* local_frame_view);

#if BUILDFLAG(ARKWEB_MENU)
  void UpdateCompositedSelectionIfNeed();
#endif
#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
  void PerformLayoutOnPreload(Document* document);
#endif

  raw_ptr<LocalFrameView> local_frame_view_;
};
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCAL_FRAME_VIEW_UTILS_H_
