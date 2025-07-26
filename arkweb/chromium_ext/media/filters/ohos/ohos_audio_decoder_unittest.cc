// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "testing/gtest/include/gtest/gtest.h"
#include <gmock/gmock.h>

#include "arkweb/chromium_ext/media/filters/ohos/ohos_audio_decoder.h"
#define private public
#include "arkweb/ohos_adapter_ndk/media_adapter/audio_codec_decoder_adapter_impl.h"
#undef private

namespace media {

class OHOSAudioCencInfoTest : public testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(OHOSAudioCencInfoTest, GetKeyID_ShouldReturnKeyId_WhenCalled)
{
    OHOSAudioCencInfo cenc;
    uint8_t* keyId = (uint8_t*)"keyId";
    cenc.SetKeyId(keyId);
    EXPECT_EQ(cenc.GetKeyId(), keyId);
}

TEST_F(OHOSAudioCencInfoTest, GetKeyIDLen_ShouldReturnKeyIdLen_WhenCalled)
{
    OHOSAudioCencInfo cenc;
    uint32_t keyIdLen = 16;
    cenc.SetKeyIdLen(keyIdLen);
    EXPECT_EQ(cenc.GetKeyIdLen(), keyIdLen);
}

TEST_F(OHOSAudioCencInfoTest, GetIv_ShouldReturnIv_WhenCalled)
{
    OHOSAudioCencInfo cenc;
    uint8_t* iv = (uint8_t*)"iv";
    cenc.SetIv(iv);
    EXPECT_EQ(cenc.GetIv(), iv);
}

TEST_F(OHOSAudioCencInfoTest, GetIvLen_ShouldReturnIvLen_WhenCalled)
{
    OHOSAudioCencInfo cenc;
    uint32_t ivLen = 16;
    cenc.SetIvLen(ivLen);
    EXPECT_EQ(cenc.GetIvLen(), ivLen);
}

TEST_F(OHOSAudioCencInfoTest, GetAlgo_ShouldReturnAlgo_WhenCalled)
{
    OHOSAudioCencInfo cenc;
    uint32_t algo = 1;
    cenc.SetAlgo(algo);
    EXPECT_EQ(cenc.GetAlgo(), algo);
}

TEST_F(OHOSAudioCencInfoTest,
         GetEncryptedBlockCount_ShouldReturnEncryptedBlockCount_WhenCalled)
{
    OHOSAudioCencInfo cenc;
    uint32_t encryptedBlockCount = 10;
    cenc.SetEncryptedBlockCount(encryptedBlockCount);
    EXPECT_EQ(cenc.GetEncryptedBlockCount(), encryptedBlockCount);
}

TEST_F(OHOSAudioCencInfoTest,
         GetSkippedBlockCount_ShouldReturnSkippedBlockCount_WhenCalled)
{
    OHOSAudioCencInfo cenc;
    uint32_t skippedBlockCount = 5;
    cenc.SetSkippedBlockCount(skippedBlockCount);
    EXPECT_EQ(cenc.GetSkippedBlockCount(), skippedBlockCount);
}

TEST_F(OHOSAudioCencInfoTest,
         GetFirstEncryptedOffset_ShouldReturnFirstEncryptedOffset_WhenCalled)
{
    OHOSAudioCencInfo cenc;
    uint32_t firstEncryptedOffset = 5;
    cenc.SetFirstEncryptedOffset(firstEncryptedOffset);
    EXPECT_EQ(cenc.GetFirstEncryptedOffset(), firstEncryptedOffset);
}

TEST_F(OHOSAudioCencInfoTest,
         GetClearHeaderLens_ShouldReturnClearHeaderLens_WhenCalled)
{
    OHOSAudioCencInfo cenc;
    std::vector<uint32_t> clearHeaderLens = {1, 2, 3};
    cenc.SetClearHeaderLens(clearHeaderLens);
    EXPECT_EQ(cenc.GetClearHeaderLens(), clearHeaderLens);
}

TEST_F(OHOSAudioCencInfoTest, GetPayLoadLens_ShouldReturnPayLoadLens_WhenCalled)
{
    OHOSAudioCencInfo cenc;
    std::vector<uint32_t> payLoadLens = {4, 5, 6};
    cenc.SetPayLoadLens(payLoadLens);
    EXPECT_EQ(cenc.GetPayLoadLens(), payLoadLens);
}

TEST_F(OHOSAudioCencInfoTest, GetMode_ShouldReturnMode_WhenCalled)
{
    OHOSAudioCencInfo cenc;
    uint32_t mode = 1;
    cenc.SetMode(mode);
    EXPECT_EQ(cenc.GetMode(), mode);
}

class OHOSAudioDecoderFormatTest : public testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(OHOSAudioCencInfoTest, FunctionsCalled)
{
    constexpr int32_t DEFAULT_SAMPLERATE = 44100;
    constexpr int64_t DEFAULT_BITRATE = 32000;
    constexpr int32_t DEFAULT_CHANNEL_COUNT = 2;
    constexpr int32_t DEFAULT_MAX_INPUT_SIZE = 1152;
    constexpr bool DEFAUL_AAC_IS_ADTS = false;
    constexpr int32_t DEFAULT_AUDIO_SAMPLE_FORMAT = 100;
    constexpr int32_t DEFAUL_ID_HEARDER = 100;
    constexpr int32_t DEFAULT_SETUP_HEADER = 100;
    uint8_t codecConfig[10] = {0};
    constexpr uint32_t DEFAULT_CODEC_Config_SIZE = 10;
    std::shared_ptr<AudioDecoderFormatAdapterImpl> format = std::make_shared<AudioDecoderFormatAdapterImpl>();
    format->SetSampleRate(DEFAULT_SAMPLERATE);
    format->SetChannelCount(DEFAULT_CHANNEL_COUNT);
    format->SetBitRate(DEFAULT_BITRATE);
    format->SetMaxInputSize(DEFAULT_MAX_INPUT_SIZE);
    format->SetAACIsAdts(DEFAUL_AAC_IS_ADTS);
    format->SetAudioSampleFormat(DEFAULT_AUDIO_SAMPLE_FORMAT);
    format->SetIdentificationHeader(DEFAUL_ID_HEARDER);
    format->SetSetupHeader(DEFAULT_SETUP_HEADER);
    format->SetCodecConfig(codecConfig);
    format->SetCodecConfigSize(DEFAULT_CODEC_Config_SIZE);
    format->PrintFormatData(format);
    EXPECT_EQ(format->GetSampleRate(), DEFAULT_SAMPLERATE);
    EXPECT_EQ(format->GetChannelCount(), DEFAULT_CHANNEL_COUNT);
    EXPECT_EQ(format->GetBitRate(), DEFAULT_BITRATE);
    EXPECT_EQ(format->GetMaxInputSize(), DEFAULT_MAX_INPUT_SIZE);
    EXPECT_EQ(format->GetAACIsAdts(), DEFAUL_AAC_IS_ADTS);
    EXPECT_EQ(format->GetAudioSampleFormat(), DEFAULT_AUDIO_SAMPLE_FORMAT);
    EXPECT_EQ(format->GetIdentificationHeader(), DEFAUL_ID_HEARDER);
    EXPECT_EQ(format->GetSetupHeader(), DEFAULT_SETUP_HEADER);
    EXPECT_EQ(format->GetCodecConfig(), codecConfig);
    EXPECT_EQ(format->GetCodecConfigSize(), DEFAULT_CODEC_Config_SIZE);
}


class AudioDecoderCallbackTest : public testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

class AudioDecoderCallbackClientMock : public AudioDecoderCallback::Client {
public:
    MOCK_METHOD(void, AddInputBuffer, (uint32_t index), (override));

    MOCK_METHOD(void, AddOutputBuffer, (uint32_t index, uint8_t* bufferData, uint32_t size, int64_t pts, BufferFlag flag), (override));

    MOCK_METHOD(void, UpdateOutputFormat, (), (override));

    MOCK_METHOD(void, OnError, (int32_t errorCode), (override));
};

TEST_F(AudioDecoderCallbackTest, FunctionsCalled) {
    AudioDecoderCallbackClientMock* client = new AudioDecoderCallbackClientMock();

    EXPECT_CALL(*client, OnError(0)).Times(1);
    EXPECT_CALL(*client, UpdateOutputFormat()).Times(1);
    EXPECT_CALL(*client, AddInputBuffer(0)).Times(1);
    EXPECT_CALL(*client, AddOutputBuffer(0, nullptr, 0, 0, static_cast<BufferFlag>(0))).Times(1);

    AudioDecoderCallback cb(client);
    cb.OnError(0);
    cb.OnOutputFormatChanged();
    cb.OnInputBufferAvailable(0);
    cb.OnOutputBufferAvailable(0, nullptr, 0, 0, 0, 0);

    delete client;
    client = nullptr;

    AudioDecoderCallback cb1(nullptr);
    cb1.OnError(0);
    cb1.OnOutputFormatChanged();
    cb1.OnInputBufferAvailable(0);
    cb1.OnOutputBufferAvailable(0, nullptr, 0, 0, 0, 0);
}
}