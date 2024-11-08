/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "audio_capturer_options_adapter_impl.h"
#include "gtest/gtest.h"

using namespace OHOS::NWeb;

class AudioCapturerOptionsAdapterImplTest : public testing::Test {
 protected:
  AudioCapturerOptionsAdapterImpl* audioCapturerOptionsAdapterImpl;

  virtual void SetUp() {
    audioCapturerOptionsAdapterImpl = new AudioCapturerOptionsAdapterImpl();
  }

  virtual void TearDown() { delete audioCapturerOptionsAdapterImpl; }
};

// Scenario1: Test GetSamplingRate method
TEST_F(AudioCapturerOptionsAdapterImplTest,
       GetSamplingRate_ShouldReturnCorrectSamplingRate) {
  AudioAdapterSamplingRate expectedSamplingRate =
      AudioAdapterSamplingRate::SAMPLE_RATE_48000;
  audioCapturerOptionsAdapterImpl->SetSamplingRate(expectedSamplingRate);
  AudioAdapterSamplingRate actualSamplingRate =
      audioCapturerOptionsAdapterImpl->GetSamplingRate();
  ASSERT_EQ(expectedSamplingRate, actualSamplingRate);
}

// Scenario1: Test GetEncoding method
TEST_F(AudioCapturerOptionsAdapterImplTest,
       GetEncoding_ShouldReturnCorrectEncoding) {
  AudioAdapterEncodingType expectedEncoding =
      AudioAdapterEncodingType::ENCODING_PCM;
  audioCapturerOptionsAdapterImpl->SetEncoding(expectedEncoding);
  AudioAdapterEncodingType actualEncoding =
      audioCapturerOptionsAdapterImpl->GetEncoding();
  EXPECT_EQ(expectedEncoding, actualEncoding);
}

// Scenario1: Test when GetSampleFormat is called then it returns the correct
// sample format.
TEST_F(AudioCapturerOptionsAdapterImplTest,
       GetSampleFormat_ShouldReturnCorrectSampleFormat) {
  AudioAdapterSampleFormat expectedFormat =
      AudioAdapterSampleFormat::SAMPLE_S16LE;
  audioCapturerOptionsAdapterImpl->SetSampleFormat(expectedFormat);
  AudioAdapterSampleFormat actualFormat =
      audioCapturerOptionsAdapterImpl->GetSampleFormat();
  EXPECT_EQ(actualFormat, expectedFormat);
}

// Scenario1: Test when GetChannels is called then it returns the correct value.
TEST_F(AudioCapturerOptionsAdapterImplTest,
       GetChannels_ShouldReturnCorrectValue) {
  AudioAdapterChannel expectedChannels = AudioAdapterChannel::MONO;
  audioCapturerOptionsAdapterImpl->SetChannels(expectedChannels);
  AudioAdapterChannel actualChannels =
      audioCapturerOptionsAdapterImpl->GetChannels();
  ASSERT_EQ(expectedChannels, actualChannels);
}

// Scenario1: Test GetCapturerFlags returns the correct capturer_flags_ value.
TEST_F(AudioCapturerOptionsAdapterImplTest,
       GetSourceType_ShouldReturnCorrectSourceType) {
  AudioAdapterSourceType expectedType = AudioAdapterSourceType::SOURCE_TYPE_MIC;
  audioCapturerOptionsAdapterImpl->SetSourceType(expectedType);
  AudioAdapterSourceType actualType =
      audioCapturerOptionsAdapterImpl->GetSourceType();
  EXPECT_EQ(expectedType, actualType);
}

// Scenario1: Test GetCapturerFlags returns the correct capturer_flags_ value.
TEST_F(AudioCapturerOptionsAdapterImplTest,
       GetCapturerFlags_ShouldReturnCorrectValue) {
  int32_t expectedFlags = 1;  // Assuming capturer_flags_ is initialized to 1
  audioCapturerOptionsAdapterImpl->SetCapturerFlags(expectedFlags);
  int32_t actualFlags = audioCapturerOptionsAdapterImpl->GetCapturerFlags();
  EXPECT_EQ(expectedFlags, actualFlags);
}

// Scenario1: Test SetSamplingRate with SAMPLING_RATE_44100
TEST_F(AudioCapturerOptionsAdapterImplTest, SetSamplingRate_44100) {
  audioCapturerOptionsAdapterImpl->SetSamplingRate(
      AudioAdapterSamplingRate::SAMPLE_RATE_44100);
  EXPECT_EQ(audioCapturerOptionsAdapterImpl->GetSamplingRate(),
            AudioAdapterSamplingRate::SAMPLE_RATE_44100);
}

// Scenario1: Test SetEncoding with AudioAdapterEncodingType::PCM.
TEST_F(AudioCapturerOptionsAdapterImplTest,
       SetEncoding_ShouldSetEncoding_WhenTypeIsPCM) {
  AudioAdapterEncodingType type = AudioAdapterEncodingType::ENCODING_PCM;
  audioCapturerOptionsAdapterImpl->SetEncoding(type);
  EXPECT_EQ(audioCapturerOptionsAdapterImpl->GetEncoding(), type);
}

// Scenario1: Test SetSampleFormat with AudioAdapterSampleFormat::SAMPLE_INVALID
TEST_F(AudioCapturerOptionsAdapterImplTest,
       SetSampleFormat_ShouldSetSampleFormat_WhenFormatIsInvalid) {
  audioCapturerOptionsAdapterImpl->SetSampleFormat(
      AudioAdapterSampleFormat::INVALID_WIDTH);
  EXPECT_EQ(audioCapturerOptionsAdapterImpl->GetSampleFormat(),
            AudioAdapterSampleFormat::INVALID_WIDTH);
}

// Scenario1: Test SetChannels with AudioAdapterChannel::MONO
TEST_F(AudioCapturerOptionsAdapterImplTest, SetChannels_ShouldSetMonoWithMono) {
  audioCapturerOptionsAdapterImpl->SetChannels(AudioAdapterChannel::MONO);
  EXPECT_EQ(audioCapturerOptionsAdapterImpl->GetChannels(),
            AudioAdapterChannel::MONO);
}

// Scenario1: Test when SetSourceType is called with
TEST_F(AudioCapturerOptionsAdapterImplTest,
       SetSourceType_ShouldSetCorrectlyWithCOMMUNICATION) {
  audioCapturerOptionsAdapterImpl->SetSourceType(
      AudioAdapterSourceType::SOURCE_TYPE_VOICE_COMMUNICATION);
  ASSERT_EQ(audioCapturerOptionsAdapterImpl->GetSourceType(),
            AudioAdapterSourceType::SOURCE_TYPE_VOICE_COMMUNICATION);
}

// Scenario1: Test SetCapturerFlags method
TEST_F(AudioCapturerOptionsAdapterImplTest, SetCapturerFlags_ShouldSetFlags) {
  int32_t flags = 1;
  audioCapturerOptionsAdapterImpl->SetCapturerFlags(flags);
  EXPECT_EQ(audioCapturerOptionsAdapterImpl->GetCapturerFlags(), flags);
}
