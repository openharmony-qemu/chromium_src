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

#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_codec_decoder_adapter.h"

namespace OHOS {
namespace NWeb {

bool OhosInterfaceMock::destroy = false;
bool OhosInterfaceMock::createByMime = false;
bool OhosInterfaceMock::createByName = false;
bool OhosInterfaceMock::formatCreate = false;
bool OhosInterfaceMock::configure = false;
bool OhosInterfaceMock::setSurface = false;
bool OhosInterfaceMock::nativeWindowHandleOpt = false;
bool OhosInterfaceMock::prepare = false;
bool OhosInterfaceMock::start = false;
bool OhosInterfaceMock::stop = false;
bool OhosInterfaceMock::flush = false;
bool OhosInterfaceMock::reset = false;
bool OhosInterfaceMock::setBufferAttr = false;
bool OhosInterfaceMock::inputBuffer = false;
bool OhosInterfaceMock::getOutputDescription = false;
bool OhosInterfaceMock::renderOutputBuffer = false;
bool OhosInterfaceMock::registerCallback = false;
bool OhosInterfaceMock::getBufferAttr = false;
bool OhosInterfaceMock::setAlgorithm = false;
bool OhosInterfaceMock::setSubsampleInfo = false;
bool OhosInterfaceMock::setMode = false;
bool OhosInterfaceMock::setDecryptionConfig = false;
bool OhosInterfaceMock::infoCreate = false;
bool OhosInterfaceMock::setAVBuffer = false;
bool OhosInterfaceMock::infoDestroy = false;
bool OhosInterfaceMock::category = false;
bool OhosInterfaceMock::getName = false;

#ifdef __cplusplus
extern "C" {
#endif

OH_AVErrCode __wrap_OH_VideoDecoder_Destroy(OH_AVCodec *codec) {
    if (OhosInterfaceMock::destroy == true) {
        return OhosInterfaceMock::GetInstance().OH_VideoDecoder_Destroy(codec);
    } else {
        return __real_OH_VideoDecoder_Destroy(codec);
    }
}

OH_AVCodec* __wrap_OH_VideoDecoder_CreateByMime(const char *mime) {
    if (OhosInterfaceMock::createByMime == true) {
        return OhosInterfaceMock::GetInstance().OH_VideoDecoder_CreateByMime(mime);
    } else {
        return __real_OH_VideoDecoder_CreateByMime(mime);
    }
}

OH_AVCodec* __wrap_OH_VideoDecoder_CreateByName(const char *name) {
    if (OhosInterfaceMock::createByName == true) {
        return OhosInterfaceMock::GetInstance().OH_VideoDecoder_CreateByName(name);
    } else {
        return __real_OH_VideoDecoder_CreateByName(name);
    }
}

OH_AVFormat* __wrap_OH_AVFormat_Create() {
    if (OhosInterfaceMock::formatCreate == true) {
        return OhosInterfaceMock::GetInstance().OH_AVFormat_Create();
    } else {
        return __real_OH_AVFormat_Create();
    }
}

OH_AVErrCode __wrap_OH_VideoDecoder_Configure(OH_AVCodec *codec, OH_AVFormat *format) {
    if (OhosInterfaceMock::configure == true) {
        return OhosInterfaceMock::GetInstance().OH_VideoDecoder_Configure(codec, format);
    } else {
        return __real_OH_VideoDecoder_Configure(codec, format);
    }
}

OH_AVErrCode __wrap_OH_VideoDecoder_SetSurface(OH_AVCodec *codec, OHNativeWindow *window) {
    if (OhosInterfaceMock::setSurface == true) {
        return OhosInterfaceMock::GetInstance().OH_VideoDecoder_SetSurface(codec, window);
    } else {
        return __real_OH_VideoDecoder_SetSurface(codec, window);
    }
}

int32_t __wrap_OH_NativeWindow_NativeWindowHandleOpt(OHNativeWindow *window, int code, uint64_t value) {
    if (OhosInterfaceMock::nativeWindowHandleOpt) {
        return OhosInterfaceMock::GetInstance().OH_NativeWindow_NativeWindowHandleOpt(window, code, value);
    } else {
        return __real_OH_NativeWindow_NativeWindowHandleOpt(window, code, value);
    }
}

OH_AVErrCode __wrap_OH_VideoDecoder_Prepare(OH_AVCodec *codec) {
    if (OhosInterfaceMock::prepare == true) {
        return OhosInterfaceMock::GetInstance().OH_VideoDecoder_Prepare(codec);
    } else {
        return __real_OH_VideoDecoder_Prepare(codec);
    }
}

OH_AVErrCode __wrap_OH_VideoDecoder_Start(OH_AVCodec *codec) {
    if (OhosInterfaceMock::start == true) {
        return OhosInterfaceMock::GetInstance().OH_VideoDecoder_Start(codec);
    } else {
        return __real_OH_VideoDecoder_Start(codec);
    }
}

OH_AVErrCode __wrap_OH_VideoDecoder_Stop(OH_AVCodec *codec) {
    if (OhosInterfaceMock::stop == true) {
        return OhosInterfaceMock::GetInstance().OH_VideoDecoder_Stop(codec);
    } else {
        return __real_OH_VideoDecoder_Stop(codec);
    }
}

OH_AVErrCode __wrap_OH_VideoDecoder_Flush(OH_AVCodec *codec) {
    if (OhosInterfaceMock::flush == true) {
        return OhosInterfaceMock::GetInstance().OH_VideoDecoder_Flush(codec);
    } else {
        return __real_OH_VideoDecoder_Flush(codec);
    }
}

OH_AVErrCode __wrap_OH_VideoDecoder_Reset(OH_AVCodec *codec) {
    if (OhosInterfaceMock::reset == true) {
        return OhosInterfaceMock::GetInstance().OH_VideoDecoder_Reset(codec);
    } else {
        return __real_OH_VideoDecoder_Reset(codec);
    }
}

OH_AVErrCode __wrap_OH_AVBuffer_SetBufferAttr(OH_AVBuffer *buffer, const OH_AVCodecBufferAttr *attr) {
    if (OhosInterfaceMock::setBufferAttr == true) {
        return OhosInterfaceMock::GetInstance().OH_AVBuffer_SetBufferAttr(buffer, attr);
    } else {
        return __real_OH_AVBuffer_SetBufferAttr(buffer, attr);
    }
}

OH_AVErrCode __wrap_OH_VideoDecoder_PushInputBuffer(OH_AVCodec *buffer, uint32_t index) {
    if (OhosInterfaceMock::inputBuffer == true) {
        return OhosInterfaceMock::GetInstance().OH_VideoDecoder_PushInputBuffer(buffer, index);
    } else {
        return __real_OH_VideoDecoder_PushInputBuffer(buffer, index);
    }
}

OH_AVFormat* __wrap_OH_VideoDecoder_GetOutputDescription(OH_AVCodec *codec) {
    if (OhosInterfaceMock::getOutputDescription) {
        return OhosInterfaceMock::GetInstance().OH_VideoDecoder_GetOutputDescription(codec);
    } else {
        return __real_OH_VideoDecoder_GetOutputDescription(codec);
    }
}

OH_AVErrCode __wrap_OH_VideoDecoder_RenderOutputBuffer(OH_AVCodec *codec, uint32_t index) {
    if (OhosInterfaceMock::renderOutputBuffer == true) {
        return OhosInterfaceMock::GetInstance().OH_VideoDecoder_RenderOutputBuffer(codec, index);
    } else {
        return __real_OH_VideoDecoder_RenderOutputBuffer(codec, index);
    }
}

OH_AVErrCode __wrap_OH_VideoDecoder_RegisterCallback(OH_AVCodec *codec, OH_AVCodecCallback callback, void *userData) {
    if (OhosInterfaceMock::registerCallback == true) {
        return OhosInterfaceMock::GetInstance().OH_VideoDecoder_RegisterCallback(codec, callback, userData);
    } else {
        return __real_OH_VideoDecoder_RegisterCallback(codec, callback, userData);
    }
}

OH_AVErrCode __wrap_OH_AVBuffer_GetBufferAttr(OH_AVBuffer *buffer, OH_AVCodecBufferAttr *attr) {
    if (OhosInterfaceMock::getBufferAttr == true) {
        return OhosInterfaceMock::GetInstance().OH_AVBuffer_GetBufferAttr(buffer, attr);
    } else {
        return __real_OH_AVBuffer_GetBufferAttr(buffer, attr);
    }
}

OH_AVErrCode __wrap_OH_AVCencInfo_SetAlgorithm(OH_AVCencInfo *cencInfo, enum DrmCencAlgorithm algo) {
    if (OhosInterfaceMock::setAlgorithm == true) {
        return OhosInterfaceMock::GetInstance().OH_AVCencInfo_SetAlgorithm(cencInfo, algo);
    } else {
        return __real_OH_AVCencInfo_SetAlgorithm(cencInfo, algo);
    }
}

OH_AVErrCode __wrap_OH_AVCencInfo_SetSubsampleInfo(OH_AVCencInfo *cencInfo, uint32_t encryptedBlockCount,
    uint32_t skippedBlockCount, uint32_t firstEncryptedOffset, uint32_t subsampleCount, DrmSubsample *subsamples) {
    if (OhosInterfaceMock::setSubsampleInfo == true) {
        return OhosInterfaceMock::GetInstance().OH_AVCencInfo_SetSubsampleInfo(cencInfo, encryptedBlockCount,
            skippedBlockCount, firstEncryptedOffset, subsampleCount, subsamples);
    } else {
        return __real_OH_AVCencInfo_SetSubsampleInfo(cencInfo, encryptedBlockCount, skippedBlockCount,
            firstEncryptedOffset, subsampleCount, subsamples);
    }
}

OH_AVErrCode __wrap_OH_AVCencInfo_SetMode(OH_AVCencInfo *cencInfo, enum DrmCencInfoMode mode) {
    if (OhosInterfaceMock::setMode == true) {
        return OhosInterfaceMock::GetInstance().OH_AVCencInfo_SetMode(cencInfo, mode);
    } else {
        return __real_OH_AVCencInfo_SetMode(cencInfo, mode);
    }
}

OH_AVErrCode __wrap_OH_VideoDecoder_SetDecryptionConfig(OH_AVCodec *codec, MediaKeySession *mediaKeySession,
    bool secureVideoPath) {
    if (OhosInterfaceMock::setDecryptionConfig == true) {
        return OhosInterfaceMock::GetInstance().OH_VideoDecoder_SetDecryptionConfig(codec, mediaKeySession,
                secureVideoPath);
    } else {
        return __real_OH_VideoDecoder_SetDecryptionConfig(codec, mediaKeySession, secureVideoPath);
    }
}

OH_AVCencInfo* __wrap_OH_AVCencInfo_Create() {
    if (OhosInterfaceMock::infoCreate == true) {
        return OhosInterfaceMock::GetInstance().OH_AVCencInfo_Create();
    } else {
        return __real_OH_AVCencInfo_Create();
    }
}

OH_AVErrCode __wrap_OH_AVCencInfo_SetAVBuffer(OH_AVCencInfo *cencInfo, OH_AVBuffer *buffer) {
    if (OhosInterfaceMock::setAVBuffer == true) {
        return OhosInterfaceMock::GetInstance().OH_AVCencInfo_SetAVBuffer(cencInfo, buffer);
    } else {
        return __real_OH_AVCencInfo_SetAVBuffer(cencInfo, buffer);
    }
}

OH_AVErrCode __wrap_OH_AVCencInfo_Destroy(OH_AVCencInfo *cencInfo) {
    if (OhosInterfaceMock::infoDestroy == true) {
        return OhosInterfaceMock::GetInstance().OH_AVCencInfo_Destroy(cencInfo);
    } else {
        return __real_OH_AVCencInfo_Destroy(cencInfo);
    }
}

OH_AVCapability* __wrap_OH_AVCodec_GetCapabilityByCategory(const char *mime, bool isEncoder,
                                                            OH_AVCodecCategory category) {
    if (OhosInterfaceMock::category == true) {
        return OhosInterfaceMock::GetInstance().OH_AVCodec_GetCapabilityByCategory(mime, isEncoder, category);
    } else {
        return __real_OH_AVCodec_GetCapabilityByCategory(mime, isEncoder, category);
    }
}

const char* __wrap_OH_AVCapability_GetName(OH_AVCapability *capability) {
    if (OhosInterfaceMock::getName == true) {
        return OhosInterfaceMock::GetInstance().OH_AVCapability_GetName(capability);
    } else {
        return __real_OH_AVCapability_GetName(capability);
    }
}

#ifdef __cplusplus
}
#endif

}  // namespace NWeb
}  // namespace OHOS
