// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_NET_OHOS_APPLINK_THROTTLE_H_
#define CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_NET_OHOS_APPLINK_THROTTLE_H_
#pragma once

#include <memory>
#include <vector>

#include "content/public/browser/frame_tree_node_id.h"
#include "third_party/blink/public/common/loader/url_loader_throttle.h"

namespace throttle {
class OhosAppLinkThrottle : public blink::URLLoaderThrottle {
 public:
  // explicit OhosAppLinkThrottle(content::FrameTreeNodeId frame_tree_node_id);
  OhosAppLinkThrottle(content::FrameTreeNodeId frame_tree_node_id,
                      bool is_client_redirect);
  OhosAppLinkThrottle(const OhosAppLinkThrottle&) = delete;
  OhosAppLinkThrottle& operator=(const OhosAppLinkThrottle&) = delete;
  ~OhosAppLinkThrottle() override;

  // blink::URLLoaderThrottle:
  void WillStartRequest(network::ResourceRequest* request,
                        bool* defer) override;
  void WillRedirectRequest(
      net::RedirectInfo* redirect_info,
      const network::mojom::URLResponseHead& response_head,
      bool* defer,
      std::vector<std::string>* to_be_removed_request_headers,
      net::HttpRequestHeaders* modified_request_headers,
      net::HttpRequestHeaders* modified_cors_exempt_request_headers) override;
  void WillProcessResponse(const GURL& response_url,
                           network::mojom::URLResponseHead* response_head,
                           bool* defer) override;
  void ContinueLoad();
  void CancelLoad();

 private:
  content::FrameTreeNodeId frame_tree_node_id_;
  bool is_client_redirect_ = false;

  SEQUENCE_CHECKER(sequence_checker_);
  base::WeakPtrFactory<OhosAppLinkThrottle> weak_factory_{this};
};

}  // namespace throttle

#endif  // CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_NET_OHOS_APPLINK_THROTTLE_H_
