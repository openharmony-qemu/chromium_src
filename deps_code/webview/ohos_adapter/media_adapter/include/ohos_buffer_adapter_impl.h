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

#ifndef OHOS_BUFFER_ADAPTER_IMPL_H
#define OHOS_BUFFER_ADAPTER_IMPL_H

#include "media_codec_adapter.h"

namespace OHOS::NWeb {

class OhosBufferAdapterImpl : public OhosBufferAdapter {
public:
    OhosBufferAdapterImpl() = default;

    uint8_t* GetAddr() override;

    uint32_t GetBufferSize() override;

    void SetAddr(uint8_t* addr);

    void SetBufferSize(uint32_t size);

private:
    uint8_t* addr_;
    uint32_t bufferSize_;
};

} // namespace OHOS::NWeb

#endif // OHOS_BUFFER_ADAPTER_IMPL_H
