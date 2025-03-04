// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/dns/dns_config_service_ohos.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/files/file_path_watcher.h"
#include "base/functional/bind.h"
#include "base/lazy_instance.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/sequence_checker.h"
#include "base/sys_byteorder.h"
#include "base/task/single_thread_task_runner.h"
#include "base/threading/scoped_blocking_call.h"
#include "base/time/time.h"
#include "build/build_config.h"
#include "net/base/network_change_notifier.h"
#include "net/dns/dns_config.h"
#include "net/dns/dns_hosts.h"
#include "net/dns/public/dns_protocol.h"
#include "net/dns/serial_worker.h"

namespace net {

namespace internal {

namespace {

const base::FilePath::CharType kFilePathHosts[] =
    FILE_PATH_LITERAL("/etc/hosts");

const base::FilePath::CharType kFilePathConfig[] =
    FILE_PATH_LITERAL(_PATH_RESCONF);

enum AddressFamily { FAMILY_IPV4 = 1, FAMILY_IPV6 = 2 };

class DnsConfigWatcher {
 public:
  using CallbackType = base::RepeatingCallback<void(bool succeeded)>;

  bool Watch(const CallbackType& callback) {
    callback_ = callback;
    return watcher_.Watch(base::FilePath(kFilePathConfig),
                          base::FilePathWatcher::Type::kNonRecursive,
                          base::BindRepeating(&DnsConfigWatcher::OnCallback,
                                              base::Unretained(this)));
  }

 private:
  void OnCallback(const base::FilePath& path, bool error) {
    callback_.Run(!error);
  }

  base::FilePathWatcher watcher_;
  CallbackType callback_;
};

absl::optional<DnsConfig> ReadDnsConfig() {
  base::ScopedBlockingCall scoped_blocking_call(FROM_HERE,
                                                base::BlockingType::MAY_BLOCK);

  absl::optional<DnsConfig> dns_config;
  absl::optional<std::vector<IPEndPoint>> new_nameservers =
      GetNameserversFromConnectProperties();
  if (!new_nameservers) {
    return absl::nullopt;
  }
  dns_config = DnsConfig(std::move(*new_nameservers));
  dns_config->unhandled_options = false;
  // Override |fallback_period| value to match default setting on Windows.
  dns_config->fallback_period = kDnsDefaultFallbackPeriod;
  return dns_config;
}

}  // namespace

class DnsConfigServiceOhos::Watcher : public DnsConfigService::Watcher {
 public:
  explicit Watcher(DnsConfigServiceOhos& service)
      : DnsConfigService::Watcher(service) {}

  Watcher(const Watcher&) = delete;
  Watcher& operator=(const Watcher&) = delete;

  ~Watcher() override = default;

  bool Watch() override {
    CheckOnCorrectSequence();

    bool success = true;
    if (!config_watcher_.Watch(base::BindRepeating(&Watcher::OnConfigChanged,
                                                   base::Unretained(this)))) {
      LOG(ERROR) << "DNS config watch failed to start.";
      success = false;
    }
    if (!hosts_watcher_.Watch(
            base::FilePath(kFilePathHosts),
            base::FilePathWatcher::Type::kNonRecursive,
            base::BindRepeating(&Watcher::OnHostsFilePathWatcherChange,
                                base::Unretained(this)))) {
      LOG(ERROR) << "DNS hosts watch failed to start.";
      success = false;
    }
    return success;
  }

 private:
  void OnHostsFilePathWatcherChange(const base::FilePath& path, bool error) {
    OnHostsChanged(!error);
  }

  DnsConfigWatcher config_watcher_;
  base::FilePathWatcher hosts_watcher_;
};

// A SerialWorker that uses libresolv to initialize res_state and converts
// it to DnsConfig.
class DnsConfigServiceOhos::ConfigReader : public SerialWorker {
 public:
  explicit ConfigReader(DnsConfigServiceOhos& service) : service_(&service) {
    // Allow execution on another thread; nothing thread-specific about
    // constructor.
    DETACH_FROM_SEQUENCE(sequence_checker_);
  }

  ~ConfigReader() override = default;

  ConfigReader(const ConfigReader&) = delete;
  ConfigReader& operator=(const ConfigReader&) = delete;

  std::unique_ptr<SerialWorker::WorkItem> CreateWorkItem() override {
    return std::make_unique<WorkItem>();
  }

  bool OnWorkFinished(std::unique_ptr<SerialWorker::WorkItem>
                          serial_worker_work_item) override {
    DCHECK(serial_worker_work_item);
    DCHECK(!IsCancelled());

    WorkItem* work_item = static_cast<WorkItem*>(serial_worker_work_item.get());
    if (work_item->dns_config_.has_value()) {
      service_->OnConfigRead(std::move(work_item->dns_config_).value());
      return true;
    } else {
      LOG(WARNING) << "Failed to read DnsConfig.";
      return false;
    }
  }

 private:
  class WorkItem : public SerialWorker::WorkItem {
   public:
    explicit WorkItem() {}

    void DoWork() override {
      dns_config_.emplace();
      dns_config_->unhandled_options = false;
      std::vector<std::string> servers = NetworkChangeNotifier::GetDnsServers();
      for (auto& server : servers) {
        if (server.empty()) {
          continue;
        }

        auto dns_address = IPAddress::FromIPLiteral(server);
        if (!dns_address || (*dns_address).empty()) {
          continue;
        }
        IPEndPoint dns_server(std::move(*dns_address),
                              dns_protocol::kDefaultPort);
        dns_config_->nameservers.push_back(dns_server);
        LOG(INFO) << "dns name server " << dns_server.ToString();
      }
    }

   private:
    friend class ConfigReader;
    absl::optional<DnsConfig> dns_config_;
  };

  // Raw pointer to owning DnsConfigService.
  const raw_ptr<DnsConfigServiceOhos> service_;
};

DnsConfigServiceOhos::DnsConfigServiceOhos()
    : DnsConfigService(kFilePathHosts) {
  // Allow constructing on one thread and living on another.
  DETACH_FROM_SEQUENCE(sequence_checker_);
}

DnsConfigServiceOhos::~DnsConfigServiceOhos() {
  if (config_reader_) {
    config_reader_->Cancel();
  }
}

void DnsConfigServiceOhos::RefreshConfig() {
  // do nothing
}

void DnsConfigServiceOhos::ReadConfigNow() {
  if (!config_reader_) {
    CreateReader();
  }
  config_reader_->WorkNow();
}

bool DnsConfigServiceOhos::StartWatching() {
  CreateReader();
  watcher_ = std::make_unique<Watcher>(*this);
  return watcher_->Watch();
}

void DnsConfigServiceOhos::CreateReader() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(!config_reader_);
  config_reader_ = std::make_unique<ConfigReader>(*this);
}

absl::optional<IPEndPoint> GetIpv4EndPoint(NetConn_NetAddr& net_addr) {
  IPEndPoint ipe;
  struct sockaddr_in sa_ip4;
  sa_ip4.sin_family = AF_INET;
  sa_ip4.sin_port =
      base::HostToNet16(net_addr.port == 0 ? NS_DEFAULTPORT : net_addr.port);
  inet_pton(AF_INET, net_addr.address, &sa_ip4.sin_addr);

  if (!ipe.FromSockAddr(reinterpret_cast<const struct sockaddr*>(&sa_ip4),
                        sizeof sa_ip4)) {
    LOG(INFO) << "GetIpv4EndPoint: sin_addr and sin_port fetch failed";
    return absl::nullopt;
  }
  return absl::make_optional(ipe);
}

absl::optional<IPEndPoint> GetIpv6EndPoint(NetConn_NetAddr& net_addr) {
  IPEndPoint ipe;
  struct sockaddr_in6 sa_ip6;
  sa_ip6.sin6_family = AF_INET6;
  sa_ip6.sin6_port =
      base::HostToNet16(net_addr.port == 0 ? NS_DEFAULTPORT : net_addr.port);
  inet_pton(AF_INET6, net_addr.address, &sa_ip6.sin6_addr);
  if (!ipe.FromSockAddr(reinterpret_cast<const struct sockaddr*>(&sa_ip6),
                        sizeof sa_ip6)) {
    LOG(INFO) << "GetIpv6EndPoint: sin6_addr and sin6_port fetch failed";
    return absl::nullopt;
  }
  return absl::make_optional(ipe);
}

absl::optional<std::vector<IPEndPoint>> GetNameserversFromConnectProperties() {
  NetConn_NetHandle netHandle;
  if (int32_t a = OH_NetConn_GetDefaultNet(&netHandle)) {
    LOG(ERROR) << "get default net fail.a = " << a;
    return absl::nullopt;
  }

  NetConn_ConnectionProperties prop;
  if (OH_NetConn_GetConnectionProperties(&netHandle, &prop)) {
    LOG(ERROR) << "get connnection properties fail.";
    return absl::nullopt;
  }

  std::vector<IPEndPoint> new_nameservers;
  // Obtain the information about the first two DNS that have values in the
  // DNS list.
  for (int i = 0; i < prop.dnsListSize; i++) {
    NetConn_NetAddr& item = prop.dnsList[i];
    absl::optional<IPEndPoint> ipe_inet;

    if (item.family == static_cast<int>(FAMILY_IPV4)) {
      ipe_inet = GetIpv4EndPoint(item);
    } else if (item.family == static_cast<int>(FAMILY_IPV6)) {
      ipe_inet = GetIpv6EndPoint(item);
    }
    if (!ipe_inet.has_value()) {
      return absl::nullopt;
    }
    new_nameservers.push_back(std::move(*ipe_inet));
  }

  return new_nameservers;
}

}  // namespace internal

// static
std::unique_ptr<DnsConfigService> DnsConfigService::CreateSystemService() {
  return std::make_unique<internal::DnsConfigServiceOhos>();
}

}  // namespace net
