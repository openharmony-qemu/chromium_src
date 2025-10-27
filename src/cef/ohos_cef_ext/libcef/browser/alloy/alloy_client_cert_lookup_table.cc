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

#include "libcef/browser/alloy/alloy_client_cert_lookup_table.h"

#include "base/logging.h"

#if BUILDFLAG(IS_ARKWEB)
std::map<std::string, AlloyCert> AlloyClientCertLookupTable::certs_;
std::set<std::string> AlloyClientCertLookupTable::denieds_;

void AlloyClientCertLookupTable::Clear() {
  certs_.clear();
  denieds_.clear();
}

bool AlloyClientCertLookupTable::IsDenied(const std::string& host, int port) {
  std::string host_port = HostAndPort(host, port);
  auto it = denieds_.find(host_port);
  if (it != denieds_.end()) {
    return true;
  }
  return false;
}

void AlloyClientCertLookupTable::Allow(const std::string& host,
                                       int port,
                                       const std::string& private_key,
                                       const std::string& cert_chain) {
  std::string host_port = HostAndPort(host, port);
  AlloyCert cert(private_key, cert_chain);
  certs_[host_port] = cert;
  denieds_.erase(host_port);
}

void AlloyClientCertLookupTable::Deny(const std::string& host, int port) {
  std::string host_port = HostAndPort(host, port);
  certs_.erase(host_port);
  denieds_.insert(host_port);
}

bool AlloyClientCertLookupTable::GetCertData(const std::string& host,
                                             int port,
                                             AlloyCert& cert) {
  std::string host_port = HostAndPort(host, port);
  auto it = certs_.find(host_port);
  if (it != certs_.end()) {
    cert = (*it).second;
    return true;
  }
  return false;
}

std::string AlloyClientCertLookupTable::HostAndPort(const std::string& host,
                                                    int port) {
  return host + ":" + std::to_string(port);
}

#endif  // IS_ARKWEB
