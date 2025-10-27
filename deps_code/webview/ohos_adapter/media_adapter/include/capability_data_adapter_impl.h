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

#ifndef CAPABILITY_DATA_ADAPTER_IMPL_H
#define CAPABILITY_DATA_ADAPTER_IMPL_H

#include "media_codec_adapter.h"

namespace OHOS::NWeb {

class CapabilityDataAdapterImpl : public CapabilityDataAdapter {
public:
    CapabilityDataAdapterImpl() = default;

    int32_t GetMaxWidth() override;

    int32_t GetMaxHeight() override;

    int32_t GetMaxframeRate() override;

    void SetMaxWidth(int32_t width);

    void SetMaxHeight(int32_t height);

    void SetMaxframeRate(int32_t frameRate);

private:
    int32_t maxWidth_ = 0;
    int32_t maxHeight_ = 0;
    int32_t maxFrameRate_ = 0;
};

} // namespace OHOS::NWeb

#endif // CAPABILITY_DATA_ADAPTER_IMPL_H
