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
#include "third_party/blink/renderer/core/loader/document_loader.h"
#include "third_party/blink/renderer/core/paint/timing/image_paint_timing_detector.h"
#include "third_party/blink/renderer/core/paint/timing/text_paint_timing_detector.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace blink {

const int32_t DEFAULT_TASK_DELAY_MS = 3000;

void FirstScreenCalculator::OnFirstScreenInvoked() {
  const base::TimeDelta as_time_delta =
      first_screen_paint_time_ - base::TimeTicks();
  base::TimeTicks navigation_delta = base::TimeTicks();
  if (frame_view_) {
    DocumentLoader* loader =
        frame_view_->GetFrame().Loader().GetDocumentLoader();
    if (loader) {
      navigation_delta = loader->GetTiming().NavigationStart();
    }
  }

  if (OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetSystemPropertiesInstance()
          .GetBoolParameter("web.debug.dumpfsprect", false)) {
    const base::TimeDelta paint_time_delta =
        first_screen_paint_time_ - navigation_delta;

    if (first_screen_paint_time_ < navigation_delta) {
      return;
    }
    std::string fsp_time_str =
        std::to_string(paint_time_delta.InMilliseconds());
    if (frame_view_) {
      std::string code1 =
          R"(Performance.mark("First Screen Paint",{ startTime : )" +
          std::to_string(paint_time_delta.InMilliseconds()) + ", ";
      std::string code2 =
          R"(detail : {devtools: {dataType: "marker", color: "secondary", properties: [ ["time stamp", )" +
          fsp_time_str;
      std::string code3 = R"( ms"] ], tooltipText: "FSP" } } } ))";
      std::string code = code1 + code2 + code3;
      bool ret = frame_view_->RunJavaScriptForFSP(code);
      if (!ret) {
        return;
      }
    }

    DumpImageRect();
    DumpTextRect();
  }
}

void FirstScreenCalculator::DumpImageRect() {
  for (auto it = image_rects_map_.begin(); it != image_rects_map_.end(); ++it) {
    LOG(INFO) << "DumpImageRect, record_id_hash: " << it->first
              << ", rect = " << it->second.rect_.ToString()
              << ", paint time = " << it->second.paint_time_;
  }
}

void FirstScreenCalculator::DumpTextRect() {
  for (auto it = text_paint_rect_.begin(); it != text_paint_rect_.end(); ++it) {
    LOG(INFO) << "DumpTextRect, rect = " << it->rect_.ToString()
              << ", paint time =  " << it->paint_time_;
  }
}

void FirstScreenCalculator::RestartTimerForFirstScreenDetection(
    base::TimeDelta delay) {
  timer_.Stop();
  if (user_scrolled_) {
    return;
  }
  timer_.Start(FROM_HERE, delay,
               base::BindOnce(&FirstScreenCalculator::OnFirstScreenInvoked,
                              weak_factory_.GetWeakPtr()));
}

void FirstScreenCalculator::NotifyImagePaint(
    MediaRecordIdHash record_id_hash,
    const ImageRecord* record,
    std::optional<uint64_t> viewport_size) {
  if (!record || user_scrolled_) {
    return;
  }

  if (image_rects_map_.empty()) {
    image_rects_map_.insert(
        {record_id_hash,
         {record->lcp_rect_info_->GetRootRectInfo(), record->paint_time}});
    return;
  }

  for (auto it = image_rects_map_.begin(); it != image_rects_map_.end(); ++it) {
    uint64_t rect_size = static_cast<uint64_t>(it->second.rect_.width() *
                                               it->second.rect_.height());
    if (it->first == record_id_hash) {
      return;
    }

    if (it->second.rect_ == record->lcp_rect_info_->GetRootRectInfo() &&
        rect_size < *viewport_size) {
      return;
    }

    if (it->second.rect_.Contains(record->lcp_rect_info_->GetRootRectInfo()) &&
        rect_size < *viewport_size) {
      return;
    }
  }
  image_rects_map_.insert(
      {record_id_hash,
       {record->lcp_rect_info_->GetRootRectInfo(), record->paint_time}});
}

void FirstScreenCalculator::NotifyTextPaint(const TextRecord* record,
                                            base::TimeTicks timestamp) {
  if (!record || user_scrolled_) {
    return;
  }

  if (text_paint_rect_.empty()) {
    text_paint_rect_.emplace_back(PaintRectInfo(
        record->lcp_rect_info_->GetRootRectInfo(), record->paint_time));
    RestartTimerForFirstScreenDetection(base::Milliseconds(DEFAULT_TASK_DELAY_MS));
    return;
  }
  for (auto it = text_paint_rect_.begin(); it != text_paint_rect_.end(); ++it) {
    if (it->rect_ == record->lcp_rect_info_->GetRootRectInfo()) {
      return;
    }

    if (it->rect_.Contains(record->lcp_rect_info_->GetRootRectInfo())) {
      return;
    }
  }

  text_paint_rect_.emplace_back(PaintRectInfo(
      record->lcp_rect_info_->GetRootRectInfo(), record->paint_time));

  for (auto it = text_paint_rect_.begin(); it != text_paint_rect_.end(); ++it) {
    if (it->rect_ != record->lcp_rect_info_->GetRootRectInfo() &&
        record->lcp_rect_info_->GetRootRectInfo().Contains(it->rect_)) {
      return;
    }

    if (it->rect_ != record->lcp_rect_info_->GetRootRectInfo() &&
        it->rect_.Intersects(record->lcp_rect_info_->GetRootRectInfo())) {
      return;
    }
  }

  if (first_screen_paint_time_.is_null() ||
      first_screen_paint_time_ < timestamp) {
    first_screen_paint_time_ = timestamp;
  }
  RestartTimerForFirstScreenDetection(base::Milliseconds(DEFAULT_TASK_DELAY_MS));
}

void FirstScreenCalculator::AssignImagePaintTime(
    MediaRecordIdHash record_id_hash,
    const gfx::Rect& rect,
    base::TimeTicks timestamp) {
  const auto& it = image_rects_map_.find(record_id_hash);
  if (it == image_rects_map_.end()) {
    return;
  }
  if (!it->second.paint_time_.is_null()) {
    return;
  }

  image_rects_map_[record_id_hash] = PaintRectInfo(rect, timestamp);
  if (!user_scrolled_ && !timestamp.is_null()) {
    if (first_screen_paint_time_.is_null() ||
        first_screen_paint_time_ < timestamp) {
      first_screen_paint_time_ = timestamp;
    }
    RestartTimerForFirstScreenDetection(base::Milliseconds(DEFAULT_TASK_DELAY_MS));
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

void FirstScreenCalculator::OnUserScroll() {
  user_scrolled_ = true;
  if (frame_view_) {
    auto detector = frame_view_->GetFrame().GetBlankScreenDetector();
    if (detector) {
      detector->OnInputOrScroll();
    }
  }
}

bool FirstScreenCalculator::HasUserScrolled() const {
  return user_scrolled_;
}

void FirstScreenCalculator::RestartRecordingFirstScreenPaint() {
  user_scrolled_ = false;
  first_screen_paint_time_ = base::TimeTicks();
  image_rects_map_.clear();
  text_paint_rect_.clear();
}

void FirstScreenCalculator::GetPaintRects(std::vector<gfx::Rect>& paint_rects) {
  for (auto it = image_rects_map_.begin(); it != image_rects_map_.end(); ++it) {
    if (it->second.paint_time_.is_null()) {
      continue;
    }
    paint_rects.emplace_back(it->second.rect_);
  }
  for (auto it = text_paint_rect_.begin(); it != text_paint_rect_.end(); ++it) {
    if (it->paint_time_.is_null()) {
      continue;
    }
    paint_rects.emplace_back(it->rect_);
  }
}
}  // namespace blink