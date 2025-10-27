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

#include "ohos_nweb/bridge/ark_web_screen_capture_access_request_impl.h"

#include "ohos_nweb/bridge/ark_web_screen_capture_config_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebScreenCaptureAccessRequestImpl::ArkWebScreenCaptureAccessRequestImpl(
    std::shared_ptr<OHOS::NWeb::NWebScreenCaptureAccessRequest> nweb_screen_capture_access_request)
    : nweb_screen_capture_access_request_(nweb_screen_capture_access_request)
{}

void ArkWebScreenCaptureAccessRequestImpl::Agree(ArkWebRefPtr<ArkWebScreenCaptureConfig> config)
{
    if (CHECK_REF_PTR_IS_NULL(config)) {
        nweb_screen_capture_access_request_->Agree(nullptr);
        return;
    }

    nweb_screen_capture_access_request_->Agree(std::make_shared<ArkWebScreenCaptureConfigWrapper>(config));
}

void ArkWebScreenCaptureAccessRequestImpl::Refuse()
{
    nweb_screen_capture_access_request_->Refuse();
}

ArkWebString ArkWebScreenCaptureAccessRequestImpl::Origin()
{
    return ArkWebStringClassToStruct(nweb_screen_capture_access_request_->Origin());
}

} // namespace OHOS::ArkWeb
