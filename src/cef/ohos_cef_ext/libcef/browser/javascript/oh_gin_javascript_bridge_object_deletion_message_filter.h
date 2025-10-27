// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OH_GIN_JAVASCRIPT_BRIDGE_OBJECT_DELETION_MESSAGE_FILTER_H
#define OH_GIN_JAVASCRIPT_BRIDGE_OBJECT_DELETION_MESSAGE_FILTER_H

#include <stdint.h>

#include <map>
#include <set>

#include "base/memory/ref_counted.h"
#include "base/synchronization/lock.h"
#include "base/thread_annotations.h"
#include "base/types/pass_key.h"
#include "content/public/browser/browser_message_filter.h"
#include "content/public/browser/child_process_termination_info.h"
#include "content/public/browser/render_process_host_observer.h"

namespace IPC {
class Message;
}

namespace content {
class RenderFrameHost;
class BrowserThread;
}  // namespace content

namespace NWEB {
class OhGinJavascriptBridgeDispatcherHost;
class OhGinJavascriptBridgeObjectDeletionMessageFilter
    : public content::BrowserMessageFilter,
      public content::RenderProcessHostObserver {
 public:
  // BrowserMessageFilter
  void OnDestruct() const override;
  bool OnMessageReceived(const IPC::Message& message) override;
  scoped_refptr<base::SequencedTaskRunner> OverrideTaskRunnerForMessage(
      const IPC::Message& message) override;

  // RenderProcessHostObserver
  void RenderProcessExited(
      content::RenderProcessHost* rph,
      const content ::ChildProcessTerminationInfo& info) override;

  // Called on the UI thread.
  void AddRoutingIdForHost(OhGinJavascriptBridgeDispatcherHost* host,
                           content::RenderFrameHost* render_frame_host);
  void RemoveHost(const OhGinJavascriptBridgeDispatcherHost* host);

  static scoped_refptr<OhGinJavascriptBridgeObjectDeletionMessageFilter>
  FromHost(content::RenderProcessHost* rph, bool create_if_not_exists);

  OhGinJavascriptBridgeObjectDeletionMessageFilter(
      base::PassKey<OhGinJavascriptBridgeObjectDeletionMessageFilter> pass_key);

 private:
  typedef int32_t ObjectID;

  friend class BrowserThread;
  friend class base::DeleteHelper<
      OhGinJavascriptBridgeObjectDeletionMessageFilter>;

  typedef std::map<int32_t, scoped_refptr<OhGinJavascriptBridgeDispatcherHost>>
      HostMap;

  ~OhGinJavascriptBridgeObjectDeletionMessageFilter() override;

  // Called on the background thread.
  scoped_refptr<OhGinJavascriptBridgeDispatcherHost> FindHost();
  void OnObjectWrapperDeleted(ObjectID object_id);

  // Accessed both from UI and background threads.
  HostMap hosts_ GUARDED_BY(hosts_lock_);
  base::Lock hosts_lock_;

  // The routing id of the RenderFrameHost whose request we are processing.
  // Used on the background thread.
  int32_t current_routing_id_;
};
}  // namespace NWEB
#endif
