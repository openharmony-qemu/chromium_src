/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
std::map<std::string, std::string> GetFetchMetadataHeaders(
    const GURL& target_url,
    network::mojom::RequestMode mode,
    bool has_user_activation,
    network::mojom::RequestDestination dest,
    const std::optional<url::Origin>& initiator) {
  std::map<std::string, std::string> headers;
  if (!IsUrlPotentiallyTrustworthy(target_url)) {
    return headers;
  }

  // Other requests default to `kSameOrigin`, and walk through the request's URL
  // chain to calculate the correct value.
  auto header_value = net::OriginRelation::kSameOrigin;
  if (!initiator.has_value()) {
    // 对于没有initiator的情况，保持header_value为kSameOrigin
    // 后续会特殊处理为"none"
  } else {
    header_value = std::max(header_value, net::GetOriginRelation(target_url, initiator.value()));
  }

  // 转换为Sec-Fetch-Site头的值
  if (!initiator.has_value()) {
    headers[std::string(kSecFetchSite)] = "none";  // 对应原来的kNoOrigin
  } else {
    // 手动转换net::OriginRelation到字符串
    std::string site_value;
    switch (header_value) {
      case net::OriginRelation::kSameOrigin:
        site_value = "same-origin";
        break;
      case net::OriginRelation::kSameSite:
        site_value = "same-site";
        break;
      case net::OriginRelation::kCrossSite:
        site_value = "cross-site";
        break;
    }
    headers[std::string(kSecFetchSite)] = site_value;
  }

  headers[std::string(kSecFetchMode)] = RequestModeToString(mode);

  if (has_user_activation) {
    headers[std::string(kSecFetchUser)] = "?1";
  }

  std::string destination_value = dest == mojom::RequestDestination::kEmpty
                                      ? "empty"
                                      : RequestDestinationToString(dest);
  headers[std::string(kSecFetchDest)] = destination_value;

  return headers;
}
#endif