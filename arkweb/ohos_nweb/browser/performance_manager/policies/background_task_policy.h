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
#include "arkweb/build/features/features.h"
#if BUILDFLAG(ARKWEB_TEST)
#undef private
#endif  // ARKWEB_TEST
#include "components/performance_manager/public/graph/graph.h"
#include "components/performance_manager/public/graph/page_node.h"
#if BUILDFLAG(ARKWEB_TEST)
#define private public
#endif  // ARKWEB_TEST

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
#if BUILDFLAG(ARKWEB_BGTASK)
  void SetBrowserForeground(const PageNode* page_node) override;
  void SetBrowserBackground(const PageNode* page_node) override;
#endif
#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
  void OnAudioContextPlaybackStarted(content::GlobalRenderFrameHostId rfh_id, int audio_context_id) override;
  void OnAudioContextPlaybackStopped(content::GlobalRenderFrameHostId rfh_id, int audio_context_id) override;

  bool IsWebAudioRequestBackgroundRunning();
  void ProcessAudioContextPlayers(const PageNode* page_node);
  void ProcessAudioContextPlayersOnUIThread(const PageNode* page_node);
  bool GetWebAudioStartBackgroundTask();
  bool GetWebAudioStartBackgroundTaskOnUIThread();

  using AudioContextIdPlayer = std::pair<content::GlobalRenderFrameHostId, int>;
  // Set of active audio contexts
  std::set<AudioContextIdPlayer> audio_context_players_num_;
#endif

  raw_ptr<const PageNode> page_node_being_removed_ = nullptr;
  std::unique_ptr<mechanism::BackgroundTaskHolder> background_task_holder_;
  bool is_request_background_task_;
  int32_t visible_page_num_;
  int32_t media_playing_num_;
  int32_t audio_state_num_;

  base::WeakPtrFactory<BackgroundTaskPolicy> weak_factory_{this};
};
#if BUILDFLAG(ARKWEB_TEST)
#undef private
#endif  // ARKWEB_TEST
}  // namespace policies
}  // namespace performance_manager
#endif  // BACKGROUND_TASK_POLICY_H_
