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

// ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION
#include "third_party/blink/renderer/bindings/core/v8/v8_video_priority.h"
// ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

namespace blink {

bool Document::isVideoPrioritySupported() {
#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
  LOG(INFO) << "VideoOpt, Document receive query, isVideoPrioritySupported()";
  bool result = false;
  bool cloudCtrl = false;

  if (GetFrame()) {
    cloudCtrl = GetFrame()->IsVideoPrioritySupported();
    if (cloudCtrl) {
      result = GetFrame()->SetNewsFeedPageFitted();
    }
  }

  is_use_video_load_opt_ = result;
  LOG(INFO) << "VideoOpt: " << __func__ << " cloudCtrl=" << cloudCtrl
            << "; result=" << result;
  return result;
#else
  return false;
#endif
}

void Document::setVideoPriority(const HeapVector<Member<VideoPriority>>& vec) {
#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
  LOG(INFO) << "VideoOpt, Document receive setVideoPriority";
  if (GetFrame()) {
    GetFrame()->SetVideoPriority(vec);
  }
#endif
}

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
void Document::SetVideoIsPlaying(std::string id, bool playing) {
  LOG(DEBUG) << "VideoOpt, Document SetVideoIsPlaying, id: " << id << ", playing:" << playing;
  if (GetFrame()) {
    GetFrame()->SetVideoIsPlaying(id, playing);
  }
}

bool Document::IsUseVideoLoadOptimization() {
  return is_use_video_load_opt_;
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

}