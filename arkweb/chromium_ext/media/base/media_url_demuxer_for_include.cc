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

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
#include "media/base/ranges.h"
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

namespace media {

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void MediaUrlDemuxer::ForwardBufferedEndTimeChangeToDemuxerHost(
    base::TimeDelta buffered_time) {
  DCHECK(host_);
  DCHECK(task_runner_->RunsTasksInCurrentSequence());
  Ranges<base::TimeDelta> buffered;
  buffered.Add(base::TimeDelta(), buffered_time);
  host_->OnBufferedTimeRangesChanged(buffered);
}

void MediaUrlDemuxer::SetPreloadType(uint32_t preload_type) {
  params_.custom_media_url_params.preload_type = preload_type;
}

void MediaUrlDemuxer::SetMediaSourceType(uint32_t media_source_type) {
  params_.custom_media_url_params.media_source_type = media_source_type;
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

}  // namespace media
