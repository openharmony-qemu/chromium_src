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

#ifndef ARK_SURFACE_BUFFER_ADAPTER_IMPL_H
#define ARK_SURFACE_BUFFER_ADAPTER_IMPL_H
#pragma once

#include "graphic_adapter.h"
#include "ohos_adapter/include/ark_graphic_adapter.h"

namespace OHOS::ArkWeb {

class ArkSurfaceBufferAdapterImpl : public ArkSurfaceBufferAdapter {
public:
    explicit ArkSurfaceBufferAdapterImpl(std::shared_ptr<OHOS::NWeb::SurfaceBufferAdapter>);

    int32_t GetFileDescriptor() override;

    int32_t GetWidth() override;

    int32_t GetHeight() override;

    int32_t GetStride() override;

    int32_t GetFormat() override;

    uint32_t GetSize() override;

    void* GetVirAddr() override;

    std::shared_ptr<OHOS::NWeb::SurfaceBufferAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkSurfaceBufferAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_SURFACE_BUFFER_ADAPTER_IMPL_H
