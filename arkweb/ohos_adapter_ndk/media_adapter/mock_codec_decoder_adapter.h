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

#ifndef MOCK_CODEC_DECODER_ADAPTER_H
#define MOCK_CODEC_DECODER_ADAPTER_H

#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include <multimedia/drm_framework/native_mediakeysystem.h>
#include <multimedia/drm_framework/native_mediakeysession.h>
#include <multimedia/player_framework/native_avformat.h>
#include <multimedia/player_framework/native_avbuffer.h>
#include <multimedia/player_framework/native_avbuffer_info.h>
#include <multimedia/drm_framework/native_drm_common.h>
#include <multimedia/player_framework/native_avcapability.h>
#include <native_buffer/native_buffer.h>
#include <multimedia/player_framework/native_cencinfo.h>
#include <bundle/native_interface_bundle.h>

namespace OHOS {
namespace NWeb {
class OhosInterfaceMock {
public:
    static OhosInterfaceMock& GetInstance() {
        static OhosInterfaceMock instance;
        return instance;
    }

    MOCK_METHOD(OH_AVErrCode, OH_VideoDecoder_Destroy, (OH_AVCodec *), (const));
    MOCK_METHOD(OH_AVCodec *, OH_VideoDecoder_CreateByMime, (const char *), (const));
    MOCK_METHOD(OH_AVCodec *, OH_VideoDecoder_CreateByName, (const char *), (const));
    MOCK_METHOD(OH_AVFormat *, OH_AVFormat_Create, (), (const));
    MOCK_METHOD(OH_AVErrCode, OH_VideoDecoder_Configure, (OH_AVCodec *, OH_AVFormat *), (const));
    MOCK_METHOD(OH_AVErrCode, OH_VideoDecoder_SetSurface, (OH_AVCodec *, OHNativeWindow *), (const));
    MOCK_METHOD(int32_t, OH_NativeWindow_NativeWindowHandleOpt, (OHNativeWindow*, int32_t, uint64_t), (const));
    MOCK_METHOD(OH_AVErrCode, OH_VideoDecoder_Prepare, (OH_AVCodec *), (const));
    MOCK_METHOD(OH_AVErrCode, OH_VideoDecoder_Start, (OH_AVCodec *), (const));
    MOCK_METHOD(OH_AVErrCode, OH_VideoDecoder_Stop, (OH_AVCodec *), (const));
    MOCK_METHOD(OH_AVErrCode, OH_VideoDecoder_Flush, (OH_AVCodec *), (const));
    MOCK_METHOD(OH_AVErrCode, OH_VideoDecoder_Reset, (OH_AVCodec *), (const));
    MOCK_METHOD(OH_AVErrCode, OH_AVBuffer_SetBufferAttr, (OH_AVBuffer *, const OH_AVCodecBufferAttr *), (const));
    MOCK_METHOD(OH_AVErrCode, OH_VideoDecoder_PushInputBuffer, (OH_AVCodec *, uint32_t), (const));
    MOCK_METHOD(OH_AVFormat *, OH_VideoDecoder_GetOutputDescription, (OH_AVCodec *), (const));
    MOCK_METHOD(OH_AVErrCode, OH_VideoDecoder_RenderOutputBuffer, (OH_AVCodec *, uint32_t), (const));
    MOCK_METHOD(OH_AVErrCode, OH_VideoDecoder_RegisterCallback, (OH_AVCodec *, OH_AVCodecCallback, void *), (const));
    MOCK_METHOD(OH_AVErrCode, OH_AVBuffer_GetBufferAttr, (OH_AVBuffer *, OH_AVCodecBufferAttr *), (const));
    MOCK_METHOD(OH_AVErrCode, OH_AVCencInfo_SetAlgorithm, (OH_AVCencInfo *, enum DrmCencAlgorithm), (const));
    MOCK_METHOD(OH_AVErrCode, OH_AVCencInfo_SetSubsampleInfo, (OH_AVCencInfo *, uint32_t, uint32_t, uint32_t,
                uint32_t, DrmSubsample *), (const));
    MOCK_METHOD(OH_AVErrCode, OH_AVCencInfo_SetMode, (OH_AVCencInfo *, enum DrmCencInfoMode), (const));
    MOCK_METHOD(OH_AVErrCode, OH_VideoDecoder_SetDecryptionConfig, (OH_AVCodec *, MediaKeySession *, bool), (const));
    MOCK_METHOD(OH_AVCencInfo *, OH_AVCencInfo_Create, (), (const));
    MOCK_METHOD(OH_AVErrCode, OH_AVCencInfo_SetAVBuffer, (OH_AVCencInfo *, OH_AVBuffer *), (const));
    MOCK_METHOD(OH_AVErrCode, OH_AVCencInfo_Destroy, (OH_AVCencInfo *), (const));
    MOCK_METHOD(OH_AVCapability *, OH_AVCodec_GetCapabilityByCategory, (const char *, bool, OH_AVCodecCategory),
                (const));
    MOCK_METHOD(const char *, OH_AVCapability_GetName, (OH_AVCapability *), (const));

    static bool destroy;
    static bool createByMime;
    static bool createByName;
    static bool formatCreate;
    static bool configure;
    static bool setSurface;
    static bool nativeWindowHandleOpt;
    static bool prepare;
    static bool start;
    static bool stop;
    static bool flush;
    static bool reset;
    static bool setBufferAttr;
    static bool inputBuffer;
    static bool getOutputDescription;
    static bool renderOutputBuffer;
    static bool registerCallback;
    static bool getBufferAttr;
    static bool setAlgorithm;
    static bool setSubsampleInfo;
    static bool setMode;
    static bool setDecryptionConfig;
    static bool infoCreate;
    static bool setAVBuffer;
    static bool infoDestroy;
    static bool category;
    static bool getName;
};

#ifdef __cplusplus
extern "C" {
#endif
    OH_AVErrCode __real_OH_VideoDecoder_Destroy(OH_AVCodec *);
    OH_AVCodec* __real_OH_VideoDecoder_CreateByMime(const char *);
    OH_AVCodec* __real_OH_VideoDecoder_CreateByName(const char *);
    OH_AVFormat* __real_OH_AVFormat_Create();
    OH_AVErrCode __real_OH_VideoDecoder_Configure(OH_AVCodec *, OH_AVFormat *);
    OH_AVErrCode __real_OH_VideoDecoder_SetSurface(OH_AVCodec *, OHNativeWindow *);
    int32_t __real_OH_NativeWindow_NativeWindowHandleOpt(OHNativeWindow *, int, uint64_t);
    OH_AVErrCode __real_OH_VideoDecoder_Prepare(OH_AVCodec *);
    OH_AVErrCode __real_OH_VideoDecoder_Start(OH_AVCodec *);
    OH_AVErrCode __real_OH_VideoDecoder_Stop(OH_AVCodec *);
    OH_AVErrCode __real_OH_VideoDecoder_Flush(OH_AVCodec *);
    OH_AVErrCode __real_OH_VideoDecoder_Reset(OH_AVCodec *);
    OH_AVErrCode __real_OH_AVBuffer_SetBufferAttr(OH_AVBuffer *, const OH_AVCodecBufferAttr *);
    OH_AVErrCode __real_OH_VideoDecoder_PushInputBuffer(OH_AVCodec *, uint32_t);
    OH_AVFormat* __real_OH_VideoDecoder_GetOutputDescription(OH_AVCodec *);
    OH_AVErrCode __real_OH_VideoDecoder_RenderOutputBuffer(OH_AVCodec *, uint32_t);
    OH_AVErrCode __real_OH_VideoDecoder_RegisterCallback(OH_AVCodec *, OH_AVCodecCallback, void *);
    OH_AVErrCode __real_OH_AVBuffer_GetBufferAttr(OH_AVBuffer *, OH_AVCodecBufferAttr *);
    OH_AVErrCode __real_OH_AVCencInfo_SetAlgorithm(OH_AVCencInfo *, enum DrmCencAlgorithm);
    OH_AVErrCode __real_OH_AVCencInfo_SetSubsampleInfo(OH_AVCencInfo *, uint32_t, uint32_t, uint32_t, uint32_t,
                                                        DrmSubsample *);
    OH_AVErrCode __real_OH_AVCencInfo_SetMode(OH_AVCencInfo *, enum DrmCencInfoMode);
    OH_AVErrCode __real_OH_VideoDecoder_SetDecryptionConfig(OH_AVCodec *, MediaKeySession *, bool);
    OH_AVCencInfo* __real_OH_AVCencInfo_Create();
    OH_AVErrCode __real_OH_AVCencInfo_SetAVBuffer(OH_AVCencInfo *, OH_AVBuffer *);
    OH_AVErrCode __real_OH_AVCencInfo_Destroy(OH_AVCencInfo *);
    OH_AVCapability* __real_OH_AVCodec_GetCapabilityByCategory(const char *, bool, OH_AVCodecCategory);
    const char* __real_OH_AVCapability_GetName(OH_AVCapability *);

#ifdef __cplusplus
}
#endif

}  // namespace NWeb
}  // namespace OHOS

#endif // MOCK_CODEC_DECODER_ADAPTER_H
