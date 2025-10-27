// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_SECURE_JAVASCRIPT_PROXY)
#include "base/threading/thread_local_storage.h"
#endif
#include "arkweb/chromium_ext/url/ohos/log_utils.h"
#include "content/browser/renderer_host/agent_scheduling_group_host.h"
#include "content/browser/renderer_host/render_frame_host_impl.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/web_contents.h"
#include "libcef/browser/javascript/oh_gin_javascript_bridge_message_filter.h"
#include "libcef/browser/javascript/oh_javascript_injector.h"
#include "libcef/common/javascript/oh_gin_javascript_bridge_messages.h"
#include "libcef/common/javascript/oh_gin_javascript_bridge_value.h"
#include "libcef/common/values_impl.h"
#include "oh_gin_javascript_bridge_dispatcher_host.h"
#include "oh_gin_javascript_bridge_object_deletion_message_filter.h"

namespace {
#define JS_BRIDGE_BINARY_ARGS_COUNT 2
// For the sake of the storage API, make this quite large.
const size_t MAX_RECURSION_DEPTH = 11;
const size_t MAX_DATA_LENGTH = 10000;

class ValueConvertState {
 public:
  // Level scope which updates the current depth of some ValueConvertState.
  class Level {
   public:
    explicit Level(ValueConvertState* state) : state_(state) {
      state_->maxRecursionDepth_--;
    }
    ~Level() { state_->maxRecursionDepth_++; }

   private:
    raw_ptr<ValueConvertState> state_;
  };

  explicit ValueConvertState() : maxRecursionDepth_(MAX_RECURSION_DEPTH) {}

  ValueConvertState(const ValueConvertState&) = delete;
  ValueConvertState& operator=(const ValueConvertState&) = delete;

  bool HasReachedMaxRecursionDepth() { return maxRecursionDepth_ <= 0; }

 private:
  size_t maxRecursionDepth_;
};
}  // namespace

namespace NWEB {
OhGinJavascriptBridgeDispatcherHost::OhGinJavascriptBridgeDispatcherHost(
    content::WebContents* web_contents,
    CefRefPtr<ArkWebClientExt> client)
    : content::WebContentsObserver(web_contents), client_(client) {}

OhGinJavascriptBridgeDispatcherHost::~OhGinJavascriptBridgeDispatcherHost() {
  LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::~"
                "OhGinJavascriptBridgeDispatcherHost called";
  client_.reset();
  std::unique_lock<std::shared_mutex> lock(share_mutex_);
  sync_method_map_.clear();
  async_method_map_.clear();
  javascript_sync_permission_map_.clear();
  javascript_async_permission_map_.clear();
  object_id_map_.clear();
  async_object_id_map_.clear();
}

// Run on the UI thread.
void OhGinJavascriptBridgeDispatcherHost::
    InstallFilterAndRegisterAllRoutingIds() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  {
    std::shared_lock<std::shared_mutex> lock(share_mutex_);
    if (sync_method_map_.empty() ||
        !web_contents()->GetPrimaryMainFrame()->GetProcess()->GetChannel()) {
      return;
    }
  }

  web_contents()->GetPrimaryMainFrame()->ForEachRenderFrameHost(
      [this](content::RenderFrameHostImpl* frame) {
        content::AgentSchedulingGroupHost& agent_scheduling_group =
            frame->GetAgentSchedulingGroup();

        scoped_refptr<OhGinJavascriptBridgeMessageFilter> per_asg_filter =
            OhGinJavascriptBridgeMessageFilter::FromHost(
                agent_scheduling_group,
                /*create_if_not_exists=*/true);
        if (base::FeatureList::IsEnabled(features::kMBIMode)) {
          scoped_refptr<OhGinJavascriptBridgeObjectDeletionMessageFilter>
              process_global_filter =
                  OhGinJavascriptBridgeObjectDeletionMessageFilter::FromHost(
                      agent_scheduling_group.GetProcess(),
                      /*create_if_not_exists=*/true);
          process_global_filter->AddRoutingIdForHost(this, frame);
        }

        per_asg_filter->AddRoutingIdForHost(this, frame);
      });
}

content::WebContentsImpl* OhGinJavascriptBridgeDispatcherHost::web_contents()
    const {
  return static_cast<content::WebContentsImpl*>(
      content::WebContentsObserver::web_contents());
}

void OhGinJavascriptBridgeDispatcherHost::DidFinishNavigation(content::NavigationHandle* navigation_handle) {
  if (!navigation_handle->HasCommitted() || navigation_handle->IsErrorPage()) {
    LOG(ERROR) << "DidFinishNavigation invalid navigation_handle";
    return;
  }

  content::RenderFrameHost* render_frame_host = navigation_handle->GetRenderFrameHost();
  if (render_frame_host == nullptr) {
    LOG(ERROR) << "DidFinishNavigation get render frame host failed";
    return;
  }

  content::SiteInstance* site_instance = render_frame_host->GetSiteInstance();
  if (site_instance == nullptr) {
    LOG(ERROR) << "DidFinishNavigation get site instance failed";
    return;
  }
  GURL site_instance_gurl = site_instance->GetSiteURL();
  content::AgentSchedulingGroupHost& agent_scheduling_group =
      static_cast<content::RenderFrameHostImpl*>(render_frame_host)
          ->GetAgentSchedulingGroup();

  scoped_refptr<OhGinJavascriptBridgeMessageFilter> filter =
          OhGinJavascriptBridgeMessageFilter::FromHost(
              agent_scheduling_group, /*create_if_not_exists=*/false);
  if(filter) {
    filter->SetSiteInstanceGurl(site_instance_gurl);
  }
}

void OhGinJavascriptBridgeDispatcherHost::RenderFrameCreated(
    content::RenderFrameHost* render_frame_host) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  content::AgentSchedulingGroupHost& agent_scheduling_group =
      static_cast<content::RenderFrameHostImpl*>(render_frame_host)
          ->GetAgentSchedulingGroup();
  if (scoped_refptr<OhGinJavascriptBridgeMessageFilter> filter =
          OhGinJavascriptBridgeMessageFilter::FromHost(
              agent_scheduling_group, /*create_if_not_exists=*/false)) {
    filter->AddRoutingIdForHost(this, render_frame_host);
  } else {
    InstallFilterAndRegisterAllRoutingIds();
    // if filter not created, it will be re-created.
    // During application startup and loading, JS pre-registers ifream-related
    // hosts.
    if (!OhGinJavascriptBridgeMessageFilter::FromHost(agent_scheduling_group,
                                                      false)) {
      scoped_refptr<OhGinJavascriptBridgeMessageFilter> filter_new =
          OhGinJavascriptBridgeMessageFilter::FromHost(
              agent_scheduling_group, /*create_if_not_exists=*/true);
      filter_new->AddRoutingIdForHost(this, render_frame_host);
    }
  }
  std::shared_lock<std::shared_mutex> lock(share_mutex_);

  for (ObjectMethodMap::const_iterator iter = async_method_map_.begin();
       iter != async_method_map_.end(); ++iter) {
    base::Value::List async_method_list;
    for (auto async_method : iter->second.second) {
      async_method_list.Append(async_method);
    }
    SendAddNameObjectToRender(
        static_cast<content::RenderFrameHostImpl*>(render_frame_host),
        iter->second.first, iter->first, async_method_list, true);
  }
  for (const auto& [object_id, object_name_set] : object_id_map_) {
    for (const auto& object_name : object_name_set) {
      // sync_method_map_ has no async method infomation, so send an empty list.
      base::Value::List empty_list;
      SendAddNameObjectToRender(
          static_cast<content::RenderFrameHostImpl*>(render_frame_host),
          object_name, object_id, empty_list, false);
    }
  }
}

void OhGinJavascriptBridgeDispatcherHost::WebContentsDestroyed() {
  // Unretained() is safe because ForEachFrame() is synchronous.
  web_contents()->GetPrimaryMainFrame()->ForEachRenderFrameHost(
      [this](content::RenderFrameHostImpl* frame) {
        content::AgentSchedulingGroupHost& agent_scheduling_group =
            frame->GetAgentSchedulingGroup();
        scoped_refptr<OhGinJavascriptBridgeMessageFilter> filter =
            OhGinJavascriptBridgeMessageFilter::FromHost(
                agent_scheduling_group, /*create_if_not_exists=*/false);

        if (filter) {
          filter->RemoveHost(this);
        }
      });
}

void OhGinJavascriptBridgeDispatcherHost::RenderViewHostChanged(
    content::RenderViewHost* old_host,
    content::RenderViewHost* new_host) {
  content::AgentSchedulingGroupHost& agent_scheduling_group =
      static_cast<content::RenderViewHostImpl*>(new_host)
          ->GetAgentSchedulingGroup();
  scoped_refptr<OhGinJavascriptBridgeMessageFilter> filter =
      OhGinJavascriptBridgeMessageFilter::FromHost(
          agent_scheduling_group,
          /*create_if_not_exists=*/false);
  if (!filter) {
    InstallFilterAndRegisterAllRoutingIds();
  }
}

void OhGinJavascriptBridgeDispatcherHost::PrimaryPageChanged(
    content::Page& page) {
  content::AgentSchedulingGroupHost& agent_scheduling_group =
      static_cast<content::PageImpl&>(page)
          .GetMainDocument()
          .GetAgentSchedulingGroup();
  scoped_refptr<OhGinJavascriptBridgeMessageFilter> filter =
      OhGinJavascriptBridgeMessageFilter::FromHost(
          agent_scheduling_group,
          /*create_if_not_exists=*/false);
  if (!filter) {
    InstallFilterAndRegisterAllRoutingIds();
  }
}

void OhGinJavascriptBridgeDispatcherHost::RenderFrameHostChanged(
    content::RenderFrameHost* old_host,
    content::RenderFrameHost* new_host) {
  if (!install_filter_when_render_process_gone_) {
    return;
  }

  if (new_host == old_host || !new_host) {
    return;
  }

  InstallFilterAndRegisterAllRoutingIds();
  install_filter_when_render_process_gone_ = false;
}

void OhGinJavascriptBridgeDispatcherHost::PrimaryMainFrameRenderProcessGone(
    base::TerminationStatus status) {
  install_filter_when_render_process_gone_ = true;
}

void OhGinJavascriptBridgeDispatcherHost::ParseJson(
    const std::string& json_data,
    const int32_t& object_id,
    bool is_async) {
  if (json_data.empty()) {
    if (is_async) {
      javascript_async_permission_map_.erase(object_id);
    } else {
      javascript_sync_permission_map_.erase(object_id);
    }
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::ParseJson: permission "
                  "string is empty";
    return;
  }
  LOG(DEBUG)
      << "OhGinJavascriptBridgeDispatcherHost::ParseJson: permission string:"
      << json_data << ", object_id:" << object_id << ", is_async:" << is_async;
  std::optional<base::Value> root = base::JSONReader::Read(json_data);
  if (root && root->is_dict()) {
    std::map<std::string, std::map<std::string, JsProxyPermissionConfigData>>
        config;
    ParseJsProxyPermissionJson(
        root, "javascriptProxyPermission.urlPermissionList", &config);
    ParseJsProxyPermissionJson(root, "javascriptProxyPermission.methodList",
                               &config);
    for (auto it : config) {
      LOG(DEBUG)
          << "OhGinJavascriptBridgeDispatcherHost::ParseJson: method_name ="
          << it.first;
      for (auto item : it.second) {
        LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::ParseJson: host = "
                   << item.first << " scheme = " << item.second.scheme
                   << " host = " << item.second.host
                   << " port = " << item.second.port
                   << " path = " << item.second.path
                   << " method_name = " << item.second.method_name;
      }
    }
    if (is_async) {
      javascript_async_permission_map_[object_id] = config;
    } else {
      javascript_sync_permission_map_[object_id] = config;
    }
  }
}

void OhGinJavascriptBridgeDispatcherHost::ParseJsProxyPermissionJson(
    std::optional<base::Value>& root,
    const std::string& path,
    std::map<std::string, std::map<std::string, JsProxyPermissionConfigData>>*
        data_ptr) {
  if (root && root->is_dict()) {
    const auto& config_data = root->GetDict().FindByDottedPath(path);
    if (!config_data || !config_data->is_list()) {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseJsProxyPermissionJson: config_data null or not list";
      return;
    }
    for (const auto& it : config_data->GetList()) {
      const base::Value::Dict* dict_val = it.GetIfDict();
      if (!dict_val) {
        continue;
      }
      const std::string* method_name = dict_val->FindString("methodName");
      if (method_name) {
        if ((*method_name).empty()) {
          continue;
        }
        const base::Value::List* list_val =
            dict_val->FindList("urlPermissionList");
        for (const auto& val : *list_val) {
          JsProxyPermissionConfigData data;
          std::map<std::string, JsProxyPermissionConfigData*> config_map_tmp;
          data.method_name = *method_name;
          const base::Value::Dict* dict_val_tmp = val.GetIfDict();
          if (!dict_val_tmp) {
            LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                          "ParseJsProxyPermissionJson: not dict";
            continue;
          }
          ParseJsProxyPermissionJsonConfigMap(dict_val_tmp, &config_map_tmp,
                                              &data);
          for (const auto& item : config_map_tmp) {
            LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                          "ParseJsProxyPermissionJson: host: "
                       << item.first;
            (*data_ptr)[*method_name][item.first] = *item.second;
          }
        }
      } else {
        JsProxyPermissionConfigData data;
        std::map<std::string, JsProxyPermissionConfigData*> config_map_tmp;
        ParseJsProxyPermissionJsonConfigMap(dict_val, &config_map_tmp, &data);
        for (const auto& item : config_map_tmp) {
          LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                        "ParseJsProxyPermissionJson: host: "
                     << item.first;
          (*data_ptr)[""][item.first] = *item.second;
        }
      }
    }
  }
}

void OhGinJavascriptBridgeDispatcherHost::ParseJsProxyPermissionJsonConfigMap(
    const base::Value::Dict* dict_val,
    std::map<std::string, JsProxyPermissionConfigData*>* config_ptr,
    JsProxyPermissionConfigData* data_ptr) {
  if (!dict_val || !config_ptr || !data_ptr) {
    return;
  }
  const std::string* scheme = dict_val->FindString("scheme");
  if (!scheme) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                  "ParseJsProxyPermissionJsonConfigMap: no scheme";
    return;
  }
  const std::string* host = dict_val->FindString("host");
  if (!host) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                  "ParseJsProxyPermissionJsonConfigMap: no host";
    return;
  }
  const std::string* port = dict_val->FindString("port");
  if (!port) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                  "ParseJsProxyPermissionJsonConfigMap: no port";
    return;
  }
  const std::string* path = dict_val->FindString("path");
  if (!path) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                  "ParseJsProxyPermissionJsonConfigMap: no path";
    return;
  }
  if ((*scheme).empty() || (*host).empty()) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                  "ParseJsProxyPermissionJsonConfigMap: scheme or host empty";
    return;
  }
  data_ptr->scheme = *scheme;
  data_ptr->host = *host;
  data_ptr->port = *port;
  data_ptr->path = *path;
  (*config_ptr)[*host] = data_ptr;
}

void OhGinJavascriptBridgeDispatcherHost::AddNamedObjectForWebController(
    const std::string& object_name,
    const std::vector<std::string>& method_list,
    const std::vector<std::string>& async_method_list,
    const std::string& permission) {
  RemoveNamedObject(object_name, method_list);
  base::Value::List async_method_for_render;
  {
    std::unique_lock<std::shared_mutex> lock(share_mutex_);
    object_id_++;

    // add sync method
    MethodPair object_pair;
    std::unordered_set<std::string> method_set;
    for (const auto& s : method_list) {
      method_set.emplace(s);
    }
    object_pair.first = object_name;
    object_pair.second = method_set;
    sync_method_map_[object_id_] = object_pair;
    ParseJson(permission, object_id_, false);
    object_id_map_[object_id_].insert(object_name);

    // add async method
    MethodPair async_object_pair;
    std::unordered_set<std::string> async_method_set;
    for (const auto& s : async_method_list) {
      async_method_set.emplace(s);
      async_method_for_render.Append(s);
    }
    async_object_pair.first = object_name;
    async_object_pair.second = async_method_set;
    async_method_map_[object_id_] = async_object_pair;
    async_object_id_map_[object_id_].insert(object_name);
    ParseJson(permission, object_id_, true);
  }

  InstallFilterAndRegisterAllRoutingIds();

  web_contents()
      ->GetPrimaryMainFrame()
      ->ForEachRenderFrameHostIncludingSpeculative(
          [&object_name, &async_method_for_render,
           this](content::RenderFrameHostImpl* render_frame_host) {
            render_frame_host->AddNamedObject(object_name, this->object_id_,
                                              async_method_for_render, true);
          });
}

void OhGinJavascriptBridgeDispatcherHost::AddNamedObjectForWebViewController(
    const std::string& object_name,
    const std::vector<std::string>& method_list,
    const std::vector<std::string>& async_method_list,
    const ObjectID object_id,
    const std::string& permission) {
  if (object_id <= 0) {
    LOG(ERROR) << "AddNamedObject:" << object_name
               << " failed due to object_id == " << object_id;
    return;
  }
  RemoveNamedObject(object_name, method_list);
  base::Value::List async_method_for_render;
  {
    std::unique_lock<std::shared_mutex> lock(share_mutex_);
    // add sync method
    MethodPair object_pair;
    std::unordered_set<std::string> method_set;
    for (std::string s : method_list) {
      method_set.emplace(s);
    }
    object_pair.first = object_name;
    object_pair.second = method_set;
    sync_method_map_[object_id] = object_pair;
    ParseJson(permission, object_id, false);
    object_id_map_[object_id].insert(object_name);

    // add async method
    MethodPair async_object_pair;
    std::unordered_set<std::string> async_method_set;
    for (std::string s : async_method_list) {
      async_method_set.emplace(s);
      async_method_for_render.Append(s);
    }
    async_object_pair.first = object_name;
    async_object_pair.second = async_method_set;
    async_method_map_[object_id] = async_object_pair;
    async_object_id_map_[object_id].insert(object_name);
    ParseJson(permission, object_id, true);
  }

  InstallFilterAndRegisterAllRoutingIds();

  web_contents()
      ->GetPrimaryMainFrame()
      ->ForEachRenderFrameHostIncludingSpeculative(
          [&object_name, &async_method_for_render, object_id,
           this](content::RenderFrameHostImpl* render_frame_host) {
            this->SendAddNameObjectToRender(render_frame_host, object_name,
                                            object_id, async_method_for_render,
                                            true);
          });
}

void OhGinJavascriptBridgeDispatcherHost::SendAddNameObjectToRender(
    content::RenderFrameHostImpl* render_frame_host,
    const std::string& object_name,
    int32_t object_id,
    base::Value::List& async_method_list,
    bool need_update) {
  if (!render_frame_host) {
    return;
  }
  // TODO(ARKWEB) lagency ipc will race with other mojom msg,
  // https://chromium-review.googlesource.com/c/chromium/src/+/4968451
  render_frame_host->AddNamedObject(object_name, object_id, async_method_list,
                                    need_update);
}

void OhGinJavascriptBridgeDispatcherHost::AddNamedObject(
    const std::string& object_name,
    const std::vector<std::string>& method_list,
    const std::vector<std::string>& async_method_list,
    const ObjectID object_id,
    const std::string& permission) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  // In order to be compatible with older webcotroller,
  // Currently, the webcontroller save the object_id on the core side, get
  // object from ace side by object_name the webviewcontroller save the
  // object_id on the web_webview side, get object from web_webview side by
  // object_id
  if (object_id ==
      static_cast<ObjectID>(JavaScriptObjIdErrorCode::WEBCONTROLLERERROR)) {
    AddNamedObjectForWebController(object_name, method_list, async_method_list,
                                   permission);
  } else {
    AddNamedObjectForWebViewController(
        object_name, method_list, async_method_list, object_id, permission);
  }
}

void OhGinJavascriptBridgeDispatcherHost::AddNativeNamedObjectForWebController(
    const std::string& object_name,
    const std::vector<std::string>& method_list,
    bool is_async,
    const std::string& permission) {
  ObjectMethodMap& other_method_map =
      is_async ? sync_method_map_ : async_method_map_;
  std::vector<std::string> other_method_list;
  {
    std::unique_lock<std::shared_mutex> lock(share_mutex_);
    ObjectMethodMap::iterator it;
    for (it = other_method_map.begin(); it != other_method_map.end(); ++it) {
      if (it->second.first == object_name) {
        std::unordered_set<std::string> method_set = it->second.second;
        for (std::string method : method_set) {
          other_method_list.emplace_back(method);
        }
        break;
      }
    }
  }
  if (is_async) {
    AddNamedObjectForWebController(object_name, other_method_list, method_list,
                                   permission);
  } else {
    AddNamedObjectForWebController(object_name, method_list, other_method_list,
                                   permission);
  }
}

void OhGinJavascriptBridgeDispatcherHost::
    AddNativeNamedObjectForWebViewController(
        const std::string& object_name,
        const std::vector<std::string>& method_list,
        const ObjectID object_id,
        bool is_async,
        const std::string& permission) {
  ObjectMethodMap& other_method_map =
      is_async ? sync_method_map_ : async_method_map_;
  std::vector<std::string> other_method_list;
  {
    ObjectMethodMap::iterator it;
    for (it = other_method_map.begin(); it != other_method_map.end(); ++it) {
      if (it->second.first == object_name) {
        std::unordered_set<std::string> method_set = it->second.second;
        for (std::string method : method_list) {
          other_method_list.emplace_back(method);
        }
        break;
      }
    }
  }
  if (is_async) {
    AddNamedObjectForWebViewController(object_name, other_method_list,
                                       method_list, object_id, permission);
  } else {
    AddNamedObjectForWebViewController(
        object_name, method_list, other_method_list, object_id, permission);
  }
}

void OhGinJavascriptBridgeDispatcherHost::AddNativeNamedObject(
    const std::string& object_name,
    const std::vector<std::string>& method_list,
    const ObjectID object_id,
    bool is_async,
    const std::string& permission) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  // In order to be compatible with older webcotroller,
  // Currently, the webcontroller save the object_id on the core side, get
  // object from ace side by object_name the webviewcontroller save the
  // object_id on the web_webview side, get object from web_webview side by
  // object_id
  std::vector<std::string> empty_sync_method_list;
  if (object_id ==
      static_cast<ObjectID>(JavaScriptObjIdErrorCode::WEBCONTROLLERERROR)) {
    AddNativeNamedObjectForWebController(object_name, method_list, is_async,
                                         permission);
  } else {
    AddNativeNamedObjectForWebViewController(object_name, method_list,
                                             object_id, is_async, permission);
  }
}

void OhGinJavascriptBridgeDispatcherHost::RemoveNamedObject(
    const std::string& object_name,
    const std::vector<std::string>& method_list) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  if (sync_method_map_.empty() && async_method_map_.empty()) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::RemoveNamedObject:Map "
                  "is empty!";
    return;
  }
  RemoveNamedObjectInternal(object_name, false);
  RemoveNamedObjectInternal(object_name, true);
}

bool OhGinJavascriptBridgeDispatcherHost::RemoveNamedObjectInternal(
    const std::string& object_name,
    bool is_async) {
  LOG(DEBUG)
      << "OhGinJavascriptBridgeDispatcherHost::RemoveNamedObjectInternal "
      << "target object name: " << object_name;
  auto& method_map = is_async ? async_method_map_ : sync_method_map_;
  bool ret = false;
  if (method_map.empty()) {
    return ret;
  }
  auto& id_map = is_async ? async_object_id_map_ : object_id_map_;
  std::unique_lock<std::shared_mutex> lock(share_mutex_);
  for (auto& [object_id, object_name_set] : id_map) {
    for (const std::string& now_object_name : object_name_set) {
      if (now_object_name == object_name) {
        object_name_set.erase(object_name);
        if (object_name_set.size() == 0) {
          int32_t object_id_n = object_id;
          id_map.erase(object_id_n);
          method_map.erase(object_id_n);
        }

        ret = true;
        // |name| may come from |named_objects_|. Make a copy of name so that if
        // |name| is from |named_objects_| it'll be valid after the remove
        // below.
        const std::string copied_name(object_name);

        web_contents()
            ->GetPrimaryMainFrame()
            ->ForEachRenderFrameHostIncludingSpeculative(
                [&copied_name](
                    content::RenderFrameHostImpl* render_frame_host) {
                  render_frame_host->Send(
                      new OhGinJavascriptBridgeMsg_RemoveNamedObject(
                          render_frame_host->GetRoutingID(), copied_name));
                });
        return ret;
      }
    }
  }
  return ret;
}

void OhGinJavascriptBridgeDispatcherHost::OnGetMethods(
    int32_t object_id,
    std::set<std::string>* returned_method_names) {
  // get from web_webview side
  std::shared_lock<std::shared_mutex> lock(share_mutex_);
  if (sync_method_map_.find(object_id) == sync_method_map_.end()) {
    if (!client_) {
      LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::"
                    "OnGetMethods: client_ is null";
      return;
    }
    CefRefPtr<CefValue> cefValue;
    client_->GetJavaScriptObjectMethods(object_id, cefValue);
    if (!cefValue) {
      LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::"
                    "OnGetMethods: cefValue is null";
      return;
    }
    size_t length = cefValue->GetList()->GetSize();
    for (size_t i = 0; i < length; ++i) {
      returned_method_names->insert(
          cefValue->GetList()->GetValue(i)->GetString().ToString());
    }
    return;
  }
  MethodPair p = sync_method_map_[object_id];
  for (auto iter = p.second.begin(); iter != p.second.end(); ++iter) {
    returned_method_names->emplace(*iter);
  }
}

void OhGinJavascriptBridgeDispatcherHost::OnHasMethod(
    int32_t object_id,
    const std::string& method_name,
    bool* result) {
  *result = false;
  std::shared_lock<std::shared_mutex> lock(share_mutex_);

  // find in sync methods
  if (sync_method_map_.find(object_id) != sync_method_map_.end()) {
    MethodPair p = sync_method_map_[object_id];
    if (p.second.find(method_name) != p.second.end()) {
      *result = true;
      return;
    }
  }

  // find in async methods
  if (async_method_map_.find(object_id) != async_method_map_.end()) {
    MethodPair p = async_method_map_[object_id];
    if (p.second.find(method_name) != p.second.end()) {
      *result = true;
      return;
    }
  }

  if (!client_) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::"
                  "OnHasMethod: client_ is null";
    return;
  }

  *result = client_->HasJavaScriptObjectMethods(object_id, method_name);
}

void OhGinJavascriptBridgeDispatcherHost::OnHasAsyncThreadMethod(
    int32_t object_id,
    const std::string& method_name,
    bool* result) {
  *result = false;
  std::shared_lock<std::shared_mutex> lock(share_mutex_);

  // find in sync methods
  if (sync_method_map_.find(object_id) != sync_method_map_.end()) {
    MethodPair p = sync_method_map_[object_id];
    if (p.second.find(method_name) != p.second.end()) {
      if (!client_) {
        LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::"
                  "OnHasAsyncThreadMethod: client_ is null";
        return;
      }
      *result = client_->HasNativeAsyncThreadJavaScriptMethods(p.first, method_name);
      return;
    }
  }
}

std::unique_ptr<base::Value> ParseCefValueTObaseValueHelper(
    CefRefPtr<CefValue> result) {
  std::unique_ptr<base::Value> baseValue = std::make_unique<base::Value>();
  if (!result.get()) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::"
                  "ParseCefValueTObaseValueHelper: result is null";
    return baseValue;
  }

  switch (result->GetType()) {
    case CefValueType::VTYPE_NULL: {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseCefValueTObaseValueHelper: VTYPE_NULL";
      return std::make_unique<base::Value>();
    }
    case CefValueType::VTYPE_INT: {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseCefValueTObaseValueHelper: VTYPE_INT = "
                 << result->GetInt();
      return std::make_unique<base::Value>(result->GetInt());
    }
    case CefValueType::VTYPE_DOUBLE: {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseCefValueTObaseValueHelper: VTYPE_DOUBLE = "
                 << result->GetDouble();
      return std::make_unique<base::Value>(result->GetDouble());
    }
    case CefValueType::VTYPE_BOOL: {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseCefValueTObaseValueHelper: VTYPE_BOOL = "
                 << result->GetBool();
      return std::make_unique<base::Value>(result->GetBool());
    }
    case CefValueType::VTYPE_STRING: {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseCefValueTObaseValueHelper: VTYPE_STRING";
      return std::make_unique<base::Value>(result->GetString().ToString());
    }
    case CefValueType::VTYPE_LIST: {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseCefValueTObaseValueHelper: VTYPE_LIST";
      auto cefArr = result->GetList();
      std::unique_ptr<base::Value::List> value =
          std::make_unique<base::Value::List>();
      if (!cefArr) {
        LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::"
                      "ParseCefValueTObaseValueHelper: cefArr is null";
        break;
      }
      int size = static_cast<int>(cefArr->GetSize());
      for (int i = 0; i < size; ++i) {
        auto cefValTmp = cefArr->GetValue(i);
        auto baseValTmp = ParseCefValueTObaseValueHelper(cefValTmp);
        value->Append(std::move(*baseValTmp));
      }
      return std::make_unique<base::Value>(std::move(*value));
    }
    case CefValueType::VTYPE_DICTIONARY: {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseCefValueTObaseValueHelper: VTYPE_DICTIONARY";
      auto cefDict = result->GetDictionary();
      std::unique_ptr<base::Value::Dict> value =
          std::make_unique<base::Value::Dict>();
      if (!cefDict) {
        LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::"
                      "ParseCefValueTObaseValueHelper: cefDict is null";
        break;
      }
      CefDictionaryValue::KeyList cefKeys;
      cefDict->GetKeys(cefKeys);
      for (auto& key : cefKeys) {
        auto cefValTmp = cefDict->GetValue(key);
        auto baseValTmp = ParseCefValueTObaseValueHelper(cefValTmp);
        value->Set(key.ToString(), std::move(*baseValTmp));
      }
      return std::make_unique<base::Value>(std::move(*value));
    }
    case CefValueType::VTYPE_BINARY: {
      auto size = result->GetBinary()->GetSize();
      auto buff = std::make_unique<char[]>(size + 1);
      result->GetBinary()->GetData(buff.get(), size, 0);
      buff[size] = '\0';
      int32_t objId;
      std::string str(buff.get(), size);
      std::vector<std::string> strList =
          base::SplitString(str, ";", base::KEEP_WHITESPACE, base::SPLIT_WANT_ALL);
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseCefValueTObaseValueHelper: strList.size() ="
                 << strList.size() << ", size =" << size;
      if (strList.size() != JS_BRIDGE_BINARY_ARGS_COUNT) {
        baseValue = OhGinJavascriptBridgeValue::CreateObjectIDValue(-1);
        break;
      }
      std::istringstream ss(strList[1]);
      ss >> objId;
      if (objId <= 0) {
        LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::"
                      "ParseCefValueTObaseValueHelper: objId == "
                   << objId << " is error";
        baseValue = OhGinJavascriptBridgeValue::CreateObjectIDValue(-1);
        break;
      }
      if (strList[0] == "TYPE_OBJECT_ID") {
        baseValue = OhGinJavascriptBridgeValue::CreateObjectIDValue(objId);
        break;
      }

      return OhGinJavascriptBridgeValue::CreateObjectIDValue(objId);
    }
    case CefValueType::VTYPE_INVALID: {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseCefValueTObaseValueHelper: VTYPE_INVALID";
      return std::make_unique<base::Value>();
    }
    default: {
      LOG(INFO) << "OhGinJavascriptBridgeDispatcherHost::"
                   "ParseCefValueTObaseValueHelper: not support value type";
      return std::make_unique<base::Value>();
    }
  }

  return baseValue;
}

CefRefPtr<CefValue> ParseBaseValueTOCefValueHelper(ValueConvertState* state,
                                                   base::Value* value) {
  CefRefPtr<CefValue> cefValue = CefValue::Create();
  ValueConvertState::Level state_level(state);
  if (state->HasReachedMaxRecursionDepth()) {
    return cefValue;
  }
  if (!value) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::"
                  "ParseBaseValueTOCefValueHelper: value is null ";
    return cefValue;
  }
  switch (value->type()) {
    case base::Value::Type::INTEGER: {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseBaseValueTOCefValueHelper: INTEGER = "
                 << value->GetInt();
      cefValue->SetInt(value->GetInt());
      return cefValue;
    }
    case base::Value::Type::DOUBLE: {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseBaseValueTOCefValueHelper: DOUBLE = "
                 << value->GetDouble();
      cefValue->SetDouble(value->GetDouble());
      return cefValue;
    }
    case base::Value::Type::BOOLEAN: {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseBaseValueTOCefValueHelper: BOOLEAN = "
                 << value->GetBool();
      cefValue->SetBool(value->GetBool());
      return cefValue;
    }
    case base::Value::Type::STRING: {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseBaseValueTOCefValueHelper: STRING";
      cefValue->SetStdString(*value->GetIfString());
      return cefValue;
    }
    case base::Value::Type::LIST: {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseBaseValueTOCefValueHelper: LIST";
      size_t length = value->GetList().size();
      length = std::min(length, MAX_DATA_LENGTH);
      auto cefList = CefListValue::Create();
      for (size_t i = 0; i < length; i++) {
        base::Value& child = value->GetList()[i];
        auto cefVal = ParseBaseValueTOCefValueHelper(state, &child);
        cefList->SetValue(i, cefVal);
      }
      cefValue->SetList(cefList);
      return cefValue;
    }
    case base::Value::Type::DICT: {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseBaseValueTOCefValueHelper: DICTIONARY";
      auto cefDict = CefDictionaryValue::Create();
      size_t index = 0;
      for (base::Value::Dict::iterator iter = value->GetDict().begin();
           iter != value->GetDict().end(); iter++) {
        index++;
        if (index > MAX_DATA_LENGTH) {
          break;
        }
        const std::string& key = iter->first;
        auto found = value->GetDict().Find(key);
        auto cefVal = ParseBaseValueTOCefValueHelper(state, found);
        cefDict->SetValue(CefString(key), cefVal);
      }
      cefValue->SetDictionary(cefDict);
      return cefValue;
    }
    case base::Value::Type::BINARY: {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseBaseValueTOCefValueHelper: BINARY";
      std::unique_ptr<const OhGinJavascriptBridgeValue> gin_value(
          OhGinJavascriptBridgeValue::FromValue(value));
      if (gin_value &&
          gin_value->GetType() == OhGinJavascriptBridgeValue::TYPE_OBJECT_ID) {
        if (OhGinJavascriptBridgeValue::ContainsOhGinJavascriptBridgeValue(
                value)) {
          OhGinJavascriptBridgeDispatcherHost::ObjectID object_id;
          if (gin_value->GetAsObjectID(&object_id)) {
            if (object_id <= 0) {
              LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::"
                            "ParseBaseValueTOCefValueHelper: object_id == "
                         << object_id << " is error";
            }
            std::string bin = std::string("TYPE_OBJECT_ID") + std::string(";") +
                              std::to_string(object_id);
            auto cefBin = CefBinaryValue::Create(bin.c_str(), bin.size());
            cefValue->SetBinary(cefBin);
          }
        }
      } else if (gin_value &&
                 gin_value->GetType() ==
                     OhGinJavascriptBridgeValue::TYPE_H5_OBJECT_ID) {
        if (OhGinJavascriptBridgeValue::ContainsOhGinJavascriptBridgeValue(
                value)) {
          std::string h5_object_id_with_names;
          if (gin_value->GetAsObjectIDWithMdNames(&h5_object_id_with_names)) {
            LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                          "ParseBaseValueTOCefValueHelper: TYPE_H5_OBJECT_ID "
                          "h5_object_id_with_names.empty()= "
                       << h5_object_id_with_names.empty();
            std::string bin = std::string("TYPE_H5_OBJECT_ID") +
                              std::string(";") + h5_object_id_with_names;
            auto cefBin = CefBinaryValue::Create(bin.c_str(), bin.size());
            cefValue->SetBinary(cefBin);
          }
        }
      } else if (gin_value &&
                 gin_value->GetType() ==
                     OhGinJavascriptBridgeValue::TYPE_H5_FUNCTION_ID) {
        if (OhGinJavascriptBridgeValue::ContainsOhGinJavascriptBridgeValue(
                value)) {
          OhGinJavascriptBridgeDispatcherHost::ObjectID h5_function_id;
          if (gin_value->GetAsObjectID(&h5_function_id)) {
            LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                          "ParseBaseValueTOCefValueHelper: TYPE_H5_FUNCTION_ID "
                          "h5_function_id == "
                       << h5_function_id;
            std::string bin = std::string("TYPE_H5_FUNCTION_ID") +
                              std::string(";") + std::to_string(h5_function_id);
            auto cefBin = CefBinaryValue::Create(bin.c_str(), bin.size());
            cefValue->SetBinary(cefBin);
          }
        }
      }
      return cefValue;
    }
    case base::Value::Type::NONE:
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::"
                    "ParseBaseValueTOCefValueHelper: NONE";
      break;
    default:
      LOG(INFO) << "OhGinJavascriptBridgeDispatcherHost::"
                   "ParseBaseValueTOCefValueHelper: not support value type";
      break;
  }
  return cefValue;
}

std::unique_ptr<base::Value> ParseCefValueTOBaseValue(
    CefRefPtr<CefListValue> result) {
  std::unique_ptr<base::Value> value = std::make_unique<base::Value>();
  if (!result.get() || result->GetSize() == 0) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::"
                  "ParseCefValueTOBaseValue: result is null or empty";
    return value;
  }

  CefRefPtr<CefValue> argument = result->GetValue(0);
  value = ParseCefValueTObaseValueHelper(argument);

  return value;
}

CefRefPtr<CefListValue> ParseBaseValueTOCefValue(base::Value::List* value) {
  auto cefList = CefListValue::Create();
  if (!value) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::"
                  "ParseBaseValueTOCefValue: value is null";
    return cefList;
  }
  for (size_t i = 0; i < value->size(); ++i) {
    base::Value& child = (*value)[i];
    ValueConvertState state;
    auto cefVal = ParseBaseValueTOCefValueHelper(&state, &child);
    cefList->SetValue(i, cefVal);
  }
  return cefList;
}

bool OhGinJavascriptBridgeDispatcherHost::CheckIsInJsPermission(
    const std::string& document_url,
    const std::string& method_name,
    int32_t object_id,
    bool is_async) {
  LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::CheckIsInJsPermission: "
             << ", method_name: " << method_name << ", object_id: " << object_id
             << ", is_async: " << is_async;
  GURL gurl(document_url);
  PermissionMap map_tmp = is_async ? javascript_async_permission_map_
                                   : javascript_sync_permission_map_;
  if (map_tmp.empty() || map_tmp.find(object_id) == map_tmp.end()) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::CheckIsInJsPermission: "
                  "permission map is empty or no object_id("
               << object_id << ") key";
    return true;
  }

  if (map_tmp[object_id].find("") == map_tmp[object_id].end() ||
      map_tmp[object_id][""].find(gurl.host()) ==
          map_tmp[object_id][""].end()) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::CheckIsInJsPermission: "
                  "object level, permission map no host("
               << gurl.host() << ") key";
    return false;
  }
  JsProxyPermissionConfigData obj_permission =
      javascript_sync_permission_map_[object_id][""][gurl.host()];
  if (gurl.scheme().empty() || gurl.scheme() != obj_permission.scheme) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::CheckIsInJsPermission: "
                  "object level, url scheme("
               << gurl.scheme() << ") mismatch permission scheme("
               << obj_permission.scheme << ")";
    return false;
  }
  if (gurl.host().empty() || gurl.host() != obj_permission.host) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::CheckIsInJsPermission: "
                  "object level, url host("
               << gurl.host() << ") mismatch permission host("
               << obj_permission.host << ")";
    return false;
  }
  if (!obj_permission.port.empty() && gurl.port() != obj_permission.port) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::CheckIsInJsPermission: "
                  "object level, url port("
               << gurl.port() << ") mismatch permission port("
               << obj_permission.port << ")";
    return false;
  }
  if (!obj_permission.path.empty() &&
      gurl.path().compare(0, obj_permission.path.size(), obj_permission.path)) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::CheckIsInJsPermission: "
                  "object level, url path("
               << gurl.path() << ") mismatch permission path("
               << obj_permission.path << ")";
    return false;
  }

  if (map_tmp[object_id].find(method_name) == map_tmp[object_id].end()) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::CheckIsInJsPermission: "
                  "method level,"
                  " permission map no method name("
               << method_name << ")";
    return true;
  }

  if (map_tmp[object_id][method_name].find(gurl.host()) ==
      map_tmp[object_id][method_name].end()) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::CheckIsInJsPermission: "
                  "method level, permission map no host key";
    return false;
  }
  JsProxyPermissionConfigData method_permission =
      javascript_sync_permission_map_[object_id][method_name][gurl.host()];
  if (gurl.scheme().empty() || gurl.scheme() != method_permission.scheme) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::CheckIsInJsPermission: "
                  "method level, url scheme("
               << gurl.scheme() << ") mismatch permission scheme("
               << method_permission.scheme << ")";
    return false;
  }
  if (gurl.host().empty() || gurl.host() != method_permission.host) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::CheckIsInJsPermission: "
                  "method level, url host("
               << gurl.host() << ") mismatch permission host("
               << method_permission.host << ")";
    return false;
  }
  if (!method_permission.port.empty() &&
      gurl.port() != method_permission.port) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::CheckIsInJsPermission: "
                  "method level, url port("
               << gurl.port() << ") mismatch permission port("
               << method_permission.port << ")";
    return false;
  }
  if (!method_permission.path.empty() &&
      gurl.path().compare(0, method_permission.path.size(),
                          method_permission.path)) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::CheckIsInJsPermission: "
                  "method level, url path("
               << gurl.path() << ") mismatch permission path("
               << method_permission.path << ")";
    return false;
  }

  return true;
}

namespace {
#if BUILDFLAG(ARKWEB_SECURE_JAVASCRIPT_PROXY)
void LastCallingFrameUrlDestructorFunc(void* value) {
  if (value) {
    delete reinterpret_cast<LastCallingFrameUrlInfo*>(value);
  }
}

base::ThreadLocalStorage::Slot& LastCallingFrameUrlContentTLS() {
  static base::NoDestructor<base::ThreadLocalStorage::Slot>
      last_calling_frame_url_tls(&LastCallingFrameUrlDestructorFunc);
  return *last_calling_frame_url_tls;
}
#endif
}  // namespace

#if BUILDFLAG(ARKWEB_SECURE_JAVASCRIPT_PROXY)
const char* OhGinJavascriptBridgeDispatcherHost::GetLastCallingFrameUrlTLS() {
  void* value = LastCallingFrameUrlContentTLS().Get();
  if (value) {
    return static_cast<LastCallingFrameUrlInfo*>(value)->url.c_str();
  }

  return nullptr;
}
#endif

void OhGinJavascriptBridgeDispatcherHost::OnInvokeMethod(
    int routing_id,
    int32_t object_id,
    const std::string& document_url,
    const std::string& method_name,
    const base::Value::List& arguments,
    base::Value::List* wrapped_result,
    OhGinJavascriptBridgeError* error_code) {
  LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::OnInvokeMethod: "
                "method name: "
             << method_name;
  if (!CheckIsInJsPermission(document_url, method_name, object_id, false) ||
      !CheckIsInJsPermission(document_url, method_name, object_id, true)) {
    *error_code =
        OhGinJavascriptBridgeError::kOhGinJavascriptBridgePermissionDenied;
    return;
  }

#if BUILDFLAG(ARKWEB_SECURE_JAVASCRIPT_PROXY)
  {
    LastCallingFrameUrlInfo* url_info = new LastCallingFrameUrlInfo();
    url_info->url = document_url;
    LastCallingFrameUrlContentTLS().Set(reinterpret_cast<void*>(url_info));
  }
#endif

  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI) &&
      web_contents()) {
    OhJavascriptInjector* javascriptInjector =
        OhJavascriptInjector::FromWebContents(web_contents());
    if (javascriptInjector) {
      javascriptInjector->SetLastCallingFrameUrl(document_url);
    }
  }
  base::Value::List* argument = const_cast<base::Value::List*>(&arguments);

  CefRefPtr<CefListValue> ceflistvalue = ParseBaseValueTOCefValue(argument);

  std::string classname = "";
  {
    std::shared_lock<std::shared_mutex> lock(share_mutex_);
    if (sync_method_map_.find(object_id) != sync_method_map_.end()) {
      MethodPair object_pair = sync_method_map_[object_id];
      classname = object_pair.first;
    } else if (async_method_map_.find(object_id) != async_method_map_.end()) {
      MethodPair object_pair = async_method_map_[object_id];
      classname = object_pair.first;
    }
  }

  std::string method = method_name;

  CefRefPtr<CefListValue> result = CefListValue::Create();

  if (!client_) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::OnInvokeMethod: "
                  "client_ is null";
    return;
  }
  // 为了兼容老版本webcotroller方式, classname可能为空
  int error = client_->NotifyJavaScriptResult(ceflistvalue, method, classname,
                                              result, routing_id, object_id);
  *error_code = static_cast<OhGinJavascriptBridgeError>(error);
  if (error != 0) {
    return;
  }

  wrapped_result->Append(
      base::Value::FromUniquePtrValue(ParseCefValueTOBaseValue(result)));
}

void OhGinJavascriptBridgeDispatcherHost::OnInvokeMethodAsync(
    int routing_id,
    int32_t object_id,
    const std::string& document_url,
    const std::string& method_name,
    const base::Value::List& arguments) {
  LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::OnInvokeMethodAsync: "
                "async method name: "
             << method_name;
  if (!CheckIsInJsPermission(document_url, method_name, object_id, false) ||
      !CheckIsInJsPermission(document_url, method_name, object_id, true)) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::OnInvokeMethodAsync: "
                  "jsb permission denied";
    return;
  }

#if BUILDFLAG(ARKWEB_SECURE_JAVASCRIPT_PROXY)
  {
    LastCallingFrameUrlInfo* url_info = new LastCallingFrameUrlInfo();
    url_info->url = document_url;
    LastCallingFrameUrlContentTLS().Set(reinterpret_cast<void*>(url_info));
  }
#endif

  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI) &&
      web_contents()) {
    OhJavascriptInjector* javascriptInjector =
        OhJavascriptInjector::FromWebContents(web_contents());
    if (javascriptInjector) {
      javascriptInjector->SetLastCallingFrameUrl(document_url);
    }
  }
  base::Value::List* argument = const_cast<base::Value::List*>(&arguments);

  CefRefPtr<CefListValue> ceflistvalue = ParseBaseValueTOCefValue(argument);

  std::string classname = "";
  {
    std::shared_lock<std::shared_mutex> lock(share_mutex_);
    if (sync_method_map_.find(object_id) != sync_method_map_.end()) {
      MethodPair object_pair = sync_method_map_[object_id];
      classname = object_pair.first;
    } else if (async_method_map_.find(object_id) != async_method_map_.end()) {
      MethodPair object_pair = async_method_map_[object_id];
      classname = object_pair.first;
    }
  }

  std::string method = method_name;

  CefRefPtr<CefListValue> result = CefListValue::Create();

  if (!client_) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::OnInvokeMethodAsync: "
                  "client_ is null";
    return;
  }
  // 为了兼容老版本webcotroller方式, classname可能为空
  client_->NotifyJavaScriptResult(ceflistvalue, method, classname, result,
                                  routing_id, object_id);
}

void OhGinJavascriptBridgeDispatcherHost::OnInvokeMethodFlowbuf(
    int routing_id,
    int32_t object_id,
    const std::string& document_url,
    const std::string& method_name,
    const base::Value::List& arguments,
    int fd,
    base::Value::List* wrapped_result,
    OhGinJavascriptBridgeError* error_code) {
  if (!CheckIsInJsPermission(document_url, method_name, object_id, false) ||
      !CheckIsInJsPermission(document_url, method_name, object_id, true)) {
    *error_code =
        OhGinJavascriptBridgeError::kOhGinJavascriptBridgePermissionDenied;
    return;
  }

#if BUILDFLAG(ARKWEB_SECURE_JAVASCRIPT_PROXY)
  {
    LastCallingFrameUrlInfo* url_info = new LastCallingFrameUrlInfo();
    url_info->url = document_url;
    LastCallingFrameUrlContentTLS().Set(reinterpret_cast<void*>(url_info));
  }
#endif

  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI) &&
      web_contents()) {
    OhJavascriptInjector* javascriptInjector =
        OhJavascriptInjector::FromWebContents(web_contents());
    if (javascriptInjector) {
      javascriptInjector->SetLastCallingFrameUrl(document_url);
    }
  }
  base::Value::List* argument = const_cast<base::Value::List*>(&arguments);

  CefRefPtr<CefListValue> ceflistvalue = ParseBaseValueTOCefValue(argument);

  std::string classname = "";
  {
    std::shared_lock<std::shared_mutex> lock(share_mutex_);
    if (sync_method_map_.find(object_id) != sync_method_map_.end()) {
      MethodPair object_pair = sync_method_map_[object_id];
      classname = object_pair.first;
    } else if (async_method_map_.find(object_id) != async_method_map_.end()) {
      MethodPair object_pair = async_method_map_[object_id];
      classname = object_pair.first;
    }
  }

  std::string method = method_name;

  CefRefPtr<CefListValue> result = CefListValue::Create();

  if (!client_) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::OnInvokeMethodFlowbuf: "
                  "client_ is null";
    return;
  }
  // 为了兼容老版本webcotroller方式, classname可能为空
  int error = client_->NotifyJavaScriptResultFlowbuf(
      ceflistvalue, method, classname, fd, result, routing_id, object_id);
  *error_code = static_cast<OhGinJavascriptBridgeError>(error);
  if (error != 0) {
    return;
  }

  wrapped_result->Append(
      base::Value::FromUniquePtrValue(ParseCefValueTOBaseValue(result)));
}

void OhGinJavascriptBridgeDispatcherHost::OnInvokeMethodFlowbufAsync(
    int routing_id,
    int32_t object_id,
    const std::string& document_url,
    const std::string& method_name,
    const base::Value::List& arguments,
    int fd) {
  if (!CheckIsInJsPermission(document_url, method_name, object_id, false) ||
      !CheckIsInJsPermission(document_url, method_name, object_id, true)) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::"
                  "OnInvokeMethodFlowbufAsync: jsb permission denied";
    return;
  }
  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI) &&
      web_contents()) {
    OhJavascriptInjector* javascriptInjector =
        OhJavascriptInjector::FromWebContents(web_contents());
    if (javascriptInjector) {
      javascriptInjector->SetLastCallingFrameUrl(document_url);
    }
  }
  base::Value::List* argument = const_cast<base::Value::List*>(&arguments);

  CefRefPtr<CefListValue> ceflistvalue = ParseBaseValueTOCefValue(argument);

  std::string classname = "";
  {
    std::shared_lock<std::shared_mutex> lock(share_mutex_);
    if (sync_method_map_.find(object_id) != sync_method_map_.end()) {
      MethodPair object_pair = sync_method_map_[object_id];
      classname = object_pair.first;
    } else if (async_method_map_.find(object_id) != async_method_map_.end()) {
      MethodPair object_pair = async_method_map_[object_id];
      classname = object_pair.first;
    }
  }

  std::string method = method_name;

  CefRefPtr<CefListValue> result = CefListValue::Create();

  if (!client_) {
    LOG(ERROR)
        << "OhGinJavascriptBridgeDispatcherHost::OnInvokeMethodFlowbufAsync: "
           "client_ is null";
    return;
  }
  // 为了兼容老版本webcotroller方式, classname可能为空
  client_->NotifyJavaScriptResultFlowbuf(ceflistvalue, method, classname, fd,
                                         result, routing_id, object_id);
}

void OhGinJavascriptBridgeDispatcherHost::OnObjectWrapperDeleted(
    int routing_id,
    ObjectID object_id) {
  LOG(DEBUG)
      << "OhGinJavascriptBridgeDispatcherHost::OnObjectWrapperDeleted called";
  if (!client_) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::"
                  "OnObjectWrapperDeleted: client_ is null";
    return;
  }
  client_->RemoveJavaScriptObjectHolder(routing_id, object_id);
}

void OhGinJavascriptBridgeDispatcherHost::DoCallH5Function(
    int32_t routing_id,
    int32_t h5_object_id,
    const std::string&
        h5_method_name,  // IF h5_method_name empty, call anonymous function
    const std::vector<CefRefPtr<CefValue>>& args) {
  LOG(DEBUG) << "OhGinJavascriptBridgeDispatcherHost::DoCallH5Function in";
  content::WebContentsImpl* web_contents_impl =
      static_cast<content::WebContentsImpl*>(web_contents());
  if (!web_contents_impl) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcherHost::DoCallH5Function "
                  "web_contents_impl null";
    return;
  }

  content::RenderFrameHost* target_frame =
      web_contents_impl->AsWebContentsImplExt()
          ->GetTargetFramesIncludingPending(routing_id);

  if (target_frame) {
    LOG(DEBUG)
        << "OhGinJavascriptBridgeDispatcherHost::DoCallH5Function called";
    base::Value::List base_args;
    std::unique_ptr<base::Value> value = std::make_unique<base::Value>();
    for (auto& item : args) {
      value = ParseCefValueTObaseValueHelper(item);
      base_args.Append(base::Value::FromUniquePtrValue(std::move(value)));
    }

    target_frame->Send(new OhGinJavascriptBridgeMsg_DoCallH5Function(
        target_frame->GetRoutingID(), routing_id, h5_object_id, h5_method_name,
        base_args));
  }
}
}  // namespace NWEB
