// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/dns/dns_config_service_ohos.h"

#include "base/files/file_path.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/sequence_checker.h"
#include "base/time/time.h"
#include "net/base/address_tracker_linux.h"
#include "net/base/ip_address.h"
#include "net/base/ip_endpoint.h"
#include "net/base/network_change_notifier.h"
#include "net/base/network_interfaces.h"
#include "net/dns/dns_config.h"
#include "net/dns/dns_config_service.h"
#include "net/dns/public/dns_protocol.h"
#include "net/dns/serial_worker.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace net {
namespace internal {

namespace {

constexpr base::FilePath::CharType kFilePathHosts[] =
    FILE_PATH_LITERAL("/etc/hosts");
}  // namespace

// static
constexpr base::TimeDelta DnsConfigServiceOhos::kConfigChangeDelay;

class DnsConfigServiceOhos::Watcher
    : public DnsConfigService::Watcher,
      public NetworkChangeNotifier::NetworkChangeObserver {
 public:
  explicit Watcher(DnsConfigServiceOhos& service)
      : DnsConfigService::Watcher(service) {}
  ~Watcher() override {
    NetworkChangeNotifier::RemoveNetworkChangeObserver(this);
  }

  Watcher(const Watcher&) = delete;
  Watcher& operator=(const Watcher&) = delete;

  // DnsConfigService::Watcher:
  bool Watch() override {
    CheckOnCorrectSequence();

    // On Harmony, assume DNS config may have changed on every network change.
    NetworkChangeNotifier::AddNetworkChangeObserver(this);
    return true;
  }

  // NetworkChangeNotifier::NetworkChangeObserver:
  void OnNetworkChanged(NetworkChangeNotifier::ConnectionType type) override {
    if (type != NetworkChangeNotifier::CONNECTION_NONE) {
      OnConfigChanged(true);
    }
  }
};

class DnsConfigServiceOhos::ConfigReader : public SerialWorker {
 public:
  explicit ConfigReader(DnsConfigServiceOhos& service)
      : service_(&service) {}

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
      for (auto &server : servers) {
        if (server.empty()) {
          continue;
        }

        auto dns_address = IPAddress::FromIPLiteral(server);
        if (!dns_address || (*dns_address).empty()) {
          continue;
        }
        IPEndPoint dns_server(std::move(*dns_address), dns_protocol::kDefaultPort);
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
    : DnsConfigService(kFilePathHosts, kConfigChangeDelay) {
  // Allow constructing on one thread and living on another.
  DETACH_FROM_SEQUENCE(sequence_checker_);
}

DnsConfigServiceOhos::~DnsConfigServiceOhos() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (config_reader_)
    config_reader_->Cancel();
}

void DnsConfigServiceOhos::RefreshConfig() {
  // do nothing
}

void DnsConfigServiceOhos::ReadConfigNow() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (!config_reader_) {
    config_reader_ =
        std::make_unique<ConfigReader>(*this);
  }
  config_reader_->WorkNow();
}

bool DnsConfigServiceOhos::StartWatching() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  // TODO(crbug.com/116139): re-start watcher if that makes sense.
  watcher_ = std::make_unique<Watcher>(*this);
  return watcher_->Watch();
}
}  // namespace internal

// static
std::unique_ptr<DnsConfigService> DnsConfigService::CreateSystemService() {
  return std::make_unique<internal::DnsConfigServiceOhos>();
}

}  // namespace net
