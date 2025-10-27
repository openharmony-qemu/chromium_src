// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/fido/ohos/ohos_fido_discovery.h"

#include "base/check.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/task/sequenced_task_runner.h"
#include "device/fido/ohos/ohos_webauthn_api.h"

namespace device {

OhosFidoDiscovery::OhosFidoDiscovery()
    : FidoDiscoveryBase(FidoTransportProtocol::kInternal) {}

OhosFidoDiscovery::~OhosFidoDiscovery() = default;

void OhosFidoDiscovery::Start()
{
  DCHECK(!authenticator_);
  if (!observer()) {
    return;
  }

  // Start() is currently invoked synchronously in the
  // FidoRequestHandler ctor. Invoke AddAuthenticator() asynchronously
  // to avoid hairpinning FidoRequestHandler::AuthenticatorAdded()
  // before the request handler has an observer.
  base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE,
      base::BindOnce(&OhosFidoDiscovery::AddAuthenticator,
                     weak_factory_.GetWeakPtr()));
}

void OhosFidoDiscovery::AddAuthenticator()
{
  bool success = false;
  if (!OhosWebAuthnApi::Instance()->IsAvailable()) {
    observer()->DiscoveryStarted(this, success);
    return;
  }
  success = true;
  authenticator_ = std::make_unique<OhosAuthenticator>();
  observer()->DiscoveryStarted(this, success, {authenticator_.get()});
}

}  // namespace device
