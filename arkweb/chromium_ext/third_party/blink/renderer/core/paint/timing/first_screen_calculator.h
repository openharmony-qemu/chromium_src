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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_FIRST_SCREEN_CALCULATOR_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_FIRST_SCREEN_CALCULATOR_H_

#include "base/time/time.h"
#include "base/timer/timer.h"
#include "third_party/blink/renderer/core/frame/local_frame_view.h"
#include "third_party/blink/renderer/core/paint/timing/media_record_id.h"
#include "third_party/blink/renderer/platform/heap/collection_support/heap_hash_set.h"
#include "third_party/blink/renderer/platform/heap/member.h"
#include "ui/gfx/geometry/rect.h"

namespace blink {
class ImageRecord;
class TextRecord;

class FirstScreenCalculator {
 public:
  explicit FirstScreenCalculator(LocalFrameView* local_frame_view)
      : frame_view_(local_frame_view) {}
  void NotifyImagePaint(MediaRecordIdHash record_id_hash,
                        const ImageRecord* record,
                        std::optional<uint64_t> viewport_size,
                        bool is_video);
  void NotifyTextPaint(const TextRecord* record, base::TimeTicks timestamp);
  void AssignImagePaintTime(MediaRecordIdHash record_id_hash,
                            const gfx::Rect& rect,
                            base::TimeTicks timestamp);
  bool RemoveImageRecord(MediaRecordIdHash record_id_hash);
  void OnUserScroll();
  bool HasUserScrolled() const;
  void RestartRecordingFirstScreenPaint();
  void GetPaintRects(std::vector<gfx::Rect>& paint_rects);

 private:
  struct PaintRectInfo {
    PaintRectInfo() {}
    PaintRectInfo(const gfx::Rect rect, base::TimeTicks timestamp)
        : rect_(rect), paint_time_(timestamp) {}
    gfx::Rect rect_;
    base::TimeTicks paint_time_ = base::TimeTicks();
  };

  void DumpImageRect();
  void DumpTextRect();
  void OnFirstScreenInvoked();
  void RestartTimerForFirstScreenDetection();
  bool IsRectContainedByExistingRects(const gfx::Rect& rect);
  bool DoesRectIntersectExistingRects(const gfx::Rect& rect);
  void RemoveExistingRectsContainedByRect(const gfx::Rect& rect);
  bool IsRectTooSmallWhenNearlyFinished(const gfx::Rect& rect);

  std::unordered_map<MediaRecordIdHash, PaintRectInfo> image_rects_map_;
  std::vector<MediaRecordIdHash> intersected_image_ids_;
  std::vector<PaintRectInfo> text_paint_rects_;
  base::OneShotTimer timer_;
  base::TimeTicks first_screen_paint_time_;
  base::TimeTicks navigation_start_time_;
  Member<LocalFrameView> frame_view_;
  bool user_scrolled_ = false;
  MediaRecordIdHash background_image_id_ = 0;
  uint64_t viewport_size_ = 0;
  gfx::Rect occupied_rect_{gfx::Rect()};
  bool nearly_finished_ = false;
  base::WeakPtrFactory<FirstScreenCalculator> weak_factory_{this};
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_FIRST_SCREEN_CALCULATOR_H_