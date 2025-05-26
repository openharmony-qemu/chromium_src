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
namespace performance_manager {
#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
void PerformanceManagerTabHelper::MediaStartedPlaying(
    const PerformanceManagerTabHelper::MediaPlayerInfo& video_type,
    const content::MediaPlayerId& id) {
  LOG(INFO) << "MediaStartedPlaying video: " << video_type.has_video
            << " audio: " << video_type.has_audio;
  PerformanceManagerImpl::CallOnGraphImpl(
      FROM_HERE, base::BindOnce(&PageNodeImpl::SetIsMediaPlaying,
                                base::Unretained(primary_page_node()), true));
}

void PerformanceManagerTabHelper::MediaStoppedPlaying(
    const PerformanceManagerTabHelper::MediaPlayerInfo& video_type,
    const content::MediaPlayerId& id,
    PerformanceManagerTabHelper::MediaStoppedReason reason) {
  LOG(INFO)
      << "MediaStartedPlaying video: " << video_type.has_video
      << " audio: " << video_type.has_audio << " "
      << (reason ==
                  PerformanceManagerTabHelper::MediaStoppedReason::kUnspecified
              ? "kUnspecified"
              : "kReachedEndOfStream");
  PerformanceManagerImpl::CallOnGraphImpl(
      FROM_HERE, base::BindOnce(&PageNodeImpl::SetIsMediaPlaying,
                                base::Unretained(primary_page_node()), false));
}

void PerformanceManagerTabHelper::OneShotMediaPlayerStopped() {
  PerformanceManagerImpl::CallOnGraphImpl(
      FROM_HERE, base::BindOnce(&PageNodeImpl::OneShotMediaPlayerStopped,
                                base::Unretained(primary_page_node())));
}
#endif
}