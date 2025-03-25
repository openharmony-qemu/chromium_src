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
#include "content/browser/media/session/media_session_impl.h"
#include "content/browser/scheduler/browser_task_executor.h"
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
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << ", media avsession IsVisible="
            << (page_node->IsVisible() ? "true" : "false")
            << ", IsMediaPlaying=" << (page_node->IsMediaPlaying() ? "true" : "false")
            << ", is_main_frame_url_changed_ = " << is_main_frame_url_changed_;

  int visible_num = page_node->IsVisible() ? 1 : -1;
  visible_page_num_ += visible_num;
  if (visible_page_num_ < 0) {
    visible_page_num_ = 0;
  }

  // switch between two tabs
  if (page_node->IsVisible()) {
    if (last_avsession_page_node_ != page_node) {
       bool ret = false;
       SetWebviewShow(page_node, true, false, ret);
       if (ret) {
         last_avsession_page_node_ = const_cast<PageNode*>(page_node);
       }
    }
  }
  // pause to bg
  else if (!page_node->IsVisible() && !page_node->IsMediaPlaying()) {
    bool ret = false;
    SetWebviewShow(page_node, false, false, ret);
    if (ret) {
        last_avsession_page_node_ = nullptr;
    }
  } else {
    is_main_frame_url_changed_ = false;
  }
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession IsVisibleChanged: "
            << (visible_num > 0 ? "true" : "false")
            << ", visible_page_num_: " << visible_page_num_
            << ", is_main_frame_url_changed_=" << is_main_frame_url_changed_;

  MaybeChangeBackgroundTask(page_node);
}

void BackgroundTaskPolicy::OnIsMediaPlayingChanged(const PageNode* page_node) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession page_node=" << page_node
            << ", last_avsession_page_node_=" << last_avsession_page_node_
            << ", is_main_frame_url_changed_=" << is_main_frame_url_changed_;
  if (page_node == nullptr) {
    LOG(ERROR) << BG_TASK_TAG << " media avsession page_node is null";
    return;
  }
  if (page_node) {
    LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << ", media avsession IsVisible="
              << (page_node->IsVisible() ? "true" : "false")
              << ", IsMediaPlaying="
              << (page_node->IsMediaPlaying() ? "true" : "false");
    // when status change to playing
    if (page_node->IsVisible() && page_node->IsMediaPlaying()) {
       bool ret = false;
       SetWebviewShow(page_node, true, false, ret);
       if (ret) {
         last_avsession_page_node_ = const_cast<PageNode*>(page_node);
       }
    }
    // when playing to background
    else if (!page_node->IsVisible() && !page_node->IsMediaPlaying() && !IsPauseByAvsession(page_node)) {
       bool ret = false;
       SetWebviewShow(page_node, false, false, ret);
       if (ret) {
         last_avsession_page_node_ = nullptr;
       }
    }
    // when change another page, and new page is not a playing page
    // pause action will not go this way
    else if (page_node->IsVisible() && !page_node->IsMediaPlaying() && is_main_frame_url_changed_) {
       bool ret = false;
       SetWebviewShow(page_node, false, false, ret);
       if (ret) {
         last_avsession_page_node_ = nullptr;
       }
    }
    // when current page to end of media 
    else if (page_node->IsVisible() && !page_node->IsMediaPlaying() &&
             (last_avsession_page_node_ == page_node) &&
             IsEndOfMedia(page_node) &&
             !is_main_frame_url_changed_) {
       LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession current page to end of media.";
       bool ret = false;
       SetWebviewShow(page_node, false, false, ret);
       if (ret) {
         last_avsession_page_node_ = nullptr;
       }
    }
    // others no change
    else {
      is_main_frame_url_changed_ = false;
    }
  }

  int media_playing_num = page_node->IsMediaPlaying() ? 1 : -1;
  media_playing_num_ += media_playing_num;
  if (media_playing_num_ < 0) {
    media_playing_num_ = 0;
  }
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << ", media avsession OnIsMediaPlayingChanged "
            << (page_node->IsMediaPlaying() ? "true" : "false")
            << " page_node=" << page_node
            << ", media_playing_num: " << media_playing_num_
            << " visible_page_num_=" << visible_page_num_
            << " page_node->IsVisible=" << (page_node->IsVisible() ? "true" : "false")
            << " last_avsession_page_node_=" << last_avsession_page_node_;
  MaybeChangeBackgroundTask(page_node);
}

#ifdef OHOS_PERFORMANCE_PERSISTENT_TASK
void BackgroundTaskPolicy::OnDecrementAudioNum(const PageNode* page_node) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession page_node=" << page_node;
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
  LOG(INFO) << BG_TASK_TAG << " OnIsAudibleChanged "
          << (page_node->IsAudible() ? "true" : "false")
          << ", audio_state_num: " << audio_state_num_;
  MaybeChangeBackgroundTask(page_node);
}

void BackgroundTaskPolicy::MaybeChangeBackgroundTask(const PageNode* page_node) {
  LOG(INFO) << "BackgroundTaskPolicy::MaybeChangeBackgroundTask "
      << " page_node=" << page_node
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

bool BackgroundTaskPolicy::IsControllable(const PageNode* page_node)
{
  bool ret = false;
 if (page_node) {
    auto webcontents = page_node->GetContentsProxy().Get();
    if (webcontents) {
      content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::FromWebContents(webcontents);
      if (mediaSession) {
        ret = mediaSession->IsControllable();
        LOG(ERROR) << BG_TASK_TAG << __FUNCTION__ << " media avsession ret=" << ret;
      }
    }
  }
  return ret;
}
 
bool BackgroundTaskPolicy::IsEndOfMedia(const PageNode* page_node)
{
  bool ret = false;
 if (page_node) {
    auto webcontents = page_node->GetContentsProxy().Get();
    if (webcontents) {
      content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::FromWebContents(webcontents);
      if (mediaSession) {
        ret = mediaSession->IsEndOfMedia();
        LOG(ERROR) << BG_TASK_TAG << __FUNCTION__ << " media avsession ret=" << ret;
      }
    }
  }
  return ret;
}

bool BackgroundTaskPolicy::IsPauseByAvsession(const PageNode* page_node) {
  bool ret = false;
  if (!page_node) {
    return ret;
  }
  auto web_contents = page_node->GetContentsProxy().Get();
  if (!web_contents) {
    return ret;
  }
  content::MediaSessionImpl* media_session =
    content::MediaSessionImpl::FromWebContents(web_contents);
  if (media_session) {
    ret = media_session->IsPauseByAvsession();
  }
  return ret;
}

void BackgroundTaskPolicy::SetWebviewShow(const PageNode* page_node,
                                          bool show,
                                          bool is_special_for_audio,
                                          bool &ret) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << ", in page_node=" << page_node
            << ", show=" << (show ? 1 : 0)
            << ", is_special_for_audio=" << (is_special_for_audio ? 1 : 0)
            << ", last_avsession_page_node_=" << last_avsession_page_node_
            << ", is_main_frame_url_changed_=" << is_main_frame_url_changed_;
  ret = false;
  if (!page_node) {
    LOG(ERROR) << BG_TASK_TAG << __FUNCTION__ << "page_node is null";
    is_main_frame_url_changed_ = false;
    return;
  }
  auto web_contents = page_node->GetContentsProxy().Get();
  if (!web_contents) {
    LOG(ERROR) << BG_TASK_TAG << __FUNCTION__ << "web_contents is null";
    is_main_frame_url_changed_ = false;
    return;
  }
  if (!web_contents->IsBeingDestroyed()) {
    content::MediaSessionImpl* media_session =
      content::MediaSessionImpl::FromWebContents(web_contents);
    if (media_session) {
      content::GetUIThreadTaskRunner({})->PostTask(
          FROM_HERE,
          base::BindOnce(&content::MediaSessionImpl::SetWebviewShow,
                        media_session->weakMediaSessionFactory_.GetWeakPtr(),
                        show, is_special_for_audio));
      ret = true;
    }
  }
  is_main_frame_url_changed_ = false;
}

// While press backward button, the page will change to a new payge,
// the Url will change, so this function will be called
void BackgroundTaskPolicy::OnMainFrameUrlChanged(const PageNode* page_node) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__
            << ", media avsession in page_node=" << page_node;
  if (page_node) {
    LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << ", media avsession IsVisible="
              << (page_node->IsVisible() ? "true" : "false")
              << ", IsMediaPlaying="
              << (page_node->IsMediaPlaying() ? "true" : "false")
              << ", IsAudible=" << (page_node->IsAudible() ? "true" : "false");
    // when backward to a playing page   1 1
    if (page_node->IsVisible() && page_node->IsMediaPlaying()) {
      bool ret = false;
      SetWebviewShow(page_node, true, false, ret);
      if (ret) {
        last_avsession_page_node_ = const_cast<PageNode*>(page_node);
      }
    }
    // when backward to a not playing page   1 0
    else if (page_node->IsVisible() && !page_node->IsMediaPlaying()) {
      bool ret = false;
      SetWebviewShow(page_node, false, false, ret);
      if (ret) {
        last_avsession_page_node_ = nullptr;
      }
    } else {
    }

    // when backward to a not playing page  1 0 for onlyaudio
    if (!page_node->IsAudible()) {
      bool ret = false;
      SetWebviewShow(page_node, false, false, ret);
      if (ret) {
        last_avsession_page_node_ = nullptr;
      }
    }
    is_main_frame_url_changed_ = true;
  }
}

void BackgroundTaskPolicy::OnPageStateChanged(const PageNode* page_node,
                        PageState old_state) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node
            << " old_state=" << (int32_t)old_state;
}

void BackgroundTaskPolicy::OnOpenerFrameNodeChanged(const PageNode* page_node, const FrameNode* previous_opener) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node;
}

void BackgroundTaskPolicy::OnEmbedderFrameNodeChanged(
    const PageNode* page_node,
    const FrameNode* previous_embedder,
    EmbeddingType previous_embedding_type) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node;
}

void BackgroundTaskPolicy::OnTypeChanged(const PageNode* page_node,
                    PageType previous_type) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node;
}

void BackgroundTaskPolicy::OnLoadingStateChanged(const PageNode* page_node,
                            PageNode::LoadingState previous_state) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node
            << ", previous_state=" << (int32_t)previous_state;
}

void BackgroundTaskPolicy::OnUkmSourceIdChanged(const PageNode* page_node) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node;
}

void BackgroundTaskPolicy::OnPageLifecycleStateChanged(const PageNode* page_node) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node;
}

void BackgroundTaskPolicy::OnPageIsHoldingWebLockChanged(const PageNode* page_node) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node;
}

void BackgroundTaskPolicy::OnPageIsHoldingIndexedDBLockChanged(const PageNode* page_node) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node;
}

void BackgroundTaskPolicy::OnMainFrameDocumentChanged(const PageNode* page_node) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node;
}

void BackgroundTaskPolicy::OnHadFormInteractionChanged(const PageNode* page_node) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node;
}

void BackgroundTaskPolicy::OnHadUserEditsChanged(const PageNode* page_node) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node;
}

void BackgroundTaskPolicy::OnTitleUpdated(const PageNode* page_node) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node;
}

void BackgroundTaskPolicy::OnFaviconUpdated(const PageNode* page_node) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node;
}

void BackgroundTaskPolicy::OnAboutToBeDiscarded(const PageNode* page_node,
                          const PageNode* new_page_node) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node;
}

void BackgroundTaskPolicy::OnFreezingVoteChanged(
    const PageNode* page_node,
    absl::optional<freezing::FreezingVote> previous_vote) {
  LOG(INFO) << BG_TASK_TAG << __FUNCTION__ << " media avsession in out page_node=" << page_node;
}
}  // namespace performance_manager::policies
                                             