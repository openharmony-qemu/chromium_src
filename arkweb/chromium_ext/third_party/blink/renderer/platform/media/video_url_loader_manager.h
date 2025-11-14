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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_VIDEO_URL_LOADER_MANAGER_H
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_VIDEO_URL_LOADER_MANAGER_H

#include <list>
#include "base/logging.h"
#include "base/memory/weak_ptr.h"

#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/public/platform/web_url_request.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_video_priority.h"
#include "third_party/blink/renderer/platform/heap/collection_support/heap_vector.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/heap/member.h"
#include "third_party/blink/renderer/platform/allow_discouraged_type.h"

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/media/video_url_loader_impl.h"

namespace blink {
class WebAssociatedURLLoaderClient;

struct PriorityLoader {
  PriorityLoader(base::WeakPtr<VideoURLLoaderImpl> load,
                 std::string id,
                 int64_t start,
                 WebURLRequest request,
                 base::WeakPtr<WebAssociatedURLLoaderClient> client);

  PriorityLoader(PriorityLoader&&) = default;

  std::string AsHumanReadableString() const;

  bool operator<(const PriorityLoader& other) const;

  base::WeakPtr<VideoURLLoaderImpl> loader_;
  uint16_t priority_;
  std::string id_;
  int64_t start_;
  WebURLRequest request_;
  base::WeakPtr<WebAssociatedURLLoaderClient> client_;
};

class VideoUrlLoaderManager {
 public:
  void AddUrlLoader(PriorityLoader);
  void RemoveUrlLoader(base::WeakPtr<VideoURLLoaderImpl>);
  void SetVideoPriority(const HeapVector<Member<VideoPriority>>&);
  void SetVideoIsPlaying(std::string id, bool playing);

 private:
  void UpdatePriority4EnsurePlaying(std::string id,
                                    uint16_t from_priority,
                                    uint16_t to_priority);
  void UpdateUrlLoader();
  void UpdateUrlLoaderList(std::list<PriorityLoader>::iterator& it);
  void StartPendingLoaderIfNeeded();
  bool RemoveLoaderFromList(std::list<PriorityLoader>*,
                            base::WeakPtr<VideoURLLoaderImpl>);

  std::map<std::string, uint16_t> loader_priority_map_
    ALLOW_DISCOURAGED_TYPE("Set video priority");
  std::list<PriorityLoader> pending_list_;
  std::list<PriorityLoader> executing_list_;
  std::set<std::string> playing_video_set_
    ALLOW_DISCOURAGED_TYPE("Set playing video");
  
  void PrintVideoPriority() const;
  base::WeakPtrFactory<VideoUrlLoaderManager> weak_factory_{this};
};

}  // namespace blink

#endif // THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_VIDEO_URL_LOADER_MANAGER_H
