// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_FIDO_OHOS_OHOS_FIDO_DISCOVER_H_
#define DEVICE_FIDO_OHOS_OHOS_FIDO_DISCOVER_H_

#include <memory>

#include "base/component_export.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "device/fido/fido_discovery_base.h"
#include "device/fido/ohos/ohos_authenticator.h"

namespace device {

class COMPONENT_EXPORT(DEVICE_FIDO) OhosFidoDiscovery
    : public FidoDiscoveryBase {
 public:
  OhosFidoDiscovery();
  ~OhosFidoDiscovery() override;

  // FidoDiscoveryBase:
  void Start() override;

 private:
  void AddAuthenticator();

  std::unique_ptr<OhosAuthenticator> authenticator_;

  base::WeakPtrFactory<OhosFidoDiscovery> weak_factory_{this};
};

} // namespace device

#endif  // DEVICE_FIDO_OHOS_OHOS_FIDO_DISCOVER_H_
