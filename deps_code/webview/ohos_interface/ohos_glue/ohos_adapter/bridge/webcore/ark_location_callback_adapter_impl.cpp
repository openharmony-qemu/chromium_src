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

#include "ohos_adapter/bridge/ark_location_callback_adapter_impl.h"

#include "ohos_adapter/bridge/ark_location_info_wrapper.h"

namespace OHOS::ArkWeb {

ArkLocationCallbackAdapterImpl::ArkLocationCallbackAdapterImpl(std::shared_ptr<OHOS::NWeb::LocationCallbackAdapter> ref)
    : real_(ref)
{}

void ArkLocationCallbackAdapterImpl::OnLocationReport(const ArkWebRefPtr<ArkLocationInfo> location)
{
    real_->OnLocationReport(std::make_shared<ArkLocationInfoWrapper>(location));
}

void ArkLocationCallbackAdapterImpl::OnLocatingStatusChange(const int status)
{
    real_->OnLocatingStatusChange(status);
}

void ArkLocationCallbackAdapterImpl::OnErrorReport(const int errorCode)
{
    real_->OnErrorReport(errorCode);
}

} // namespace OHOS::ArkWeb
