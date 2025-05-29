/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef NWEB_EXTENSION_ACTION_CEF_DELEGATE_H
#define NWEB_EXTENSION_ACTION_CEF_DELEGATE_H

#include "base/values.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/image/image_skia_rep.h"

#include "ohos_nweb/src/capi/nweb_extension_api_callback.h"

namespace OHOS::NWeb {

NWebExtensionActionIcon CreateFromImageSkiaReps(
    const std::vector<gfx::ImageSkiaRep>& imageSkiaReps);

class NWebExtensionActionCefDelegate {
 public:
  static void RegisterWebExtensionApiListener(
    std::shared_ptr<NWebExtensionActionApiCallback> action_api_listener);
  static void UnRegisterWebExtensionApiListener();
  static void OnSetIcon(std::string extension_id,
                        const gfx::Image& icon_image,
                        int32_t tab_id);
};

}  // namespace OHOS::NWeb

#endif // NWEB_EXTENSION_ACTION_CEF_DELEGATE_H
