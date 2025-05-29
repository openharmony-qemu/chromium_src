// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/bindings/parkable_string_utils.h"
#include "third_party/blink/renderer/platform/bindings/parkable_string_manager.h"
#include "third_party/blink/renderer/platform/bindings/parkable_string.h"

namespace blink {

scoped_refptr<ParkableStringImpl> ParkableStringUtils::GetParkableString(
    scoped_refptr<StringImpl>&& impl, bool is_parkable)
{
   if (!impl) {
     return nullptr;
   }

  return is_parkable
      ? ParkableStringManager::Instance().Add(std::move(impl), nullptr)
      : ParkableStringImpl::MakeNonParkable(std::move(impl));
}

}  // namespace blink