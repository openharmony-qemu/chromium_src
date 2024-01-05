/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "background_task_policy.h"

#include <memory>

#include "background_task_adapter.h"
#include "ohos_nweb/browser/performance_manager/mechanisms/background_task_holder.h"

namespace performance_manager::policies {
using namespace OHOS::NWeb;

enum class RequestBackgroundTaskReason : int32_t {
  NEED_BG_TASK = 0,
  NO_NEED_BG_TASK,
  NO_CHANGE_BG_TASK,
};

const std::string BG_TASK_TAG = "[BGTASKPOLICY]";

BackgroundTaskPolicy::BackgroundTaskPolicy()
    : background_task_holder_(
          std::make_unique<mechanism::BackgroundTaskHolder>()),
      is_request_background_task_(false),
      visible_page_num_(0),
      media_playing_num_(0),
      audio_state_num_(0) {}
BackgroundTaskPolicy::~BackgroundTaskPolicy() = default;

void BackgroundTaskPolicy::OnBeforeGraphDestroyed(Graph* graph) {
  if (graph == nullptr) {
    LOG(ERROR) << BG_TASK_TAG << " graph is null";
    return;
  }
  graph->RemovePageNodeObserver(this);
  graph->RemoveGraphObserver(this);
}

void BackgroundTaskPolicy::OnPassedToGraph(Graph* graph) {
  if (graph == nullptr) {
    LOG(ERROR) << BG_TASK_TAG << " graph is null";
    return;
  }

  LOG(INFO) << BG_TASK_TAG << " OnPassedToGraph";
  graph->AddGraphObserver(this);
  graph->AddPageNodeObserver(this);
}

void BackgroundTaskPolicy::OnPageNodeAdded(const PageNode* page_node) {
  if (page_node == nullptr) {
    LOG(ERROR) << BG_TASK_TAG << " page_node is null";
    return;
  }

  LOG(INFO) << BG_TASK_TAG << " OnPageNodeAdded";
  visible_page_num_++;
  MaybeChangeBackgroundTask(page_node);
}

void BackgroundTaskPolicy::OnBeforePageNodeRemoved(const PageNode* page_node) {}

void BackgroundTaskPolicy::OnIsVisibleChanged(const PageNode* page_node) {
  if (page_node == nullptr) {
    LOG(ERROR) << BG_TASK_TAG << " page_node is null";
    return;
  }

  int visible_num = page_node->IsVisible() ? 1 : -1;
  visible_page_num_ += visible_num;
  if (visible_page_num_ < 0) {
    visible_page_num_ = 0;
  }
  LOG(INFO) << BG_TASK_TAG << " OnIsVisibleChanged "
            << (visible_num > 0 ? "true" : "false")
            << ", visible_page_num: " << visible_page_num_;
  MaybeChangeBackgroundTask(page_node);
}

void BackgroundTaskPolicy::OnIsMediaPlayingChanged(const PageNode* page_node) {
  if (page_node == nullptr) {
    LOG(ERROR) << BG_TASK_TAG << " page_node is null";
    return;
  }

  int media_playing_num = page_node->IsMediaPlaying() ? 1 : -1;
  media_playing_num_ += media_playing_num;
  if (media_playing_num_ < 0) {
    media_playing_num_ = 0;
  }
  LOG(INFO) << BG_TASK_TAG << " OnIsMediaPlayingChanged "
          << (page_node->IsMediaPlaying() ? "true" : "false")
          << ", media_playing_num: " << media_playing_num_;
  MaybeChangeBackgroundTask(page_node);
}

void BackgroundTaskPolicy::OnIsAudibleChanged(const PageNode* page_node) {
  if (page_node == nullptr) {
    LOG(ERROR) << BG_TASK_TAG << " page_node is null";
    return;
  }

  int audio_state_num = page_node->IsAudible() ? 1 : -1;
  audio_state_num_ += audio_state_num;
  if (audio_state_num_ < 0) {
    audio_state_num_ = 0;
  }
  LOG(INFO) << BG_TASK_TAG << " OnIsAudibleChanged "
          << (page_node->IsAudible() ? "true" : "false")
          << ", audio_state_num: " << audio_state_num_;
  MaybeChangeBackgroundTask(page_node);
}

void BackgroundTaskPolicy::MaybeChangeBackgroundTask(
    const PageNode* page_node) {
  RequestBackgroundTaskReason reason =
      RequestBackgroundTaskReason::NO_CHANGE_BG_TASK;
  if (is_request_background_task_ &&
      (visible_page_num_ > 0 ||
       (media_playing_num_ == 0 && audio_state_num_ == 0))) {
    reason = RequestBackgroundTaskReason::NO_NEED_BG_TASK;
  } else if (!is_request_background_task_ &&
             (visible_page_num_ == 0 &&
              (media_playing_num_ > 0 || audio_state_num_ > 0))) {
    reason = RequestBackgroundTaskReason::NEED_BG_TASK;
  }

  if (reason == RequestBackgroundTaskReason::NO_CHANGE_BG_TASK) {
    LOG(INFO) << BG_TASK_TAG << " no change";
    return;
  }

  bool need_request = reason == RequestBackgroundTaskReason::NEED_BG_TASK;
  bool ret = background_task_holder_->MaybeRequestBackgroundRunning(
      need_request, BackgroundModeAdapter::AUDIO_PLAYBACK);
  is_request_background_task_ = need_request;
  if (ret) {
    LOG(INFO) << BG_TASK_TAG << " request bg task success, reason: "
              << static_cast<int32_t>(reason);
  } else {
    LOG(INFO) << BG_TASK_TAG << " request bg task failed";
  }
}
}  // namespace performance_manager::policies
                                             