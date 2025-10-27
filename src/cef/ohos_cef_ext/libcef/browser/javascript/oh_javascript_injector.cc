// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "oh_javascript_injector.h"

#include "oh_gin_javascript_bridge_dispatcher_host.h"
namespace NWEB {
OhJavascriptInjector::OhJavascriptInjector(content::WebContents* web_contents,
                                           CefRefPtr<ArkWebClientExt> client)
    : content::WebContentsUserData<OhJavascriptInjector>(*web_contents) {
  javascript_bridge_dispatcher_host_ =
      new OhGinJavascriptBridgeDispatcherHost(web_contents, client);
  web_contents->SetUserData(UserDataKey(), base::WrapUnique(this));
}

OhJavascriptInjector::~OhJavascriptInjector() {}

void OhJavascriptInjector::AddNativeInterface(
    const std::string& object_name,
    const std::vector<std::string> method_list,
    const int32_t object_id,
    bool is_async,
    const std::string& permission) {
  if (!javascript_bridge_dispatcher_host_) {
    return;
  }

  javascript_bridge_dispatcher_host_->AddNativeNamedObject(
      object_name, method_list, object_id, is_async, permission);
}

void OhJavascriptInjector::AddInterface(
    const std::string& object_name,
    const std::vector<std::string> method_list,
    const std::vector<std::string> async_method_list,
    const int32_t object_id,
    const std::string& permission) {
  if (!javascript_bridge_dispatcher_host_) {
    return;
  }

  javascript_bridge_dispatcher_host_->AddNamedObject(
      object_name, method_list, async_method_list, object_id, permission);
}

void OhJavascriptInjector::RemoveInterface(
    const std::string& object_name,
    const std::vector<std::string> method_list) {
  if (!javascript_bridge_dispatcher_host_) {
    return;
  }

  javascript_bridge_dispatcher_host_->RemoveNamedObject(object_name,
                                                        method_list);
}

void OhJavascriptInjector::DoCallH5Function(
    int32_t routing_id,
    int32_t h5_object_id,
    const std::string& h5_method_name,
    const std::vector<CefRefPtr<CefValue>>& args) {
  if (!javascript_bridge_dispatcher_host_) {
    return;
  }

  javascript_bridge_dispatcher_host_->DoCallH5Function(routing_id, h5_object_id,
                                                       h5_method_name, args);
}

WEB_CONTENTS_USER_DATA_KEY_IMPL(OhJavascriptInjector);
}  // namespace NWEB
