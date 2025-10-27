/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_display_listener_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkDisplayListenerAdapterImpl::ArkDisplayListenerAdapterImpl(std::shared_ptr<OHOS::NWeb::DisplayListenerAdapter> ref)
    : real_(ref)
{}

void ArkDisplayListenerAdapterImpl::OnCreate(uint64_t displayId)
{
    real_->OnCreate(displayId);
}

void ArkDisplayListenerAdapterImpl::OnDestroy(uint64_t displayId)
{
    real_->OnDestroy(displayId);
}

void ArkDisplayListenerAdapterImpl::OnChange(uint64_t displayId)
{
    real_->OnChange(displayId);
}

} // namespace OHOS::ArkWeb
