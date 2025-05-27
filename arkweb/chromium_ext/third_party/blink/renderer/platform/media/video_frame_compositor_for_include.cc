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

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
const int kFrameFreezeTimeMs = 100;
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_MEDIA)
void VideoFrameCompositor::SetFinishPaintCallback(
    FinishPaintCallback callback) {
  finish_paint_cb_ = std::move(callback);
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
void VideoFrameCompositor::SetStartTime(int64_t start_time) {
  last_frame_time_ = start_time;
  is_playing_ = (start_time != 0);
}

int64_t VideoFrameCompositor::GetFreezeTime() {
  if (is_playing_ && last_frame_time_ > 0) {
    int64_t now =
        (base::Time::Now() - base::Time::UnixEpoch()).InMilliseconds();
    int64_t delta_time = now - last_frame_time_;
    if (delta_time > kFrameFreezeTimeMs) {
      total_freeze_time_ += delta_time;
    }
    last_frame_time_ = now;
  }
  int64_t freeze_time = total_freeze_time_;
  total_freeze_time_ = 0;
  return freeze_time;
}
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
void StatisticsTotalFreeTime(bool is_playing,
                             int64_t& last_frame_time,
                             int64_t& total_freeze_time) {
  int64_t now = (base::Time::Now() - base::Time::UnixEpoch()).InMilliseconds();
  if (is_playing && last_frame_time > 0) {
    int64_t delta_time = now - last_frame_time;
    if (delta_time > kFrameFreezeTimeMs) {
      total_freeze_time += delta_time;
    }
  }
  last_frame_time = now;
}
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE