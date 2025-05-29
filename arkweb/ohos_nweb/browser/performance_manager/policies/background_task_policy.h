// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on page_node.h originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

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
class BackgroundTaskPolicy : public GraphOwnedDefaultImpl,
                             public PageNode::ObserverDefaultImpl {
 public:
  BackgroundTaskPolicy();
  BackgroundTaskPolicy(const BackgroundTaskPolicy&) = delete;
  BackgroundTaskPolicy(BackgroundTaskPolicy&&) = delete;
  BackgroundTaskPolicy& operator=(const BackgroundTaskPolicy&) = delete;
  BackgroundTaskPolicy& operator=(BackgroundTaskPolicy&&) = delete;
  ~BackgroundTaskPolicy() override;

 private:
  //
  void OnTakenFromGraph(Graph* graph) override;

  // GraphOwned implementation:
  void OnPassedToGraph(Graph* graph) override;

  // PageNodeObserver implementation:
  void OnPageNodeAdded(const PageNode* page_node) override;
  void OnBeforePageNodeRemoved(const PageNode* page_node) override;
  void OnIsVisibleChanged(const PageNode* page_node) override;
  void OnIsMediaPlayingChanged(const PageNode* page_node) override;
  void OnIsAudibleChanged(const PageNode* page_node) override;
  void OnMainFrameUrlChanged(const PageNode* page_node) override;
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

  void MaybeChangeBackgroundTask(const PageNode* page_node);
  bool IsControllable(const PageNode* page_node);
  bool IsEndOfMedia(const PageNode* page_node);
  bool IsPauseByAvsession(const PageNode* page_node);
  void SetWebviewShow(const PageNode* page_node,
                      bool show,
                      bool is_special_for_audio,
                      bool& ret);
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
