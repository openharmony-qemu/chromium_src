/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
