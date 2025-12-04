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
const float NINETY_PERCENT = 0.9f;

void FirstScreenCalculator::OnFirstScreenInvoked() {
  const base::TimeDelta as_time_delta =
      first_screen_paint_time_ - base::TimeTicks();
  base::TimeTicks navigation_start_time = base::TimeTicks();
  if (frame_view_) {
    DocumentLoader* loader =
        frame_view_->GetFrame().Loader().GetDocumentLoader();
    if (loader) {
      navigation_start_time = loader->GetTiming().NavigationStart();
#if BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT)
      if (navigation_start_time_ != navigation_start_time &&
          !first_screen_paint_time_.is_null()) {
        navigation_start_time_ = navigation_start_time;
        frame_view_->GetFrame().OnFirstScreenPaint(loader->GetUrl().GetString(),
                                                   navigation_start_time_,
                                                   first_screen_paint_time_);
      }
#endif
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

void FirstScreenCalculator::RestartTimerForFirstScreenDetection() {
  timer_.Stop();
  if (user_scrolled_) {
    return;
  }

  if (!frame_view_) {
    return;
  }
  auto window = frame_view_->GetFrame().DomWindow();
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
               base::BindOnce(&FirstScreenCalculator::OnFirstScreenInvoked,
                              weak_factory_.GetSafeRef()));
}

void FirstScreenCalculator::NotifyImagePaint(
    MediaRecordIdHash record_id_hash,
    const ImageRecord* record,
    std::optional<uint64_t> viewport_size,
    bool is_video) {
  if (!record || user_scrolled_ || !record->lcp_rect_info_) {
    return;
  }
  gfx::Rect root_rect_info = record->lcp_rect_info_->GetRootRectInfo();
  if (is_video) {
    root_rect_info.set_x(root_rect_info.x() / 2);
    root_rect_info.set_y(root_rect_info.y() / 2);
  }
  if (image_rects_map_.empty()) {
    image_rects_map_.insert(
        {record_id_hash, {root_rect_info, record->paint_time}});
    RestartTimerForFirstScreenDetection();
    return;
  }

  for (auto it = image_rects_map_.begin(); it != image_rects_map_.end(); ++it) {
    uint64_t rect_size = static_cast<uint64_t>(it->second.rect_.width() *
                                               it->second.rect_.height());
    if (it->first == record_id_hash) {
      return;
    }

    if (it->second.rect_ == root_rect_info &&
        rect_size < *viewport_size * NINETY_PERCENT) {
      return;
    }

    if (it->second.rect_.Contains(root_rect_info) &&
        rect_size < *viewport_size * NINETY_PERCENT) {
      return;
    }
  }
  image_rects_map_.insert(
      {record_id_hash, {root_rect_info, record->paint_time}});
  RestartTimerForFirstScreenDetection();
}

void FirstScreenCalculator::NotifyTextPaint(const TextRecord* record,
                                            base::TimeTicks timestamp) {
  if (!record || user_scrolled_ || !record->lcp_rect_info_) {
    return;
  }

  if (text_paint_rect_.empty()) {
    text_paint_rect_.emplace_back(PaintRectInfo(
        record->lcp_rect_info_->GetRootRectInfo(), record->paint_time));
    if (first_screen_paint_time_.is_null() || timestamp > first_screen_paint_time_) {
      first_screen_paint_time_ = timestamp;
    }
    RestartTimerForFirstScreenDetection();
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
  if (first_screen_paint_time_.is_null() || timestamp > first_screen_paint_time_) {
      first_screen_paint_time_ = timestamp;
  }
  RestartTimerForFirstScreenDetection();
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
    if (first_screen_paint_time_.is_null() || timestamp > first_screen_paint_time_) {
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

void FirstScreenCalculator::OnUserScroll() {
  user_scrolled_ = true;
  OnFirstScreenInvoked();
#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  if (frame_view_) {
    auto detector = frame_view_->GetFrame().GetBlankScreenDetector();
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