// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_cef_ext/libcef/browser/net/ohos_applink_throttle.h"

#include "base/logging.h"
#include "content/public/browser/web_contents.h"
#include "include/cef_browser.h"
#include "libcef/browser/browser_host_base.h"
#include "libcef/common/frame_util.h"
#include "services/network/public/cpp/resource_request.h"

namespace throttle {
class CefOpenAppLinkCallbackImpl : public CefOpenAppLinkCallback {
 public:
  explicit CefOpenAppLinkCallbackImpl(
      base::WeakPtr<OhosAppLinkThrottle> throttle)
      : throttle_(throttle) {}

  void Continue() override {
    if (throttle_) {
      throttle_->ContinueLoad();
    }
  }

  void Cancel() override {
    if (throttle_) {
      throttle_->CancelLoad();
    }
  }

 private:
  base::WeakPtr<OhosAppLinkThrottle> throttle_;
  IMPLEMENT_REFCOUNTING(CefOpenAppLinkCallbackImpl);
};

// OhosAppLinkThrottle::OhosAppLinkThrottle(content::FrameTreeNodeId
// frame_tree_node_id)
//   : frame_tree_node_id_(frame_tree_node_id) {}

OhosAppLinkThrottle::OhosAppLinkThrottle(
    content::FrameTreeNodeId frame_tree_node_id,
    bool is_client_redirect)
    : frame_tree_node_id_(frame_tree_node_id),
      is_client_redirect_(is_client_redirect) {}

OhosAppLinkThrottle::~OhosAppLinkThrottle() {}

void OhosAppLinkThrottle::WillStartRequest(network::ResourceRequest* request,
                                           bool* defer) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  content::WebContents* web_contents =
      content::WebContents::FromFrameTreeNodeId(frame_tree_node_id_);
  if (web_contents == nullptr) {
    return;
  }

  CefRefPtr<CefBrowserHostBase> browser_host =
      CefBrowserHostBase::GetBrowserForContents(web_contents);
  if (browser_host == nullptr) {
    return;
  }

  if (auto client = browser_host->GetClient()) {
    if (auto handler = client->GetRequestHandler()) {
      CefRefPtr<CefOpenAppLinkCallback> callback =
          new CefOpenAppLinkCallbackImpl(weak_factory_.GetWeakPtr());
      *defer = handler->AsCefRequestHandlerExt()->OnOpenAppLink(
          CefString(request->url.spec()), callback);
    }
  }
}

void OhosAppLinkThrottle::WillRedirectRequest(
    net::RedirectInfo* redirect_info,
    const network::mojom::URLResponseHead& response_head,
    bool* defer,
    std::vector<std::string>* to_be_removed_request_headers,
    net::HttpRequestHeaders* modified_request_headers,
    net::HttpRequestHeaders* modified_cors_exempt_request_headers) {}

void OhosAppLinkThrottle::WillProcessResponse(
    const GURL& response_url,
    network::mojom::URLResponseHead* response_head,
    bool* defer) {}

void OhosAppLinkThrottle::ContinueLoad() {
  if (delegate_) {
    delegate_->Resume();
  }
}

void OhosAppLinkThrottle::CancelLoad() {
  if (delegate_) {
    if (is_client_redirect_) {
      delegate_->Resume();
    } else {
      delegate_->CancelWithError(net::ERR_ABORTED);
    }
  }
}
}  // namespace throttle
