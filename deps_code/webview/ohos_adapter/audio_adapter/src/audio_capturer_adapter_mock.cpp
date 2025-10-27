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

#include "audio_capturer_adapter_impl.h"

namespace OHOS::NWeb {

int32_t AudioCapturerAdapterImpl::Create(
    const std::shared_ptr<AudioCapturerOptionsAdapter> capturerOptions, std::string cachePath)
{
    return -1;
}

bool AudioCapturerAdapterImpl::Start()
{
    return false;
}

bool AudioCapturerAdapterImpl::Stop()
{
    return false;
}

bool AudioCapturerAdapterImpl::Release()
{
    return false;
}

int32_t AudioCapturerAdapterImpl::SetCapturerReadCallback(std::shared_ptr<AudioCapturerReadCallbackAdapter> callback)
{
    return -1;
}

int32_t AudioCapturerAdapterImpl::GetBufferDesc(std::shared_ptr<BufferDescAdapter> bufferDesc)
{
    return -1;
}

int32_t AudioCapturerAdapterImpl::Enqueue(const std::shared_ptr<BufferDescAdapter> bufferDesc)
{
    return -1;
}

int32_t AudioCapturerAdapterImpl::GetFrameCount(uint32_t& frameCount)
{
    return -1;
}

int64_t AudioCapturerAdapterImpl::GetAudioTime()
{
    return -1;
}
} // namespace OHOS::NWeb
