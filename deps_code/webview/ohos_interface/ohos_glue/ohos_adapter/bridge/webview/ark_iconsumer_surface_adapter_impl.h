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

#ifndef ARK_ICONSUMER_SURFACE_ADAPTER_IMPL_H
#define ARK_ICONSUMER_SURFACE_ADAPTER_IMPL_H
#pragma once

#include "graphic_adapter.h"
#include "ohos_adapter/include/ark_graphic_adapter.h"

namespace OHOS::ArkWeb {

class ArkIConsumerSurfaceAdapterImpl : public ArkIConsumerSurfaceAdapter {
public:
    explicit ArkIConsumerSurfaceAdapterImpl(std::shared_ptr<OHOS::NWeb::IConsumerSurfaceAdapter>);

    int32_t RegisterConsumerListener(ArkWebRefPtr<ArkIBufferConsumerListenerAdapter> listener) override;

    int32_t ReleaseBuffer(ArkWebRefPtr<ArkSurfaceBufferAdapter> buffer, int32_t fence) override;

    int32_t SetUserData(const ArkWebString& key, const ArkWebString& val) override;

    int32_t SetQueueSize(uint32_t queueSize) override;

    std::shared_ptr<OHOS::NWeb::IConsumerSurfaceAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkIConsumerSurfaceAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_ICONSUMER_SURFACE_ADAPTER_IMPL_H
