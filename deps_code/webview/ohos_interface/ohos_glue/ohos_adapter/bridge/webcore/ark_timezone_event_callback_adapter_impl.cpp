/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "ohos_adapter/bridge/ark_timezone_event_callback_adapter_impl.h"

#include "ohos_adapter/bridge/ark_web_timezone_info_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkTimezoneEventCallbackAdapterImpl::ArkTimezoneEventCallbackAdapterImpl(
    std::shared_ptr<OHOS::NWeb::TimezoneEventCallbackAdapter> ref)
    : real_(ref)
{}

void ArkTimezoneEventCallbackAdapterImpl::TimezoneChanged(ArkWebRefPtr<ArkWebTimezoneInfo> info)
{
    if (CHECK_REF_PTR_IS_NULL(info)) {
        return real_->TimezoneChanged(nullptr);
    }

    real_->TimezoneChanged(std::make_shared<ArkWebTimezoneInfoWrapper>(info));
}

} // namespace OHOS::ArkWeb
