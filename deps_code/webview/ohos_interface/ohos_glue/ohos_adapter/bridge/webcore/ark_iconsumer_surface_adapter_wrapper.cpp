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

#include "ohos_adapter/bridge/ark_iconsumer_surface_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_ibuffer_consumer_listener_adapter_impl.h"
#include "ohos_adapter/bridge/ark_surface_buffer_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkIConsumerSurfaceAdapterWrapper::ArkIConsumerSurfaceAdapterWrapper(ArkWebRefPtr<ArkIConsumerSurfaceAdapter> ref)
    : ctocpp_(ref)
{}

int32_t ArkIConsumerSurfaceAdapterWrapper::RegisterConsumerListener(
    std::shared_ptr<OHOS::NWeb::IBufferConsumerListenerAdapter> listener)
{
    if (CHECK_SHARED_PTR_IS_NULL(listener)) {
        return ctocpp_->RegisterConsumerListener(nullptr);
    }

    return ctocpp_->RegisterConsumerListener(new ArkIBufferConsumerListenerAdapterImpl(listener));
}

int32_t ArkIConsumerSurfaceAdapterWrapper::ReleaseBuffer(
    std::shared_ptr<OHOS::NWeb::SurfaceBufferAdapter> buffer, int32_t fence)
{
    std::shared_ptr<ArkSurfaceBufferAdapterWrapper> bufferWrapper =
        std::static_pointer_cast<ArkSurfaceBufferAdapterWrapper>(buffer);
    return ctocpp_->ReleaseBuffer(bufferWrapper->ctocpp_, fence);
}

int32_t ArkIConsumerSurfaceAdapterWrapper::SetUserData(const std::string& key, const std::string& val)
{
    ArkWebString ark_key = ArkWebStringClassToStruct(key);
    ArkWebString ark_val = ArkWebStringClassToStruct(val);
    int32_t result = ctocpp_->SetUserData(ark_key, ark_val);
    ArkWebStringStructRelease(ark_key);
    ArkWebStringStructRelease(ark_val);
    return result;
}

int32_t ArkIConsumerSurfaceAdapterWrapper::SetQueueSize(uint32_t queueSize)
{
    return ctocpp_->SetQueueSize(queueSize);
}

} // namespace OHOS::ArkWeb
