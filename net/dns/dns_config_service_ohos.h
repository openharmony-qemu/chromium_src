// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
 
#ifndef NET_DNS_DNS_CONFIG_SERVICE_OHOS_H_
#define NET_DNS_DNS_CONFIG_SERVICE_OHOS_H_
 
#include <memory>
 
#include "base/time/time.h"
#include "net/base/net_export.h"
#include "net/dns/dns_config_service.h"
// #include "net/base/network_change_notifier.h"
 
namespace net {
 
// Use DnsConfigService::CreateSystemService to use it outside of tests.
namespace internal {
 
// Service for reading and watching harmony system DNS settings. This object is
// not thread-safe and methods may perform blocking I/O so methods must be
// called on a sequence that allows blocking (i.e. base::MayBlock). It may be
// constructed on a different sequence than which it's later called on.
class NET_EXPORT_PRIVATE DnsConfigServiceOhos : public DnsConfigService {
 public:
  static constexpr base::TimeDelta kConfigChangeDelay = base::Milliseconds(50);
 
  DnsConfigServiceOhos();
  ~DnsConfigServiceOhos() override;
 
  DnsConfigServiceOhos(const DnsConfigServiceOhos&) = delete;
  DnsConfigServiceOhos& operator=(const DnsConfigServiceOhos&) = delete;
 
  void RefreshConfig() override;
 
 protected:
  // DnsConfigService:
  void ReadConfigNow() override;
  bool StartWatching() override;
 
 private:
  class Watcher;
  class ConfigReader;
 
  std::unique_ptr<Watcher> watcher_;
  std::unique_ptr<ConfigReader> config_reader_;
};
 
}  // namespace internal
}  // namespace net
 
#endif  // NET_DNS_DNS_CONFIG_SERVICE_OHOS_H_