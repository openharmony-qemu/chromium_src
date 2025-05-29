/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "chrome/browser/ui/views/chrome_browser_main_extra_parts_views_ohos.h"

#include "base/logging.h"
#include "ui/native_theme/native_theme.h"

ChromeBrowserMainExtraPartsViewsOHOS::ChromeBrowserMainExtraPartsViewsOHOS() =
    default;

ChromeBrowserMainExtraPartsViewsOHOS::~ChromeBrowserMainExtraPartsViewsOHOS() =
    default;

void ChromeBrowserMainExtraPartsViewsOHOS::ToolkitInitialized() {
  ChromeBrowserMainExtraPartsViews::ToolkitInitialized();
}
