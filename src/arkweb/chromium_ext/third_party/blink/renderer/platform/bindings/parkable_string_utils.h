// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_PARKABLE_STRING_UTILS_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_PARKABLE_STRING_UTILS_H_


#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class ParkableStringImpl;

class ParkableStringUtils {
 public:
#if BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)
  static scoped_refptr<ParkableStringImpl> GetParkableString(scoped_refptr<StringImpl>&& impl,
                                         bool is_parkable);
#endif  // BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_PARKABLE_STRING_UTILS_H_