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

#include "capability_data_adapter_impl.h"

namespace OHOS::NWeb {

int32_t CapabilityDataAdapterImpl::GetMaxWidth()
{
    return maxWidth_;
}

int32_t CapabilityDataAdapterImpl::GetMaxHeight()
{
    return maxHeight_;
}

int32_t CapabilityDataAdapterImpl::GetMaxframeRate()
{
    return maxFrameRate_;
}

void CapabilityDataAdapterImpl::SetMaxWidth(int32_t width)
{
    maxWidth_ = width;
}

void CapabilityDataAdapterImpl::SetMaxHeight(int32_t height)
{
    maxHeight_ = height;
}

void CapabilityDataAdapterImpl::SetMaxframeRate(int32_t frameRate)
{
    maxFrameRate_ = frameRate;
}

} // namespace OHOS::NWeb
