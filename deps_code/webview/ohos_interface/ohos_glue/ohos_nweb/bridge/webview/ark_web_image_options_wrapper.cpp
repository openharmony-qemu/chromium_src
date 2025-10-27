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

#include "ohos_nweb/bridge/ark_web_image_options_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebImageOptionsWrapper::ArkWebImageOptionsWrapper(ArkWebRefPtr<ArkWebImageOptions> ark_web_image_options)
    : ark_web_image_options_(ark_web_image_options)
{}

size_t ArkWebImageOptionsWrapper::GetWidth()
{
    return ark_web_image_options_->GetWidth();
}

size_t ArkWebImageOptionsWrapper::GetHeight()
{
    return ark_web_image_options_->GetHeight();
}

ArkWebImageAlphaType ArkWebImageOptionsWrapper::GetAlphaType()
{
    return static_cast<ArkWebImageAlphaType>(ark_web_image_options_->GetAlphaType());
}

ArkWebImageColorType ArkWebImageOptionsWrapper::GetColorType()
{
    return static_cast<ArkWebImageColorType>(ark_web_image_options_->GetColorType());
}

} // namespace OHOS::ArkWeb
