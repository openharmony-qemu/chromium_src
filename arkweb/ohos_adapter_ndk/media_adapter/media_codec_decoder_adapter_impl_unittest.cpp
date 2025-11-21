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

#include "gtest/gtest.h"
#include <gmock/gmock.h>
#define private public
#include "media_codec_decoder_adapter_impl.h"
#include "audio_cenc_info_adapter_impl.h"
#undef private
#include <native_window/external_window.h>
#include <multimedia/drm_framework/native_mediakeysystem.h>
#include <multimedia/drm_framework/native_mediakeysession.h>
#include <multimedia/player_framework/native_avformat.h>
#include <multimedia/player_framework/native_avbuffer.h>
#include <multimedia/player_framework/native_avbuffer_info.h>
#include <multimedia/drm_framework/native_drm_common.h>
#include <multimedia/player_framework/native_avcapability.h>
#include <native_buffer/native_buffer.h>
#include <multimedia/player_framework/native_cencinfo.h>
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_codec_decoder_adapter.h"

using namespace testing;
using namespace std;

static const std::string GetKeySystemName()
{
    if (OH_MediaKeySystem_IsSupported("com.clearplay.drm")) {
        return "com.clearplay.drm";
    } else if (OH_MediaKeySystem_IsSupported("com.wiseplay.drm")) {
        return "com.wiseplay.drm";
    } else {
        return "ERROR";
    }
}

namespace OHOS {
namespace NWeb {
class DecoderCallbackImplTest : public testing::Test {};

class DecoderCallbackAdapterMock : public DecoderCallbackAdapter {
public:
    DecoderCallbackAdapterMock() = default;

    ~DecoderCallbackAdapterMock() override = default;

    void OnError(ErrorType errorType, int32_t errorCode) override {}

    void OnStreamChanged(int32_t width, int32_t height, double frameRate) override {}

    void OnNeedInputData(uint32_t index, std::shared_ptr<OhosBufferAdapter> buffer) override {}

    void OnNeedOutputData(uint32_t index, std::shared_ptr<BufferInfoAdapter> info, BufferFlag flag) override {}
};

class DecoderFormatAdapterMock : public DecoderFormatAdapter {
public:
    DecoderFormatAdapterMock() = default;

    int32_t GetWidth() override
    {
        return width;
    }

    int32_t GetHeight() override
    {
        return height;
    }

    double GetFrameRate() override
    {
        return frameRate;
    }

    void SetWidth(int32_t w) override
    {
        width = w;
    }

    void SetHeight(int32_t h) override
    {
        height = h;
    }

    void SetFrameRate(double fr) override
    {
        frameRate = fr;
    }

    int32_t width;
    int32_t height;
    double frameRate;
};

class MediaCodecDecoderAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void SetCencInfoAboutKeyIdIvAlgo(std::shared_ptr<AudioCencInfoAdapter> cencInfo);
    void SetCencInfoAboutClearHeaderAndPayLoadLens(std::shared_ptr<AudioCencInfoAdapter> cencInfo);

protected:
    std::shared_ptr<DecoderFormatAdapterMock> format_ = nullptr;
    std::shared_ptr<MediaCodecDecoderAdapterImpl> mediaCodecDecoderAdapterImpl_ = nullptr;
};

void MediaCodecDecoderAdapterImplTest::SetUpTestCase(void) {}

void MediaCodecDecoderAdapterImplTest::TearDownTestCase(void) {}

void MediaCodecDecoderAdapterImplTest::SetUp()
{
    constexpr int32_t testWidth = 200;
    constexpr int32_t testHeight = 300;
    constexpr int32_t testFrameRate = 20;
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_, nullptr);
    mediaCodecDecoderAdapterImpl_ = std::make_unique<MediaCodecDecoderAdapterImpl>();
    EXPECT_EQ(format_, nullptr);
    format_ = std::make_unique<DecoderFormatAdapterMock>();
    EXPECT_NE(format_, nullptr);
    format_->width = testWidth;
    format_->height = testHeight;
    format_->frameRate = testFrameRate;
}

void MediaCodecDecoderAdapterImplTest::TearDown(void) {}

void MediaCodecDecoderAdapterImplTest::SetCencInfoAboutKeyIdIvAlgo(
    std::shared_ptr<AudioCencInfoAdapter> cencInfo)
{
    uint8_t keyId[] = {
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
    uint8_t iv[] = {
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
    cencInfo->SetAlgo(0);
    cencInfo->SetKeyId(keyId);
    cencInfo->SetKeyIdLen(DRM_KEY_ID_SIZE);
    cencInfo->SetIv(iv);
    cencInfo->SetIvLen(DRM_KEY_ID_SIZE);
}

void MediaCodecDecoderAdapterImplTest::SetCencInfoAboutClearHeaderAndPayLoadLens(
    std::shared_ptr<AudioCencInfoAdapter> cencInfo)
{
    const std::vector<uint32_t> clearHeaderLens = {1, 2, 3};
    const std::vector<uint32_t> payLoadLens = {4, 5, 6};
    const uint32_t ERR_BLOCK_COUNT = 10000;
    cencInfo->SetClearHeaderLens(clearHeaderLens);
    cencInfo->SetPayLoadLens(payLoadLens);
    cencInfo->SetEncryptedBlockCount(ERR_BLOCK_COUNT);
    cencInfo->SetSkippedBlockCount(0);
    cencInfo->SetFirstEncryptedOffset(0);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_CreateVideoDecoderByName_001.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::CreateVideoDecoderByName() CreateVideoDecoderByName()
 * @tc.type: FUNC.
 */
TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_CreateVideoDecoderByName_001)
{
    // create error decoder
    const std::string errorMimetype = "testmimeType";
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(
	    errorMimetype), DecoderAdapterCode::DECODER_ERROR);
    const std::string errorName = "testname";
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByName(errorName), DecoderAdapterCode::DECODER_ERROR);

    // create normal decoder
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseDecoder(), DecoderAdapterCode::DECODER_OK);

}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_InvalidValueTest_002.
 * @tc.desc: test of InvalidValueScene in MediaCodecDecoderAdapterImpl
 * @tc.type: FUNC.
 */
TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_InvalidValueTest_002)
{
    // not create decoder and test function
    uint32_t index_ = 0;
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->QueueInputBufferDec(index_, 0, 0, 0, BufferFlag::CODEC_BUFFER_FLAG_NONE),
        DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetOutputFormatDec(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseOutputBufferDec(index_, true), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetParameterDecoder(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetOutputSurface(nullptr), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->PrepareDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->StartDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->StopDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->FlushDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ResetDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseDecoder(), DecoderAdapterCode::DECODER_ERROR);
    std::shared_ptr<DecoderCallbackAdapter> callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetDecryptionConfig(nullptr, false), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, nullptr), DecoderAdapterCode::DECODER_ERROR);

    // create decoder and test function in error case
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(nullptr), DecoderAdapterCode::DECODER_ERROR);
    constexpr int32_t errWidth = 10000;
    format_->width = errWidth;
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetParameterDecoder(nullptr), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetParameterDecoder(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetOutputFormatDec(nullptr), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->QueueInputBufferDec(index_, 0, 0, 0, BufferFlag::CODEC_BUFFER_FLAG_NONE),
        DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetOutputSurface(nullptr), DecoderAdapterCode::DECODER_ERROR);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_NormalTest_003.
 * @tc.desc: test of NormalScene in MediaCodecDecoderAdapterImpl
 * @tc.type: FUNC.
 */
TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_NormalTest_003)
{
    uint32_t index_ = 0;
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(format_), DecoderAdapterCode::DECODER_OK);

    std::shared_ptr<DecoderCallbackAdapter> callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->PrepareDecoder(), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->StartDecoder(), DecoderAdapterCode::DECODER_OK);

    constexpr int32_t MEMSIZE = 1024 * 1024;
    OH_AVBuffer* buffer = OH_AVBuffer_Create(MEMSIZE);
    mediaCodecDecoderAdapterImpl_->OnInputBufferAvailable(index_, buffer);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->QueueInputBufferDec(index_, 0, 0, 0, BufferFlag::CODEC_BUFFER_FLAG_NONE),
        DecoderAdapterCode::DECODER_ERROR);
    OH_AVBuffer_Destroy(buffer);
    buffer = nullptr;

    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetOutputFormatDec(format_), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->FlushDecoder(), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetParameterDecoder(format_), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->StopDecoder(), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ResetDecoder(), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseOutputBufferDec(index_, true), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseOutputBufferDec(index_, false), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseDecoder(), DecoderAdapterCode::DECODER_OK);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_SetCallbackDec_004.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::SetCallbackDec
 * @tc.type: FUNC.
 */
TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_SetCallbackDec_004)
{
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);

    std::shared_ptr<DecoderCallbackAdapter> callback = nullptr;
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_ERROR);
    callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseDecoder(), DecoderAdapterCode::DECODER_OK);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_GetTypeOrFlag_005.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::GetBufferFlag() GetAVBufferFlag()
 * @tc.type: FUNC.
 */
TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_GetTypeOrFlag_005)
{
    EXPECT_EQ(
        mediaCodecDecoderAdapterImpl_->GetBufferFlag(OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_EOS),
        BufferFlag::CODEC_BUFFER_FLAG_EOS);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetAVBufferFlag(BufferFlag::CODEC_BUFFER_FLAG_EOS),
        OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_EOS);

    OH_AVCodecBufferFlags testAVCodecBufferFlag = static_cast<OH_AVCodecBufferFlags>(100);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetBufferFlag(testAVCodecBufferFlag), BufferFlag::CODEC_BUFFER_FLAG_NONE);
    BufferFlag testBufferFlag = static_cast<BufferFlag>(100);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetAVBufferFlag(testBufferFlag),
        OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_NONE);
	const std::string codecName = "video/avc";
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByName(codecName), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(format_), DecoderAdapterCode::DECODER_OK);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_OnError_006.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::OnError() OnOutputFormatChanged() OnInputBufferAvailable()
 * OnOutputBufferAvailable()
 * @tc.type: FUNC.
 */
TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_OnError_006)
{
    auto mediaCodecDecoderAdapterImpl_ = std::make_unique<MediaCodecDecoderAdapterImpl>();
    std::string mimetype_ = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(mimetype_);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    std::shared_ptr<DecoderCallbackAdapter> callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_NE(callback, nullptr);
    mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback);

    mediaCodecDecoderAdapterImpl_->OnError(100);
    mediaCodecDecoderAdapterImpl_->OnOutputFormatChanged(nullptr);
    mediaCodecDecoderAdapterImpl_->OnInputBufferAvailable(1, nullptr);
    mediaCodecDecoderAdapterImpl_->OnOutputBufferAvailable(1, nullptr);

    constexpr int32_t MEMSIZE = 1024 * 1024;
    OH_AVFormat* codecFormat = OH_AVFormat_Create();
    OH_AVBuffer* buffer = OH_AVBuffer_Create(MEMSIZE);
    mediaCodecDecoderAdapterImpl_->OnError(1);
    mediaCodecDecoderAdapterImpl_->OnOutputFormatChanged(codecFormat);
    mediaCodecDecoderAdapterImpl_->OnInputBufferAvailable(1, buffer);
    mediaCodecDecoderAdapterImpl_->OnOutputBufferAvailable(1, buffer);
    OH_AVFormat_Destroy(codecFormat);
    OH_AVBuffer_Destroy(buffer);
    codecFormat = nullptr;
    buffer = nullptr;

    mediaCodecDecoderAdapterImpl_->callback_ = nullptr;
    mediaCodecDecoderAdapterImpl_->OnError(100);
    mediaCodecDecoderAdapterImpl_->OnOutputFormatChanged(nullptr);
    mediaCodecDecoderAdapterImpl_->OnInputBufferAvailable(1, nullptr);
    mediaCodecDecoderAdapterImpl_->OnOutputBufferAvailable(1, nullptr);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_SetAVCencInfo_007.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::SetAVCencInfo()
 * OnOutputBufferAvailable()
 * @tc.type: FUNC.
 */
TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_SetAVCencInfo_007)
{
    auto mediaCodecDecoderAdapterImpl_ = std::make_unique<MediaCodecDecoderAdapterImpl>();
    std::string mimetype_ = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(mimetype_);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);

    std::shared_ptr<AudioCencInfoAdapter> cencInfo = std::make_shared<AudioCencInfoAdapterImpl>();
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);

    const uint32_t ERR_ALGO = 10000;
    cencInfo->SetAlgo(ERR_ALGO);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    cencInfo->SetAlgo(0);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    SetCencInfoAboutKeyIdIvAlgo(cencInfo);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    SetCencInfoAboutClearHeaderAndPayLoadLens(cencInfo);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    cencInfo->SetEncryptedBlockCount(0);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    const uint32_t ERR_MODE = 10000;
    cencInfo->SetMode(ERR_MODE);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    cencInfo->SetMode(0);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    constexpr int32_t MEMSIZE = 1024 * 1024;
    OH_AVBuffer* buffer = OH_AVBuffer_Create(MEMSIZE);
    mediaCodecDecoderAdapterImpl_->OnInputBufferAvailable(0, buffer);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    OH_AVBuffer_Destroy(buffer);
    buffer = nullptr;
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_CreateVideoDecoderByMime_008)
{
    OhosInterfaceMock::createByMime = true;
    const std::string validMimetype = "video/avc";
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_CreateByMime(testing::_))
        .WillOnce(testing::Return(nullptr));
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    OhosInterfaceMock::createByMime = false;
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_CreateVideoDecoderByMime_009)
{
    const std::string validMimetype = "video/avc";
    mediaCodecDecoderAdapterImpl_->isSecure_ = true;
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_CreateVideoDecoderByName_010)
{
    const std::string validMimetype = "video/avc";
    mediaCodecDecoderAdapterImpl_->isSecure_ = true;
    OhosInterfaceMock::createByName = true;
    OhosInterfaceMock::category = true;
    OhosInterfaceMock::getName = true;
    const char *name = "video/avc";
    OH_AVCapability *getCategory = reinterpret_cast<OH_AVCapability *>(0x12345678);
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCodec_GetCapabilityByCategory(testing::_, testing::_,
        testing::_)).WillRepeatedly(testing::Return(getCategory));
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCapability_GetName(testing::_))
        .WillRepeatedly(testing::Return(name));
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_CreateByName(testing::_))
        .WillRepeatedly(testing::Return(nullptr));

    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByName(validMimetype);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_ERROR);
    OhosInterfaceMock::createByName = false;
    OhosInterfaceMock::category = false;
    OhosInterfaceMock::getName = false;
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_CreateVideoDecoderByMime_011)
{
    const std::string validMimetype = "video/avc";
    OhosInterfaceMock::destroy = true;
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_Destroy(testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_CreateVideoDecoderByName_012)
{
    OhosInterfaceMock::createByName = true;
    const std::string validMimetype = "video/avc";
    OH_AVCodec *mockCreateByName = reinterpret_cast<OH_AVCodec *>(0x12345678);
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_CreateByName(testing::_))
        .WillOnce(testing::Return(nullptr))
        .WillOnce(testing::Return(mockCreateByName));
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_Destroy(testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));

    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByName(validMimetype);
    OhosInterfaceMock::createByName = false;
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_ConfigureDecoder_013)
{
    OhosInterfaceMock::destroy = false;
    OhosInterfaceMock::formatCreate = true;
    const std::string validMimetype = "video/avc";
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVFormat_Create())
        .WillOnce(testing::Return(nullptr))
        .WillOnce(testing::Return(nullptr));
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetParameterDecoder(format_), DecoderAdapterCode::DECODER_ERROR);
    OhosInterfaceMock::formatCreate = false;
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_ConfigureDecoder_014)
{
    OhosInterfaceMock::configure = true;
    const std::string validMimetype = "video/avc";
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_Configure(testing::_, testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(format_), DecoderAdapterCode::DECODER_ERROR);
    OhosInterfaceMock::configure = false;
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_SetOutputSurface_015)
{
    OhosInterfaceMock::setSurface = true;
    OhosInterfaceMock::nativeWindowHandleOpt = true;
    const std::string validMimetype = "video/avc";
    void *invalidWindow = reinterpret_cast<void *>(0x12345678);
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);

    mediaCodecDecoderAdapterImpl_->isHardwareDecode_ = true;
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_SetSurface(testing::_, testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeWindow_NativeWindowHandleOpt(testing::_, testing::_,
        testing::_)).WillOnce(testing::Return(0));
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetOutputSurface(invalidWindow), DecoderAdapterCode::DECODER_ERROR);
    OhosInterfaceMock::nativeWindowHandleOpt = false;
    OhosInterfaceMock::setSurface = false;
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_SetOutputSurface_016)
{
    OhosInterfaceMock::setSurface = true;
    OhosInterfaceMock::nativeWindowHandleOpt = true;
    OhosInterfaceMock::prepare = true;
    const std::string validMimetype = "video/avc";
    void *invalidWindow = reinterpret_cast<void *>(0x12345678);
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);

    mediaCodecDecoderAdapterImpl_->isHardwareDecode_ = true;
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_SetSurface(testing::_, testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_OK));
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeWindow_NativeWindowHandleOpt(testing::_, testing::_,
        testing::_)).WillOnce(testing::Return(0));
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_Prepare(testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetOutputSurface(invalidWindow), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->PrepareDecoder(), DecoderAdapterCode::DECODER_ERROR);
    OhosInterfaceMock::setSurface = false;
    OhosInterfaceMock::nativeWindowHandleOpt = false;
    OhosInterfaceMock::prepare = false;
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_StartDecoder_017)
{
    OhosInterfaceMock::start = true;
    OhosInterfaceMock::stop = true;
    OhosInterfaceMock::flush = true;
    OhosInterfaceMock::reset = true;
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);

    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_Start(testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_Stop(testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_Flush(testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_Reset(testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->StartDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->StopDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->FlushDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ResetDecoder(), DecoderAdapterCode::DECODER_ERROR);
    OhosInterfaceMock::start = false;
    OhosInterfaceMock::stop = false;
    OhosInterfaceMock::flush = false;
    OhosInterfaceMock::reset = false;
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_QueueInputBufferDec_018)
{
    OhosInterfaceMock::setBufferAttr = true;
    uint32_t index_ = 0;
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);

    std::shared_ptr<DecoderCallbackAdapter> callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_OK);
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVBuffer_SetBufferAttr(testing::_, testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    constexpr int32_t MEMSIZE = 1024 * 1024;
    OH_AVBuffer *buffer = OH_AVBuffer_Create(MEMSIZE);
    mediaCodecDecoderAdapterImpl_->OnInputBufferAvailable(index_, buffer);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->QueueInputBufferDec(index_, 0, 0, 0, BufferFlag::CODEC_BUFFER_FLAG_NONE),
        DecoderAdapterCode::DECODER_ERROR);
    OH_AVBuffer_Destroy(buffer);
    buffer = nullptr;
    OhosInterfaceMock::setBufferAttr = false;
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_QueueInputBufferDec_019)
{
    OhosInterfaceMock::getOutputDescription = true;
    OhosInterfaceMock::renderOutputBuffer = true;
    OhosInterfaceMock::inputBuffer = true;
    OhosInterfaceMock::getBufferAttr = true;
    uint32_t index_ = 0;
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);

    std::shared_ptr<DecoderCallbackAdapter> callback = std::make_shared<DecoderCallbackAdapterMock>();
    constexpr int32_t MEMSIZE = 1024 * 1024;
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_OK);
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_PushInputBuffer(testing::_, testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_OK));
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVBuffer_GetBufferAttr(testing::_, testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    OH_AVBuffer* buffer = OH_AVBuffer_Create(MEMSIZE);
    mediaCodecDecoderAdapterImpl_->OnInputBufferAvailable(index_, buffer);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->QueueInputBufferDec(index_, 0, 0, 0, BufferFlag::CODEC_BUFFER_FLAG_NONE),
        DecoderAdapterCode::DECODER_OK);
    ASSERT_NO_FATAL_FAILURE(mediaCodecDecoderAdapterImpl_->OnOutputBufferAvailable(index_, buffer));
    OH_AVBuffer_Destroy(buffer);
    buffer = nullptr;

    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_GetOutputDescription(testing::_))
        .WillOnce(testing::Return(nullptr));
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_RenderOutputBuffer(testing::_, testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_OK));
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetOutputFormatDec(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseOutputBufferDec(index_, true), DecoderAdapterCode::DECODER_OK);

    OhosInterfaceMock::inputBuffer = false;
    OhosInterfaceMock::getOutputDescription = false;
    OhosInterfaceMock::renderOutputBuffer = false;
    OhosInterfaceMock::getBufferAttr = false;
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_SetCallbackDec_020)
{
    OhosInterfaceMock::registerCallback = true;
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);

    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_RegisterCallback(testing::_, testing::_,
        testing::_)).WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    std::shared_ptr<DecoderCallbackAdapter> callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_ERROR);
    OhosInterfaceMock::registerCallback = false;
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_SetAVCencInfoStruct_021)
{
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);

    std::shared_ptr<AudioCencInfoAdapter> cencInfo = std::make_shared<AudioCencInfoAdapterImpl>();
    OH_AVCencInfo *avCencInfo = OH_AVCencInfo_Create();
    std::vector<uint32_t> clearHeaderLens = {0, 1};
    cencInfo->SetClearHeaderLens(clearHeaderLens);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfoStruct(avCencInfo, cencInfo),
        DecoderAdapterCode::DECODER_ERROR);

    clearHeaderLens.clear();
    cencInfo->SetClearHeaderLens(clearHeaderLens);
    OhosInterfaceMock::setAlgorithm = true;
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCencInfo_SetAlgorithm(testing::_, testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfoStruct(avCencInfo, cencInfo),
        DecoderAdapterCode::DECODER_ERROR);
    OhosInterfaceMock::setAlgorithm = false;

    cencInfo->SetAlgo(0);
    SetCencInfoAboutKeyIdIvAlgo(cencInfo);
    OhosInterfaceMock::setSubsampleInfo = true;
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCencInfo_SetSubsampleInfo(testing::_, testing::_, testing::_,
                testing::_, testing::_, testing::_)).WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfoStruct(avCencInfo, cencInfo),
            DecoderAdapterCode::DECODER_ERROR);
    OhosInterfaceMock::setSubsampleInfo = false;

    OhosInterfaceMock::setMode = true;
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCencInfo_SetMode(testing::_, testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfoStruct(avCencInfo, cencInfo),
            DecoderAdapterCode::DECODER_ERROR);
    OhosInterfaceMock::setMode = false;
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_SetDecryptionConfig_022)
{
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetDecryptionConfig(nullptr, true), DecoderAdapterCode::DECODER_OK);

    OhosInterfaceMock::setDecryptionConfig = true;
    void *session = reinterpret_cast<void *>(0x12345678);
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_VideoDecoder_SetDecryptionConfig(testing::_, testing::_,
        testing::_)).WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_OK));
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetDecryptionConfig(session, true), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetDecryptionConfig(session, true), DecoderAdapterCode::DECODER_OK);
    OhosInterfaceMock::setDecryptionConfig = false;
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_SetAVCencInfo_023)
{
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    std::shared_ptr<DecoderCallbackAdapter> callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_OK);

    std::shared_ptr<AudioCencInfoAdapter> cencInfo = std::make_shared<AudioCencInfoAdapterImpl>();
    cencInfo->SetAlgo(0);
    SetCencInfoAboutKeyIdIvAlgo(cencInfo);
    uint32_t index_ = 0;
    OhosInterfaceMock::infoCreate = true;
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCencInfo_Create())
        .WillOnce(testing::Return(nullptr));
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(index_, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    OhosInterfaceMock::infoCreate = false;

    constexpr int32_t MEMSIZE = 1024 * 1024;
    OH_AVBuffer* buffer = OH_AVBuffer_Create(MEMSIZE);
    mediaCodecDecoderAdapterImpl_->OnInputBufferAvailable(index_, buffer);
    OhosInterfaceMock::setAVBuffer = true;
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCencInfo_SetAVBuffer(testing::_, testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(index_, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    OhosInterfaceMock::setAVBuffer = false;

    OhosInterfaceMock::infoDestroy = true;
    EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCencInfo_Destroy(testing::_))
        .WillOnce(testing::Return(OH_AVErrCode::AV_ERR_NO_MEMORY));
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(index_, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    OhosInterfaceMock::infoDestroy = false;
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(index_, cencInfo), DecoderAdapterCode::DECODER_OK);
    OH_AVBuffer_Destroy(buffer);
    buffer = nullptr;
}

TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_OnError_024)
{
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    std::shared_ptr<DecoderCallbackAdapter> callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_OK);

    OH_AVCodec *codec = reinterpret_cast<OH_AVCodec *>(0x12345678);
    OH_AVFormat *codecFormat = OH_AVFormat_Create();
    constexpr int32_t MEMSIZE = 1024 * 1024;
    OH_AVBuffer *buffer = OH_AVBuffer_Create(MEMSIZE);
    std::shared_ptr<VideoDecoderCallbackManager> Impl_ = std::make_unique<VideoDecoderCallbackManager>();
    Impl_->decoders_[mediaCodecDecoderAdapterImpl_->GetAVDecoder()] = mediaCodecDecoderAdapterImpl_.get();
    Impl_->OnError(nullptr, 0, 0);
    Impl_->OnStreamChanged(nullptr, codecFormat, 0);
    Impl_->OnNeedInputBuffer(nullptr, 0, buffer, 0);
    Impl_->OnNewOutputBuffer(nullptr, 0, buffer, 0);
    Impl_->OnError(codec, 0, 0);
    Impl_->OnStreamChanged(codec, codecFormat, 0);
    Impl_->OnNeedInputBuffer(codec, 0, buffer, 0);
    Impl_->OnNewOutputBuffer(codec, 0, buffer, 0);
    Impl_->OnError(mediaCodecDecoderAdapterImpl_->GetAVDecoder(), 0, 0);
    Impl_->OnStreamChanged(mediaCodecDecoderAdapterImpl_->GetAVDecoder(), codecFormat, 0);
    Impl_->OnNewOutputBuffer(mediaCodecDecoderAdapterImpl_->GetAVDecoder(), 0, buffer, 0);
    Impl_->DeleteVideoDecoder(nullptr);
    Impl_->AddVideoDecoder(nullptr);
    Impl_->AddVideoDecoder(mediaCodecDecoderAdapterImpl_.get());
    EXPECT_TRUE(Impl_->decoders_.size() == 1);
}
}
} // namespace OHOS::NWeb
