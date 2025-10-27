/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NWEB_ICON_UTILS_H_
#define NWEB_ICON_UTILS_H_

#include "ohos_nweb/src/capi/nweb_extension_action_icon.h"
#include "ui/gfx/image/image.h"

namespace OHOS::NWeb {

NWebExtensionActionIcon* CreateActionIcon(const gfx::Image& icon_image);

void ReleaseActionIcon(NWebExtensionActionIcon* icon);

}  // namespace OHOS::NWeb

#endif  // NWEB_ICON_UTILS_H_
