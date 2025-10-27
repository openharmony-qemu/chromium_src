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

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/audio/delay_utils.h"
#include "base/logging.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace blink {

class DelayUtilsTest : public testing::Test {
 public:
  void SetUp() override {
    delay_utils_ = std::make_unique<DelayUtils>(nullptr);
  }

  void TearDown() override {
    delay_utils_.reset();
  }

 protected:
  std::unique_ptr<DelayUtils> delay_utils_;
};

TEST_F(DelayUtilsTest, MemcpyMediaAudio1) {
  float sample[10] = {0};
  float read_pointer[10] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
  float buffer[10] = {10.0f, 20.0f, 30.0f};
  uint32_t remainder = 8;
  uint32_t frames_to_process = 5;
  int read_index = 2;
  bool result = delay_utils_->MemcpyMediaAudio(sample, remainder, read_pointer, 
      frames_to_process, read_index, buffer);
  EXPECT_FALSE(result);
}

TEST_F(DelayUtilsTest, MemcpyMediaAudio2) {
  float sample[10] = {0};
  float read_pointer[10] = {1.0f, 2.0f, 3.0f};
  float buffer[10] = {10.0f, 20.0f, 30.0f};
  uint32_t remainder = 3;
  uint32_t frames_to_process = 5;
  int read_index = 2;
  bool result = delay_utils_->MemcpyMediaAudio(sample, remainder, read_pointer, 
      frames_to_process, read_index, buffer);
  EXPECT_FALSE(result);
}

TEST_F(DelayUtilsTest, MemcpyMediaAudioTwo1) {
  float sample_one[10] = {0};
  float sample_two[10] = {0};
  float read_pointer[10] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
  float buffer[10] = {10.0f, 20.0f, 30.0f};
  uint32_t remainder = 8;
  uint32_t frames_to_process = 4;
  int read_index = 2;
  bool result = delay_utils_->MemcpyMediaAudioTwo(sample_one, sample_two, remainder, 
      read_pointer, frames_to_process, read_index, buffer);
  EXPECT_FALSE(result);
}

TEST_F(DelayUtilsTest, MemcpyMediaAudioTwo2) {
  float sample_one[10] = {0};
  float sample_two[10] = {0};
  float read_pointer[10] = {1.0f, 2.0f, 3.0f};
  float buffer[10] = {10.0f, 20.0f, 30.0f};
  uint32_t remainder = 2;
  uint32_t frames_to_process = 4;
  int read_index = 2;
  bool result = delay_utils_->MemcpyMediaAudioTwo(sample_one, sample_two, remainder, 
      read_pointer, frames_to_process, read_index, buffer);
  EXPECT_FALSE(result);
}

TEST_F(DelayUtilsTest, MemcpyMediaAudioUtils) {
  float write_pointer[10] = {0};
  float source[10] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
  float buffer[10] = {0};
  int remainder = 3;
  int write_index = 2;
  uint32_t frames_to_process = 4;
  bool result = MemcpyMediaAudioUtils(write_pointer, remainder, source, 
      write_index, frames_to_process, buffer);
  EXPECT_FALSE(result);
}

TEST_F(DelayUtilsTest, MemcpyMediaAudioUtilsFailed1) {
  float write_pointer[2] = {0};
  float source[10] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
  float buffer[1] = {0};
  int remainder = 1;
  int write_index = 1;
  uint32_t frames_to_process = 10;

  bool result = MemcpyMediaAudioUtils(write_pointer, remainder, source, 
      write_index, frames_to_process, buffer);
  EXPECT_TRUE(result);
}

TEST_F(DelayUtilsTest, MemcpyMediaAudioUtilsFailed2) {
  float write_pointer[5] = {0};
  float source[5] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
  float buffer[5] = {0};
  int remainder = 0;
  int write_index = 5;
  uint32_t frames_to_process = 2;
  bool result = MemcpyMediaAudioUtils(write_pointer, remainder, source, 
      write_index, frames_to_process, buffer);
  EXPECT_TRUE(result);
}

TEST_F(DelayUtilsTest, MemcpyMediaAudioFailed1) {
  float sample[5] = {0};
  float read_pointer[5] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
  float buffer[5] = {0};
  uint32_t remainder = 0;
  uint32_t frames_to_process = 2;
  int read_index = 5;
  bool result = delay_utils_->MemcpyMediaAudio(sample, remainder, read_pointer, 
      frames_to_process, read_index, buffer);
  EXPECT_TRUE(result);
}

TEST_F(DelayUtilsTest, MemcpyMediaAudioFailed2) {
  float sample[10] = {0}; 
  float read_pointer[10] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
  float* invalid_buffer = nullptr; 
  uint32_t remainder = 2;
  uint32_t frames_to_process = 5;
  int read_index = 3;
  bool result = delay_utils_->MemcpyMediaAudio(sample, remainder, read_pointer, 
      frames_to_process, read_index, invalid_buffer);
  EXPECT_TRUE(result);
}

TEST_F(DelayUtilsTest, MemcpyMediaAudioTwoFailed1) {
  float sample_one[5] = {0};
  float sample_two[5] = {0};
  float read_pointer[5] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
  float buffer[5] = {0};
  uint32_t remainder = 0;
  uint32_t frames_to_process = 2;
  int read_index = 5;
  bool result = delay_utils_->MemcpyMediaAudioTwo(sample_one, sample_two, remainder, 
      read_pointer, frames_to_process, read_index, buffer);
  EXPECT_TRUE(result);
}

TEST_F(DelayUtilsTest, MemcpyMediaAudioTwoFailed2) {
  float sample_one[10] = {0};
  float sample_two[10] = {0};
  float read_pointer[10] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
  float* invalid_buffer = nullptr; 

  uint32_t remainder = 2;
  uint32_t frames_to_process = 5;
  int read_index = 3;
  bool result = delay_utils_->MemcpyMediaAudioTwo(sample_one, sample_two, remainder, 
      read_pointer, frames_to_process, read_index, invalid_buffer);
  EXPECT_TRUE(result);
}

} // namespace blink