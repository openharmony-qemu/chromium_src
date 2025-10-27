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

#include "ohos_adapter/bridge/ark_iconsumer_surface_adapter_impl.h"

#include "ohos_adapter/bridge/ark_ibuffer_consumer_listener_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_surface_buffer_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkIConsumerSurfaceAdapterImpl::ArkIConsumerSurfaceAdapterImpl(std::shared_ptr<OHOS::NWeb::IConsumerSurfaceAdapter> ref)
    : real_(ref)
{}

int32_t ArkIConsumerSurfaceAdapterImpl::RegisterConsumerListener(
    ArkWebRefPtr<ArkIBufferConsumerListenerAdapter> listener)
{
    if (CHECK_REF_PTR_IS_NULL(listener)) {
        return real_->RegisterConsumerListener(nullptr);
    }

    return real_->RegisterConsumerListener(std::make_shared<ArkIBufferConsumerListenerAdapterWrapper>(listener));
}

int32_t ArkIConsumerSurfaceAdapterImpl::ReleaseBuffer(ArkWebRefPtr<ArkSurfaceBufferAdapter> buffer, int32_t fence)
{
    ArkSurfaceBufferAdapterImpl* imp = static_cast<ArkSurfaceBufferAdapterImpl*>(buffer.get());
    return real_->ReleaseBuffer(std::move(imp->real_), fence);
}

int32_t ArkIConsumerSurfaceAdapterImpl::SetUserData(const ArkWebString& key, const ArkWebString& val)
{
    std::string key_str = ArkWebStringStructToClass(key);
    std::string val_str = ArkWebStringStructToClass(val);
    return real_->SetUserData(key_str, val_str);
}

int32_t ArkIConsumerSurfaceAdapterImpl::SetQueueSize(uint32_t queueSize)
{
    return real_->SetQueueSize(queueSize);
}

} // namespace OHOS::ArkWeb
