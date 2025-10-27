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

#ifndef ARK_BUFFER_DESC_ADAPTER_WRAPPER_H
#define ARK_BUFFER_DESC_ADAPTER_WRAPPER_H
#pragma once

#include "audio_capturer_adapter.h"
#include "ohos_adapter/include/ark_audio_capturer_adapter.h"

namespace OHOS::ArkWeb {

class ArkBufferDescAdapterWrapper : public NWeb::BufferDescAdapter {
public:
    explicit ArkBufferDescAdapterWrapper(ArkWebRefPtr<ArkBufferDescAdapter>);

    uint8_t* GetBuffer() override;

    size_t GetBufLength() override;

    size_t GetDataLength() override;

    void SetBuffer(uint8_t* buffer) override;

    void SetBufLength(size_t bufLength) override;

    void SetDataLength(size_t dataLength) override;

private:
    ArkWebRefPtr<ArkBufferDescAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_BUFFER_DESC_ADAPTER_WRAPPER_H
