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

#include "ohos_cef_ext/libcef/browser/net/extra_headers_throttle.h"
#include "base/feature_list.h"
#include "base/strings/string_util.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "net/http/http_request_headers.h"
#include "net/url_request/redirect_info.h"
#include "services/network/public/cpp/resource_request.h"

#include "base/logging.h"

namespace throttle {

std::map<std::string, std::string> ExtraHeadersThrottle::extra_headers_map;

ExtraHeadersThrottle::ExtraHeadersThrottle() {}

ExtraHeadersThrottle::~ExtraHeadersThrottle() = default;

void ExtraHeadersThrottle::WillStartRequest(network::ResourceRequest* request,
                                            bool* defer) {
  AddExtraHeadersIfNeeded(request->url, &request->headers);
  if (!added_headers_.empty()) {
    original_origin_ = url::Origin::Create(request->url);
  }
}

void ExtraHeadersThrottle::WillRedirectRequest(
    net::RedirectInfo* redirect_info,
    const network::mojom::URLResponseHead& response_head,
    bool* defer,
    std::vector<std::string>* to_be_removed_request_headers,
    net::HttpRequestHeaders* modified_request_headers,
    net::HttpRequestHeaders* modified_cors_exempt_request_headers) {

  if (!added_headers_.empty()) {
    bool is_same_domain = net::registry_controlled_domains::SameDomainOrHost(
        redirect_info->new_url, original_origin_,
        net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES);

    if (!is_same_domain) {
      // The headers we added must be removed.
      to_be_removed_request_headers->insert(
          to_be_removed_request_headers->end(),
          std::make_move_iterator(added_headers_.begin()),
          std::make_move_iterator(added_headers_.end()));
      added_headers_.clear();
    }
  }
}

void ExtraHeadersThrottle::AddExtraHeadersIfNeeded(
    const GURL& url,
    net::HttpRequestHeaders* headers) {
  std::string extra_headers = ExtraHeadersThrottle::GetExtraHeaders(url);
  if (extra_headers.empty())
    return;

  net::HttpRequestHeaders temp_headers;
  temp_headers.AddHeadersFromString(extra_headers);
  for (net::HttpRequestHeaders::Iterator it(temp_headers); it.GetNext();) {
    if (headers->HasHeader(it.name()))
      continue;

    headers->SetHeader(it.name(), it.value());
    added_headers_.push_back(it.name());
  }
}

void ExtraHeadersThrottle::SetExtraHeaders(const GURL& url,
    const std::string& headers) {
  if (!url.is_valid()) {
    return;
  }
  if (!headers.empty()) {
    ExtraHeadersThrottle::extra_headers_map[url.spec()] = headers;
  } else {
    ExtraHeadersThrottle::extra_headers_map.erase(url.spec());
  }
}

std::string ExtraHeadersThrottle::GetExtraHeaders(const GURL& url) {
  if (!url.is_valid()) {
    return std::string();
  }
  std::map<std::string, std::string>::iterator iter =
      ExtraHeadersThrottle::extra_headers_map.find(url.spec());
  return iter != ExtraHeadersThrottle::extra_headers_map.end() ? iter->second : std::string();
}

}  // namespace throttle
