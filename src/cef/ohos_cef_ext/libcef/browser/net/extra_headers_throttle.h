/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#ifndef CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_NET_EXTRA_HEADERS_THROTTLE_H_
#define CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_NET_EXTRA_HEADERS_THROTTLE_H_

#include <string>
#include <vector>
#include <map>

#include "base/memory/raw_ptr.h"
#include "third_party/blink/public/common/loader/url_loader_throttle.h"
#include "url/origin.h"

class GURL;

namespace net {
class HttpRequestHeaders;
}

namespace throttle {

class ExtraHeadersThrottle : public blink::URLLoaderThrottle {
 public:
  ExtraHeadersThrottle();

  ExtraHeadersThrottle(const ExtraHeadersThrottle&) = delete;
  ExtraHeadersThrottle& operator=(const ExtraHeadersThrottle&) = delete;

  ~ExtraHeadersThrottle() override;

  // blink::URLLoaderThrottle implementation:
  void WillStartRequest(network::ResourceRequest* request,
                        bool* defer) override;
  void WillRedirectRequest(
      net::RedirectInfo* redirect_info,
      const network::mojom::URLResponseHead& response_head,
      bool* defer,
      std::vector<std::string>* to_be_removed_request_headers,
      net::HttpRequestHeaders* modified_request_headers,
      net::HttpRequestHeaders* modified_cors_exempt_request_headers) override;

  static void SetExtraHeaders(const GURL& url, const std::string& headers);
  static std::string GetExtraHeaders(const GURL& url);

 private:
  void AddExtraHeadersIfNeeded(const GURL& url,
                               net::HttpRequestHeaders* headers);

  static std::map<std::string, std::string> extra_headers_map;
  std::vector<std::string> added_headers_;
  url::Origin original_origin_;
};

}  // namespace throttle

#endif  // CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_NET_EXTRA_HEADERS_THROTTLE_H_
