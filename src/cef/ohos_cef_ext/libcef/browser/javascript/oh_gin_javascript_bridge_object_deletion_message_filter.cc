// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "oh_gin_javascript_bridge_object_deletion_message_filter.h"

#include "base/auto_reset.h"
#include "base/feature_list.h"
#include "base/types/pass_key.h"
#include "build/build_config.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_features.h"
#include "libcef/common/javascript/oh_gin_javascript_bridge_messages.h"
#include "oh_gin_javascript_bridge_dispatcher_host.h"

namespace {

const char kOhGinJavascriptBridgeObjectDeletionMessageFilterKey[] =
    "OhGinJavascriptBridgeObjectDeletionMessageFilter";

}  // namespace
namespace NWEB {
OhGinJavascriptBridgeObjectDeletionMessageFilter::
    OhGinJavascriptBridgeObjectDeletionMessageFilter(
        base::PassKey<OhGinJavascriptBridgeObjectDeletionMessageFilter>
            pass_key)
    : BrowserMessageFilter(OhGinJavascriptBridgeMsgStart),
      current_routing_id_(MSG_ROUTING_NONE) {
  DCHECK(base::FeatureList::IsEnabled(features::kMBIMode));
}

OhGinJavascriptBridgeObjectDeletionMessageFilter::
    ~OhGinJavascriptBridgeObjectDeletionMessageFilter() {}

void OhGinJavascriptBridgeObjectDeletionMessageFilter::OnDestruct() const {
  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    delete this;
  } else {
    content::GetUIThreadTaskRunner({})->DeleteSoon(FROM_HERE, this);
  }
}

bool OhGinJavascriptBridgeObjectDeletionMessageFilter::OnMessageReceived(
    const IPC::Message& message) {
  (void)message;
  return false;
}

scoped_refptr<base::SequencedTaskRunner>
OhGinJavascriptBridgeObjectDeletionMessageFilter::OverrideTaskRunnerForMessage(
    const IPC::Message& message) {
  (void)message;
  return nullptr;
}

void OhGinJavascriptBridgeObjectDeletionMessageFilter::AddRoutingIdForHost(
    OhGinJavascriptBridgeDispatcherHost* host,
    content::RenderFrameHost* render_frame_host) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  base::AutoLock locker(hosts_lock_);
  hosts_[render_frame_host->GetRoutingID()] = host;
}

void OhGinJavascriptBridgeObjectDeletionMessageFilter::RemoveHost(
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

void OhGinJavascriptBridgeObjectDeletionMessageFilter::RenderProcessExited(
    content::RenderProcessHost* rph,
    const content::ChildProcessTerminationInfo& info) {
#if DCHECK_IS_ON()
  {
    scoped_refptr<OhGinJavascriptBridgeObjectDeletionMessageFilter> filter =
        base::UserDataAdapter<
            OhGinJavascriptBridgeObjectDeletionMessageFilter>::
            Get(rph, kOhGinJavascriptBridgeObjectDeletionMessageFilterKey);
    DCHECK_EQ(this, filter.get());
  }
#endif
  rph->RemoveObserver(this);
  rph->RemoveUserData(kOhGinJavascriptBridgeObjectDeletionMessageFilterKey);
}

// static
scoped_refptr<OhGinJavascriptBridgeObjectDeletionMessageFilter>
OhGinJavascriptBridgeObjectDeletionMessageFilter::FromHost(
    content::RenderProcessHost* rph,
    bool create_if_not_exists) {
  DCHECK(base::FeatureList::IsEnabled(features::kMBIMode));
  scoped_refptr<OhGinJavascriptBridgeObjectDeletionMessageFilter> filter =
      base::UserDataAdapter<OhGinJavascriptBridgeObjectDeletionMessageFilter>::
          Get(rph, kOhGinJavascriptBridgeObjectDeletionMessageFilterKey);
  if (!filter && create_if_not_exists) {
    filter =
        base::MakeRefCounted<OhGinJavascriptBridgeObjectDeletionMessageFilter>(
            base::PassKey<OhGinJavascriptBridgeObjectDeletionMessageFilter>());
#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
    rph->AddFilter(filter.get());
#endif
    rph->AddObserver(filter.get());

    rph->SetUserData(
        kOhGinJavascriptBridgeObjectDeletionMessageFilterKey,
        std::make_unique<base::UserDataAdapter<
            OhGinJavascriptBridgeObjectDeletionMessageFilter>>(filter.get()));
  }
  return filter;
}

scoped_refptr<OhGinJavascriptBridgeDispatcherHost>
OhGinJavascriptBridgeObjectDeletionMessageFilter::FindHost() {
  base::AutoLock locker(hosts_lock_);
  auto iter = hosts_.find(current_routing_id_);
  if (iter != hosts_.end()) {
    return iter->second;
  }

  return nullptr;
}

void OhGinJavascriptBridgeObjectDeletionMessageFilter::OnObjectWrapperDeleted(
    ObjectID object_id) {
  (void)object_id;
}
}  // namespace NWEB
