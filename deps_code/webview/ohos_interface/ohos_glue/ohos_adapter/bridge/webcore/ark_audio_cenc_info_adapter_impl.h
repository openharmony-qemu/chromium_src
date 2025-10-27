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

#ifndef ARK_AUDIO_CENC_INFO_ADAPTER_IMPL_H
#define ARK_AUDIO_CENC_INFO_ADAPTER_IMPL_H
#pragma once

#include "audio_cenc_info_adapter.h"
#include "ohos_adapter/include/ark_audio_codec_decoder_adapter.h"

namespace OHOS::ArkWeb {

class ArkAudioCencInfoAdapterImpl : public ArkAudioCencInfoAdapter {
public:
    explicit ArkAudioCencInfoAdapterImpl(std::shared_ptr<OHOS::NWeb::AudioCencInfoAdapter> ref);

    uint8_t* GetKeyId() override;

    uint32_t GetKeyIdLen() override;

    uint8_t* GetIv() override;

    uint32_t GetIvLen() override;

    uint32_t GetAlgo() override;

    uint32_t GetEncryptedBlockCount() override;

    uint32_t GetSkippedBlockCount() override;

    uint32_t GetFirstEncryptedOffset() override;

    ArkWebUint32Vector GetClearHeaderLens() override;

    ArkWebUint32Vector GetPayLoadLens() override;

    uint32_t GetMode() override;

    void SetKeyId(uint8_t* keyId) override;

    void SetKeyIdLen(uint32_t keyIdLen) override;

    void SetIv(uint8_t* iv) override;

    void SetIvLen(uint32_t ivLen) override;

    void SetAlgo(uint32_t algo) override;

    void SetEncryptedBlockCount(uint32_t blockCount) override;

    void SetSkippedBlockCount(uint32_t blockCount) override;

    void SetFirstEncryptedOffset(uint32_t offset) override;

    void SetClearHeaderLens(const ArkWebUint32Vector& lens) override;

    void SetPayLoadLens(const ArkWebUint32Vector& lens) override;

    void SetMode(uint32_t mode) override;

private:
    std::shared_ptr<OHOS::NWeb::AudioCencInfoAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkAudioCencInfoAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_CENC_INFO_ADAPTER_IMPL_H
