/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_nweb/src/cef_delegate/nweb_scheme_handler_factory.h"

#include "base/logging.h"
#include "cef/libcef/browser/thread_util.h"
#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"
#include "ohos_nweb/src/nweb_impl.h"

namespace {

typedef std::map<std::string, CefRefPtr<OHOS::NWeb::NWebSchemeHandlerFactory>>
    SchemeHandlerFactoryMap;
SchemeHandlerFactoryMap g_scheme_handler_factory_map;
base::Lock g_scheme_handler_factory_map_lock;
}  // namespace

namespace OHOS::NWeb {

CefRefPtr<NWebSchemeHandlerFactory>
NWebSchemeHandlerFactory::GetOrCreateForScheme(const std::string& scheme) {
  base::AutoLock scoped_lock(g_scheme_handler_factory_map_lock);
  if (!g_scheme_handler_factory_map.count(scheme)) {
    CefRefPtr<NWebSchemeHandlerFactory> factory =
        new NWebSchemeHandlerFactory();
    CefRegisterSchemeHandlerFactory(scheme, "", factory, false);
    CefRegisterSchemeHandlerFactory(scheme, "", factory, true);
    g_scheme_handler_factory_map[scheme] = factory;
  }

  return g_scheme_handler_factory_map[scheme];
}

void NWebSchemeHandlerFactory::ClearSchemeHandlers(const std::string& web_tag) {
  LOG(DEBUG) << "ClearSchemeHandlers web_tag: " << web_tag;
  base::AutoLock scoped_lock(g_scheme_handler_factory_map_lock);
  for (auto& factory : g_scheme_handler_factory_map) {
    if (factory.second) {
      factory.second->RemoveSchemeHandler(web_tag);
    }
  }
}

void NWebSchemeHandlerFactory::ClearAllSchemeHandlers(ArkWeb_SchemeHandler* scheme_handler) {
  LOG(DEBUG) << "ClearAllSchemeHandlers";
  base::AutoLock scoped_lock(g_scheme_handler_factory_map_lock);
  for (auto& factory : g_scheme_handler_factory_map) {
    if (factory.second) {
        factory.second->RemoveSchemeHandler(scheme_handler);
    }
  }
}

void NWebSchemeHandlerFactory::RemoveSchemeHandler(ArkWeb_SchemeHandler* scheme_handler) {
    base::AutoLock scoped_lock(lock_);
    for (auto it = scheme_handler_map_.begin(); it != scheme_handler_map_.end();) {
        if (it->second == scheme_handler) {
            LOG(ERROR) << "Destroy a not cleared schemehandler! ";
            it = scheme_handler_map_.erase(it);
        } else {
            ++it;
        }
    }
}


void NWebSchemeHandlerFactory::ClearServiceWorkerSchemeHandler() {
  base::AutoLock scoped_lock(g_scheme_handler_factory_map_lock);
  for (auto& factory : g_scheme_handler_factory_map) {
    if (factory.second) {
      factory.second->RemoveServiceWorkerSchemeHandler();
    }
  }
}

NWebSchemeHandlerFactory::NWebSchemeHandlerFactory() {}

std::string NWebSchemeHandlerFactory::GetWebTag(CefRefPtr<CefBrowser> browser) {
  if (!browser) {
    return "";
  }

  int nweb_id = -1;
  nweb_id = browser->GetHost()->GetNWebId();
  NWebImpl* nweb = NWebImpl::FromID(nweb_id);
  if (!nweb) {
    return "";
  }
  std::string web_tag = nweb->GetWebTag();
  LOG(DEBUG) << "scheme_handler nweb_id: " << nweb_id
             << " web_tag: " << web_tag;
  return web_tag;
}

NO_SANITIZE("cfi-icall")
CefRefPtr<CefResourceHandler> NWebSchemeHandlerFactory::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request) {
  bool intercept = false;
  bool from_service_worker = (frame == nullptr);

  if (from_service_worker) {
    if (!scheme_handler_for_sw_ || !scheme_handler_for_sw_->on_request_start) {
      LOG(INFO) << "scheme_handler not set handler for service worker.";
      return nullptr;
    }
    if (scheme_handler_for_sw_->fromEts && !CEF_CURRENTLY_ON_UIT()) {
      LOG(INFO) << "scheme_handler from ets should from UI thread";
      return nullptr;
    }
    if (!scheme_handler_for_sw_->fromEts && !CEF_CURRENTLY_ON_IOT()) {
      LOG(INFO) << "scheme_handler not from ets should from IO thread";
      return nullptr;
    }
    ArkWeb_ResourceRequest* resource_request =
        new ArkWeb_ResourceRequest(request);
    if (scheme_handler_for_sw_->fromEts) {
      resource_request->SetForEts(true);
    }
    ArkWeb_ResourceHandler* resource_handler =
        new ArkWeb_ResourceHandler(resource_request, this, "", true);
    scheme_handler_for_sw_->on_request_start(
        scheme_handler_for_sw_, resource_request, resource_handler, &intercept);
    if (!intercept) {
      delete resource_request;
      delete resource_handler;
      return nullptr;
    }
    return resource_handler->pipe_resource_handler_;
  }

  // Try to get handler for web tag.
  std::string web_tag = GetWebTag(browser);
  if (web_tag == "") {
    LOG(INFO) << "scheme_handler web_tag is empty";
    return nullptr;
  }

  ArkWeb_SchemeHandler* handler = FromTag(web_tag);
  if (!handler || !handler->on_request_start) {
    LOG(INFO) << "scheme_handler not set handler for " << web_tag;
    return nullptr;
  }
  if (handler->fromEts && !CEF_CURRENTLY_ON_UIT()) {
    LOG(DEBUG) << "scheme handler from ets should from UI thread";
    return nullptr;
  }

  if (!handler->fromEts && !CEF_CURRENTLY_ON_IOT()) {
    LOG(DEBUG) << "scheme handler not from ets should from IO thread";
    return nullptr;
  }
  ArkWeb_ResourceRequest* resource_request =
      new ArkWeb_ResourceRequest(request);
  if (handler->fromEts) {
    resource_request->SetForEts(true);
  }
  ArkWeb_ResourceHandler* resource_handler =
      new ArkWeb_ResourceHandler(resource_request, this, web_tag, false);
  handler->on_request_start(handler, resource_request, resource_handler,
                            &intercept);
  LOG(INFO) << "scheme_handler on_request_start intercept: " << intercept
            << " webTag: " << web_tag;
  if (!intercept) {
    delete resource_request;
    delete resource_handler;
    return nullptr;
  }
  return resource_handler->pipe_resource_handler_;
}

void NWebSchemeHandlerFactory::SetSchemeHandler(
    const std::string& web_tag,
    ArkWeb_SchemeHandler* scheme_handler) {
  base::AutoLock scoped_lock(lock_);
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return;
  }

  if (web_tag == "") {
    LOG(ERROR) << "scheme_handler web tag is empty";
    return;
  }
  LOG(ERROR) << "scheme_handler SetSchemeHandler for " << web_tag;
  scheme_handler_map_[web_tag] = scheme_handler;
}

void NWebSchemeHandlerFactory::SetSchemeHandlerForSW(
    ArkWeb_SchemeHandler* scheme_handler) {
  base::AutoLock scoped_lock(lock_);
  if (!scheme_handler) {
    LOG(ERROR) << "scheme_handler scheme handler is nullptr.";
    return;
  }
  scheme_handler_for_sw_ = scheme_handler;
}

void NWebSchemeHandlerFactory::RemoveSchemeHandler(const std::string& web_tag) {
  base::AutoLock scoped_lock(lock_);
  scheme_handler_map_.erase(web_tag);
}

void NWebSchemeHandlerFactory::RemoveServiceWorkerSchemeHandler() {
  base::AutoLock scoped_lock(lock_);
  scheme_handler_for_sw_ = nullptr;
}

ArkWeb_SchemeHandler* NWebSchemeHandlerFactory::FromTag(
    const std::string& web_tag) {
  base::AutoLock scoped_lock(lock_);
  if (auto it = scheme_handler_map_.find(web_tag);
      it != scheme_handler_map_.end()) {
    return it->second;
  }

  return nullptr;
}

NO_SANITIZE("cfi-icall")
void NWebSchemeHandlerFactory::OnRequestStop(
    const ArkWeb_ResourceRequest* resource_request,
    const std::string& web_tag,
    bool from_service_worker) {
  if (from_service_worker) {
    if (!scheme_handler_for_sw_ || !scheme_handler_for_sw_->on_request_stop) {
      LOG(ERROR) << "scheme_handler handler for service worker is not found.";
      return;
    }
    scheme_handler_for_sw_->on_request_stop(scheme_handler_for_sw_,
                                            resource_request);
    return;
  }

  ArkWeb_SchemeHandler* handler = FromTag(web_tag);
  if (!handler || !handler->on_request_stop) {
    LOG(DEBUG) << "scheme_handler not set handler for " << web_tag;
    return;
  }
  handler->on_request_stop(handler, resource_request);
}
}  //  namespace OHOS::NWeb
