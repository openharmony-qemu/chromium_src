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

#ifndef ARK_NATIVE_BUFFER_CONFIG_ADAPTER_WRAPPER_H
#define ARK_NATIVE_BUFFER_CONFIG_ADAPTER_WRAPPER_H
#pragma once

#include "ohos_native_buffer_adapter.h"
#include "ohos_adapter/include/ark_ohos_native_buffer_adapter.h"

namespace OHOS::ArkWeb {

class ArkNativeBufferConfigAdapterWrapper : public OHOS::NWeb::NativeBufferConfigAdapter {
public:
    explicit ArkNativeBufferConfigAdapterWrapper(ArkWebRefPtr<ArkNativeBufferConfigAdapter>);

    int GetBufferWidth() override;

    int GetBufferHeight() override;

    int GetBufferFormat() override;

    int GetBufferUsage() override;

    int GetBufferStride() override;

    void SetBufferWidth(int width) override;

    void SetBufferHeight(int height) override;

    void SetBufferFormat(int format) override;

    void SetBufferUsage(int usage) override;

    void SetBufferStride(int stride) override;

private:
    ArkWebRefPtr<ArkNativeBufferConfigAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_NATIVE_BUFFER_CONFIG_ADAPTER_WRAPPER_H
