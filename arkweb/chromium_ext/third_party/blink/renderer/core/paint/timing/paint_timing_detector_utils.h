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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_PAINT_TIMING_DETECTOR_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_PAINT_TIMING_DETECTOR_UTILS_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/timing/ptd_supplement_for_bl.h"
#include "third_party/blink/renderer/core/layout/layout_object.h"
#include "third_party/blink/renderer/platform/graphics/image.h"
#include "third_party/blink/renderer/core/style/style_image.h"
#include "third_party/blink/renderer/platform/graphics/paint/property_tree_state.h"
#include "third_party/blink/renderer/platform/loader/fetch/media_timing.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "third_party/blink/renderer/core/loader/resource/image_resource_content.h"
#endif
#if BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT) || BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/timing/first_screen_calculator.h"
#endif
namespace blink {

class PaintTimingDetector;
class PTDSupplementForBL;
#if BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT) || BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
class FirstScreenCalculator;
#endif

class PaintTimingDetectorUtils {
 public:
  DISALLOW_NEW();
  Member<PaintTimingDetector> paint_timing_detector_;
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  PaintTimingDetectorUtils() = default;
  PaintTimingDetectorUtils(PaintTimingDetector* paint_timing_detector, bool need_supplement_for_bl = true);
#else
  explicit PaintTimingDetectorUtils(PaintTimingDetector* paint_timing_detector);
#endif
#if BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT) || BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  std::shared_ptr<FirstScreenCalculator> GetFirstScreenCalculator();
  void RestartRecordingFirstScreenPaint();
  void OnUserScroll();
#endif
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  bool ForwardNotifyPaintFinished();
  void ForwardNotifyBackgroundImagePaint(const Node& node, const Image& image, const StyleImage& style_image,
    const PropertyTreeStateOrAlias& current_paint_chunk_properties, const gfx::Rect& image_border);
  bool ForwardNotifyImagePaint(const LayoutObject& object,
                              const gfx::Size& intrinsic_size,
                              const MediaTiming& media_timing,
                              const PropertyTreeStateOrAlias& current_paint_chunk_properties,
                              const gfx::Rect& image_border);
  void ForwardNotifyImageFinished(const LayoutObject& object, const MediaTiming* media_timing);
  void ForwardLayoutObjectWillBeDestroyed(const LayoutObject& object);
  void ForwardNotifyImageRemoved(const LayoutObject& object, const ImageResourceContent* cached_image);
  void ForwardOnInputOrScroll();
  void ForwardRestartRecordingLCP();
  void ForwardSoftNavigationDetected(LocalDOMWindow* window);
  void ForwardRestartRecordingLCPToUkm();
  void ForwardReportIgnoredContent();
  void ForwardUpdateLcpCandidate();
  void NotifyLcpForBlankless();
  void CheckNotifyLcpForBlankless();
  void RestartRecordingForBlankless();
  bool HaveSupplementForBL() const { return need_supplement_for_bl_; }
  void SyncIPTDFrameIdxToBLIPTD(unsigned frame_index);
  void SyncTPTDFrameIdxToBLTPTD(unsigned frame_index);
  void Trace(Visitor* visitor) const;
#endif
private:
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void SyncBLPTDFrameIdxToPTD();
  bool need_supplement_for_bl_ = false;
  PTDSupplementForBL ptd_supplement_for_bl_;
#endif
#if BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT) || BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  std::shared_ptr<FirstScreenCalculator> first_screen_calculator_;
#endif
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_PAINT_TIMING_DETECTOR_UTILS_H_