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

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/media/video_url_loader_manager.h"

#include "base/hash/hash.h"
#include "base/task/single_thread_task_runner.h"
#include "third_party/blink/public/web/web_associated_url_loader_client.h"

namespace blink {
// The priority of the video being played, the highest priority for H5 set to 1.
static const uint16_t kPlayingPriority = 0;

// Default priority, the priority will not trigger a download event.
static const uint16_t kDefaultPriority = UINT16_MAX;

PriorityLoader::PriorityLoader(base::WeakPtr<VideoURLLoaderImpl> load,
                               std::string id,
                               int64_t start,
                               WebURLRequest request,
                               base::WeakPtr<WebAssociatedURLLoaderClient> client)
    : loader_(load),
      id_(id),
      start_(start),
      request_(std::move(request)),
      client_(client) {}

std::string PriorityLoader::AsHumanReadableString() const {
  std::ostringstream s;
  s << "[VideoOpt: pri=" << priority_
    << ", start=" << start_ << "]"
    << "(hash" << std::hex << base::FastHash(base::byte_span_from_ref(this)) << ")";

  return s.str();
}

bool PriorityLoader::operator<(const PriorityLoader& other) const {
  if (priority_ < other.priority_) {
    return true;
  } else if (priority_ > other.priority_) {
    return false;
  }

  if (id_ == other.id_) {
    if (start_ < other.start_) {
      return true;
    } else {
      return false;
    }
  }

  return false;
}

void VideoUrlLoaderManager::SetVideoIsPlaying(std::string id, bool playing) {
  if (playing_video_set_.count(id) && !playing) {
    UpdatePriority4EnsurePlaying(id, kPlayingPriority, kDefaultPriority);
    playing_video_set_.erase(id);
    return;
  }

  if (!playing_video_set_.count(id) && playing) {
    UpdatePriority4EnsurePlaying(id, kDefaultPriority, kPlayingPriority);
    playing_video_set_.emplace(id);
  }
}

void VideoUrlLoaderManager::UpdatePriority4EnsurePlaying(std::string id,
                                                         uint16_t from_priority,
                                                         uint16_t to_priority) {
  for (std::list<PriorityLoader>::iterator it = executing_list_.begin();
       it != executing_list_.end();) {
    if (!it->loader_) {
      it = executing_list_.erase(it);
    } else {
      if (it->id_ != id) {
        ++it;
        continue;
      }

      if (it->priority_ == from_priority) {
        it->priority_ = to_priority;
      } else {
        return;
      }
      ++it;
    }
  }

  for (std::list<PriorityLoader>::iterator it = pending_list_.begin();
       it != pending_list_.end();) {
    if (!it->loader_) {
      it = pending_list_.erase(it);
    } else {
      if (it->id_ != id) {
        ++it;
        continue;
      }

      if (it->priority_ == from_priority) {
        it->priority_ = to_priority;
      } else {
        return;
      }
      ++it;
    }
  }

  pending_list_.sort();

  StartPendingLoaderIfNeeded();
}

void VideoUrlLoaderManager::SetVideoPriority(
    const HeapVector<Member<VideoPriority>>& vec) {
  loader_priority_map_.clear();
  for (auto it : vec) {
    if (it) {
      if (it->priority() == kPlayingPriority) {
        continue;
      }
      loader_priority_map_[it->id().Utf8()] = it->priority();
    }
  }
  UpdateUrlLoader();
}

void VideoUrlLoaderManager::UpdateUrlLoader() {
  for (std::list<PriorityLoader>::iterator it = executing_list_.begin();
       it != executing_list_.end();) {
    if (!it->loader_) {
      it = executing_list_.erase(it);
    } else {
      UpdateUrlLoaderList(it);
      ++it;
    }
  }

  for (std::list<PriorityLoader>::iterator it = pending_list_.begin();
       it != pending_list_.end();) {
    if (!it->loader_) {
      it = pending_list_.erase(it);
    } else {
      UpdateUrlLoaderList(it);
      ++it;
    }
  }

  pending_list_.sort();
  StartPendingLoaderIfNeeded();
}

void VideoUrlLoaderManager::UpdateUrlLoaderList(std::list<PriorityLoader>::iterator& it) {
  if (loader_priority_map_.count(it->id_)) {
    it->priority_ = loader_priority_map_[it->id_];
  } else {
    if (playing_video_set_.count(it->id_)) {
      it->priority_ = kPlayingPriority;
    } else {
      it->priority_ = kDefaultPriority;
    }
  }
}

void VideoUrlLoaderManager::AddUrlLoader(PriorityLoader priLoader) {
  if (loader_priority_map_.count(priLoader.id_)) {
    priLoader.priority_ = loader_priority_map_[priLoader.id_];
  } else {
    if (playing_video_set_.count(priLoader.id_)) {
      priLoader.priority_ = kPlayingPriority;
    } else {
      priLoader.priority_ = kDefaultPriority;
    }
  }

  pending_list_.push_back(std::move(priLoader));
  pending_list_.sort();

  StartPendingLoaderIfNeeded();
}

void VideoUrlLoaderManager::RemoveUrlLoader(
    base::WeakPtr<VideoURLLoaderImpl> loader) {
  if (RemoveLoaderFromList(&executing_list_, loader)) {
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(FROM_HERE,
        base::BindOnce(&VideoUrlLoaderManager::StartPendingLoaderIfNeeded,
            weak_factory_.GetWeakPtr()));
    return;
  }

  RemoveLoaderFromList(&pending_list_, loader);
}

void VideoUrlLoaderManager::StartPendingLoaderIfNeeded() {
  std::list<PriorityLoader>::iterator pit = pending_list_.begin();
  while (pit != pending_list_.end() && (!pit->loader_ || !pit->client_)) {
    LOG(ERROR) << "VideoOpt: pending loader error:" << pit->AsHumanReadableString()
               << "; loader=" << (!!pit->loader_) << "; client=" << (!!pit->client_);
    pit = pending_list_.erase(pit);
  }

  if (pit == pending_list_.end() || pit->priority_ == kDefaultPriority) {
    return;
  }

  std::list<PriorityLoader>::iterator eit = executing_list_.begin();
  if (eit != executing_list_.end()) {
    if (*eit < *pit) {
      return;
    }
    if (eit->loader_ && eit->client_) {
      eit->loader_->Cancel();
      eit->client_->DidReStart();
    } else {
      LOG(ERROR) << "VideoOpt: executing loader error:" << eit->AsHumanReadableString()
                 << "; loader=" << (!!eit->loader_) << "; client=" << (!!eit->client_);
    }
    executing_list_.erase(eit);
  }

  executing_list_.push_back(std::move(*pit));
  pending_list_.erase(pit);
  auto& iter = executing_list_.back();

  iter.loader_->LoadAsynchronously(iter.request_, iter.client_.get());

  PrintVideoPriority();
  // Beware, `pit`, `pit->loader_` and `pit->client_` may be deleted after LoadAsynchronously.
}

bool VideoUrlLoaderManager::RemoveLoaderFromList(
    std::list<PriorityLoader>* lst,
    base::WeakPtr<VideoURLLoaderImpl> loader) {
  if (!loader) {
    return false;
  }

  for (std::list<PriorityLoader>::iterator it = lst->begin();
       it != lst->end();) {
    if (!it->loader_) {
      it = lst->erase(it);
    } else {
      if (it->loader_.get() == loader.get()) {
        lst->erase(it);
        return true;
      }
      ++it;
    }
  }
  return false;
}

void VideoUrlLoaderManager::PrintVideoPriority() const {
  LOG(DEBUG) << "VideoOpt, pending size:" << pending_list_.size();
  for (const auto& pendVideo : pending_list_) {
    LOG(DEBUG) << "VideoOpt, pending:" << pendVideo.AsHumanReadableString();
  }

  LOG(DEBUG) << "VideoOpt, execute size:" << executing_list_.size();
  for (const auto& executeVideo : executing_list_) {
    LOG(DEBUG) << "VideoOpt, execute:" << executeVideo.AsHumanReadableString();
  }
}

}  // namespace blink
