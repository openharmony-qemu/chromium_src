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

#ifndef AUDIO_CENC_INFO_ADAPTER_H
#define AUDIO_CENC_INFO_ADAPTER_H

#include <vector>
#include <cstdint>

namespace OHOS::NWeb {

// 和native_cencinfo.h中DrmCencAlgorithm保持一致
enum class DrmCencAlgorithmAdapter {
    /**
    * Unencrypted.
    */
    DRM_ALG_CENC_UNENCRYPTED = 0x0,
    /**
    * Aes ctr.
    */
    DRM_ALG_CENC_AES_CTR = 0x1,
    /**
    * Aes wv.
    */
    DRM_ALG_CENC_AES_WV = 0x2,
    /**
    * Aes cbc.
    */
    DRM_ALG_CENC_AES_CBC = 0x3,
    /**
    * Sm4 cbc.
    */
    DRM_ALG_CENC_SM4_CBC = 0x4,
    /**
    * Sm4 ctr.
    */
    DRM_ALG_CENC_SM4_CTR = 0x5
};

// 和native_cencinfo.h中DrmCencInfoMode保持一致
enum class DrmCencInfoModeAdapter {
    /* key/iv/subsample set. */
    DRM_CENC_INFO_KEY_IV_SUBSAMPLES_SET = 0x0,
    /* key/iv/subsample not set. */
    DRM_CENC_INFO_KEY_IV_SUBSAMPLES_NOT_SET = 0x1
};

class AudioCencInfoAdapter {
public:
    AudioCencInfoAdapter() = default;

    virtual ~AudioCencInfoAdapter() = default;

    virtual uint8_t* GetKeyId() = 0;

    virtual uint32_t GetKeyIdLen() = 0;

    virtual uint8_t* GetIv() = 0;

    virtual uint32_t GetIvLen() = 0;

    virtual uint32_t GetEncryptedBlockCount() = 0;

    virtual uint32_t GetAlgo() = 0;

    virtual uint32_t GetSkippedBlockCount() = 0;

    virtual uint32_t GetFirstEncryptedOffset() = 0;

    virtual std::vector<uint32_t> GetClearHeaderLens() = 0;

    virtual std::vector<uint32_t> GetPayLoadLens() = 0;

    virtual uint32_t GetMode() = 0;

    virtual void SetKeyId(uint8_t* keyId) = 0;

    virtual void SetKeyIdLen(uint32_t keyIdLen) = 0;

    virtual void SetIv(uint8_t* iv) = 0;

    virtual void SetIvLen(uint32_t ivLen) = 0;

    virtual void SetAlgo(uint32_t algo) = 0;

    virtual void SetEncryptedBlockCount(uint32_t encryptedBlockCount) = 0;

    virtual void SetSkippedBlockCount(uint32_t skippedBlockCount) = 0;

    virtual void SetFirstEncryptedOffset(uint32_t firstEncryptedOffset) = 0;

    virtual void SetClearHeaderLens(const std::vector<uint32_t>& clearHeaderLens) = 0;

    virtual void SetPayLoadLens(const std::vector<uint32_t>& payLoadLens) = 0;

    virtual void SetMode(uint32_t mode) = 0;
};

} // namespace OHOS::NWeb

#endif // AUDIO_CODEC_DECODER_ADAPTER_H