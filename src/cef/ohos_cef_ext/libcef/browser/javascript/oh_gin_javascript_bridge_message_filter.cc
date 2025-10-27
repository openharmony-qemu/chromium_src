// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "oh_gin_javascript_bridge_message_filter.h"

#include <thread>

#include "base/memory/scoped_refptr.h"
#include "base/task/thread_pool.h"
#include "base/types/pass_key.h"
#include "cef/libcef/browser/thread_util.h"
#include "cef/ohos_cef_ext/libcef/common/javascript/oh_gin_javascript_bridge_messages.h"
#include "content/browser/renderer_host/agent_scheduling_group_host.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/site_isolation_policy.h"
#include "ipc/ipc_platform_file_attachment_posix.h"
#include "oh_gin_javascript_bridge_dispatcher_host.h"

namespace {

const char kOhGinJavascriptBridgeMessageFilterKey[] =
    "OhGinJavascriptBridgeMessageFilter";

}  // namespace

namespace NWEB {
OhGinJavascriptBridgeMessageFilter::OhGinJavascriptBridgeMessageFilter(
    base::PassKey<OhGinJavascriptBridgeMessageFilter> pass_key,
    content::AgentSchedulingGroupHost& agent_scheduling_group)
    : BrowserMessageFilter(OhGinJavascriptBridgeMsgStart),
      agent_scheduling_group_(agent_scheduling_group),
      current_routing_id_(MSG_ROUTING_NONE) {
  async_task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
      {base::TaskPriority::USER_BLOCKING},
      base::SingleThreadTaskRunnerThreadMode::DEDICATED);
}

OhGinJavascriptBridgeMessageFilter::~OhGinJavascriptBridgeMessageFilter() {
  LOG(INFO) << "OhGinJavascriptBridgeMessageFilter dtor";
}

void OhGinJavascriptBridgeMessageFilter::OnDestruct() const {
  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    delete this;
  } else {
    content::GetUIThreadTaskRunner({})->DeleteSoon(FROM_HERE, this);
  }
}

bool OhGinJavascriptBridgeMessageFilter::OnMessageReceived(
    const IPC::Message& message) {
  base::PickleIterator iter(message);
  int32_t routing_id = -1;
  int32_t object_id = -1;
  bool msg_correct = false;
  bool isAsyncThread = false;
  if (message.is_sync()) {
    msg_correct = iter.ReadInt(&routing_id) && iter.ReadInt(&object_id);
    std::string url;
    std::string method_name;
    if (iter.ReadString(&url) && iter.ReadString(&method_name)) {
      base::AutoReset<int32_t> routingId(&current_routing_id_,
                                         message.routing_id());
      scoped_refptr<OhGinJavascriptBridgeDispatcherHost> host = FindHost();
      if (host) {
        host->OnHasAsyncThreadMethod(object_id, method_name, &isAsyncThread);
      }
    }
  } else {
    msg_correct = iter.ReadInt(&object_id);
  }

  if (!async_task_runner_) {
    async_task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
        {base::TaskPriority::USER_BLOCKING},
        base::SingleThreadTaskRunnerThreadMode::DEDICATED);
  }
#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
  if (message.HasAttachments()) {
    if (msg_correct && (!message.is_sync() || isAsyncThread) &&
        object_id >= OhGinJavascriptBridgeDispatcherHost::MIN_NATIVE_OBJ_ID) {
      async_task_runner_->PostTask(
          FROM_HERE, base::BindOnce(base::IgnoreResult(
                                        &OhGinJavascriptBridgeMessageFilter::
                                            OnMessageReceivedThreadFlowbuf),
                                    base::WrapRefCounted(this), message));
    } else {
      CEF_POST_TASK(CEF_UIT,
                    base::BindOnce(
                        base::IgnoreResult(&OhGinJavascriptBridgeMessageFilter::
                                               OnMessageReceivedThreadFlowbuf),
                        base::WrapRefCounted(this), message));
    }
  } else {
    if (msg_correct && (!message.is_sync() || isAsyncThread) &&
        object_id >= OhGinJavascriptBridgeDispatcherHost::MIN_NATIVE_OBJ_ID) {
      async_task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(
              base::IgnoreResult(
                  &OhGinJavascriptBridgeMessageFilter::OnMessageReceivedThread),
              base::WrapRefCounted(this), message));
    } else {
      CEF_POST_TASK(
          CEF_UIT,
          base::BindOnce(
              base::IgnoreResult(
                  &OhGinJavascriptBridgeMessageFilter::OnMessageReceivedThread),
              base::WrapRefCounted(this), message));
    }
  }
  return true;
#else   // CONTENT_ENABLE_LEGACY_IPC
  return false;
#endif  // CONTENT_ENABLE_LEGACY_IPC
}

#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
bool OhGinJavascriptBridgeMessageFilter::OnMessageReceivedThread(
    const IPC::Message& message) {
  base::AutoReset<int32_t> routing_id(&current_routing_id_,
                                      message.routing_id());
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(OhGinJavascriptBridgeMessageFilter, message)
    IPC_MESSAGE_HANDLER(OhGinJavascriptBridgeHostMsg_GetMethods, OnGetMethods)
    IPC_MESSAGE_HANDLER(OhGinJavascriptBridgeHostMsg_HasMethod, OnHasMethod)
    IPC_MESSAGE_HANDLER(OhGinJavascriptBridgeHostMsg_InvokeMethod,
                        OnInvokeMethod)
    IPC_MESSAGE_HANDLER(OhGinJavascriptBridgeHostMsg_InvokeMethod_Async,
                        OnInvokeMethodAsync)
    IPC_MESSAGE_HANDLER(OhGinJavascriptBridgeHostMsg_ObjectWrapperDeleted,
                        OnObjectWrapperDeleted)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

bool OhGinJavascriptBridgeMessageFilter::OnMessageReceivedThreadFlowbuf(
    const IPC::Message& message) {
  base::AutoReset<int32_t> routing_id(&current_routing_id_,
                                      message.routing_id());
  bool handled = true;
  base::PickleIterator iter(message);
  scoped_refptr<base::Pickle::Attachment> attachment;
  if (!iter.SkipBytes(message.payload_size() - sizeof(int))) {
    LOG(ERROR) << "OnMessageReceivedThreadFlowbuf Failed to skip bytes for "
                  "message iterator";
    return handled;
  }
  if (!message.ReadAttachment(&iter, &attachment)) {
    LOG(ERROR) << "OnMessageReceivedThreadFlowbuf Failed to read attachment "
                  "for message pipe";
    return handled;
  }

  int fd = static_cast<IPC::internal::PlatformFileAttachment*>(attachment.get())
               ->file();
  IPC_BEGIN_MESSAGE_MAP_WITH_PARAM(OhGinJavascriptBridgeMessageFilter, message,
                                   &fd)
    IPC_MESSAGE_HANDLER(OhGinJavascriptBridgeHostMsg_GetMethods, OnGetMethods)
    IPC_MESSAGE_HANDLER(OhGinJavascriptBridgeHostMsg_HasMethod, OnHasMethod)
    IPC_MESSAGE_HANDLER_PARAM(OhGinJavascriptBridgeHostMsg_InvokeMethod,
                              OnInvokeMethodFlowbuf)
    IPC_MESSAGE_HANDLER(OhGinJavascriptBridgeHostMsg_InvokeMethod_Async,
                        OnInvokeMethodFlowbufAsync)
    IPC_MESSAGE_HANDLER(OhGinJavascriptBridgeHostMsg_ObjectWrapperDeleted,
                        OnObjectWrapperDeleted)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}
#endif  // CONTENT_ENABLE_LEGACY_IPC

void OhGinJavascriptBridgeMessageFilter::AddRoutingIdForHost(
    OhGinJavascriptBridgeDispatcherHost* host,
    content::RenderFrameHost* render_frame_host) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  base::AutoLock locker(hosts_lock_);
  hosts_[render_frame_host->GetRoutingID()] = host;
}

void OhGinJavascriptBridgeMessageFilter::RemoveHost(
    const OhGinJavascriptBridgeDispatcherHost* host) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  base::AutoLock locker(hosts_lock_);
  auto iter = hosts_.begin();
  while (iter != hosts_.end()) {
    if (iter->second == host) {
      hosts_.erase(iter++);
    } else {
      ++iter;
    }
  }
}

void OhGinJavascriptBridgeMessageFilter::RenderProcessExited(
    content::RenderProcessHost* rph,
    const content::ChildProcessTerminationInfo& info) {
#if DCHECK_IS_ON()
  {
    scoped_refptr<OhGinJavascriptBridgeMessageFilter> filter =
        base::UserDataAdapter<OhGinJavascriptBridgeMessageFilter>::Get(
            &(*agent_scheduling_group_), kOhGinJavascriptBridgeMessageFilterKey);
    DCHECK_EQ(this, filter.get());
  }
#endif

  rph->RemoveObserver(this);

  (*agent_scheduling_group_).RemoveUserData(
      kOhGinJavascriptBridgeMessageFilterKey);
}

scoped_refptr<OhGinJavascriptBridgeMessageFilter>
OhGinJavascriptBridgeMessageFilter::FromHost(
    content::AgentSchedulingGroupHost& agent_scheduling_group,
    bool create_if_not_exists) {
  scoped_refptr<OhGinJavascriptBridgeMessageFilter> filter =
      base::UserDataAdapter<OhGinJavascriptBridgeMessageFilter>::Get(
          &agent_scheduling_group, kOhGinJavascriptBridgeMessageFilterKey);

  if (!filter && create_if_not_exists) {
    filter = base::MakeRefCounted<OhGinJavascriptBridgeMessageFilter>(
        base::PassKey<OhGinJavascriptBridgeMessageFilter>(),
        agent_scheduling_group);
#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
    agent_scheduling_group.AddFilter(filter.get());
#endif  // CONTENT_ENABLE_LEGACY_IPC
    agent_scheduling_group.GetProcess()->AddObserver(filter.get());

    agent_scheduling_group.SetUserData(
        kOhGinJavascriptBridgeMessageFilterKey,
        std::make_unique<
            base::UserDataAdapter<OhGinJavascriptBridgeMessageFilter>>(
            filter.get()));
  }
  return filter;
}

scoped_refptr<OhGinJavascriptBridgeDispatcherHost>
OhGinJavascriptBridgeMessageFilter::FindHost() {
  base::AutoLock locker(hosts_lock_);
  auto iter = hosts_.find(current_routing_id_);
  if (iter != hosts_.end()) {
    return iter->second;
  }
  LOG(WARNING) << "JSBridge host not found, routingID:" << current_routing_id_;
  return nullptr;
}

void OhGinJavascriptBridgeMessageFilter::OnGetMethods(
    int32_t object_id,
    std::set<std::string>* returned_method_names) {
  scoped_refptr<OhGinJavascriptBridgeDispatcherHost> host = FindHost();
  if (host) {
    host->OnGetMethods(object_id, returned_method_names);
  } else {
    *returned_method_names = std::set<std::string>();
  }
}

void OhGinJavascriptBridgeMessageFilter::OnHasMethod(
    int32_t object_id,
    const std::string& method_name,
    bool* result) {
  scoped_refptr<OhGinJavascriptBridgeDispatcherHost> host = FindHost();
  if (host) {
    host->OnHasMethod(object_id, method_name, result);
  } else {
    *result = false;
  }
}

void OhGinJavascriptBridgeMessageFilter::SetSiteInstanceGurl(const GURL& site_instance_url) {
  if (site_instance_gurl_.is_empty()) {
    site_instance_gurl_ = site_instance_url;
  }
}

bool OhGinJavascriptBridgeMessageFilter::IsSameSite(const GURL& document_gurl) {
  if (!site_instance_gurl_.SchemeIsHTTPOrHTTPS()) {
    return true;
  }

  if (document_gurl.scheme().empty() ||
      document_gurl.scheme() != site_instance_gurl_.scheme()) {
    LOG(INFO) << "IsSameSite scheme not same";
    return false;
  }

  if (document_gurl.host().empty() ||
      document_gurl.host() != site_instance_gurl_.host()) {
    const size_t suffix_length = site_instance_gurl_.host().length() + 1;
    const std::string url_host = document_gurl.host();

    if (url_host.length() <= suffix_length) {
      LOG(INFO) << "IsSameSite host not same";
      return false;
    } else {
      const size_t dot_pos = url_host.length() - suffix_length;
      const std::string host_suffix = url_host.substr(dot_pos + 1);
      if (url_host[dot_pos] != '.' ||
          host_suffix != site_instance_gurl_.host()) {
        LOG(INFO) << "IsSameSite host not subdomain";
        return false;
      }
    }
  }
  return true;
}

void OhGinJavascriptBridgeMessageFilter::OnInvokeMethod(
    int32_t object_id,
    const std::string& document_url,
    const std::string& method_name,
    const base::Value::List& arguments,
    base::Value::List* wrapped_result,
    OhGinJavascriptBridgeError* error_code) {
  bool is_same_site = true;
  if (content::SiteIsolationPolicy::UseDedicatedProcessesForAllSites()) {
    is_same_site = IsSameSite(GURL(document_url));
  }
  scoped_refptr<OhGinJavascriptBridgeDispatcherHost> host = FindHost();
  if (host && is_same_site) {
    host->OnInvokeMethod(current_routing_id_, object_id, document_url,
                         method_name, arguments, wrapped_result, error_code);
  } else {
    wrapped_result->Append(base::Value());
    *error_code = kOhGinJavascriptBridgeRenderFrameDeleted;
  }
}

void OhGinJavascriptBridgeMessageFilter::OnInvokeMethodAsync(
    int32_t object_id,
    const std::string& document_url,
    const std::string& method_name,
    const base::Value::List& arguments) {
  bool is_same_site = true;
  if (content::SiteIsolationPolicy::UseDedicatedProcessesForAllSites()) {
    is_same_site = IsSameSite(GURL(document_url));
  }
  scoped_refptr<OhGinJavascriptBridgeDispatcherHost> host = FindHost();
  if (host && is_same_site) {
    host->OnInvokeMethodAsync(current_routing_id_, object_id, document_url,
                              method_name, arguments);
  }
}

void OhGinJavascriptBridgeMessageFilter::OnInvokeMethodFlowbuf(
    int* fd,
    int32_t object_id,
    const std::string& document_url,
    const std::string& method_name,
    const base::Value::List& arguments,
    base::Value::List* wrapped_result,
    OhGinJavascriptBridgeError* error_code) {
  bool is_same_site = true;
  if (content::SiteIsolationPolicy::UseDedicatedProcessesForAllSites()) {
    is_same_site = IsSameSite(GURL(document_url));
  }
  scoped_refptr<OhGinJavascriptBridgeDispatcherHost> host = FindHost();
  if (host && is_same_site) {
    host->OnInvokeMethodFlowbuf(current_routing_id_, object_id, document_url,
                                method_name, arguments, *fd, wrapped_result,
                                error_code);
  } else {
    wrapped_result->Append(base::Value());
    *error_code = kOhGinJavascriptBridgeRenderFrameDeleted;
  }
}

void OhGinJavascriptBridgeMessageFilter::OnInvokeMethodFlowbufAsync(
    int* fd,
    int32_t object_id,
    const std::string& document_url,
    const std::string& method_name,
    const base::Value::List& arguments) {
  bool is_same_site = true;
  if (content::SiteIsolationPolicy::UseDedicatedProcessesForAllSites()) {
    is_same_site = IsSameSite(GURL(document_url));
  }
  scoped_refptr<OhGinJavascriptBridgeDispatcherHost> host = FindHost();
  if (host && is_same_site) {
    host->OnInvokeMethodFlowbufAsync(current_routing_id_, object_id,
                                     document_url, method_name, arguments, *fd);
  }
}

void OhGinJavascriptBridgeMessageFilter::OnObjectWrapperDeleted(int object_id) {
  scoped_refptr<OhGinJavascriptBridgeDispatcherHost> host = FindHost();
  if (host) {
    host->OnObjectWrapperDeleted(current_routing_id_, object_id);
  }
}
}  // namespace NWEB
