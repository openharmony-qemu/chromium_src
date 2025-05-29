// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_BLUETOOTH_BLUETOOTH_DISCOVERY_MANAGER_OHOS_H_
#define DEVICE_BLUETOOTH_BLUETOOTH_DISCOVERY_MANAGER_OHOS_H_

#include "base/memory/raw_ptr.h"

namespace device {

class BluetoothDiscoveryManagerOhos {
 public:
  class Observer {
   public:
    virtual void ClassicDeviceFound(const std::string& address) = 0;
    virtual void ClassicDiscoveryStopped(bool unexpected) = 0;

   protected:
    virtual ~Observer() {}
  };

  BluetoothDiscoveryManagerOhos(const BluetoothDiscoveryManagerOhos&) = delete;
  BluetoothDiscoveryManagerOhos& operator=(
      const BluetoothDiscoveryManagerOhos&) = delete;

  virtual ~BluetoothDiscoveryManagerOhos();

  // Returns true, if discovery is currently being performed.
  virtual bool IsDiscovering() const = 0;

  // Initiates a discovery session. Returns true on success or if discovery
  // is already running. Returns false on failure.
  virtual bool StartDiscovery() = 0;

  // Stops a discovery session. Returns true on success or if discovery is
  // already not running. Returns false on failure.
  virtual bool StopDiscovery() = 0;

  // Creates a discovery manager for Bluetooth Classic device discovery with
  // observer |observer|. Note that the life-time of |observer| should not
  // end before that of the returned BluetoothDiscoveryManager, as that may
  // lead to use after free errors.
  static BluetoothDiscoveryManagerOhos* CreateClassic(Observer* observer);

 protected:
  explicit BluetoothDiscoveryManagerOhos(Observer* observer);

  // Observer interested in notifications from us.
  raw_ptr<Observer> observer_;
};

}  // namespace device

#endif  // DEVICE_BLUETOOTH_BLUETOOTH_DISCOVERY_MANAGER_OHOS_H_
