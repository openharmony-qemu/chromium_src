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

// |ImageRecordsManagerUtils| is the manager of all of the images that Accumulate visual size since LCP.
class CORE_EXPORT ImageRecordsManagerUtils {
    DISALLOW_NEW();
    explicit ImageRecordsManagerUtils(ImageRecordsManager& image_records_manager);
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
    void TraceForALCP(Visitor* visitor) const;
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
#endif
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_IMAGE_PAINT_TIMING_DETECTOR_UTILS_H_