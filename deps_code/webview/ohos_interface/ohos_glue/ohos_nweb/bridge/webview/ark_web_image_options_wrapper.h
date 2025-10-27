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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ARK_WEB_IMAGE_OPTIONS_WRAPPER_H_
#define ARK_WEB_IMAGE_OPTIONS_WRAPPER_H_
#pragma once

#include "include/nweb_handler.h"
#include "ohos_nweb/include/ark_web_image_options.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

using ArkWebImageColorType = OHOS::NWeb::ImageColorType;
using ArkWebImageAlphaType = OHOS::NWeb::ImageAlphaType;

class ArkWebImageOptionsWrapper : public OHOS::NWeb::NWebImageOptions {
public:
    ArkWebImageOptionsWrapper(ArkWebRefPtr<ArkWebImageOptions> ark_web_image_options);
    ~ArkWebImageOptionsWrapper() = default;

    size_t GetWidth() override;

    size_t GetHeight() override;

    ArkWebImageAlphaType GetAlphaType() override;

    ArkWebImageColorType GetColorType() override;

private:
    ArkWebRefPtr<ArkWebImageOptions> ark_web_image_options_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_IMAGE_OPTIONS_WRAPPER_H_
