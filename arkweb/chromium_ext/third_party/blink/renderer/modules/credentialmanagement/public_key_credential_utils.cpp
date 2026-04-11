// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/modules/credentialmanagement/public_key_credential_utils.h"

#include "third_party/blink/renderer/platform/runtime_enabled_features.h"

namespace blink {

void OnGetClientCapabilitiesFullComplete(
    ScriptPromiseResolver<IDLRecord<IDLString, IDLBoolean>>* resolver,
    const Vector<mojom::blink::WebAuthnClientCapabilityPtr> capabilities)
{
    Vector<std::pair<String, bool>> results;
    for (const auto& capability : capabilities) {
        results.emplace_back(std::move(capability->name), capability->supported);
    }

    // Results should be sorted lexicographically based on the keys.
    std::sort(
        results.begin(), results.end(),
        [](const std::pair<String, bool>& a, const std::pair<String, bool>& b) {
          return CodeUnitCompare(a.first, b.first) < 0;
        });
    resolver->Resolve(std::move(results));
}

} // namspace blink
