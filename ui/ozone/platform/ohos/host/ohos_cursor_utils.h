// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_CURSOR_UTILS_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_CURSOR_UTILS_H_

#include "ohos/adapter/cursor/cursor.h"
#include "ui/base/cursor/mojom/cursor_type.mojom-shared.h"

namespace ui {
ohos::adapter::OhosCursorType ConvertToOhosCursorType(
    const mojom::CursorType type);
}

#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_CURSOR_UTILS_H_
