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
  #ifdef OHOS_PERFORMANCE_PERSISTENT_TASK
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