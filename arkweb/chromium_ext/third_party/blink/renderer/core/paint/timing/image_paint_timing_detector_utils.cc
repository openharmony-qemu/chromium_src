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

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/timing/image_paint_timing_detector_utils.h"
#include "third_party/blink/public/common/features.h"
#include "third_party/blink/renderer/core/frame/visual_viewport.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_frame_view.h"
#include "third_party/blink/renderer/core/layout/layout_object.h"
#include "third_party/blink/renderer/core/paint/timing/image_element_timing.h"
#include "third_party/blink/renderer/core/paint/timing/image_paint_timing_detector.h"
#include "third_party/blink/renderer/platform/instrumentation/tracing/trace_event.h"
#include "third_party/blink/renderer/platform/instrumentation/tracing/traced_value.h"
#include "v8/include/cppgc/allocation.h"

namespace blink {

ImageRecordsManagerUtils::ImageRecordsManagerUtils(ImageRecordsManager& image_records_manager)
    : image_records_manager_(image_records_manager) {}

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
void ImageRecordsManagerUtils::UpdateViewportSize(const std::optional<uint64_t>& size) {
    viewport_size_ = size;
}

bool ImageRecordsManagerUtils::CheckALCPRecord(const MediaRecordIdHash& record_id_hash,
    const MediaTiming& media_timing, const StyleImage* style_image, unsigned frame_index, bool new_lcp_record) {
    if (new_lcp_record) {
        if (!alcp_pending_images_added_) {
            return false;
        }
        alcp_pending_images_added_ = false;
    }

    auto it = alcp_pending_images_.find(record_id_hash);
    if (it == alcp_pending_images_.end()) {
        return false;
    }

    ImageRecord* record = it->value.Get();
    if (!record) {
        return false;
    }
    bool ret = false;
    if (media_timing.IsPaintedFirstFrame() &&
        RuntimeEnabledFeatures::LCPAnimatedImagesWebExposedEnabled()) {
        if (record->media_timing && !record->media_timing->GetFirstVideoFrameTime().is_null()) {
            // If this is a video record, then we can get the first frame time from the
            // MediaTiming object, and can use that to set the first frame time in the
            // ImageRecord object.
            record->first_animated_frame_time = record->media_timing->GetFirstVideoFrameTime();
        } else if (record->first_animated_frame_time.is_null()) {
            // Otherwise, this is an animated images, and so we should wait for the
            // presentation callback to fire to set the first frame presentation time.
            record->queue_animated_paint = true;
            image_records_manager_.QueueToMeasurePaintTime(record, frame_index);
            ret = true;
        }
    }

    if (!record->loaded && media_timing.IsSufficientContentLoadedForPaint()) {
        if (!style_image) {
            auto iter = image_records_manager_.image_finished_times_.find(record_id_hash);
            if (iter != image_records_manager_.image_finished_times_.end()) {
                record->load_time = iter->value;
                DCHECK(!record->load_time.is_null());
            }
        } else {
            Document* document = image_records_manager_.frame_view_->GetFrame().GetDocument();
            if (document && document->domWindow()) {
                record->load_time =
                    ImageElementTiming::From(*document->domWindow()).GetBackgroundImageLoadTime(style_image);
                record->origin_clean = style_image->IsOriginClean();
            }
        }
        image_records_manager_.OnImageLoadedInternal(record, frame_index);
        ret = true;
    }
    TRACE_EVENT2("blink", "ImageRecordsManagerUtils::CheckALCPRecord res",
        "new_lcp_record:", new_lcp_record, "ret:", ret);
    return ret;
}

bool ImageRecordsManagerUtils::TakeIfHasALCP() {
    bool ret = false;
    if (alcp_image_ && alcp_image_->recorded_size > 0) {
        ret = true;
    }
    TRACE_EVENT2("blink,benchmark", "blankless ImageRecordsManagerUtils::TakeIfHasALCP-",
        "alcp_image size:", (alcp_image_ ? alcp_image_->recorded_size : 0), "ret:", ret);
    alcp_image_ = nullptr;
    return ret;
}

void ImageRecordsManagerUtils::ClearForALCP() {
    viewport_size_ = std::nullopt;
    accumulate_size_since_largest_ = 0;
    alcp_pending_images_.clear();
    alcp_pending_images_added_ = false;
    alcp_image_ = nullptr;
}

void ImageRecordsManagerUtils::Trace(Visitor* visitor) const {
    visitor->Trace(alcp_image_);
    visitor->Trace(alcp_pending_images_);
}

void ImageRecordsManagerUtils::AssignPaintTimeToRegisteredQueuedRecordsForALCP(
    const MediaRecordIdHash& record_id_hash, const base::TimeTicks& timestamp) {
    auto it = alcp_pending_images_.find(record_id_hash);
    if (it == alcp_pending_images_.end() || !it->value) {
        return;
    }
    ImageRecord* record = it->value;
    record->paint_time = timestamp;
    TRACE_EVENT2("blink", "ImageRecordsManagerUtils::AssignPaintTimeToRegisteredQueuedRecordsForALCP",
        "lastest ALCP.size:", (alcp_image_ ? alcp_image_->recorded_size : 0), "opt alcp size:", record->recorded_size);
    if (!alcp_image_ || alcp_image_->recorded_size < record->recorded_size) {
        alcp_image_ = std::move(it->value);
    }
    alcp_pending_images_.erase(it);
}

void ImageRecordsManagerUtils::ALCPCalculate(const MediaRecordId& record_id, const uint64_t& visual_size,
    const gfx::Rect& frame_visual_rect, const gfx::RectF& root_visual_rect, double bpp) {
    if (base::FeatureList::IsEnabled(features::kExcludeLowEntropyImagesFromLCP) &&
        bpp < features::kMinimumEntropyForLCP.Get()) {
        return;
    }

    if (!record_id.GetLayoutObject() || !record_id.GetLayoutObject()->GetNode()) {
        return;
    }

    if (visual_size == 0) {
        return;
    }

    accumulate_size_since_largest_ += visual_size;
    uint64_t latest_alcp_size = CalculateLatestALCPSize();
    if (accumulate_size_since_largest_ < latest_alcp_size) {
        return;
    }
    TRACE_EVENT2("blink", "ImageRecordsManagerUtils::ALCPCalculate accumulated size updated",
        "accumulated_size:", accumulate_size_since_largest_, "ALCP.size:", latest_alcp_size);
    
    DCHECK_GT(visual_size, 0u);
    Node* node = record_id.GetLayoutObject()->GetNode();
    DOMNodeId node_id = node->GetDomNodeId();
    ImageRecord* record = MakeGarbageCollected<ImageRecord>(node_id, record_id.GetMediaTiming(), visual_size,
        frame_visual_rect, root_visual_rect, record_id.GetHash());
    alcp_pending_images_.insert(record->hash, record);
    accumulate_size_since_largest_ = 0;
    alcp_pending_images_added_ = true;
}

void ImageRecordsManagerUtils::ALCPProcessBeforeLcpRecord() {
    accumulate_size_since_largest_ = 0;
}

uint64_t ImageRecordsManagerUtils::CalculateLatestALCPSize() const {
    uint64_t latest_lcp_size = UINT64_MAX;
    if (!viewport_size_.has_value() || (!image_records_manager_.largest_painted_image_ &&
        !image_records_manager_.largest_pending_image_)) {
        LOG(WARNING) << "blankless no valid viewport size or no valid lcp, viewportsize:" <<
            (viewport_size_.has_value() ? *viewport_size_ : 0);
        return latest_lcp_size;
    }
    if (image_records_manager_.largest_painted_image_) {
        latest_lcp_size = image_records_manager_.largest_painted_image_->recorded_size;
    } else if (image_records_manager_.largest_pending_image_) {
        latest_lcp_size = image_records_manager_.largest_pending_image_->recorded_size;
    }

    // The latest ALCPSize is calculated by comparing the LCP size with the viewport size using a weighted ratio.
    // 5 is the total level for Weighted Strategy.
    float weight = static_cast<float>(latest_lcp_size) / (*viewport_size_);
    constexpr int level_count = 5;
    constexpr float min_weight = 0.05;
    const float vp_weights[level_count] = {0.1, 0.2, 0.33, 0.6, 0.8};
    const float alcp_weights[level_count] = {1.5, 0.8, 0.5, 0.25, 0.2};
    for (int i = 0; i < level_count; ++i) {
        if (latest_lcp_size <= ((*viewport_size_) * vp_weights[i])) {
            // Check if the multiplication will cause overflow.
            if (latest_lcp_size > (std::numeric_limits<uint64_t>::max() / alcp_weights[i])) {
                latest_lcp_size = std::numeric_limits<uint64_t>::max();
            } else {
                latest_lcp_size = latest_lcp_size * alcp_weights[i];
            }
            if (min_weight * (*viewport_size_) > latest_lcp_size) {
                latest_lcp_size = min_weight * (*viewport_size_);
            }
            break;
        }
    }

    return latest_lcp_size;
}

void ImageRecordsManagerUtils::SetForBlankless() {
    is_for_blankless_only_ = true;
}

bool ImageRecordsManagerUtils::IsForBlankless() const {
    return is_for_blankless_only_;
}

void ImageRecordsManager::UpdateViewportSize(const std::optional<uint64_t>& size) {
    image_record_manager_utils_.UpdateViewportSize(size);
}

bool ImageRecordsManager::TakeIfHasALCP() {
    return image_record_manager_utils_.TakeIfHasALCP();
}

void ImageRecordsManager::ALCPProcessBeforeLcpRecord() {
    image_record_manager_utils_.ALCPProcessBeforeLcpRecord();
}

bool ImageRecordsManager::CheckALCPRecord(const MediaRecordIdHash& record_id_hash, const MediaTiming& media_timing,
    const StyleImage* style_image, unsigned frame_index, bool new_lcp_record) {
    return image_record_manager_utils_.CheckALCPRecord(record_id_hash, media_timing, style_image, frame_index,
        new_lcp_record);
}

void ImageRecordsManager::SetForBlankless() {
    image_record_manager_utils_.SetForBlankless();
}

bool ImagePaintTimingDetector::TakeIfHasALCP() {
    return records_manager_.TakeIfHasALCP();
}

void ImagePaintTimingDetector::SetForBlankless() {
    is_for_blankless_only_ = true;
    records_manager_.SetForBlankless();
}

unsigned ImagePaintTimingDetector::GetFrameIndex() const {
    return frame_index_;
}

void ImagePaintTimingDetector::SetFrameIndex(unsigned frame_index) {
    frame_index_ = frame_index;
}
#endif // ARKWEB_BLANK_OPTIMIZE

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION) || BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT)
void ImageRecordsManagerUtils::AssignImagePaintTimeFromRecord(
    ImageRecord* record,
    const base::TimeTicks& timestamp) {
    auto first_screen_calculator =
        image_records_manager_.frame_view_->GetPaintTimingDetector()
            .GetFirstScreenCalculator();
    if (first_screen_calculator) {
        if (record->lcp_rect_info_) {
            first_screen_calculator->AssignImagePaintTime(
                record->hash, record->lcp_rect_info_->GetRootRectInfo(),
                timestamp);
        }
    }
}

void ImageRecordsManagerUtils::AssignImagePaintTimeFromRejectedImages(
    const base::TimeTicks& timestamp,
    unsigned last_queued_frame_index) {
    auto first_screen_calculator =
        image_records_manager_.frame_view_->GetPaintTimingDetector()
            .GetFirstScreenCalculator();
    while (!rejected_images_queued_for_paint_time_.empty()) {
        ImageRecord* record = rejected_images_queued_for_paint_time_.front();
        if (!record) {
            rejected_images_queued_for_paint_time_.pop_front();
            continue;
        }
        if (record->frame_index > last_queued_frame_index) {
            break;
        }
        if (record->queue_animated_paint) {
            record->first_animated_frame_time = timestamp;
            record->queue_animated_paint = false;
        }

        auto it = rejected_image_records_.find(record->hash);
        rejected_images_queued_for_paint_time_.pop_front();

        if (!record->loaded || !record->paint_time.is_null() ||
            it == rejected_image_records_.end()) {
            continue;
        }

        record->paint_time = timestamp;

        if (first_screen_calculator) {
            if (record->lcp_rect_info_) {
               first_screen_calculator->AssignImagePaintTime(
                    record->hash, record->lcp_rect_info_->GetRootRectInfo(),
                    timestamp);
            }
        }

        rejected_image_records_.erase(it);
    }
}

void ImageRecordsManagerUtils::RemoveRecordFromFirstScreenCalculator(
    MediaRecordIdHash record_id_hash) {
    auto it = rejected_image_records_.find(record_id_hash);
    if (it != rejected_image_records_.end()) {
        rejected_image_records_.erase(it);
    }

    auto first_screen_calculator =
        image_records_manager_.frame_view_->GetPaintTimingDetector()
            .GetFirstScreenCalculator();
    if (first_screen_calculator) {
        first_screen_calculator->RemoveImageRecord(record_id_hash);
    }
}

void ImageRecordsManagerUtils::QueueToMeasurePaintTimeForRejected(
    ImageRecord* record,
    unsigned current_frame_index) {
    CHECK(record);
    record->frame_index = current_frame_index;
    rejected_images_queued_for_paint_time_.push_back(record);
}

ImageRecord* ImageRecordsManagerUtils::GetRejectedImage(
    MediaRecordIdHash record_id_hash) {
    auto it = rejected_image_records_.find(record_id_hash);
    return it == rejected_image_records_.end() ? nullptr : it->value.Get();
}

bool ImageRecordsManagerUtils::IsRejectedDueToSize(
    MediaRecordIdHash record_id_hash) {
    return rejected_image_records_.Contains(record_id_hash);
}

bool ImageRecordsManagerUtils::OnFirstAnimatedFramePaintedForRejected(
    MediaRecordIdHash record_id_hash,
    unsigned current_frame_index) {
    auto it = rejected_image_records_.find(record_id_hash);
    if (it == rejected_image_records_.end()) {
        return false;
    }

    ImageRecord* record = it->value;
    DCHECK(record);

    if (record->media_timing &&
        !record->media_timing->GetFirstVideoFrameTime().is_null()) {
        record->first_animated_frame_time =
            record->media_timing->GetFirstVideoFrameTime();
    } else if (record->first_animated_frame_time.is_null()) {
        record->queue_animated_paint = true;
        QueueToMeasurePaintTimeForRejected(record, current_frame_index);
        return true;
    }
    return false;
}

void ImageRecordsManagerUtils::OnImageLoadedForRejected(
    MediaRecordIdHash record_id_hash,
    unsigned current_frame_index,
    const StyleImage* style_image) {
    auto it = rejected_image_records_.find(record_id_hash);
    if (it == rejected_image_records_.end()) {
        return;
    }

    ImageRecord* record = it->value;
    DCHECK(record);

    if (!style_image) {
        auto finished_it =
            image_records_manager_.image_finished_times_.find(record_id_hash);
        if (finished_it != image_records_manager_.image_finished_times_.end()) {
            record->load_time = finished_it->value;
            DCHECK(!record->load_time.is_null());
        }
    } else {
        Document* document =
            image_records_manager_.frame_view_->GetFrame().GetDocument();
        if (document && document->domWindow()) {
            record->load_time = ImageElementTiming::From(*document->domWindow())
                                    .GetBackgroundImageLoadTime(style_image);
            record->origin_clean = style_image->IsOriginClean();
        }
    }

    image_records_manager_.SetLoaded(record);
    QueueToMeasurePaintTimeForRejected(record, current_frame_index);
}

void ImageRecordsManagerUtils::NotifyImagePaintForFirstScreenCalculator(
    MediaRecordIdHash hash,
    ImageRecord* record,
    bool is_video) {
    auto first_screen_calculator =
        image_records_manager_.frame_view_->GetPaintTimingDetector()
            .GetFirstScreenCalculator();
    if (first_screen_calculator) {
        first_screen_calculator->NotifyImagePaint(
            hash, record,
            image_records_manager_.frame_view_->GetPaintTimingDetector()
                .GetImagePaintTimingDetector()
                .GetViewportSize(),
            is_video);
    }
}

void ImageRecordsManagerUtils::InsertRejectedImageRecords(
    MediaRecordIdHash hash,
    ImageRecord* record) {
    rejected_image_records_.insert(hash, record);
}

void ImageRecordsManagerUtils::ClearRejectedImagesQueuedForPaintTime() {
    rejected_images_queued_for_paint_time_.clear();
}

void ImageRecordsManagerUtils::ClearRejectedImageRecords() {
    rejected_image_records_.clear();
}

void ImageRecordsManagerUtils::TraceRejectedImages(Visitor* visitor) const {
    visitor->Trace(rejected_image_records_);
    visitor->Trace(rejected_images_queued_for_paint_time_);
}

void ImageRecordsManagerUtils::GetAddedEntryInLatestFrameByRejectedImage(
    const MediaTiming& media_timing,
    const LayoutObject& object,
    MediaRecordIdHash record_id_hash,
    unsigned frame_index,
    bool& added_entry_in_latest_frame,
    const StyleImage* style_image,
    const gfx::Rect& image_border,
    const PropertyTreeStateOrAlias& current_paint_chunk_properties) {
    ImageRecord* rejected_record = GetRejectedImage(record_id_hash);
    if (rejected_record) {
        if (media_timing.IsPaintedFirstFrame() &&
            RuntimeEnabledFeatures::LCPAnimatedImagesWebExposedEnabled()) {
            added_entry_in_latest_frame |=
                OnFirstAnimatedFramePaintedForRejected(record_id_hash,
                                                       frame_index);
        }
        if (!rejected_record->loaded &&
            media_timing.IsSufficientContentLoadedForPaint()) {
            OnImageLoadedForRejected(record_id_hash, frame_index, style_image);
            added_entry_in_latest_frame = true;
            if (std::optional<PaintTimingVisualizer>& visualizer =
                    image_records_manager_.frame_view_->GetPaintTimingDetector()
                        .Visualizer()) {
                gfx::RectF mapped_visual_rect =
                    image_records_manager_.frame_view_->GetPaintTimingDetector()
                        .CalculateVisualRect(image_border,
                                             current_paint_chunk_properties);
                visualizer->DumpImageDebuggingRect(
                    object, mapped_visual_rect,
                    media_timing.IsSufficientContentLoadedForPaint(),
                    media_timing.Url());
            }
        }
    }
}

void ImageRecordsManagerUtils::GetAddedEntryInLatestFrameByRejectedDueToSize(
    const MediaTiming& media_timing,
    MediaRecordIdHash record_id_hash,
    unsigned frame_index,
    bool& added_entry_in_latest_frame,
    const StyleImage* style_image) {
    if (IsRejectedDueToSize(record_id_hash)) {
        if (media_timing.IsPaintedFirstFrame() &&
            RuntimeEnabledFeatures::LCPAnimatedImagesWebExposedEnabled()) {
            added_entry_in_latest_frame |=
                OnFirstAnimatedFramePaintedForRejected(record_id_hash,
                                                       frame_index);
        }
        if (media_timing.IsSufficientContentLoadedForPaint()) {
            OnImageLoadedForRejected(record_id_hash, frame_index, style_image);
            added_entry_in_latest_frame = true;
        }
    }
}
#endif

}  // namespace blink