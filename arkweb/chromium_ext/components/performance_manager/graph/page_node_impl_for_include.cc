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
void PageNodeImpl::SetIsMediaPlaying(bool is_media_playing) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (is_media_playing) {
    media_playing_num_++;
  } else {
    media_playing_num_--;
  }
  if (media_playing_num_ == 0 ||
      (is_media_playing && media_playing_num_ == 1)) {
    is_media_playing_.SetAndMaybeNotify(this, is_media_playing);
  }
}

void PageNodeImpl::OneShotMediaPlayerStopped() {
  for (auto& observer : GetObservers()) {
    observer.OnDecrementAudioNum(this);
  }
}

bool PageNodeImpl::is_media_playing() const {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  return is_media_playing_.value();
}

bool PageNodeImpl::IsMediaPlaying() const {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  return is_media_playing();
}
#endif
}