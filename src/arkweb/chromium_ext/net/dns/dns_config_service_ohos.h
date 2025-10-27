// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_DNS_DNS_CONFIG_SERVICE_OHOS_H_
#define NET_DNS_DNS_CONFIG_SERVICE_OHOS_H_

#include <netinet/in.h>
#include <network/netmanager/net_connection.h>
#include <resolv.h>
#include <sys/types.h>

#include <memory>

#include "base/compiler_specific.h"
#include "base/gtest_prod_util.h"
#include "net/base/ip_endpoint.h"
#include "net/base/net_export.h"
#include "net/dns/dns_config_service.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace net {
struct DnsConfig;

// Use DnsConfigService::CreateSystemService to use it outside of tests.
namespace internal {

// Service for reading and watching Ohos system DNS
// settings.
// This object is not thread-safe and methods may perform blocking I/O so
// methods must be called on a sequence that allows blocking (i.e.
// base::MayBlock). It may be constructed on a different sequence than which
// it's later called on. WatchConfig() must be called prior to ReadConfig().
class NET_EXPORT_PRIVATE DnsConfigServiceOhos : public DnsConfigService {
 public:
  DnsConfigServiceOhos();

  DnsConfigServiceOhos(const DnsConfigServiceOhos&) = delete;
  DnsConfigServiceOhos& operator=(const DnsConfigServiceOhos&) = delete;

  ~DnsConfigServiceOhos() override;

  void RefreshConfig() override;

 protected:
  // DnsConfigService:
  void ReadConfigNow() override;
  bool StartWatching() override;

  // Create |config_reader_|.
  void CreateReader();

 private:
  FRIEND_TEST_ALL_PREFIXES(DnsConfigServiceOhosTest, ChangeConfigMultipleTimes);
  class Watcher;
  class ConfigReader;

  std::unique_ptr<Watcher> watcher_;
  std::unique_ptr<ConfigReader> config_reader_;
};

absl::optional<IPEndPoint> NET_EXPORT_PRIVATE
GetIpv4EndPoint(NetConn_NetAddr& netAddr);

absl::optional<IPEndPoint> NET_EXPORT_PRIVATE
GetIpv6EndPoint(NetConn_NetAddr& netAddr);

absl::optional<std::vector<IPEndPoint>> NET_EXPORT_PRIVATE
GetNameserversFromConnectProperties();

}  // namespace internal

}  // namespace net

#endif  // NET_DNS_DNS_CONFIG_SERVICE_OHOS_H_
