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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_IMAGE_PAINT_TIMING_DETECTOR_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_IMAGE_PAINT_TIMING_DETECTOR_UTILS_H_

#include <optional>
#include "base/notreached.h"
#include "third_party/blink/renderer/core/paint/timing/media_record_id.h"
#include "third_party/blink/renderer/core/style/style_image.h"
#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
#include "third_party/blink/renderer/platform/heap/collection_support/heap_deque.h"
#endif
#include "third_party/blink/renderer/platform/heap/collection_support/heap_hash_map.h"
#include "third_party/blink/renderer/platform/heap/member.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"
#include "third_party/blink/renderer/platform/loader/fetch/media_timing.h"
#include "third_party/blink/renderer/platform/wtf/allocator/allocator.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"

namespace blink {

class ImageRecordsManager;   // forward declaration
class ImageRecord;           // forward declaration
#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
class PropertyTreeStateOrAlias;
#endif

// |ImageRecordsManagerUtils| is the manager of all of the images that Accumulate visual size since LCP.
class CORE_EXPORT ImageRecordsManagerUtils : public GarbageCollected<ImageRecordsManagerUtils> {
 public:
    explicit ImageRecordsManagerUtils(ImageRecordsManager& image_records_manager);
    void Trace(Visitor* visitor) const;
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
    void UpdateViewportSize(const std::optional<uint64_t>& size);
    bool CheckALCPRecord(const MediaRecordIdHash& record_id_hash, const MediaTiming& media_timing,
        const StyleImage* style_image, unsigned frame_index, bool new_lcp_record);
    bool TakeIfHasALCP();
    inline void RemoveRecord(MediaRecordIdHash record_id_hash) {
        auto it = alcp_pending_images_.find(record_id_hash);
        if (it != alcp_pending_images_.end()) {
            alcp_pending_images_.erase(it);
        }
    }
    void ALCPCalculate(const MediaRecordId& record_id, const uint64_t& visual_size,
        const gfx::Rect& frame_visual_rect, const gfx::RectF& root_visual_rect, double bpp);
    void ALCPProcessBeforeLcpRecord();
    uint64_t CalculateLatestALCPSize() const;
    void AssignPaintTimeToRegisteredQueuedRecordsForALCP(const MediaRecordIdHash& record_id_hash,
        const base::TimeTicks& timestamp);
    void ClearForALCP();
    void SetForBlankless();
    bool IsForBlankless() const;
#endif
#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION) || BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT)
    void AssignImagePaintTimeFromRejectedImages(
        const base::TimeTicks& timestamp,
        unsigned last_queued_frame_index);
    void AssignImagePaintTimeFromRecord(ImageRecord* record,
                                        const base::TimeTicks& timestamp);
    void RemoveRecordFromFirstScreenCalculator(
        MediaRecordIdHash record_id_hash);
    void NotifyImagePaintForFirstScreenCalculator(MediaRecordIdHash hash,
                                                  ImageRecord* record,
                                                  bool is_video);
    void InsertRejectedImageRecords(MediaRecordIdHash hash,
                                    ImageRecord* record);
    void ClearRejectedImagesQueuedForPaintTime();
    void ClearRejectedImageRecords();
    void TraceRejectedImages(Visitor* visitor) const;
    void GetAddedEntryInLatestFrameByRejectedImage(
        const MediaTiming& media_timing,
        const LayoutObject& object,
        MediaRecordIdHash record_id_hash,
        unsigned frame_index,
        bool& added_entry_in_latest_frame,
        const StyleImage* style_image,
        const gfx::Rect& image_border,
        const PropertyTreeStateOrAlias& current_paint_chunk_properties);
    void GetAddedEntryInLatestFrameByRejectedDueToSize(
        const MediaTiming& media_timing,
        MediaRecordIdHash record_id_hash,
        unsigned frame_index,
        bool& added_entry_in_latest_frame,
        const StyleImage* style_image);
#endif
 private:
    ImageRecordsManager& image_records_manager_;
    ImageRecordsManagerUtils(const ImageRecordsManagerUtils&) = delete;
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
    ImageRecordsManager& operator=(const ImageRecordsManagerUtils&) = delete;

    std::optional<uint64_t> viewport_size_ = std::nullopt;
    // ALCP: Accumulate size since LCP.
    uint64_t accumulate_size_since_largest_ = 0;
    HeapHashMap<MediaRecordIdHash, Member<ImageRecord>> alcp_pending_images_;
    bool alcp_pending_images_added_ = false;
    Member<ImageRecord> alcp_image_;
    bool is_for_blankless_only_ = false;
#endif
#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION) || BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT)
    ImageRecord* GetRejectedImage(MediaRecordIdHash record_id_hash);
    bool IsRejectedDueToSize(MediaRecordIdHash record_id_hash);
    void QueueToMeasurePaintTimeForRejected(ImageRecord* record,
                                            unsigned current_frame_index);
    bool OnFirstAnimatedFramePaintedForRejected(
        MediaRecordIdHash record_id_hash,
        unsigned current_frame_index);
    void OnImageLoadedForRejected(MediaRecordIdHash record_id_hash,
                                  unsigned current_frame_index,
                                  const StyleImage* style_image);
    HeapDeque<Member<ImageRecord>> rejected_images_queued_for_paint_time_;
    HeapHashMap<MediaRecordIdHash, Member<ImageRecord>> rejected_image_records_;
#endif
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_IMAGE_PAINT_TIMING_DETECTOR_UTILS_H_