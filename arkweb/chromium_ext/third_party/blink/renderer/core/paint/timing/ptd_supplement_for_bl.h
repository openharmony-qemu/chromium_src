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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_PTD_SUPPLEMENT_FOR_BL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_PTD_SUPPLEMENT_FOR_BL_H_

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"
#include "third_party/blink/renderer/platform/heap/visitor.h"
#endif

namespace blink {
class LocalFrameView;

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
class PaintTimingDetector;
class ImagePaintTimingDetector;
class TextPaintTimingDetector;
#endif

// paint timing detector hook for blankless
class PTDSupplementForBL {
 public:
    DISALLOW_NEW();
    PTDSupplementForBL() = default;
    explicit PTDSupplementForBL(LocalFrameView* lfv);

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
    void NotifyPaintFinished();
    void OnInputOrScroll();
    void RestartRecordingLCP();
    void RestartRecordingLCPToUkm();
    void SoftNavigationDetected(LocalDOMWindow* window);
    void ReportIgnoredContent();
    void UpdateLcpCandidate();
    PaintTimingDetector& GetPaintTimingDetector() const;
    ImagePaintTimingDetector& GetImagePaintTimingDetector() const;
    TextPaintTimingDetector& GetTextPaintTimingDetector() const;
    void StopRecordingLCP(bool stop);
    void Trace(Visitor* visitor) const;
 private:
    bool is_stop_ = false;
    Member<PaintTimingDetector> ptd_for_bl_ = nullptr;
#endif
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_PTD_SUPPLEMENT_FOR_BL_H_