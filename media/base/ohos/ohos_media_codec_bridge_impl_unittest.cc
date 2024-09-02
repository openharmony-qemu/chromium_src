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
#include "ohos_media_codec_bridge_impl.h"
#undef private
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

std::string test = "create/mime";
std::string& refToMyString = test;

namespace media {
using namespace OHOS::NWeb;

class MockMediaCodecAdapter : public MediaCodecAdapter {
 public:
  MOCK_METHOD(CodecCodeAdapter,
              CreateVideoCodecByMime,
              (const std::string),
              (override));
  MOCK_METHOD(CodecCodeAdapter,
              CreateVideoCodecByName,
              (const std::string),
              (override));
  MOCK_METHOD(CodecCodeAdapter,
              Configure,
              (const std::shared_ptr<CodecConfigParaAdapter>),
              (override));
  MOCK_METHOD(CodecCodeAdapter, Prepare, (), (override));
  MOCK_METHOD(CodecCodeAdapter, Start, (), (override));
  MOCK_METHOD(CodecCodeAdapter, Stop, (), (override));
  MOCK_METHOD(CodecCodeAdapter, RequestKeyFrameSoon, (), (override));
  MOCK_METHOD(CodecCodeAdapter, Reset, (), (override));
  MOCK_METHOD(CodecCodeAdapter, Release, (), (override));
  MOCK_METHOD(CodecCodeAdapter,
              ReleaseOutputBuffer,
              (uint32_t, bool),
              (override));
  MOCK_METHOD(CodecCodeAdapter,
              SetCodecCallback,
              (const std::shared_ptr<CodecCallbackAdapter>),
              (override));
  MOCK_METHOD(std::shared_ptr<ProducerSurfaceAdapter>,
              CreateInputSurface,
              (),
              (override));
};

class MockBufferInfoAdapter : public BufferInfoAdapter {
 public:
  MOCK_METHOD(int64_t, GetPresentationTimeUs, (), (override));
  MOCK_METHOD(int32_t, GetSize, (), (override));
  MOCK_METHOD(int32_t, GetOffset, (), (override));
};

class MockOhosBufferAdapter : public OhosBufferAdapter {
 public:
  MOCK_METHOD(uint8_t*, GetAddr, (), (override));
  MOCK_METHOD(uint32_t, GetBufferSize, (), (override));
};

class OHOSMediaCodecBridgeImplTest : public ::testing::Test {
 public:
  MockMediaCodecAdapter mock_codec_adapter;
  OHOSMediaCodecBridgeImpl bridge_impl =
      OHOSMediaCodecBridgeImpl(refToMyString);
};

class CodecEncodeBridgeCallbackTest : public ::testing::Test {
 protected:
  void SetUp() override {
    signal_ = std::make_shared<CodecBridgeSignal>();
    callback_ = std::make_shared<CodecEncodeBridgeCallback>(signal_);
  }

  std::shared_ptr<CodecBridgeSignal> signal_;
  std::shared_ptr<CodecEncodeBridgeCallback> callback_;
  BufferInfo buffer_info_;
  OhosBuffer buffer_data_;
  EncodeOutputBuffer output_buffer_;
};

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoEncoder) {
  std::unique_ptr<OHOSMediaCodecBridge> mediaCodecBridge =
      bridge_impl.CreateVideoEncoder(refToMyString);
  EXPECT_NE(mediaCodecBridge, nullptr);
}

TEST_F(OHOSMediaCodecBridgeImplTest, OHOSMediaCodecBridgeImpl) {
  OHOSMediaCodecBridgeImpl kExpected = OHOSMediaCodecBridgeImpl(refToMyString);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByMime) {
  const std::string kExpectedMimetype = "invalid/mime";
  CodecCodeAdapter result =
      bridge_impl.CreateVideoCodecByMime(kExpectedMimetype);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByName) {
  const std::string kExpectedName = "invalid/name";
  CodecCodeAdapter result = bridge_impl.CreateVideoCodecByName(kExpectedName);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, UpdateStatusAndClearCache) {
  const bool kExpected = true;
  bridge_impl.UpdateStatusAndClearCache(kExpected);
}

TEST_F(OHOSMediaCodecBridgeImplTest, Configure) {
  CodecConfigPara kExpectedConfigure = {1280, 720, 1000000, 30.0};
  scoped_refptr<base::SequencedTaskRunner> codec_task_runner =
      bridge_impl.codec_task_runner_;
  CodecCodeAdapter result =
      bridge_impl.Configure(kExpectedConfigure, codec_task_runner);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestPrepare) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.Prepare();
  ASSERT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestStart) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.Start();
  ASSERT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestStop) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.Stop();
  ASSERT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestReset) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.Reset();
  ASSERT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestRelease) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.Release();
  ASSERT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestCreateInputSurface) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.CreateInputSurface();
  ASSERT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(CodecEncodeBridgeCallbackTest, OnStreamChangedTest) {
  std::shared_ptr<CodecFormatAdapter> format;
  callback_->OnStreamChanged(format);
}

TEST_F(CodecEncodeBridgeCallbackTest, ClearBufferInfoCacheTest) {
  buffer_info_.presentationTimeUs = 1000;
  buffer_info_.size = 2048;
  buffer_info_.offset = 128;

  callback_->ClearBufferInfoCache(buffer_info_);

  EXPECT_EQ(buffer_info_.presentationTimeUs, 0);
  EXPECT_EQ(buffer_info_.size, 0);
  EXPECT_EQ(buffer_info_.offset, 0);
}

TEST_F(CodecEncodeBridgeCallbackTest, ClearBufferDataCacheTest) {
  buffer_data_.addr = reinterpret_cast<uint8_t*>(0x12345678);
  buffer_data_.bufferSize = 4096;

  callback_->ClearBufferDataCache(buffer_data_);

  EXPECT_EQ(buffer_data_.addr, nullptr);
  EXPECT_EQ(buffer_data_.bufferSize, 0);
}

TEST_F(CodecEncodeBridgeCallbackTest, ClearConfigCacheTest) {
  callback_->config_data_.buffer_info.presentationTimeUs = 1000;
  callback_->config_data_.buffer_info.size = 2048;
  callback_->config_data_.buffer_info.offset = 128;
  callback_->config_data_.buffer_data.addr =
      reinterpret_cast<uint8_t*>(0x12345678);
  callback_->config_data_.buffer_data.bufferSize = 4096;

  callback_->ClearConfigCache();

  EXPECT_EQ(callback_->config_data_.buffer_info.presentationTimeUs, 0);
  EXPECT_EQ(callback_->config_data_.buffer_info.size, 0);
  EXPECT_EQ(callback_->config_data_.buffer_info.offset, 0);
  EXPECT_EQ(callback_->config_data_.buffer_data.addr, nullptr);
  EXPECT_EQ(callback_->config_data_.buffer_data.bufferSize, 0);
}

TEST_F(CodecEncodeBridgeCallbackTest, InitEncodeOutputBufferTest) {
  callback_->InitEncodeOuputBuffer(output_buffer_);

  EXPECT_FALSE(output_buffer_.is_contain_config_data);

  EXPECT_EQ(output_buffer_.config_data.buffer_info.presentationTimeUs, 0);
  EXPECT_EQ(output_buffer_.config_data.buffer_info.size, 0);
  EXPECT_EQ(output_buffer_.config_data.buffer_info.offset, 0);
  EXPECT_EQ(output_buffer_.config_data.buffer_data.addr, nullptr);
  EXPECT_EQ(output_buffer_.config_data.buffer_data.bufferSize, 0);

  EXPECT_EQ(output_buffer_.buffer_info.presentationTimeUs, 0);
  EXPECT_EQ(output_buffer_.buffer_info.size, 0);
  EXPECT_EQ(output_buffer_.buffer_info.offset, 0);

  EXPECT_EQ(output_buffer_.buffer_data.addr, nullptr);
  EXPECT_EQ(output_buffer_.buffer_data.bufferSize, 0);

  EXPECT_EQ(output_buffer_.flag, BufferFlag::CODEC_BUFFER_FLAG_NONE);
}
}  // namespace media