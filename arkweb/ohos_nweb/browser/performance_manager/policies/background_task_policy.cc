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
#include "base/hash/hash.h"
#include "background_task_adapter.h"
#include "content/browser/scheduler/browser_task_executor.h"
#include "ohos_nweb/browser/performance_manager/mechanisms/background_task_holder.h"

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/command_line.h"
#include "base/memory/safe_ref.h"
#include "content/public/browser/web_contents.h"
#endif

namespace performance_manager::policies {
using namespace OHOS::NWeb;

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
namespace {
enum BackgroundTaskType {
  // web page audio play in background.
  AUDIO_PLAYBACK = 0,

  // web page web audio play in background.
  WEB_AUDIO_PLAYBACK = 1,
};
}  // namespace
#endif  // ARKWEB_PERFORMANCE_PERSISTENT_TASK

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

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
BackgroundTaskPolicy::~BackgroundTaskPolicy() {
  audio_context_players_num_.clear();
}
#else
BackgroundTaskPolicy::~BackgroundTaskPolicy() = default;
#endif // ARKWEB_PERFORMANCE_PERSISTENT_TASK

void BackgroundTaskPolicy::OnTakenFromGraph(Graph* graph) {
  if (graph == nullptr) {
    LOG(ERROR) << BG_TASK_TAG << " graph is null";
    return;
  }
  graph->RemovePageNodeObserver(this);
}

void BackgroundTaskPolicy::OnPassedToGraph(Graph* graph) {
  if (graph == nullptr) {
    LOG(ERROR) << BG_TASK_TAG << " graph is null";
    return;
  }

  LOG(INFO) << BG_TASK_TAG << " OnPassedToGraph";
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

  LOG(INFO) << BG_TASK_TAG << __FUNCTION__
            << " media avsession IsVisibleChanged: "
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
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__
            << ", OnIsMediaPlayingChanged "
            << (page_node->IsMediaPlaying() ? "true" : "false")
            << " page_node hash=" << std::hex << base::FastHash(base::byte_span_from_ref(page_node))
            << ", media_playing_num: " << media_playing_num_
            << " page_node->IsVisible=" << (page_node->IsVisible() ? "true" : "false");
  MaybeChangeBackgroundTask(page_node);
}

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
void BackgroundTaskPolicy::OnDecrementAudioNum(const PageNode* page_node) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession page_node hash="
            << std::hex << base::FastHash(base::byte_span_from_ref(page_node));
  if (page_node == nullptr) {
    LOG(ERROR) << BG_TASK_TAG << __FUNCTION__ << " page_node is null return";
    return;
  }
  LOG(INFO) << BG_TASK_TAG << "OnDecrementAudioNum IsMediaPlaying: " << page_node->IsMediaPlaying()
            << " IsAudible: " << page_node->IsAudible();

  media_playing_num_ -= static_cast<int>(page_node->IsMediaPlaying());
  if (media_playing_num_ < 0) {
    media_playing_num_ = 0;
  }
  audio_state_num_ -= static_cast<int>(page_node->IsAudible());
  if (audio_state_num_ < 0) {
    audio_state_num_ = 0;
  }
  if (audio_state_num_ == 0) {
    audio_context_players_num_.clear();
  }
  LOG(INFO) << BG_TASK_TAG << " OnDecrementAudioNum media_playing_num_: " << media_playing_num_
            << "audio_state_num_: "<< audio_state_num_;
  MaybeChangeBackgroundTask(page_node);
}
#endif

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
#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
  if (audio_state_num_ == 0) {
    audio_context_players_num_.clear();
  }
  if (!page_node->IsAudible()) {
    ProcessAudioContextPlayers(page_node);
  }
#endif
  LOG(INFO) << BG_TASK_TAG << " OnIsAudibleChanged "
            << (page_node->IsAudible() ? "true" : "false")
            << ", audio_state_num: " << audio_state_num_;
  MaybeChangeBackgroundTask(page_node);
}

void BackgroundTaskPolicy::MaybeChangeBackgroundTask(
    const PageNode* page_node) {
  LOG(INFO) << "BackgroundTaskPolicy::MaybeChangeBackgroundTask "
            << " page_node hash=" << std::hex << base::FastHash(base::byte_span_from_ref(page_node))
            << " visible_page_num_: " << visible_page_num_
            << " media_playing_num_: " << media_playing_num_
            << " audio_state_num_: " << audio_state_num_
            << " is_request_background_task_: " << is_request_background_task_;

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
  } else {
  }

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
    if ((reason == RequestBackgroundTaskReason::NEED_BG_TASK) &&
        !IsWebAudioRequestBackgroundRunning()) {
      LOG(INFO) << "is_web_audio_request is false";
      reason = RequestBackgroundTaskReason::NO_CHANGE_BG_TASK;
    }
    if (is_request_background_task_ && visible_page_num_ == 0 &&
        audio_context_players_num_.size() > 0 &&
        !IsWebAudioRequestBackgroundRunning()) {
      LOG(INFO) << "need cancel bg task";
      reason = RequestBackgroundTaskReason::NO_NEED_BG_TASK;
    }
#endif
  if (reason == RequestBackgroundTaskReason::NO_CHANGE_BG_TASK) {
    LOG(INFO) << BG_TASK_TAG << " no change return";
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

#if BUILDFLAG(ARKWEB_BGTASK)
void BackgroundTaskPolicy::SetBrowserForeground(const PageNode* page_node)
{
  LOG(INFO) << BG_TASK_TAG << "BackgroundTaskPolicy::" << __FUNCTION__;
  bool ret = background_task_holder_->MaybeRequestBackgroundRunning(false, BackgroundModeAdapter::AUDIO_PLAYBACK);
  if (ret) {
    LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << "request bg task success";

  } else {
    LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << "request bg task failed";
  }
}

void BackgroundTaskPolicy::SetBrowserBackground(const PageNode* page_node)
{
  LOG(INFO) << BG_TASK_TAG << "BackgroundTaskPolicy::" << __FUNCTION__;
  if (media_playing_num_ > 0 || audio_state_num_ > 0) {
#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
    if (!IsWebAudioRequestBackgroundRunning()) {
      LOG(INFO) << "is_web_audio_request is false, return";
      return;
    }
#endif
    bool ret = background_task_holder_->MaybeRequestBackgroundRunning(true, BackgroundModeAdapter::AUDIO_PLAYBACK);
    if (ret) {
      LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << "request bg task success";
    } else {
      LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << "request bg task failed";
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
void BackgroundTaskPolicy::OnAudioContextPlaybackStarted(content::GlobalRenderFrameHostId rfh_id,
                                                         int audio_context_id) {
  AudioContextIdPlayer player{rfh_id, audio_context_id};
  audio_context_players_num_.insert(player);
}

void BackgroundTaskPolicy::OnAudioContextPlaybackStopped(content::GlobalRenderFrameHostId rfh_id,
                                                         int audio_context_id) {
  AudioContextIdPlayer player{rfh_id, audio_context_id};
  audio_context_players_num_.erase(player);
}

bool BackgroundTaskPolicy::IsWebAudioRequestBackgroundRunning() {
  auto currentProcess = base::CommandLine::ForCurrentProcess();
  if (currentProcess &&
      !currentProcess->HasSwitch(switches::kEnableWebAudioBackgroundTask)) {
    return true;
  }
  LOG(INFO) << "audio_state_num_=" << audio_state_num_
            << ", audio_context_players_num_="
            << audio_context_players_num_.size();
  if (static_cast<size_t>(audio_state_num_) !=
      audio_context_players_num_.size()) {
    return true;
  }
  return GetWebAudioStartBackgroundTask();
}

void BackgroundTaskPolicy::ProcessAudioContextPlayers(
    const PageNode* page_node) {
  auto currentProcess = base::CommandLine::ForCurrentProcess();
  if (currentProcess &&
      !currentProcess->HasSwitch(switches::kEnableWebAudioBackgroundTask)) {
    return;
  }
  if (!page_node) {
    LOG(ERROR) << __func__ << ", page_node is nullptr";
    return;
  }
  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    ProcessAudioContextPlayersOnUIThread(page_node);
  } else {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(
            &BackgroundTaskPolicy::ProcessAudioContextPlayersOnUIThread,
            weak_factory_.GetSafeRef(), page_node));
  }
}

void BackgroundTaskPolicy::ProcessAudioContextPlayersOnUIThread(const PageNode* page_node) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
  if (!page_node) {
    LOG(ERROR) << __func__ << ", page_node is nullptr";
    return;
  }
  if (audio_state_num_ == 0) {
    audio_context_players_num_.clear();
    return;
  }
  if (audio_context_players_num_.empty()) {
    return;
  }
  LOG(INFO) << __func__ << ", audio_context_players_num_ = "
            << audio_context_players_num_.size();
  for (auto iter = audio_context_players_num_.begin();
       iter != audio_context_players_num_.end();) {
    content::GlobalRenderFrameHostId rfh_id = iter->first;
    content::RenderFrameHost* render_frame_host = content::RenderFrameHost::FromID(rfh_id);
    if (!render_frame_host) {
      iter = audio_context_players_num_.erase(iter);
      continue;
    }
    if (content::WebContents::FromRenderFrameHost(render_frame_host) ==
        page_node->GetWebContents().get()) {
        iter = audio_context_players_num_.erase(iter);
    } else {
        ++iter;
    }
  }
}

bool BackgroundTaskPolicy::GetWebAudioStartBackgroundTask() {
  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    return GetWebAudioStartBackgroundTaskOnUIThread();
  }
  bool result = true;
  base::WaitableEvent event(
        base::WaitableEvent::ResetPolicy::AUTOMATIC,
        base::WaitableEvent::InitialState::NOT_SIGNALED);
  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(
          [](base::WeakPtr<BackgroundTaskPolicy> weak_self, bool* out_result,
             base::WaitableEvent* out_event) {
            if (!weak_self) {
              LOG(ERROR) << __func__ << ", weak_self is nullptr";
              return;
            }
            *out_result = weak_self->GetWebAudioStartBackgroundTaskOnUIThread();
            out_event->Signal();
          },
          weak_factory_.GetWeakPtr(), &result, &event));
  event.Wait();
  return result;
}

bool BackgroundTaskPolicy::GetWebAudioStartBackgroundTaskOnUIThread() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
  if (audio_context_players_num_.empty()) {
    return true;
  }
  bool result = false;
  for (const auto& player : audio_context_players_num_) {
    content::GlobalRenderFrameHostId rfh_id = player.first;
    content::RenderFrameHost* render_frame_host = content::RenderFrameHost::FromID(rfh_id);
    if (!render_frame_host) {
        continue;
    }
    if (!render_frame_host->IsRenderFrameLive()) {
        continue;
    }
    auto web_content = content::WebContents::FromRenderFrameHost(render_frame_host);
    if (!web_content || web_content->IsBeingDestroyed() ||
        !web_content->GetPrimaryMainFrame()) {
        LOG(ERROR) << "GetWebAudioStartBackgroundTaskOnUIThread get web_content "
                      "failed or it is being destroyed or get PrimaryMainFrame failed.";
        continue;
    }
    result = web_content->OnStartBackgroundTask(WEB_AUDIO_PLAYBACK,
                                               "web audio playback scenarios");
    if (result) {
        break;
    }
  }
  return result;
}
#endif // ARKWEB_PERFORMANCE_PERSISTENT_TASK

}  // namespace performance_manager::policies