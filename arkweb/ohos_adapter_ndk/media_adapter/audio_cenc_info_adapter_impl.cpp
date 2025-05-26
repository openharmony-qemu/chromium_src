/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "audio_cenc_info_adapter_impl.h"

namespace OHOS::NWeb {

AudioCencInfoAdapterImpl::~AudioCencInfoAdapterImpl() {}

uint8_t* AudioCencInfoAdapterImpl::GetKeyId()
{
    return keyId_;
}

uint32_t AudioCencInfoAdapterImpl::GetKeyIdLen()
{
    return keyIdLen_;
}

uint8_t* AudioCencInfoAdapterImpl::GetIv()
{
    return iv_;
}

uint32_t AudioCencInfoAdapterImpl::GetIvLen()
{
    return ivLen_;
}

uint32_t AudioCencInfoAdapterImpl::GetAlgo()
{
    return algo_;
}

uint32_t AudioCencInfoAdapterImpl::GetEncryptedBlockCount()
{
    return encryptedBlockCount_;
}

uint32_t AudioCencInfoAdapterImpl::GetSkippedBlockCount()
{
    return skippedBlockCount_;
}

uint32_t AudioCencInfoAdapterImpl::GetFirstEncryptedOffset()
{
    return firstEncryptedOffset_;
}

std::vector<uint32_t> AudioCencInfoAdapterImpl::GetClearHeaderLens()
{
    return clearHeaderLens_;
}

std::vector<uint32_t> AudioCencInfoAdapterImpl::GetPayLoadLens()
{
    return payLoadLens_;
}

uint32_t AudioCencInfoAdapterImpl::GetMode()
{
    return mode_;
}

void AudioCencInfoAdapterImpl::SetKeyId(uint8_t* keyId)
{
    keyId_ = keyId;
}

void AudioCencInfoAdapterImpl::SetKeyIdLen(uint32_t keyIdLen)
{
    keyIdLen_ = keyIdLen;
}

void AudioCencInfoAdapterImpl::SetIv(uint8_t* iv)
{
    iv_ = iv;
}

void AudioCencInfoAdapterImpl::SetIvLen(uint32_t ivLen)
{
    ivLen_ = ivLen;
}

void AudioCencInfoAdapterImpl::SetAlgo(uint32_t algo)
{
    algo_ = algo;
}

void AudioCencInfoAdapterImpl::SetEncryptedBlockCount(uint32_t count)
{
    encryptedBlockCount_ = count;
}

void AudioCencInfoAdapterImpl::SetSkippedBlockCount(uint32_t count)
{
    skippedBlockCount_ = count;
}

void AudioCencInfoAdapterImpl::SetFirstEncryptedOffset(uint32_t offset)
{
    firstEncryptedOffset_ = offset;
}

void AudioCencInfoAdapterImpl::SetClearHeaderLens(const std::vector<uint32_t>& clearHeaderLens)
{
    clearHeaderLens_ = clearHeaderLens;
}

void AudioCencInfoAdapterImpl::SetPayLoadLens(const std::vector<uint32_t>& payLoadLens)
{
    payLoadLens_ = payLoadLens;
}

void AudioCencInfoAdapterImpl::SetMode(uint32_t mode)
{
    mode_ = mode;
}

} // namespace OHOS::NWeb
