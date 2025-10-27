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

#include "ohos_adapter/bridge/ark_producer_surface_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_buffer_flush_config_adapter_impl.h"
#include "ohos_adapter/bridge/ark_buffer_request_config_adapter_impl.h"
#include "ohos_adapter/bridge/ark_ibuffer_consumer_listener_adapter_impl.h"
#include "ohos_adapter/bridge/ark_surface_buffer_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkProducerSurfaceAdapterWrapper::ArkProducerSurfaceAdapterWrapper(ArkWebRefPtr<ArkProducerSurfaceAdapter> ref)
    : ctocpp_(ref)
{}

std::shared_ptr<OHOS::NWeb::SurfaceBufferAdapter> ArkProducerSurfaceAdapterWrapper::RequestBuffer(
    int32_t& fence, std::shared_ptr<OHOS::NWeb::BufferRequestConfigAdapter> config)
{
    ArkWebRefPtr<ArkSurfaceBufferAdapter> surface;
    if (CHECK_SHARED_PTR_IS_NULL(config)) {
        surface = ctocpp_->RequestBuffer(fence, nullptr);
    } else {
        surface = ctocpp_->RequestBuffer(fence, new ArkBufferRequestConfigAdapterImpl(config));
    }

    if (CHECK_REF_PTR_IS_NULL(surface)) {
        return nullptr;
    }

    return std::make_shared<ArkSurfaceBufferAdapterWrapper>(surface);
}

int32_t ArkProducerSurfaceAdapterWrapper::FlushBuffer(std::shared_ptr<OHOS::NWeb::SurfaceBufferAdapter> buffer,
    int32_t fence, std::shared_ptr<OHOS::NWeb::BufferFlushConfigAdapter> config)
{
    std::shared_ptr<ArkSurfaceBufferAdapterWrapper> bufferWrapper =
        std::static_pointer_cast<ArkSurfaceBufferAdapterWrapper>(buffer);
    if (CHECK_SHARED_PTR_IS_NULL(config)) {
        return ctocpp_->FlushBuffer(bufferWrapper->ctocpp_, fence, nullptr);
    }

    return ctocpp_->FlushBuffer(bufferWrapper->ctocpp_, fence, new ArkBufferFlushConfigAdapterImpl(config));
}
} // namespace OHOS::ArkWeb
