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

#ifndef BUFFER_INFO_ADAPTER_IMPL_H
#define BUFFER_INFO_ADAPTER_IMPL_H

#include "media_codec_adapter.h"

namespace OHOS::NWeb {

class BufferInfoAdapterImpl : public BufferInfoAdapter {
public:
    BufferInfoAdapterImpl() = default;

    int64_t GetPresentationTimeUs() override;

    int32_t GetSize() override;

    int32_t GetOffset() override;

    void SetPresentationTimeUs(int64_t time);

    void SetSize(int32_t size);

    void SetOffset(int32_t offset);

private:
    int64_t presentationTimeUs_ = 0;
    int32_t size_ = 0;
    int32_t offset_ = 0;
};

} // namespace OHOS::NWeb

#endif // BUFFER_INFO_ADAPTER_IMPL_H
