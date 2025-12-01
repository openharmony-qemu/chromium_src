// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_OZONE_PLATFORM_OHOS_H_
#define UI_OZONE_PLATFORM_OHOS_OZONE_PLATFORM_OHOS_H_

namespace ui {

class OzonePlatform;

// Constructor hook for use in ozone_platform_list.cc
OzonePlatform* CreateOzonePlatformOhos();

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_OZONE_PLATFORM_OHOS_H_
