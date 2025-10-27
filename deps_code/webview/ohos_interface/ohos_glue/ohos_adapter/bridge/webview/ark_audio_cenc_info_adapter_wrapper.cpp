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

#include "ohos_adapter/bridge/ark_audio_cenc_info_adapter_wrapper.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioCencInfoAdapterWrapper::ArkAudioCencInfoAdapterWrapper(ArkWebRefPtr<ArkAudioCencInfoAdapter> ref) : ctocpp_(ref)
{}

uint8_t* ArkAudioCencInfoAdapterWrapper::GetKeyId()
{
    return ctocpp_->GetKeyId();
}

uint32_t ArkAudioCencInfoAdapterWrapper::GetKeyIdLen()
{
    return ctocpp_->GetKeyIdLen();
}

uint8_t* ArkAudioCencInfoAdapterWrapper::GetIv()
{
    return ctocpp_->GetIv();
}

uint32_t ArkAudioCencInfoAdapterWrapper::GetIvLen()
{
    return ctocpp_->GetIvLen();
}

uint32_t ArkAudioCencInfoAdapterWrapper::GetAlgo()
{
    return ctocpp_->GetAlgo();
}

uint32_t ArkAudioCencInfoAdapterWrapper::GetEncryptedBlockCount()
{
    return ctocpp_->GetEncryptedBlockCount();
}

uint32_t ArkAudioCencInfoAdapterWrapper::GetSkippedBlockCount()
{
    return ctocpp_->GetSkippedBlockCount();
}

uint32_t ArkAudioCencInfoAdapterWrapper::GetFirstEncryptedOffset()
{
    return ctocpp_->GetFirstEncryptedOffset();
}

std::vector<uint32_t> ArkAudioCencInfoAdapterWrapper::GetClearHeaderLens()
{
    ArkWebUint32Vector lens = ctocpp_->GetClearHeaderLens();

    std::vector<uint32_t> objLens = ArkWebBasicVectorStructToClass<uint32_t, ArkWebUint32Vector>(lens);
    ArkWebBasicVectorStructRelease<ArkWebUint32Vector>(lens);
    return objLens;
}

std::vector<uint32_t> ArkAudioCencInfoAdapterWrapper::GetPayLoadLens()
{
    ArkWebUint32Vector lens = ctocpp_->GetPayLoadLens();

    std::vector<uint32_t> objLens = ArkWebBasicVectorStructToClass<uint32_t, ArkWebUint32Vector>(lens);
    ArkWebBasicVectorStructRelease<ArkWebUint32Vector>(lens);
    return objLens;
}

uint32_t ArkAudioCencInfoAdapterWrapper::GetMode()
{
    return ctocpp_->GetMode();
}

void ArkAudioCencInfoAdapterWrapper::SetKeyId(uint8_t* keyId)
{
    ctocpp_->SetKeyId(keyId);
}

void ArkAudioCencInfoAdapterWrapper::SetKeyIdLen(uint32_t keyIdLen)
{
    ctocpp_->SetKeyIdLen(keyIdLen);
}

void ArkAudioCencInfoAdapterWrapper::SetIv(uint8_t* iv)
{
    ctocpp_->SetIv(iv);
}

void ArkAudioCencInfoAdapterWrapper::SetIvLen(uint32_t ivLen)
{
    ctocpp_->SetIvLen(ivLen);
}

void ArkAudioCencInfoAdapterWrapper::SetAlgo(uint32_t algo)
{
    ctocpp_->SetAlgo(algo);
}

void ArkAudioCencInfoAdapterWrapper::SetEncryptedBlockCount(uint32_t count)
{
    ctocpp_->SetEncryptedBlockCount(count);
}

void ArkAudioCencInfoAdapterWrapper::SetSkippedBlockCount(uint32_t count)
{
    ctocpp_->SetSkippedBlockCount(count);
}

void ArkAudioCencInfoAdapterWrapper::SetFirstEncryptedOffset(uint32_t offset)
{
    ctocpp_->SetFirstEncryptedOffset(offset);
}

void ArkAudioCencInfoAdapterWrapper::SetClearHeaderLens(const std::vector<uint32_t>& clearHeaderLens)
{
    ArkWebUint32Vector lens = ArkWebBasicVectorClassToStruct<uint32_t, ArkWebUint32Vector>(clearHeaderLens);

    ctocpp_->SetClearHeaderLens(lens);

    ArkWebBasicVectorStructRelease<ArkWebUint32Vector>(lens);
}

void ArkAudioCencInfoAdapterWrapper::SetPayLoadLens(const std::vector<uint32_t>& payLoadLens)
{
    ArkWebUint32Vector lens = ArkWebBasicVectorClassToStruct<uint32_t, ArkWebUint32Vector>(payLoadLens);

    ctocpp_->SetPayLoadLens(lens);

    ArkWebBasicVectorStructRelease<ArkWebUint32Vector>(lens);
}

void ArkAudioCencInfoAdapterWrapper::SetMode(uint32_t mode)
{
    ctocpp_->SetMode(mode);
}

} // namespace OHOS::ArkWeb
