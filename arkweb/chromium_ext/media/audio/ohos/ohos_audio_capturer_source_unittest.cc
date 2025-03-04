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

#define private public
#include "media/audio/ohos/ohos_audio_capturer_source.h"
#undef private
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using namespace media;

class MockReadDataCallback {
 public:
  MOCK_METHOD(void, Run, (), ());
};

class AudioCapturerReadCallbackTest : public ::testing::Test {
 public:
  AudioCapturerReadCallbackTest()
      : mock_callback(),
        read_callback(base::BindRepeating(&MockReadDataCallback::Run,
                                          base::Unretained(&mock_callback))) {}

 protected:
  MockReadDataCallback mock_callback;
  AudioCapturerReadCallback read_callback;
};

TEST_F(AudioCapturerReadCallbackTest, OnReadDataTest) {
  size_t length = 10;
  bool result = read_callback.readDataCallback_.is_null();
  EXPECT_CALL(mock_callback, Run()).Times(1);
  read_callback.OnReadData(length);
  EXPECT_FALSE(result);
}

TEST_F(AudioCapturerReadCallbackTest, OnReadDataNoCallbackTest) {
  size_t length = 10;
  read_callback.readDataCallback_ = base::RepeatingCallback<void()>();
  bool result = read_callback.readDataCallback_.is_null();
  EXPECT_CALL(mock_callback, Run()).Times(0);
  read_callback = AudioCapturerReadCallback(base::RepeatingCallback<void()>());
  read_callback.OnReadData(length);
  EXPECT_TRUE(result);
}
 