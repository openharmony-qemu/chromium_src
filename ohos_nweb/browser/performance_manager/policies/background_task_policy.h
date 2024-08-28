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

#ifndef BACKGROUND_TASK_POLICY_H_
#define BACKGROUND_TASK_POLICY_H_

#include <memory>

#include "components/performance_manager/public/graph/graph.h"
#include "components/performance_manager/public/graph/page_node.h"

namespace performance_manager {

namespace mechanism {
class BackgroundTaskHolder;
}  // namespace mechanism
namespace policies {
class BackgroundTaskPolicy : public GraphObserver,
                             public GraphOwnedDefaultImpl,
                             public PageNode::ObserverDefaultImpl {
 public:
  BackgroundTaskPolicy();
  BackgroundTaskPolicy(const BackgroundTaskPolicy&) = delete;
  BackgroundTaskPolicy(BackgroundTaskPolicy&&) = delete;
  BackgroundTaskPolicy& operator=(const BackgroundTaskPolicy&) = delete;
  BackgroundTaskPolicy& operator=(BackgroundTaskPolicy&&) = delete;
  ~BackgroundTaskPolicy() override;

 private:
  // GraphObserver implementation:
  void OnBeforeGraphDestroyed(Graph* graph) override;

  // GraphOwned implementation:
  void OnPassedToGraph(Graph* graph) override;

  // PageNodeObserver implementation:
  void OnPageNodeAdded(const PageNode* page_node) override;
  void OnBeforePageNodeRemoved(const PageNode* page_node) override;
  void OnIsVisibleChanged(const PageNode* page_node) override;
  void OnIsMediaPlayingChanged(const PageNode* page_node) override;
  void OnIsAudibleChanged(const PageNode* page_node) override;
  void OnMainFrameUrlChanged(const PageNode* page_node) override;
  void OnPageStateChanged(const PageNode* page_node,
                          PageState old_state) override;
  void OnOpenerFrameNodeChanged(const PageNode* page_node,
                                const FrameNode* previous_opener) override;
  void OnEmbedderFrameNodeChanged(
      const PageNode* page_node,
      const FrameNode* previous_embedder,
      EmbeddingType previous_embedding_type) override;
  void OnTypeChanged(const PageNode* page_node,
                     PageType previous_type) override;
  void OnLoadingStateChanged(const PageNode* page_node,
                             PageNode::LoadingState previous_state) override;
  void OnUkmSourceIdChanged(const PageNode* page_node) override;
  void OnPageLifecycleStateChanged(const PageNode* page_node) override;
  void OnPageIsHoldingWebLockChanged(const PageNode* page_node) override;
  void OnPageIsHoldingIndexedDBLockChanged(const PageNode* page_node) override;
  void OnMainFrameDocumentChanged(const PageNode* page_node) override;
  void OnHadFormInteractionChanged(const PageNode* page_node) override;
  void OnHadUserEditsChanged(const PageNode* page_node) override;
  void OnTitleUpdated(const PageNode* page_node) override;
  void OnFaviconUpdated(const PageNode* page_node) override;
  void OnAboutToBeDiscarded(const PageNode* page_node,
                            const PageNode* new_page_node) override;
  void OnFreezingVoteChanged(
      const PageNode* page_node,
      absl::optional<freezing::FreezingVote> previous_vote) override;

  void MaybeChangeBackgroundTask(const PageNode* page_node);
  void SetWebviewShow(const PageNode* page_node, bool show, bool &ret);
  void SetWebviewShowForAudio(const PageNode* page_node, bool show, bool &ret);
  bool IsControllable(const PageNode* page_node);
  bool IsEndOfMedia(const PageNode* page_node);
  raw_ptr<const PageNode> page_node_being_removed_ = nullptr;
  std::unique_ptr<mechanism::BackgroundTaskHolder> background_task_holder_;
  bool is_request_background_task_;
  int32_t visible_page_num_;
  int32_t media_playing_num_;
  int32_t audio_state_num_;
  PageNode* last_avsession_page_node_;
  bool is_main_frame_url_changed_;
};
}  // namespace policies
}  // namespace performance_manager
#endif  // BACKGROUND_TASK_POLICY_H_