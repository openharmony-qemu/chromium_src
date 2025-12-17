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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_ndk_api.h"
#define private public
#include "audio_capturer_adapter_impl.h"
#undef private

using namespace testing;
using namespace MockNdkApi;
using namespace OHOS::NWeb;
class MockAudioCapturerReadCallbackAdapter : public AudioCapturerReadCallbackAdapter {
  public:
    MOCK_METHOD(void, OnReadData, (size_t length), (override));
};

class MockBufferDescAdapter : public BufferDescAdapter {
  public:
    MOCK_METHOD(uint8_t*, GetBuffer, (), (override));
    MOCK_METHOD(size_t, GetBufLength, (), (override));
    MOCK_METHOD(size_t, GetDataLength, (), (override));
    MOCK_METHOD(void, SetBuffer, (uint8_t*), (override));
    MOCK_METHOD(void, SetBufLength, (size_t), (override));
    MOCK_METHOD(void, SetDataLength, (size_t), (override));
};

class MockAudioCapturerOptions : public OHOS::NWeb::AudioCapturerOptionsAdapter {
  public:
    MOCK_METHOD(int32_t, GetCapturerFlags, (), (override));
    MOCK_METHOD(AudioAdapterSamplingRate, GetSamplingRate, (), (override));
    MOCK_METHOD(AudioAdapterEncodingType, GetEncoding, (), (override));
    MOCK_METHOD(AudioAdapterSampleFormat, GetSampleFormat, (), (override));
    MOCK_METHOD(AudioAdapterChannel, GetChannels, (), (override));
    MOCK_METHOD(AudioAdapterSourceType, GetSourceType, (), (override));
};

class AudioCapturerAdapterImplTest : public testing::Test {
  protected:
   void SetAllMockType(bool type) {
     MockAudioCommonEventSupport::start = type;
     MockAudioCommonEventSupport::stop = type;
     MockAudioCommonEventSupport::release = type;
     MockAudioCommonEventSupport::getFrameSizeInCallback = type;
     MockAudioCommonEventSupport::getTimestamp = type;
     MockAudioCommonEventSupport::create = type;
     MockAudioCommonEventSupport::setSamplingRate = type;
     MockAudioCommonEventSupport::destroy = type;
     MockAudioCommonEventSupport::setEncodingType = type;
     MockAudioCommonEventSupport::setSampleFormat = type;
     MockAudioCommonEventSupport::setChannelCount = type;
     MockAudioCommonEventSupport::setCapturerInfo = type;
     MockAudioCommonEventSupport::setBufferAttr = type;
     MockAudioCommonEventSupport::setLatencyMode = type;
     MockAudioCommonEventSupport::setCapturerCallback = type;
     MockAudioCommonEventSupport::generateCapturer = type;
   }

    void SetUp() override {
      adapter_ = std::make_shared<AudioCapturerAdapterImpl>();
      mockCallback_ = std::make_shared<MockAudioCapturerReadCallbackAdapter>();
      bufferDesc_ = std::make_shared<MockBufferDescAdapter>();
      options_ = std::make_shared<MockAudioCapturerOptions>();
      adapter_->callback_index_ = 0;
      SetAllMockType(true);
    }
    void TearDown() override {
      SetAllMockType(false);
      adapter_->audio_capturer_ = nullptr;
    }

    std::shared_ptr<AudioCapturerAdapterImpl> adapter_;
    std::shared_ptr<MockAudioCapturerReadCallbackAdapter> mockCallback_;
    std::shared_ptr<UserDataCallBack> userDataCallBack_;
    std::shared_ptr<MockBufferDescAdapter> bufferDesc_;
    std::shared_ptr<MockAudioCapturerOptions> options_;
};

TEST_F(AudioCapturerAdapterImplTest, Start_NullCapturer) {
  adapter_->audio_capturer_ = nullptr;
  bool result = adapter_->Start();
  EXPECT_FALSE(result);
}

TEST_F(AudioCapturerAdapterImplTest, Stop_NullCapturer) {
  adapter_->audio_capturer_ = nullptr;
  bool result = adapter_->Stop();
  EXPECT_FALSE(result);
}

TEST_F(AudioCapturerAdapterImplTest, Release_NullCapturer) {
  adapter_->audio_capturer_ = nullptr;
  bool result = adapter_->Release();
  EXPECT_FALSE(result);
}

TEST_F(AudioCapturerAdapterImplTest, GetAudioTime_ReturnsError) {
  adapter_->audio_capturer_ = nullptr;
  EXPECT_EQ(adapter_->GetAudioTime(), AUDIO_NULL_ERROR);
}

TEST_F(AudioCapturerAdapterImplTest, GetBufferDesc_ReturnsError) {
  adapter_->callback_index_= adapter_->callback_wrapper_.AddCallback(nullptr);
  EXPECT_EQ(adapter_->GetBufferDesc(nullptr), AUDIO_NULL_ERROR);
}

TEST_F(AudioCapturerAdapterImplTest, GetBufferDesc_ReturnsErrorWhenUserDataCallbackIsNull) {
  adapter_->callback_index_= adapter_->callback_wrapper_.AddCallback(nullptr);
  EXPECT_EQ(adapter_->GetBufferDesc(bufferDesc_), AUDIO_NULL_ERROR);
}

TEST_F(AudioCapturerAdapterImplTest, GetBufferDesc_ReturnsErrorWhenUserDataBufferIsNull) {
  auto userCallBack = std::make_shared<UserDataCallBack>();
  userCallBack->buffer = nullptr;
  userCallBack->length = 100;
  adapter_->callback_index_= adapter_->callback_wrapper_.AddCallback(userCallBack);
  EXPECT_EQ(adapter_->GetBufferDesc(bufferDesc_), AUDIO_NULL_ERROR);
}


TEST_F(AudioCapturerAdapterImplTest, GetBufferDesc_Success) {
  auto mockDesc = std::make_shared<MockBufferDescAdapter>();

  auto userCallBack = std::make_shared<UserDataCallBack>();
  const size_t testLength = 256;
  uint8_t testBuffer[testLength]{};
  userCallBack->buffer = testBuffer;
  userCallBack->length = testLength;
  adapter_->callback_index_= adapter_->callback_wrapper_.AddCallback(userCallBack);

  EXPECT_CALL(*mockDesc, SetBuffer(testBuffer)).Times(1);
  EXPECT_CALL(*mockDesc, SetBufLength(testLength)).Times(1);
  EXPECT_CALL(*mockDesc, SetDataLength(testLength)).Times(1);

  EXPECT_EQ(adapter_->GetBufferDesc(mockDesc), AUDIO_OK);
}

TEST_F(AudioCapturerAdapterImplTest, GetFrameCount_WhenCapturerIsNull_ReturnsError) {
  uint32_t frameCount = 0;
  adapter_->audio_capturer_ = nullptr; 
  int32_t ret = adapter_->GetFrameCount(frameCount);
  EXPECT_EQ(ret, AUDIO_NULL_ERROR);
  EXPECT_EQ(frameCount, 0U);
}


TEST_F(AudioCapturerAdapterImplTest, SetCapturerReadCallback_Success) {
  adapter_->audio_capturer_ = reinterpret_cast<OH_AudioCapturer*>(0x2345);
  auto userCallBack = std::make_shared<UserDataCallBack>();
  const size_t testLength = 256;
  uint8_t testBuffer[testLength]{};
  userCallBack->buffer = testBuffer;
  userCallBack->length = testLength;
  adapter_->callback_index_= adapter_->callback_wrapper_.AddCallback(userCallBack);
  int32_t ret = adapter_->SetCapturerReadCallback(mockCallback_);
  EXPECT_EQ(ret, AUDIO_OK);
}

TEST_F(AudioCapturerAdapterImplTest, SetCapturerReadCallback_NullCallback) {
  int32_t ret = adapter_->SetCapturerReadCallback(nullptr);
  EXPECT_EQ(ret, AUDIO_NULL_ERROR);
}

TEST_F(AudioCapturerAdapterImplTest, SetCapturerReadCallback_NullAudioCapturer) {
  adapter_->audio_capturer_ = nullptr;
  int32_t ret = adapter_->SetCapturerReadCallback(mockCallback_);
  EXPECT_EQ(ret, AUDIO_NULL_ERROR);
}

TEST_F(AudioCapturerAdapterImplTest, SetCapturerReadCallback_NullUserDataCallback) {
  adapter_->audio_capturer_ = reinterpret_cast<OH_AudioCapturer*>(2345);
  adapter_->callback_index_= adapter_->callback_wrapper_.AddCallback(nullptr);
  int32_t ret = adapter_->SetCapturerReadCallback(mockCallback_);
  EXPECT_EQ(ret, AUDIO_NULL_ERROR);
}

TEST_F(AudioCapturerAdapterImplTest, Enqueue_WhenAudioCapturerIsNull_ReturnsAudioNullError) {
  adapter_->audio_capturer_ = nullptr;
  int32_t result = adapter_->Enqueue(bufferDesc_);
  EXPECT_EQ(result, AUDIO_NULL_ERROR);
}

TEST_F(AudioCapturerAdapterImplTest, GetAudioSamplingRate_ValidInput_ReturnsCorrectType) {
  EXPECT_EQ(adapter_->GetAudioSamplingRate(AudioAdapterSamplingRate::SAMPLE_RATE_8000), 
           static_cast<int>(AudioAdapterSamplingRate::SAMPLE_RATE_8000));
}

TEST_F(AudioCapturerAdapterImplTest, GetAudioSamplingRate_InvalidInput_ReturnsDefault) {
  AudioAdapterSamplingRate  invalidType = static_cast<AudioAdapterSamplingRate>(-1);
  EXPECT_EQ(adapter_->GetAudioSamplingRate(invalidType), 
           static_cast<int>(AudioAdapterSamplingRate::SAMPLE_RATE_44100));
}

TEST_F(AudioCapturerAdapterImplTest, GetAudioEncodingType_ValidInput_ReturnsCorrectType) {
  EXPECT_EQ(adapter_->GetAudioEncodingType(AudioAdapterEncodingType::ENCODING_PCM), 
           OH_AudioStream_EncodingType::AUDIOSTREAM_ENCODING_TYPE_RAW);
}

TEST_F(AudioCapturerAdapterImplTest, GetAudioEncodingType_InvalidInput_ReturnsDefault) {
  AudioAdapterEncodingType  invalidType = static_cast<AudioAdapterEncodingType>(-1);
  EXPECT_EQ(adapter_->GetAudioEncodingType(invalidType), 
           OH_AudioStream_EncodingType::AUDIOSTREAM_ENCODING_TYPE_RAW);
}

TEST_F(AudioCapturerAdapterImplTest, GetAudioSampleFormat_ValidInput) {
  EXPECT_EQ(adapter_->GetAudioSampleFormat(AudioAdapterSampleFormat::SAMPLE_S16LE), 
           OH_AudioStream_SampleFormat::AUDIOSTREAM_SAMPLE_S16LE);
}

TEST_F(AudioCapturerAdapterImplTest, GetAudioSampleFormat_InvalidInput) {
  AudioAdapterSampleFormat invalidFormat = static_cast<AudioAdapterSampleFormat>(-1);
  EXPECT_EQ(adapter_->GetAudioSampleFormat(invalidFormat), 
           OH_AudioStream_SampleFormat::AUDIOSTREAM_SAMPLE_U8);
}

TEST_F(AudioCapturerAdapterImplTest, GetAudioSourceType_ValidInput_ReturnsCorrectType) {
  EXPECT_EQ(adapter_->GetAudioSourceType(AudioAdapterSourceType::SOURCE_TYPE_MIC), 
           OH_AudioStream_SourceType::AUDIOSTREAM_SOURCE_TYPE_MIC);
}

TEST_F(AudioCapturerAdapterImplTest, GetAudioSourceType_InvalidInput_ReturnsDefault) {
  AudioAdapterSourceType invalidType = static_cast<AudioAdapterSourceType>(999);
  EXPECT_EQ(adapter_->GetAudioSourceType(invalidType), 
           OH_AudioStream_SourceType::AUDIOSTREAM_SOURCE_TYPE_VOICE_RECOGNITION);
}

TEST_F(AudioCapturerAdapterImplTest, GetAudioChannel_ValidInputs) {
  EXPECT_EQ(adapter_->GetAudioChannel(AudioAdapterChannel::MONO), 
           static_cast<int>(AudioAdapterChannel::MONO));
}

TEST_F(AudioCapturerAdapterImplTest, GetAudioChannel_InvalidInput_ReturnsDefault) {
  constexpr AudioAdapterChannel INVALID_CHANNEL = static_cast<AudioAdapterChannel>(999);
  EXPECT_EQ(adapter_->GetAudioChannel(INVALID_CHANNEL), static_cast<int>(AudioAdapterChannel::STEREO));
}

TEST_F(AudioCapturerAdapterImplTest, StartStop) {
  adapter_->audio_capturer_ = reinterpret_cast<OH_AudioCapturer*>(0x2345);
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioCapturer_Start(adapter_->audio_capturer_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioCapturer_Stop(adapter_->audio_capturer_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioCapturer_Release(adapter_->audio_capturer_))
    .WillRepeatedly(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_TRUE(adapter_->Start());
  EXPECT_TRUE(adapter_->Stop());
  EXPECT_TRUE(adapter_->Release());
}

TEST_F(AudioCapturerAdapterImplTest, Enqueue_Success) {
  adapter_->audio_capturer_ = reinterpret_cast<OH_AudioCapturer*>(0x2345);
  int32_t result = adapter_->Enqueue(bufferDesc_);
  EXPECT_EQ(result, AUDIO_OK);
}

TEST_F(AudioCapturerAdapterImplTest, GetFrameCount_Success) {
  uint32_t frameCount = 0;
  adapter_->audio_capturer_ = reinterpret_cast<OH_AudioCapturer*>(0x2345);
  int32_t frameCountValue = 10;
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(),
      OH_AudioCapturer_GetFrameSizeInCallback(adapter_->audio_capturer_,_))
        .WillOnce(DoAll(SetArgPointee<1>(frameCountValue), Return(AUDIOSTREAM_SUCCESS)));
  EXPECT_EQ(adapter_->GetFrameCount(frameCount), AUDIO_OK);
  EXPECT_EQ(frameCount, 10U);
}

TEST_F(AudioCapturerAdapterImplTest, GetFrameCount_NotSuccess) {
  uint32_t frameCount = 0;
  adapter_->audio_capturer_ = reinterpret_cast<OH_AudioCapturer*>(0x2345);
  int32_t frameCountValue = 10;
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(),
      OH_AudioCapturer_GetFrameSizeInCallback(adapter_->audio_capturer_, _))
        .WillOnce(DoAll(SetArgPointee<1>(frameCountValue), Return(AUDIOSTREAM_ERROR_SYSTEM)));
  EXPECT_EQ(adapter_->GetFrameCount(frameCount), AUDIO_ERROR);
  EXPECT_EQ(frameCount, 0U);
}

TEST_F(AudioCapturerAdapterImplTest, GetAudioTime_ReturnsError2) {
  adapter_->audio_capturer_ = reinterpret_cast<OH_AudioCapturer*>(0x2345);

  int64_t timestamp = 10;
  EXPECT_CALL(
      MockAudioCommonEventSupport::GetInstance(), OH_AudioCapturer_GetTimestamp(adapter_->audio_capturer_, _, _, _))
      .WillOnce(DoAll(SetArgPointee<3>(timestamp), Return(AUDIOSTREAM_ERROR_SYSTEM)));
  EXPECT_EQ(adapter_->GetAudioTime(), AUDIO_ERROR);
}


TEST_F(AudioCapturerAdapterImplTest, GetAudioTime_Success) {
  adapter_->audio_capturer_ = reinterpret_cast<OH_AudioCapturer*>(0x2345);
  int64_t timestamp = 10;
  EXPECT_CALL(
      MockAudioCommonEventSupport::GetInstance(), OH_AudioCapturer_GetTimestamp(adapter_->audio_capturer_, _, _, _))
      .WillOnce(DoAll(SetArgPointee<3>(timestamp), Return(AUDIOSTREAM_SUCCESS)));
  EXPECT_EQ(adapter_->GetAudioTime(), timestamp);
}

TEST_F(AudioCapturerAdapterImplTest, Create_ReturnsErrorWhenOptionsNull) {
  int32_t result = adapter_->Create(nullptr, "");
  EXPECT_EQ(result, AUDIO_ERROR);
}

TEST_F(AudioCapturerAdapterImplTest, Create_ReturnsErrorWhenCreate) {
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Create(_,_))
    .WillOnce(Return(AUDIOSTREAM_ERROR_INVALID_PARAM));
  int32_t result = adapter_->Create(options_, "");
  EXPECT_EQ(result, AUDIO_NULL_ERROR);
}

TEST_F(AudioCapturerAdapterImplTest, Create_ReturnsErrorWhenSetSamplingRate) {
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Create(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetSamplingRate(_,_))
    .WillOnce(Return(AUDIOSTREAM_ERROR_INVALID_PARAM));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Destroy(_));
  int32_t result = adapter_->Create(options_, "");
  EXPECT_EQ(result, AUDIO_ERROR);
}

TEST_F(AudioCapturerAdapterImplTest, Create_ReturnsErrorWhenSetEncodingType) {
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Create(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetSamplingRate(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetEncodingType(_,_))
    .WillOnce(Return(AUDIOSTREAM_ERROR_INVALID_PARAM));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Destroy(_));
  int32_t result = adapter_->Create(options_, "");
  EXPECT_EQ(result, AUDIO_ERROR);
}

TEST_F(AudioCapturerAdapterImplTest, Create_ReturnsErrorWhenSetSampleFormat) {
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Create(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetSamplingRate(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetEncodingType(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetSampleFormat(_,_))
    .WillOnce(Return(AUDIOSTREAM_ERROR_INVALID_PARAM));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Destroy(_));
  int32_t result = adapter_->Create(options_, "");
  EXPECT_EQ(result, AUDIO_ERROR);
}

TEST_F(AudioCapturerAdapterImplTest, Create_ReturnsErrorWhenSetChannelCount) {
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Create(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetSamplingRate(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetEncodingType(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetSampleFormat(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetChannelCount(_,_))
    .WillOnce(Return(AUDIOSTREAM_ERROR_INVALID_PARAM));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Destroy(_));
  int32_t result = adapter_->Create(options_, "");
  EXPECT_EQ(result, AUDIO_ERROR);
}

TEST_F(AudioCapturerAdapterImplTest, Create_ReturnsErrorWhenSetCapturerInfo) {
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Create(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetSamplingRate(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetEncodingType(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetSampleFormat(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetChannelCount(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetCapturerInfo(_,_))
    .WillOnce(Return(AUDIOSTREAM_ERROR_INVALID_PARAM));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Destroy(_));
  int32_t result = adapter_->Create(options_, "");
  EXPECT_EQ(result, AUDIO_ERROR);
}

TEST_F(AudioCapturerAdapterImplTest, Create_ReturnsErrorWhenGenerateCapturer) {
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Create(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetSamplingRate(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetEncodingType(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetSampleFormat(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetChannelCount(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetCapturerInfo(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(*options_, GetCapturerFlags()).WillOnce(Return(1));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetLatencyMode(_,_))
    .WillOnce(Return(AUDIOSTREAM_ERROR_INVALID_PARAM));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_GenerateCapturer(_,_))
    .WillOnce(Return(AUDIOSTREAM_ERROR_INVALID_PARAM));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Destroy(_));
  int32_t result = adapter_->Create(options_, "");
  EXPECT_EQ(result, AUDIO_ERROR);
}

TEST_F(AudioCapturerAdapterImplTest, Create_Success) {
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Create(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetSamplingRate(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetEncodingType(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetSampleFormat(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetChannelCount(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetCapturerInfo(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(*options_, GetCapturerFlags()).WillOnce(Return(1));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetLatencyMode(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_GenerateCapturer(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Destroy(_));
  int32_t result = adapter_->Create(options_, "");
  EXPECT_EQ(result, AUDIO_OK);
}

TEST_F(AudioCapturerAdapterImplTest, Create_Success2) {
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Create(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetSamplingRate(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetEncodingType(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetSampleFormat(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetChannelCount(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_SetCapturerInfo(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(*options_, GetCapturerFlags()).WillOnce(Return(0));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_GenerateCapturer(_,_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioStreamBuilder_Destroy(_));
  int32_t result = adapter_->Create(options_, "");
  EXPECT_EQ(result, AUDIO_OK);
}

TEST_F(AudioCapturerAdapterImplTest, OnReadData_Capturer) {
  OH_AudioCapturer* invalidCapturer = reinterpret_cast<OH_AudioCapturer*>(0x5678);
  int result = adapter_->OnReadData(invalidCapturer, nullptr, nullptr, 0);
  EXPECT_EQ(result, -1);
}

TEST_F(AudioCapturerAdapterImplTest, OnReadData_UserData) {
  OH_AudioCapturer* invalidCapturer = reinterpret_cast<OH_AudioCapturer*>(0x5678);
  adapter_->audio_capturer_ = invalidCapturer;
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioCapturer_Start(adapter_->audio_capturer_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_TRUE(adapter_->Start());
  int result = adapter_->OnReadData(adapter_->audio_capturer_, nullptr, nullptr, 0);
  EXPECT_EQ(result, -1);
}

TEST_F(AudioCapturerAdapterImplTest, OnReadData_UserDataCallback) {
  OH_AudioCapturer* invalidCapturer = reinterpret_cast<OH_AudioCapturer*>(0x5678);
  void* user_data = reinterpret_cast<void*>(0x1234);
  adapter_->audio_capturer_ = invalidCapturer;
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioCapturer_Start(adapter_->audio_capturer_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_TRUE(adapter_->Start());
  int result = adapter_->OnReadData(adapter_->audio_capturer_, user_data, nullptr, 0);
  EXPECT_EQ(result, -1);
}

TEST_F(AudioCapturerAdapterImplTest, OnReadData_UserDataCallback_Callback) {
  OH_AudioCapturer* invalidCapturer = reinterpret_cast<OH_AudioCapturer*>(0x5678);

  adapter_->audio_capturer_ = invalidCapturer;
   auto userCallBack = std::make_shared<UserDataCallBack>();
  const size_t testLength = 256;
  uint8_t testBuffer[testLength]{};
  userCallBack->buffer = testBuffer;
  userCallBack->length = testLength;
  userCallBack->callback = nullptr;
  adapter_->callback_index_= adapter_->callback_wrapper_.AddCallback(userCallBack);  
  void* user_data = reinterpret_cast<void*>(adapter_->callback_index_);

  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioCapturer_Start(adapter_->audio_capturer_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_TRUE(adapter_->Start());
  int result = adapter_->OnReadData(adapter_->audio_capturer_, user_data, testBuffer, 0);
  EXPECT_EQ(result, -1);
}

TEST_F(AudioCapturerAdapterImplTest, OnReadData_Success) {
  OH_AudioCapturer* invalidCapturer = reinterpret_cast<OH_AudioCapturer*>(0x5678);

  adapter_->audio_capturer_ = invalidCapturer;
   auto userCallBack = std::make_shared<UserDataCallBack>();
  const size_t testLength = 256;
  uint8_t testBuffer[testLength]{};
  userCallBack->buffer = testBuffer;
  userCallBack->length = testLength;
  userCallBack->callback = mockCallback_;
  adapter_->callback_index_= adapter_->callback_wrapper_.AddCallback(userCallBack);  
  void* user_data = reinterpret_cast<void*>(adapter_->callback_index_);

  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioCapturer_Start(adapter_->audio_capturer_))
    .WillOnce(Return(AUDIOSTREAM_SUCCESS));
  EXPECT_CALL(*mockCallback_, OnReadData(_));
  EXPECT_TRUE(adapter_->Start());
  int result = adapter_->OnReadData(adapter_->audio_capturer_, user_data, testBuffer, testLength);
  EXPECT_EQ(result, 0);
}

TEST_F(AudioCapturerAdapterImplTest, Start_Insert) {
  adapter_->audio_capturer_ = reinterpret_cast<OH_AudioCapturer*>(0x2345);
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioCapturer_Start(adapter_->audio_capturer_))
    .WillOnce(Return(AUDIOSTREAM_ERROR_SYSTEM));
  EXPECT_CALL(MockAudioCommonEventSupport::GetInstance(), OH_AudioCapturer_Release(adapter_->audio_capturer_))
    .WillOnce(Return(AUDIOSTREAM_ERROR_SYSTEM));

  EXPECT_FALSE(adapter_->Start());
  EXPECT_FALSE(adapter_->Release());
}