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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/timing/paint_timing_detector_utils.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/paint/timing/paint_timing_detector.h"

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "arkweb/chromium_ext/base/ohos/blankless/blankless_controller.h"
#include "third_party/blink/renderer/core/loader/document_loader.h"
#include "third_party/blink/renderer/core/paint/timing/image_paint_timing_detector.h"
#include "third_party/blink/renderer/core/paint/timing/largest_contentful_paint_calculator.h"
#include "third_party/blink/renderer/core/paint/timing/text_paint_timing_detector.h"
#include "third_party/blink/renderer/core/layout/layout_view.h"
#endif

namespace blink {

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
PaintTimingDetectorUtils::PaintTimingDetectorUtils(PaintTimingDetector* paint_timing_detector,
  bool need_supplement_for_bl /* = true */) {
#else
PaintTimingDetectorUtils::PaintTimingDetectorUtils(PaintTimingDetector* paint_timing_detector) {
#endif
  this->paint_timing_detector_ = paint_timing_detector;
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  if (need_supplement_for_bl) {
    need_supplement_for_bl_ = need_supplement_for_bl;
    ptd_supplement_for_bl_ = PTDSupplementForBL(paint_timing_detector->frame_view_);
  } else {
    paint_timing_detector->GetImagePaintTimingDetector().SetForBlankless();
    paint_timing_detector->GetTextPaintTimingDetector().SetForBlankless();
  }
#endif
#if BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT) || BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  if (paint_timing_detector_ && paint_timing_detector_->frame_view_ &&
      !paint_timing_detector_->frame_view_->GetFrame().Parent()) {
    first_screen_calculator_ = std::make_shared<FirstScreenCalculator>(
        paint_timing_detector->frame_view_);
  }
#endif
}

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
static bool IsBgImgContentful(const LayoutObject& object, const Image& image) {
  if (IsA<LayoutView>(object) || object.IsBody() || object.IsDocumentElement()) {
    return false;
  }
  return true;
}

void PaintTimingDetectorUtils::NotifyLcpForBlankless() {
  LOG(DEBUG) << "blankless PaintTimingDetectorUtils::NotifyLcpForBlankless";
  if (!base::ohos::BlanklessController::CheckGlobalProperty()) {
    return;
  }
  if (!paint_timing_detector_ || !paint_timing_detector_->frame_view_) {
    return;
  }
  Document* document = paint_timing_detector_->frame_view_->GetFrame().GetDocument();
  if (!document) {
    return;
  }
  DocumentLoader* loader = document->Loader();
  if (!loader || !loader->AsArkWebDocumentLoaderExt()) {
    return;
  }
  loader->AsArkWebDocumentLoaderExt()->NotifyLcpForBlankless();
}

void PaintTimingDetectorUtils::CheckNotifyLcpForBlankless() {
  ImagePaintTimingDetector& image_paint_timing_detector = paint_timing_detector_->GetImagePaintTimingDetector();
  if (image_paint_timing_detector.TakeIfHasALCP()) {
    TRACE_EVENT0("blink", "blankless lcp:PaintTimingDetector::UpdateLcpCandidate have one lcp");
    LOG(INFO) << "blankless lcp:PaintTimingDetector::UpdateLcpCandidate have one lcp, have supplement:"
      << HaveSupplementForBL();
    NotifyLcpForBlankless();
  } else {
    ForwardUpdateLcpCandidate();
  }
}

bool PaintTimingDetectorUtils::ForwardNotifyPaintFinished() {
  if (!need_supplement_for_bl_) {
    return false;
  }
  ptd_supplement_for_bl_.NotifyPaintFinished();
  return true;
}

void PaintTimingDetectorUtils::ForwardNotifyBackgroundImagePaint(const Node& node, const Image& image,
                                      const StyleImage& style_image,
                                      const PropertyTreeStateOrAlias& current_paint_chunk_properties,
                                      const gfx::Rect& image_border) {
  if (!need_supplement_for_bl_) {
    return;
  }

  LayoutObject* object = node.GetLayoutObject();
  if (!object) {
    return false;
  }
  PaintTimingDetector& paint_timing_detector = ptd_supplement_for_bl_.GetPaintTimingDetector();
  ImagePaintTimingDetector& image_paint_timing_detector = paint_timing_detector.GetImagePaintTimingDetector();
  if (!image_paint_timing_detector.IsRecordingLargestImagePaint()) {
    return false;
  }
  if (!IsBgImgContentful(*object, image)) {
    return false;
  }

  ImageResourceContent* cached_image = style_image.CachedImage();
  DCHECK(cached_image);
  return image_paint_timing_detector.RecordImage(*object, image.Size(), *cached_image, current_paint_chunk_properties,
    &style_image, image_border);
}

bool PaintTimingDetectorUtils::ForwardNotifyImagePaint(const LayoutObject& object,
                                                      const gfx::Size& intrinsic_size,
                                                      const MediaTiming& media_timing,
                                                      const PropertyTreeStateOrAlias& current_paint_chunk_properties,
                                                      const gfx::Rect& image_border) {
  if (!need_supplement_for_bl_) {
    return;
  }

  PaintTimingDetector& paint_timing_detector = ptd_supplement_for_bl_.GetPaintTimingDetector();
  ImagePaintTimingDetector& image_paint_timing_detector = paint_timing_detector.GetImagePaintTimingDetector();
  if (!image_paint_timing_detector.IsRecordingLargestImagePaint()) {
    return false;
  }

  return image_paint_timing_detector.RecordImage(object, intrinsic_size, media_timing, current_paint_chunk_properties,
    nullptr, image_border);
}

void PaintTimingDetectorUtils::ForwardNotifyImageFinished(const LayoutObject& object,
  const MediaTiming* media_timing) {
  if (!need_supplement_for_bl_) {
    return;
  }
  if (paint_timing_detector_->GetImagePaintTimingDetector().IsRecordingLargestImagePaint()) {
    return;
  }
  PaintTimingDetector& paint_timing_detector = ptd_supplement_for_bl_.GetPaintTimingDetector();
  ImagePaintTimingDetector& image_paint_timing_detector = paint_timing_detector.GetImagePaintTimingDetector();
  if (!image_paint_timing_detector.IsRecordingLargestImagePaint()) {
    return;
  }

  image_paint_timing_detector.NotifyImageFinished(object, media_timing);
}

void PaintTimingDetectorUtils::ForwardLayoutObjectWillBeDestroyed(const LayoutObject& object) {
  if (!need_supplement_for_bl_) {
    return;
  }
  PaintTimingDetector& paint_timing_detector = ptd_supplement_for_bl_.GetPaintTimingDetector();
  TextPaintTimingDetector& text_paint_timing_detector = paint_timing_detector.GetTextPaintTimingDetector();
  text_paint_timing_detector.LayoutObjectWillBeDestroyed(object);
}

void PaintTimingDetectorUtils::ForwardNotifyImageRemoved(const LayoutObject& object,
  const ImageResourceContent* cached_image) {
  if (!need_supplement_for_bl_) {
    return;
  }
  PaintTimingDetector& paint_timing_detector = ptd_supplement_for_bl_.GetPaintTimingDetector();
  ImagePaintTimingDetector& image_paint_timing_detector = paint_timing_detector.GetImagePaintTimingDetector();
  image_paint_timing_detector.NotifyImageRemoved(object, cached_image);
}

void PaintTimingDetectorUtils::ForwardOnInputOrScroll() {
  if (!need_supplement_for_bl_) {
    return;
  }
  ptd_supplement_for_bl_.OnInputOrScroll();
  return;
}

void PaintTimingDetectorUtils::ForwardRestartRecordingLCP() {
  if (!need_supplement_for_bl_) {
    return;
  }

  // sync blankless only PTD frame index to PTD.
  SyncBLPTDFrameIdxToPTD();

  ptd_supplement_for_bl_.RestartRecordingLCP();
}

void PaintTimingDetectorUtils::ForwardRestartRecordingLCPToUkm() {
  if (!need_supplement_for_bl_) {
    return;
  }

  // sync blankless only PTD frame index to PTD.
  SyncBLPTDFrameIdxToPTD();

  ptd_supplement_for_bl_.RestartRecordingLCPToUkm();
}

void PaintTimingDetectorUtils::ForwardSoftNavigationDetected(LocalDOMWindow* window) {
  if (!need_supplement_for_bl_) {
    return;
  }
  ptd_supplement_for_bl_.SoftNavigationDetected(window);
  return;
}

void PaintTimingDetectorUtils::ForwardReportIgnoredContent() {
  if (!need_supplement_for_bl_) {
    return;
  }
  ptd_supplement_for_bl_.ReportIgnoredContent();
}

void PaintTimingDetectorUtils::ForwardUpdateLcpCandidate() {
  if (!need_supplement_for_bl_) {
    return;
  }
  ptd_supplement_for_bl_.UpdateLcpCandidate();
}

void PaintTimingDetectorUtils::RestartRecordingForBlankless() {
  if (!need_supplement_for_bl_) {
    return;
  }
  LOG(INFO) << "blankless lcp:PaintTimingDetectorUtils::RestartRecordingForBlankless";
  ptd_supplement_for_bl_.StopRecordingLCP(false);
}

void PaintTimingDetectorUtils::SyncBLPTDFrameIdxToPTD() {
  if (!need_supplement_for_bl_) {
    return;
  }
  paint_timing_detector_->GetImagePaintTimingDetector().SetFrameIndex(
    ptd_supplement_for_bl_.GetImagePaintTimingDetector().GetFrameIndex());
  paint_timing_detector_->GetTextPaintTimingDetector().SetFrameIndex(
    ptd_supplement_for_bl_.GetTextPaintTimingDetector().GetFrameIndex());
}

void PaintTimingDetectorUtils::SyncIPTDFrameIdxToBLIPTD(unsigned frame_index) {
  if (!need_supplement_for_bl_) {
    return;
  }
  ptd_supplement_for_bl_.GetImagePaintTimingDetector().SetFrameIndex(frame_index);
}

void PaintTimingDetectorUtils::SyncTPTDFrameIdxToBLTPTD(unsigned frame_index) {
  if (!need_supplement_for_bl_) {
    return;
  }
  ptd_supplement_for_bl_.GetTextPaintTimingDetector().SetFrameIndex(frame_index);
}

void PaintTimingDetectorUtils::Trace(Visitor* visitor) const {
  visitor->Trace(paint_timing_detector_);
  if (need_supplement_for_bl_) {
    visitor->Trace(ptd_supplement_for_bl_);
  }
}

void PaintTimingDetector::RestartRecordingForBlankless() {
  paint_timing_detector_utils_.RestartRecordingForBlankless();
}

void PaintTimingDetector::SyncIPTDFrameIdxToBLIPTD(unsigned frame_index) {
  paint_timing_detector_utils_.SyncIPTDFrameIdxToBLIPTD(frame_index);
}

void PaintTimingDetector::SyncTPTDFrameIdxToBLTPTD(unsigned frame_index) {
  paint_timing_detector_utils_.SyncTPTDFrameIdxToBLTPTD(frame_index);
}

void TextPaintTimingDetector::SetForBlankless() {
  is_for_blankless_only_ = true;
}

uint32_t TextPaintTimingDetector::GetFrameIndex() const {
  return frame_index_;
}

void TextPaintTimingDetector::SetFrameIndex(uint32_t frame_index) {
  frame_index_ = frame_index;
}

void LargestContentfulPaintCalculator::SetForBlankless() {
  is_for_blankless_only_ = true;
}
#endif

#if BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT) || BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
std::shared_ptr<FirstScreenCalculator>
PaintTimingDetectorUtils::GetFirstScreenCalculator() {
  if (!paint_timing_detector_ || !paint_timing_detector_->frame_view_) {
    return nullptr;
  }
  Frame* parent_frame =
      paint_timing_detector_->frame_view_->GetFrame().Parent();
  if (!parent_frame) {
    return first_screen_calculator_;
  }

  LocalFrame* local_frame = static_cast<LocalFrame*>(parent_frame);
  if (!local_frame || !local_frame->View()) {
    return nullptr;
  }
  return local_frame->View()
      ->GetPaintTimingDetector()
      .GetFirstScreenCalculator();
}

void PaintTimingDetectorUtils::RestartRecordingFirstScreenPaint() {
  if (auto first_screen_calculator = GetFirstScreenCalculator()) {
    first_screen_calculator->RestartRecordingFirstScreenPaint(); 
  }
}

void PaintTimingDetectorUtils::OnUserScroll() {
  if (auto first_screen_calculator = GetFirstScreenCalculator()) {
    first_screen_calculator->OnUserScroll(); 
  }
}
#endif
}  // namespace blink