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

#include "ohos_nweb/bridge/ark_web_image_options_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebImageOptionsImpl::ArkWebImageOptionsImpl(std::shared_ptr<OHOS::NWeb::NWebImageOptions> nweb_image_options)
    : nweb_image_options_(nweb_image_options)
{}

size_t ArkWebImageOptionsImpl::GetWidth()
{
    return nweb_image_options_->GetWidth();
}

size_t ArkWebImageOptionsImpl::GetHeight()
{
    return nweb_image_options_->GetHeight();
}

int ArkWebImageOptionsImpl::GetAlphaType()
{
    return static_cast<int>(nweb_image_options_->GetAlphaType());
}

int ArkWebImageOptionsImpl::GetColorType()
{
    return static_cast<int>(nweb_image_options_->GetColorType());
}

} // namespace OHOS::ArkWeb
