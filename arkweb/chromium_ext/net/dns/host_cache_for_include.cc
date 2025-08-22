/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
std::vector<IPEndPoint> HostCache::LookupByHost(
    url::SchemeHostPort destination) {
  if (!IsValidHostname(destination.host())) {
    return std::vector<IPEndPoint>();
  }

  DnsQueryType query_type = DnsQueryType::UNSPECIFIED;
  HostResolverFlags flags = HOST_RESOLVER_DEFAULT_FAMILY_SET_DUE_TO_NO_IPV6;
  HostResolverSource source = HostResolverSource::ANY;
  Key key(destination, query_type, flags, source, NetworkAnonymizationKey());
  const std::pair<const HostCache::Key, HostCache::Entry>* cache_result =
      Lookup(key, base::TimeTicks::Now(), true);
  if (!cache_result || cache_result->second.ip_endpoints().empty()) {
    key.host_resolver_flags = 0;
    const std::pair<const HostCache::Key, HostCache::Entry>* cache_result_new =
        Lookup(key, base::TimeTicks::Now(), true);
    if (!cache_result_new || cache_result_new->second.ip_endpoints().empty()) {
      return std::vector<IPEndPoint>();
    }
    return cache_result_new->second.ip_endpoints();
  }
  return cache_result->second.ip_endpoints();
}
#endif  // ARKWEB_LOGGER_REPORT