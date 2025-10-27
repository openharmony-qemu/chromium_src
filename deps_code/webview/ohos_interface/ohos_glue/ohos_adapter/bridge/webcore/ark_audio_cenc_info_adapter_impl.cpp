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

#include "ohos_adapter/bridge/ark_audio_cenc_info_adapter_impl.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

ArkAudioCencInfoAdapterImpl::ArkAudioCencInfoAdapterImpl(std::shared_ptr<OHOS::NWeb::AudioCencInfoAdapter> ref)
    : real_(ref)
{}

uint8_t* ArkAudioCencInfoAdapterImpl::GetKeyId()
{
    return real_->GetKeyId();
}

uint32_t ArkAudioCencInfoAdapterImpl::GetKeyIdLen()
{
    return real_->GetKeyIdLen();
}

uint8_t* ArkAudioCencInfoAdapterImpl::GetIv()
{
    return real_->GetIv();
}

uint32_t ArkAudioCencInfoAdapterImpl::GetIvLen()
{
    return real_->GetIvLen();
}

uint32_t ArkAudioCencInfoAdapterImpl::GetAlgo()
{
    return real_->GetAlgo();
}

uint32_t ArkAudioCencInfoAdapterImpl::GetEncryptedBlockCount()
{
    return real_->GetEncryptedBlockCount();
}

uint32_t ArkAudioCencInfoAdapterImpl::GetSkippedBlockCount()
{
    return real_->GetSkippedBlockCount();
}

uint32_t ArkAudioCencInfoAdapterImpl::GetFirstEncryptedOffset()
{
    return real_->GetFirstEncryptedOffset();
}

ArkWebUint32Vector ArkAudioCencInfoAdapterImpl::GetClearHeaderLens()
{
    return ArkWebBasicVectorClassToStruct<uint32_t, ArkWebUint32Vector>(real_->GetClearHeaderLens());
}

ArkWebUint32Vector ArkAudioCencInfoAdapterImpl::GetPayLoadLens()
{
    return ArkWebBasicVectorClassToStruct<uint32_t, ArkWebUint32Vector>(real_->GetPayLoadLens());
}

uint32_t ArkAudioCencInfoAdapterImpl::GetMode()
{
    return real_->GetMode();
}

void ArkAudioCencInfoAdapterImpl::SetKeyId(uint8_t* keyId)
{
    real_->SetKeyId(keyId);
}

void ArkAudioCencInfoAdapterImpl::SetKeyIdLen(uint32_t keyIdLen)
{
    real_->SetKeyIdLen(keyIdLen);
}

void ArkAudioCencInfoAdapterImpl::SetIv(uint8_t* iv)
{
    real_->SetIv(iv);
}

void ArkAudioCencInfoAdapterImpl::SetIvLen(uint32_t ivLen)
{
    real_->SetIvLen(ivLen);
}

void ArkAudioCencInfoAdapterImpl::SetAlgo(uint32_t algo)
{
    real_->SetAlgo(algo);
}

void ArkAudioCencInfoAdapterImpl::SetEncryptedBlockCount(uint32_t count)
{
    real_->SetEncryptedBlockCount(count);
}

void ArkAudioCencInfoAdapterImpl::SetSkippedBlockCount(uint32_t count)
{
    real_->SetSkippedBlockCount(count);
}

void ArkAudioCencInfoAdapterImpl::SetFirstEncryptedOffset(uint32_t offset)
{
    real_->SetFirstEncryptedOffset(offset);
}

void ArkAudioCencInfoAdapterImpl::SetClearHeaderLens(const ArkWebUint32Vector& clearHeaderLens)
{
    real_->SetClearHeaderLens(ArkWebBasicVectorStructToClass<uint32_t, ArkWebUint32Vector>(clearHeaderLens));
}

void ArkAudioCencInfoAdapterImpl::SetPayLoadLens(const ArkWebUint32Vector& payLoadLens)
{
    real_->SetPayLoadLens(ArkWebBasicVectorStructToClass<uint32_t, ArkWebUint32Vector>(payLoadLens));
}

void ArkAudioCencInfoAdapterImpl::SetMode(uint32_t mode)
{
    real_->SetMode(mode);
}

} // namespace OHOS::NWeb
