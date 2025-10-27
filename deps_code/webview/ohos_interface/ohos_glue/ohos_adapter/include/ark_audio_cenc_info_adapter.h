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

#ifndef ARK_AUDIO_CENC_INFO_ADAPTER_H
#define ARK_AUDIO_CENC_INFO_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkAudioCencInfoAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual uint8_t* GetKeyId() = 0;

    /*--ark web()--*/
    virtual uint32_t GetKeyIdLen() = 0;

    /*--ark web()--*/
    virtual uint8_t* GetIv() = 0;

    /*--ark web()--*/
    virtual uint32_t GetIvLen() = 0;

    /*--ark web()--*/
    virtual uint32_t GetAlgo() = 0;

    /*--ark web()--*/
    virtual uint32_t GetEncryptedBlockCount() = 0;

    /*--ark web()--*/
    virtual uint32_t GetSkippedBlockCount() = 0;

    /*--ark web()--*/
    virtual uint32_t GetFirstEncryptedOffset() = 0;

    /*--ark web()--*/
    virtual ArkWebUint32Vector GetClearHeaderLens() = 0;

    /*--ark web()--*/
    virtual ArkWebUint32Vector GetPayLoadLens() = 0;

    /*--ark web()--*/
    virtual uint32_t GetMode() = 0;

    /*--ark web()--*/
    virtual void SetKeyId(uint8_t* keyId) = 0;

    /*--ark web()--*/
    virtual void SetKeyIdLen(uint32_t keyIdLen) = 0;

    /*--ark web()--*/
    virtual void SetIv(uint8_t* iv) = 0;

    /*--ark web()--*/
    virtual void SetIvLen(uint32_t ivLen) = 0;

    /*--ark web()--*/
    virtual void SetAlgo(uint32_t algo) = 0;

    /*--ark web()--*/
    virtual void SetEncryptedBlockCount(uint32_t blockCount) = 0;

    /*--ark web()--*/
    virtual void SetSkippedBlockCount(uint32_t blockCount) = 0;

    /*--ark web()--*/
    virtual void SetFirstEncryptedOffset(uint32_t offset) = 0;

    /*--ark web()--*/
    virtual void SetClearHeaderLens(const ArkWebUint32Vector& lens) = 0;

    /*--ark web()--*/
    virtual void SetPayLoadLens(const ArkWebUint32Vector& lens) = 0;

    /*--ark web()--*/
    virtual void SetMode(uint32_t mode) = 0;
};


} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_CODEC_DECODER_ADAPTER_H
