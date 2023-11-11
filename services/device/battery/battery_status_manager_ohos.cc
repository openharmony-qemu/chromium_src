// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/device/battery/battery_status_manager.h"

#include <memory>

#include "base/memory/ref_counted.h"
#include "ohos_adapter_helper.h"

using namespace OHOS::NWeb;
namespace device {
namespace {
class BatteryManagerListener : public base::RefCountedThreadSafe<BatteryManagerListener> {
 public:
  BatteryManagerListener(const BatteryManagerListener&) = delete;
  BatteryManagerListener& operator=(const BatteryManagerListener&) = delete;

  explicit BatteryManagerListener(
      const BatteryStatusService::BatteryUpdateCallback& callback) : callback_(callback), isListen(false) {
        batteryClient = OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateBatteryClientAdapter();
        if (batteryClient == nullptr) {
            return;
        }
        batteryClient->RegBatteryEvent([this](WebBatteryInfo& info) {
            this->BatteryChanged(info);
        });
      }

  bool StartListen() {
    LOG(INFO) << "start listen";
    if (isListen) {
        return true;
    }
    if (batteryClient == nullptr) {
        return false;
    }
    if (!batteryClient->StartListen()) {
        LOG(ERROR) << "start listen fail";
        return false;
    }
    LOG(INFO) << "fisrt request battery info";
    std::unique_ptr<WebBatteryInfo> batteryInfo = batteryClient->RequestBatteryInfo();
    if (batteryInfo != nullptr) {
        mojom::BatteryStatus status;
        status.level = batteryInfo->GetLevel();
        status.charging = batteryInfo->IsCharging();
        status.charging_time = std::numeric_limits<double>::infinity();
        if(status.charging) {
            status.discharging_time = std::numeric_limits<double>::infinity();
        } else {
            status.discharging_time = 0.0;
        }
        callback_.Run(status);
    } else {
        callback_.Run(mojom::BatteryStatus());
    }
    isListen = true;
    return true;
  }

  void StopListen() {
    if (!isListen) {
       return;
    }
    LOG(INFO) << "stop Listen";
    if (batteryClient == nullptr) {
        return;
    }
    batteryClient->StopListen();
    isListen = false;
  }

 private:
  void BatteryChanged(WebBatteryInfo& info) {
    mojom::BatteryStatus status;
    status.level = info.GetLevel();
    status.charging = info.IsCharging();
    status.charging_time = std::numeric_limits<double>::infinity();
    if(status.charging) {
        status.discharging_time = std::numeric_limits<double>::infinity();
    } else {
        status.discharging_time = 0.0;
    }
    LOG(INFO) << "recive battery changed" << status.level << status.charging;
    callback_.Run(status);
  }

  BatteryStatusService::BatteryUpdateCallback callback_;
  bool isListen;
  std::unique_ptr<BatteryMgrClientAdapter> batteryClient = nullptr;
};

class BatteryStatusManagerOhos: public BatteryStatusManager {
 public:
  BatteryStatusManagerOhos(const BatteryStatusManagerOhos&) = delete;
  BatteryStatusManagerOhos& operator=(const BatteryStatusManagerOhos&) = delete;

  explicit BatteryStatusManagerOhos(
      const BatteryStatusService::BatteryUpdateCallback& callback)
      : observer_(base::MakeRefCounted<BatteryManagerListener>(callback)) {}

  ~BatteryStatusManagerOhos() override {
    LOG(INFO) << "battery release";
    observer_->StopListen();
  }

 private:
  bool StartListeningBatteryChange() override {
    LOG(INFO) << "start listening battery change";
    return observer_->StartListen();
  }

  void StopListeningBatteryChange() override {
    LOG(INFO) << "stop listen battery change";
    observer_->StopListen();
  }

  scoped_refptr<BatteryManagerListener> observer_;
};

}

std::unique_ptr<BatteryStatusManager> BatteryStatusManager::Create(
    const BatteryStatusService::BatteryUpdateCallback& callback) {
  return std::make_unique<BatteryStatusManagerOhos>(callback);
}

}
