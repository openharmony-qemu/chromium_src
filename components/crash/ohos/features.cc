// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/crash/ohos/features.h"

namespace crashpad::ohos::features {

bool IsCrashpadEnabled() {
#if defined(ENABLE_CRASHPAD)
  return true;
#else
  return false;
#endif
}

}  // namespace crashpad::ohos::features
