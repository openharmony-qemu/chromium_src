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
#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
  void OnDecrementAudioNum(const PageNode* page_node) override;
#endif
  void MaybeChangeBackgroundTask(const PageNode* page_node);
  raw_ptr<const PageNode> page_node_being_removed_ = nullptr;
  std::unique_ptr<mechanism::BackgroundTaskHolder> background_task_holder_;
  bool is_request_background_task_;
  int32_t visible_page_num_;
  int32_t media_playing_num_;
  int32_t audio_state_num_;
};
}  // namespace policies
}  // namespace performance_manager
#endif  // BACKGROUND_TASK_POLICY_H_
