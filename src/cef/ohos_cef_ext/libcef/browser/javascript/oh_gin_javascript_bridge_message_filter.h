// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OH_GIN_JAVASCRIPT_BRIDGE_MESSAGE_FILTER_H
#define OH_GIN_JAVASCRIPT_BRIDGE_MESSAGE_FILTER_H

#include "base/types/pass_key.h"
#include "base/values.h"
#include "base/memory/raw_ref.h"
#include "content/public/browser/render_process_host_observer.h"
#include "content/public/renderer/render_frame_observer.h"
#include "libcef/common/javascript/oh_gin_javascript_bridge_errors.h"
#include "url/gurl.h"

namespace IPC {
class Message;
}

namespace content {
class AgentSchedulingGroupHost;
class RenderFrameHost;
class BrowserThread;
}  // namespace content

namespace NWEB {
class OhGinJavascriptBridgeDispatcherHost;
class OhGinJavascriptBridgeMessageFilter
    : public content::BrowserMessageFilter,
      public content::RenderProcessHostObserver {
 public:
  OhGinJavascriptBridgeMessageFilter(
      base::PassKey<OhGinJavascriptBridgeMessageFilter> pass_key,
      content::AgentSchedulingGroupHost& agent_scheduling_group);

  // BrowserMessageFilter
  void OnDestruct() const override;
  bool OnMessageReceived(const IPC::Message& message) override;
  // scoped_refptr<base::SequencedTaskRunner> OverrideTaskRunnerForMessage(
  //     const IPC::Message& message) override;

  // RenderProcessHostObserver
  void RenderProcessExited(
      content::RenderProcessHost* rph,
      const content::ChildProcessTerminationInfo& info) override;

  // Called on the UI thread.
  void AddRoutingIdForHost(OhGinJavascriptBridgeDispatcherHost* host,
                           content::RenderFrameHost* render_frame_host);
  void RemoveHost(const OhGinJavascriptBridgeDispatcherHost* host);

  static scoped_refptr<OhGinJavascriptBridgeMessageFilter> FromHost(
      content::AgentSchedulingGroupHost& agent_scheduling_group,
      bool create_if_not_exists);

  void SetSiteInstanceGurl(const GURL& site_instance_url);

 private:
  friend class BrowserThread;
  friend class base::DeleteHelper<OhGinJavascriptBridgeMessageFilter>;

  typedef std::map<int32_t, scoped_refptr<OhGinJavascriptBridgeDispatcherHost>>
      HostMap;
  ~OhGinJavascriptBridgeMessageFilter() override;

#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
  bool OnMessageReceivedThread(const IPC::Message& message);

  bool OnMessageReceivedThreadFlowbuf(const IPC::Message& message);
#endif  // CONTENT_ENABLE_LEGACY_IPC

  // Called on the background thread.
  scoped_refptr<OhGinJavascriptBridgeDispatcherHost> FindHost();

  void OnGetMethods(int32_t object_id,
                    std::set<std::string>* returned_method_names);
  void OnHasMethod(int32_t object_id,
                   const std::string& method_name,
                   bool* result);
  void OnInvokeMethod(int32_t object_id,
                      const std::string& document_url,
                      const std::string& method_name,
                      const base::Value::List& arguments,
                      base::Value::List* result,
                      OhGinJavascriptBridgeError* error_code);
  void OnInvokeMethodAsync(int32_t object_id,
                           const std::string& document_url,
                           const std::string& method_name,
                           const base::Value::List& arguments);
  void OnInvokeMethodFlowbuf(int* fd,
                             int32_t object_id,
                             const std::string& document_url,
                             const std::string& method_name,
                             const base::Value::List& arguments,
                             base::Value::List* result,
                             OhGinJavascriptBridgeError* error_code);
  void OnInvokeMethodFlowbufAsync(int* fd,
                                  int32_t object_id,
                                  const std::string& document_url,
                                  const std::string& method_name,
                                  const base::Value::List& arguments);
  void OnObjectWrapperDeleted(int object_id);

  bool IsSameSite(const GURL& document_gurl);

  // Accessed both from UI and background threads.
  HostMap hosts_ GUARDED_BY(hosts_lock_);
  base::Lock hosts_lock_;

  // The `AgentSchedulingGroupHost` that this object is associated with. This
  // filter is installed on the host's channel.
  raw_ref<content::AgentSchedulingGroupHost> agent_scheduling_group_;

  // The routing id of the RenderFrameHost whose request we are processing.
  // Used on the background thread.
  int32_t current_routing_id_;

  scoped_refptr<base::SingleThreadTaskRunner> async_task_runner_;

  GURL site_instance_gurl_;
};
}  // namespace NWEB
#endif
