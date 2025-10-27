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

#include "ohos_adapter/bridge/ark_producer_surface_adapter_impl.h"

#include "ohos_adapter/bridge/ark_buffer_flush_config_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_buffer_request_config_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_surface_buffer_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkProducerSurfaceAdapterImpl::ArkProducerSurfaceAdapterImpl(std::shared_ptr<OHOS::NWeb::ProducerSurfaceAdapter> ref)
    : real_(ref)
{}

ArkWebRefPtr<ArkSurfaceBufferAdapter> ArkProducerSurfaceAdapterImpl::RequestBuffer(
    int32_t& fence, ArkWebRefPtr<ArkBufferRequestConfigAdapter> config)
{
    std::shared_ptr<OHOS::NWeb::SurfaceBufferAdapter> buffer = nullptr;
    if (CHECK_REF_PTR_IS_NULL(config)) {
        buffer = real_->RequestBuffer(fence, nullptr);
    } else {
        buffer = real_->RequestBuffer(fence, std::make_shared<ArkBufferRequestConfigAdapterWrapper>(config));
    }

    if (CHECK_SHARED_PTR_IS_NULL(buffer)) {
        return nullptr;
    }

    return new ArkSurfaceBufferAdapterImpl(buffer);
}

int32_t ArkProducerSurfaceAdapterImpl::FlushBuffer(
    ArkWebRefPtr<ArkSurfaceBufferAdapter> buffer, int32_t fence, ArkWebRefPtr<ArkBufferFlushConfigAdapter> flushConfig)
{
    ArkSurfaceBufferAdapterImpl* imp = static_cast<ArkSurfaceBufferAdapterImpl*>(buffer.get());
    if (CHECK_REF_PTR_IS_NULL(flushConfig)) {
        return real_->FlushBuffer(std::move(imp->real_), fence, nullptr);
    }

    return real_->FlushBuffer(
        std::move(imp->real_), fence, std::make_shared<ArkBufferFlushConfigAdapterWrapper>(flushConfig));
}

} // namespace OHOS::ArkWeb
