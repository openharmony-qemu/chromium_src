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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/timing/first_screen_calculator.h"

#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/timing/blank_screen_detector.h"
#include "base/memory/safe_ref.h"
#include "third_party/blink/renderer/bindings/modules/v8/v8_connection_type.h"
#include "third_party/blink/renderer/bindings/modules/v8/v8_effective_connection_type.h"
#include "third_party/blink/renderer/core/frame/navigator.h"
#include "third_party/blink/renderer/core/loader/document_loader.h"
#include "third_party/blink/renderer/core/paint/timing/image_paint_timing_detector.h"
#include "third_party/blink/renderer/core/paint/timing/text_paint_timing_detector.h"
#include "third_party/blink/renderer/modules/netinfo/network_information.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace blink {
const int32_t TASK_DELAY_MS_OFFLINE = 2000;
const int32_t TASK_DELAY_MS_4G = 3000;
const int32_t TASK_DELAY_MS_3G = 4000;
const int32_t TASK_DELAY_MS_2G = 5000;
const int32_t TASK_DELAY_MS_SLOW_2G = 6000;
const double BACKGROUND_IMAGE_THRESHOLD = 0.75;
const double NEARLY_FINISHED_THRESHOLD = 0.8;
const double SMALL_RECT_THRESHOLD = 0.01;

void FirstScreenCalculator::OnFirstScreenInvoked() {
#if BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT)
  const base::TimeDelta as_time_delta =
      first_screen_paint_time_ - base::TimeTicks();
  base::TimeTicks navigation_start_time = base::TimeTicks();
  if (local_frame_) {
    DocumentLoader* loader =
        local_frame_->Loader().GetDocumentLoader();
    if (loader) {
      navigation_start_time = loader->GetTiming().NavigationStart();
      if (navigation_start_time_ != navigation_start_time &&
          !first_screen_paint_time_.is_null()) {
        navigation_start_time_ = navigation_start_time;
        local_frame_->OnFirstScreenPaint(
            loader->GetUrl().GetString(), navigation_start_time_,
            first_screen_paint_time_);
      }
    }
  }

  if (OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetSystemPropertiesInstance()
          .GetBoolParameter("web.debug.dumpfsprect", false)) {
    const base::TimeDelta paint_time_delta =
        first_screen_paint_time_ - navigation_start_time;

    if (first_screen_paint_time_ < navigation_start_time) {
      return;
    }
    std::string fsp_time_str =
        std::to_string(paint_time_delta.InMilliseconds());
    if (local_frame_ && local_frame_->View()) {
      std::string code1 =
          R"(Performance.mark("First Screen Paint",{ startTime : )" +
          std::to_string(paint_time_delta.InMilliseconds()) + ", ";
      std::string code2 =
          R"(detail : {devtools: {dataType: "marker", color: "secondary", properties: [ ["time stamp", )" +
          fsp_time_str;
      std::string code3 = R"( ms"] ], tooltipText: "FSP" } } } ))";
      std::string code = code1 + code2 + code3;
      bool ret = local_frame_->View()->RunJavaScriptForFSP(code);
      if (!ret) {
        return;
      }
    }

    DumpImageRect();
    DumpTextRect();
  }
#endif
}

void FirstScreenCalculator::DumpImageRect() {
  for (auto it = image_rects_map_.begin(); it != image_rects_map_.end(); ++it) {
    LOG(INFO) << "DumpImageRect, record_id_hash: " << it->first
              << ", rect = " << it->second.rect_.ToString()
              << ", paint time = " << it->second.paint_time_;
  }
}

void FirstScreenCalculator::DumpTextRect() {
  for (auto it = text_paint_rects_.begin(); it != text_paint_rects_.end(); ++it) {
    LOG(INFO) << "DumpTextRect, rect = " << it->rect_.ToString()
              << ", paint time =  " << it->paint_time_;
  }
}

void FirstScreenCalculator::RestartTimerForFirstScreenDetection() {
  timer_.Stop();
  if (user_scrolled_) {
    return;
  }

  if (!local_frame_) {
    return;
  }
  auto window = local_frame_->DomWindow();
  if (!window || !window->navigator()) {
    return;
  }
  int32_t task_delay_ms = TASK_DELAY_MS_OFFLINE;
#if !defined(COMPONENT_BUILD)
  NetworkInformation* info = NetworkInformation::connection(
      *static_cast<NavigatorBase*>(window->navigator()));
  if (info && info->type() != V8ConnectionType::Enum::kNone) {
    switch (info->effectiveType().AsEnum()) {
      case V8EffectiveConnectionType::Enum::kSlow2G:
        task_delay_ms = TASK_DELAY_MS_SLOW_2G;
        break;
      case V8EffectiveConnectionType::Enum::k2G:
        task_delay_ms = TASK_DELAY_MS_2G;
        break;
      case V8EffectiveConnectionType::Enum::k3G:
        task_delay_ms = TASK_DELAY_MS_3G;
        break;
      case V8EffectiveConnectionType::Enum::k4G:
        task_delay_ms = TASK_DELAY_MS_4G;
        break;
      default:
        task_delay_ms = TASK_DELAY_MS_SLOW_2G;
        break;
    }
  }

#endif
  timer_.Start(FROM_HERE, base::Milliseconds(task_delay_ms),
               WTF::BindOnce(&FirstScreenCalculator::OnFirstScreenInvoked,
                             WrapWeakPersistent(this)));
}

bool FirstScreenCalculator::IsRectContainedByExistingRects(
    const gfx::Rect& rect) {
  for (auto it = image_rects_map_.begin(); it != image_rects_map_.end(); ++it) {
    if (it->second.rect_ == rect || it->second.rect_.Contains(rect)) {
      return true;
    }
  }
  for (auto it = text_paint_rects_.begin(); it != text_paint_rects_.end();
       ++it) {
    if (it->rect_ == rect || it->rect_.Contains(rect)) {
      return true;
    }
  }
  return false;
}

void FirstScreenCalculator::RemoveExistingRectsContainedByRect(
    const gfx::Rect& rect) {
  for (auto it = text_paint_rects_.begin(); it != text_paint_rects_.end();) {
    if (rect.Contains(it->rect_)) {
      it = text_paint_rects_.erase(it);
    } else {
      ++it;
    }
  }

  for (auto it = image_rects_map_.begin(); it != image_rects_map_.end();) {
    if (rect.Contains(it->second.rect_)) {
      it = image_rects_map_.erase(it);
    } else {
      ++it;
    }
  }
}

bool FirstScreenCalculator::DoesRectIntersectExistingRects(
    const gfx::Rect& rect) {
  for (auto it = text_paint_rects_.begin(); it != text_paint_rects_.end();
       ++it) {
    if (it->rect_ != rect && it->rect_.Intersects(rect)) {
      return true;
    }
  }

  for (auto it = image_rects_map_.begin(); it != image_rects_map_.end(); ++it) {
    if (it->second.rect_ != rect && it->second.rect_.Intersects(rect)) {
      return true;
    }
  }
  return false;
}

bool FirstScreenCalculator::IsRectTooSmallWhenNearlyFinished(
    const gfx::Rect& rect) {
  if (!nearly_finished_) {
    occupied_rect_.Union(rect);
    double occupied_ratio =
        occupied_rect_.size().GetArea() * 1.0 / viewport_rect_.size().GetArea();
    if (occupied_ratio > NEARLY_FINISHED_THRESHOLD) {
      nearly_finished_ = true;
    } else {
      return false;
    }
  }

  double rect_ratio =
      rect.size().GetArea() * 1.0 / viewport_rect_.size().GetArea();
  if (rect_ratio < SMALL_RECT_THRESHOLD) {
    return true;
  }
  return false;
}

void FirstScreenCalculator::NotifyImagePaint(
    MediaRecordIdHash record_id_hash,
    const ImageRecord* record,
    std::optional<uint64_t> viewport_size,
    bool is_video) {
  if (!record || user_scrolled_ || !record->lcp_rect_info_) {
    return;
  }
  gfx::Rect rect = record->lcp_rect_info_->GetRootRectInfo();
  if (!GetViewportAreaAndTrimRect(rect)) {
    return;
  }
  double image_ratio = rect.size().GetArea() * 1.0 / viewport_rect_.size().GetArea();
  if (image_ratio > BACKGROUND_IMAGE_THRESHOLD) {
    LOG(INFO) << "FirstScreenCalculator::NotifyImagePaint image_ratio "
              << image_ratio << " is too large.";
    if (!DoesRectIntersectExistingRects(rect)) {
      background_image_id_ = record_id_hash;
    }
    return;
  }
  if (is_video) {
    rect.set_x(rect.x() / 2);
    rect.set_y(rect.y() / 2);
  }

  if (image_rects_map_.empty()) {
    image_rects_map_.insert({record_id_hash, {rect, record->paint_time}});
    RestartTimerForFirstScreenDetection();
    return;
  }
  for (auto it = image_rects_map_.begin(); it != image_rects_map_.end(); ++it) {
    if (it->first == record_id_hash) {
      return;
    }
  }
  if (IsRectContainedByExistingRects(rect) ||
      IsRectTooSmallWhenNearlyFinished(rect)) {
    return;
  }
  RemoveExistingRectsContainedByRect(rect);
  image_rects_map_.insert({record_id_hash, {rect, record->paint_time}});
  if (DoesRectIntersectExistingRects(rect)) {
    intersected_image_ids_.emplace_back(record_id_hash);
    return;
  }
  RestartTimerForFirstScreenDetection();
}

void FirstScreenCalculator::NotifyTextPaint(const TextRecord* record,
                                            const base::TimeTicks& timestamp) {
  if (!record || user_scrolled_ || !record->lcp_rect_info_) {
    return;
  }
  gfx::Rect rect = record->lcp_rect_info_->GetRootRectInfo();
  if (!GetViewportAreaAndTrimRect(rect)) {
    return;
  }
  if (text_paint_rects_.empty()) {
    text_paint_rects_.emplace_back(PaintRectInfo(rect, record->paint_time));
    if (first_screen_paint_time_.is_null() ||
        timestamp > first_screen_paint_time_) {
      first_screen_paint_time_ = timestamp;
    }
    RestartTimerForFirstScreenDetection();
    return;
  }
  if (IsRectContainedByExistingRects(rect) ||
      IsRectTooSmallWhenNearlyFinished(rect)) {
    return;
  }
  RemoveExistingRectsContainedByRect(rect);
  text_paint_rects_.emplace_back(PaintRectInfo(rect, record->paint_time));
  if (DoesRectIntersectExistingRects(rect)) {
    return;
  }

  if (first_screen_paint_time_.is_null() ||
      timestamp > first_screen_paint_time_) {
    first_screen_paint_time_ = timestamp;
  }
  RestartTimerForFirstScreenDetection();
}

void FirstScreenCalculator::AssignImagePaintTime(
    MediaRecordIdHash record_id_hash,
    const gfx::Rect& rect,
    const base::TimeTicks& timestamp) {
  if (record_id_hash != background_image_id_) {
    const auto& it = image_rects_map_.find(record_id_hash);
    if (it == image_rects_map_.end() || !it->second.paint_time_.is_null()) {
      return;
    }

    image_rects_map_[record_id_hash] = PaintRectInfo(rect, timestamp);
    for (const auto& id : intersected_image_ids_) {
      if (id == record_id_hash) {
        return;
      }
    }
  }

  if (!user_scrolled_ && !timestamp.is_null()) {
    if (first_screen_paint_time_.is_null() ||
        timestamp > first_screen_paint_time_) {
      first_screen_paint_time_ = timestamp;
    }
    RestartTimerForFirstScreenDetection();
  }
}

bool FirstScreenCalculator::RemoveImageRecord(
    MediaRecordIdHash record_id_hash) {
  if (user_scrolled_) {
    return false;
  }

  auto it = image_rects_map_.find(record_id_hash);
  if (it == image_rects_map_.end()) {
    return false;
  }
  if (image_rects_map_[record_id_hash].paint_time_.is_null()) {
    image_rects_map_.erase(it);
    return true;
  }

  return false;
}

bool FirstScreenCalculator::GetViewportAreaAndTrimRect(gfx::Rect& rect) {
  if (local_frame_ && local_frame_->View() && local_frame_->View()->ViewportWidth() > 0 &&
      local_frame_->View()->ViewportHeight() > 0) {
    viewport_rect_.set_width(local_frame_->View()->ViewportWidth());
    viewport_rect_.set_height(local_frame_->View()->ViewportHeight());
  } else {
    return false;
  }
  rect.Intersect(viewport_rect_);
  if (!rect.size().GetArea()) {
    return false;
  }
  return true;
}

void FirstScreenCalculator::OnUserScroll() {
  user_scrolled_ = true;
  OnFirstScreenInvoked();
#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  if (local_frame_) {
    auto detector = local_frame_->GetBlankScreenDetector();
    if (detector) {
      detector->OnInputOrScroll();
    }
  }
#endif
}

bool FirstScreenCalculator::HasUserScrolled() const {
  return user_scrolled_;
}

void FirstScreenCalculator::RestartRecordingFirstScreenPaint() {
  user_scrolled_ = false;
  nearly_finished_ = false;
  first_screen_paint_time_ = base::TimeTicks();
  background_image_id_ = 0;
  image_rects_map_.clear();
  text_paint_rects_.clear();
  intersected_image_ids_.clear();
  occupied_rect_ = gfx::Rect();
  viewport_rect_ = gfx::Rect();
}

void FirstScreenCalculator::GetPaintRects(std::vector<gfx::Rect>& paint_rects) {
  for (auto it = image_rects_map_.begin(); it != image_rects_map_.end(); ++it) {
    if (it->second.paint_time_.is_null()) {
      continue;
    }
    paint_rects.emplace_back(it->second.rect_);
  }
  for (auto it = text_paint_rects_.begin(); it != text_paint_rects_.end(); ++it) {
    if (it->paint_time_.is_null()) {
      continue;
    }
    paint_rects.emplace_back(it->rect_);
  }
}

void FirstScreenCalculator::Trace(Visitor* visitor) const {
  visitor->Trace(local_frame_);
}
}  // namespace blink