// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_CREDENTIALMANAGEMENT_PUBLIC_KEY_CREDENTIAL_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_CREDENTIALMANAGEMENT_PUBLIC_KEY_CREDENTIAL_UTILS_H_

#include "third_party/blink/public/mojom/webauthn/authenticator.mojom-blink.h"
#include "third_party/blink/renderer/bindings/core/v8/idl_types.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise_resolver.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

void OnGetClientCapabilitiesFullComplete(
    ScriptPromiseResolver<IDLRecord<IDLString, IDLBoolean>>* resolver,
    const Vector<mojom::blink::WebAuthnClientCapabilityPtr> capabilities);

} // namspace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_CREDENTIALMANAGEMENT_PUBLIC_KEY_CREDENTIAL_UTILS_H_
