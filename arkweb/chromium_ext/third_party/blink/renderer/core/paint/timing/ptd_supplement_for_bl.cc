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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/timing/ptd_supplement_for_bl.h"
#include "third_party/blink/renderer/core/frame/local_frame_view.h"

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "arkweb/chromium_ext/base/ohos/blankless/blankless_controller.h"
#include "third_party/blink/renderer/core/paint/timing/paint_timing_detector.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/loader/document_loader.h"
#endif

namespace blink {

PTDSupplementForBL::PTDSupplementForBL(LocalFrameView* lfv) {
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
    ptd_for_bl_ = MakeGarbageCollected<PaintTimingDetector>(lfv, false);
#else
    (void)lfv;
#endif
}

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
void PTDSupplementForBL::NotifyPaintFinished() {
    ptd_for_bl_->NotifyPaintFinished();
}

void PTDSupplementForBL::OnInputOrScroll() {
    is_stop_ = true;
    ptd_for_bl_ ->OnInputOrScroll();
}

void PTDSupplementForBL::RestartRecordingLCP() {
    ptd_for_bl_->RestartRecordingLCP();
}

void PTDSupplementForBL::SoftNavigationDetected(LocalDOMWindow* window) {
    ptd_for_bl_->SoftNavigationDetected(window);
}

void PTDSupplementForBL::RestartRecordingLCPToUkm() {
    ptd_for_bl_->RestartRecordingLCPToUkm();
}

void PTDSupplementForBL::ReportIgnoredContent() {
    ptd_for_bl_->ReportIgnoredContent();
}

void PTDSupplementForBL::UpdateLcpCandidate() {
    if(is_stop_) {
        return;
    }
    ptd_for_bl_->UpdateLcpCandidate();
}

PaintTimingDetector& PTDSupplementForBL::GetPaintTimingDetector() const {
    return *ptd_for_bl_;
}

ImagePaintTimingDetector& PTDSupplementForBL::GetImagePaintTimingDetector() const {
    return ptd_for_bl_->GetImagePaintTimingDetector();
}

TextPaintTimingDetector& PTDSupplementForBL::GetTextPaintTimingDetector() const {
    return ptd_for_bl_->GetTextPaintTimingDetector();
}

void PTDSupplementForBL::StopRecordingLCP(bool stop) {
    is_stop_ = stop;
    if (!stop) {
        ptd_for_bl_->RestartRecordingLCP();
        ptd_for_bl_->record_lcp_to_metrics_ = true;
    }
}

void PTDSupplementForBL::Trace(Visitor* visitor) const {
    visitor->Trace(ptd_for_bl_);
}
#endif

}  // namespace blink