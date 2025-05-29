// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_COMMON_OHOS_UTIL_H_
#define UI_OZONE_PLATFORM_OHOS_COMMON_OHOS_UTIL_H_

#include <string>

#include "ui/gfx/native_widget_types.h"

namespace ui {

namespace util {

void* GetWindow(std::string& window_id);

void* GetWindowFromWidget(gfx::AcceleratedWidget widget_id);

int32_t GetNextWindowWidgetId();

std::string ConvertWidgetIdToWindowId(const gfx::AcceleratedWidget widget_id);

gfx::AcceleratedWidget ConvertWindowIdToWidgetId(std::string& window_id);

}  // namespace util
}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_COMMON_OHOS_UTIL_H_
