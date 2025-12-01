// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_CLIENT_NATIVE_PIXMAP_FACTORY_OHOS_H_
#define UI_OZONE_PLATFORM_OHOS_CLIENT_NATIVE_PIXMAP_FACTORY_OHOS_H_

namespace gfx {
class ClientNativePixmapFactory;
}

namespace ui {

// Constructor hook for use in constructor_list.cc
gfx::ClientNativePixmapFactory* CreateClientNativePixmapFactoryOhos();

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_CLIENT_NATIVE_PIXMAP_FACTORY_OHOS_H_
