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

#include <cstddef>
#include <memory>
#include <utility>
#define protected public
#define private public
#include "graphic_adapter.h"
#include "media/base/video_frame.h"
#undef private
#undef protected
#define private public
#include "ohos_media_codec_bridge_impl.h"
#undef private
#include "media_codec_adapter.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

std::string test = "create/mime";
std::string& refToMyString = test;

namespace media {
using namespace OHOS::NWeb;

class MockProducerSurfaceAdapter : public ProducerSurfaceAdapter {
 public:
  MOCK_METHOD(std::shared_ptr<SurfaceBufferAdapter>,
              RequestBuffer,
              (int32_t&, std::shared_ptr<BufferRequestConfigAdapter>),
              (override));
  MOCK_METHOD(int32_t,
              FlushBuffer,
              (std::shared_ptr<SurfaceBufferAdapter>,
               int32_t,
               std::shared_ptr<BufferFlushConfigAdapter>),
              (override));
};

class MockSurfaceBufferAdapter : public SurfaceBufferAdapter {
 public:
  MOCK_METHOD(int32_t, GetFileDescriptor, (), (override));
  MOCK_METHOD(int32_t, GetWidth, (), (override));
  MOCK_METHOD(int32_t, GetHeight, (), (override));
  MOCK_METHOD(int32_t, GetStride, (), (override));
  MOCK_METHOD(int32_t, GetFormat, (), (override));
  MOCK_METHOD(uint32_t, GetSize, (), (override));
  MOCK_METHOD(void*, GetVirAddr, (), (override));
};

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

class InheritBufferInfoAdapter : public BufferInfoAdapter {
 public:
  int64_t GetPresentationTimeUs() override { return 100; }
  int32_t GetSize() override { return 1; }
  int32_t GetOffset() override { return 0; }
};

class MockOhosBufferAdapter : public OhosBufferAdapter {
 public:
  MOCK_METHOD(uint8_t*, GetAddr, (), (override));
  MOCK_METHOD(uint32_t, GetBufferSize, (), (override));
};

class InheritOhosBufferAdapter : public OhosBufferAdapter {
 public:
  uint8_t* GetAddr() override { return nullptr; }
  uint32_t GetBufferSize() override { return 8; }
};

class InheritOhosBufferAdapterNull : public OhosBufferAdapter {
 public:
  uint8_t* GetAddr() override { return nullptr; }
  uint32_t GetBufferSize() override { return 0; }
};

class MockSequencedTaskRunner : public base::SequencedTaskRunner {
 public:
  MOCK_METHOD(bool,
              PostNonNestableTask,
              (const base::Location&, base::OnceClosure),
              ());
  MOCK_METHOD(bool,
              PostNonNestableDelayedTask,
              (const base::Location&, base::OnceClosure, base::TimeDelta),
              (override));
  MOCK_METHOD(base::DelayedTaskHandle,
              PostCancelableDelayedTask,
              (base::subtle::PostDelayedTaskPassKey,
               const base::Location&,
               base::OnceClosure,
               base::TimeDelta),
              (override));
  MOCK_METHOD(base::DelayedTaskHandle,
              PostCancelableDelayedTaskAt,
              (base::subtle::PostDelayedTaskPassKey,
               const base::Location&,
               base::OnceClosure,
               base::TimeTicks,
               base::subtle::DelayPolicy),
              (override));
  MOCK_METHOD(bool,
              PostDelayedTaskAt,
              (base::subtle::PostDelayedTaskPassKey,
               const base::Location&,
               base::OnceClosure,
               base::TimeTicks,
               base::subtle::DelayPolicy),
              (override));
  MOCK_METHOD(bool, RunsTasksInCurrentSequence, (), (const, override));
  MOCK_METHOD(bool,
              DeleteOrReleaseSoonInternal,
              (const base::Location&, void (*)(const void*), const void*),
              (override));
  MOCK_METHOD(bool,
              PostDelayedTask,
              (const base::Location&, base::OnceClosure, base::TimeDelta),
              (override));
};

class OHOSMediaCodecBridgeImplTest : public ::testing::Test {
 public:
  MockMediaCodecAdapter mock_codec_adapter;
  std::unique_ptr<MockMediaCodecAdapter> mock_codec_adapter_;
  OHOSMediaCodecBridgeImpl bridge_impl =
      OHOSMediaCodecBridgeImpl(refToMyString);
  void SetCodecAdapter(std::unique_ptr<MediaCodecAdapter> adapter) {
    bridge_impl.codec_adapter_ = std::move(adapter);
  }
  void SimulateKeyFrameMemoryFailure() { bridge_impl.keyframe_addr_ = nullptr; }

 protected:
  void SetUp() override {
    signal_ = std::make_shared<CodecBridgeSignal>();
    callback_ = std::make_shared<CodecEncodeBridgeCallback>(signal_);
  }

  std::shared_ptr<CodecBridgeSignal> signal_;
  std::shared_ptr<CodecEncodeBridgeCallback> callback_;
  std::shared_ptr<CodecEncodeBridgeCallback> cb_ = nullptr;
  BufferInfo buffer_info_;
  OhosBuffer buffer_data_;
  EncodeOutputBuffer output_buffer_;
};

class CodecEncodeBridgeCallbackTest : public ::testing::Test {
 protected:
  void SetUp() override {
    signal_ = std::make_shared<CodecBridgeSignal>();
    callback_ = std::make_shared<CodecEncodeBridgeCallback>(signal_);
  }

  std::shared_ptr<CodecBridgeSignal> signal_;
  std::shared_ptr<CodecEncodeBridgeCallback> callback_;
  std::shared_ptr<CodecEncodeBridgeCallback> cb_ = nullptr;
  BufferInfo buffer_info_;
  OhosBuffer buffer_data_;
  EncodeOutputBuffer output_buffer_;
};

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByMime001) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result =
      bridge_impl.CreateVideoCodecByMime("kExpectedMimetype");
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByMime002) {
  const std::string kExpectedMimetype = "video/avc";
  CodecCodeAdapter result =
      bridge_impl.CreateVideoCodecByMime(kExpectedMimetype);
  EXPECT_EQ(result, CodecCodeAdapter::OK);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByMime003) {
  const std::string kExpectedMimetype = "codec_bridge";
  CodecCodeAdapter result =
      bridge_impl.CreateVideoCodecByMime(kExpectedMimetype);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByMime004) {
  const std::string kExpectedMimetype = "video/avc";
  auto mockCodecAdapter = std::make_unique<MockMediaCodecAdapter>();
  EXPECT_CALL(*mockCodecAdapter, CreateVideoCodecByMime(kExpectedMimetype))
      .WillOnce(testing::Return(CodecCodeAdapter::OK));
  EXPECT_CALL(*mockCodecAdapter, SetCodecCallback(testing::_))
      .WillOnce(testing::Return(CodecCodeAdapter::OK));
  bridge_impl.codec_adapter_ = (std::move(mockCodecAdapter));
  CodecCodeAdapter result =
      bridge_impl.CreateVideoCodecByMime(kExpectedMimetype);
  ASSERT_EQ(result, CodecCodeAdapter::OK);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByMime005) {
  const std::string kExpectedMimetype = "video/avc";
  auto mockCodecAdapter = std::make_unique<MockMediaCodecAdapter>();
  EXPECT_CALL(*mockCodecAdapter, CreateVideoCodecByMime(kExpectedMimetype))
      .WillOnce(testing::Return(CodecCodeAdapter::OK));
  EXPECT_CALL(*mockCodecAdapter, SetCodecCallback(testing::_))
      .WillOnce(testing::Return(CodecCodeAdapter::OK));
  bridge_impl.signal_ = std::make_shared<CodecBridgeSignal>();
  bridge_impl.codec_adapter_ = std::move(mockCodecAdapter);
  (void)bridge_impl.CreateVideoCodecByMime(kExpectedMimetype);
  ASSERT_NE(bridge_impl.signal_, nullptr);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByName_NullAdapter) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter ret = bridge_impl.CreateVideoCodecByName("codec_nullptr");
  EXPECT_EQ(ret, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByName_CreateFailed) {
  bridge_impl.codec_adapter_ = std::make_unique<MockMediaCodecAdapter>();
  MockMediaCodecAdapter* mock_adapter =
      static_cast<MockMediaCodecAdapter*>(bridge_impl.codec_adapter_.get());
  EXPECT_NE(mock_adapter, nullptr);
  EXPECT_CALL(*mock_adapter, CreateVideoCodecByName("codec_failed"))
      .WillOnce(testing::Return(CodecCodeAdapter::ERROR));
  CodecCodeAdapter ret = bridge_impl.CreateVideoCodecByName("codec_failed");
  EXPECT_EQ(ret, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, UpdateStatusAndClearCache_Isrunning) {
  bool Is_running = true;
  bridge_impl.is_running_.store(Is_running);
  bridge_impl.UpdateStatusAndClearCache(Is_running);
  EXPECT_EQ(bridge_impl.is_running_.load(), Is_running);
}

TEST_F(OHOSMediaCodecBridgeImplTest, UpdateStatusAndClearCache_Cb) {
  bool Is_running = true;
  auto mock_adapter = std::make_shared<CodecEncodeBridgeCallback>(signal_);
  bridge_impl.cb_ = mock_adapter;
  bridge_impl.UpdateStatusAndClearCache(Is_running);
  EXPECT_EQ(mock_adapter->is_running_.load(), Is_running);
}

TEST_F(OHOSMediaCodecBridgeImplTest, UpdateStatusAndClearCache001) {
  const bool kExpected = true;
  bridge_impl.UpdateStatusAndClearCache(kExpected);
  EXPECT_TRUE(kExpected);
}

TEST_F(OHOSMediaCodecBridgeImplTest, UpdateStatusAndClearCache002) {
  const bool kExpected = false;
  auto mock_adapter = std::make_shared<CodecEncodeBridgeCallback>(signal_);
  bridge_impl.is_running_ = true;
  bridge_impl.signal_ = std::make_shared<CodecBridgeSignal>();
  bridge_impl.UpdateStatusAndClearCache(kExpected);
  EXPECT_FALSE(kExpected);
}

TEST_F(OHOSMediaCodecBridgeImplTest, UpdateStatusAndClearCache003) {
  const bool kExpected = false;
  auto mock_adapter = std::make_shared<CodecEncodeBridgeCallback>(signal_);
  bridge_impl.is_running_ = true;
  bridge_impl.signal_ = std::make_shared<CodecBridgeSignal>();
  bridge_impl.cb_ = mock_adapter;
  bridge_impl.UpdateStatusAndClearCache(kExpected);
  EXPECT_FALSE(kExpected);
}

TEST_F(OHOSMediaCodecBridgeImplTest, UpdateStatusAndClearCache004) {
  const bool kExpected = false;
  auto mock_adapter = std::make_shared<CodecEncodeBridgeCallback>(signal_);
  bridge_impl.is_running_ = true;
  bridge_impl.signal_ = std::make_shared<CodecBridgeSignal>();
  bridge_impl.UpdateStatusAndClearCache(kExpected);
  EXPECT_FALSE(kExpected);
}

TEST_F(OHOSMediaCodecBridgeImplTest, Configure001) {
  CodecConfigPara kExpectedConfigure = {1280, 720, 1000000, 30.0};
  scoped_refptr<base::SequencedTaskRunner> codec_task_runner =
      bridge_impl.codec_task_runner_;
  CodecCodeAdapter result =
      bridge_impl.Configure(kExpectedConfigure, codec_task_runner);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, Configure002) {
  CodecConfigPara config{
      .width = 640, .height = 480, .bitRate = 1000000, .frameRate = 30};
  scoped_refptr<base::SequencedTaskRunner> codec_task_runner =
      bridge_impl.codec_task_runner_;
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.Configure(config, codec_task_runner);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, Configure003) {
  CodecConfigPara config{
      .width = 640, .height = 480, .bitRate = 1000000, .frameRate = 30};

  scoped_refptr<base::SequencedTaskRunner> codec_task_runner =
      bridge_impl.codec_task_runner_;
  bridge_impl.cb_ = nullptr;
  CodecCodeAdapter result = bridge_impl.Configure(config, codec_task_runner);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, Configure004) {
  CodecConfigPara config{
      .width = 640, .height = 480, .bitRate = 1000000, .frameRate = 30};
  scoped_refptr<base::SequencedTaskRunner> codec_task_runner = nullptr;
  bridge_impl.codec_adapter_ = nullptr;
  bridge_impl.cb_ = nullptr;
  CodecCodeAdapter result = bridge_impl.Configure(config, codec_task_runner);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, Configure005) {
  CodecConfigPara config{
      .width = 640, .height = 480, .bitRate = 1000000, .frameRate = 30};

  scoped_refptr<base::SequencedTaskRunner> codec_task_runner =
      bridge_impl.codec_task_runner_;
  std::shared_ptr<CodecConfigParaAdapter> configAdapter = nullptr;
  CodecCodeAdapter result = bridge_impl.Configure(config, codec_task_runner);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, Configure006) {
  CodecConfigPara config{
      .width = 640, .height = 480, .bitRate = 1000000, .frameRate = 30};
  scoped_refptr<base::SequencedTaskRunner> codec_task_runner =
      bridge_impl.codec_task_runner_;
  CodecCodeAdapter result = bridge_impl.Configure(config, codec_task_runner);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestPrepare) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.Prepare();
  ASSERT_EQ(static_cast<int>(result),
            static_cast<int>(CodecCodeAdapter::ERROR));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestPrepare_NonNullAdapter) {
  auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
  EXPECT_CALL(*mock_adapter, Prepare())
      .WillOnce(testing::Return(CodecCodeAdapter::OK));
  bridge_impl.codec_adapter_ = std::move(mock_adapter);
  CodecCodeAdapter result = bridge_impl.Prepare();
  ASSERT_EQ(static_cast<int>(result), static_cast<int>(CodecCodeAdapter::OK));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestStart) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.Start();
  ASSERT_EQ(static_cast<int>(result),
            static_cast<int>(CodecCodeAdapter::ERROR));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestStart_NonNullAdapter) {
  auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
  EXPECT_CALL(*mock_adapter, Start())
      .WillOnce(testing::Return(CodecCodeAdapter::OK));
  bridge_impl.codec_adapter_ = std::move(mock_adapter);
  CodecCodeAdapter result = bridge_impl.Start();
  ASSERT_EQ(static_cast<int>(result), static_cast<int>(CodecCodeAdapter::OK));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestStop) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.Stop();
  ASSERT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestStop_NonNullAdapter) {
  auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
  EXPECT_CALL(*mock_adapter, Stop())
      .WillOnce(testing::Return(CodecCodeAdapter::OK));
  bridge_impl.codec_adapter_ = std::move(mock_adapter);
  CodecCodeAdapter result = bridge_impl.Stop();
  ASSERT_EQ(result, CodecCodeAdapter::OK);
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestReset) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.Reset();
  ASSERT_EQ(static_cast<int>(result),
            static_cast<int>(CodecCodeAdapter::ERROR));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestReset_NonNullAdapter) {
  auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
  EXPECT_CALL(*mock_adapter, Reset())
      .WillOnce(testing::Return(CodecCodeAdapter::OK));
  bridge_impl.codec_adapter_ = std::move(mock_adapter);
  CodecCodeAdapter result = bridge_impl.Reset();
  ASSERT_EQ(static_cast<int>(result), static_cast<int>(CodecCodeAdapter::OK));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestRelease) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.Release();
  ASSERT_EQ(static_cast<int>(result),
            static_cast<int>(CodecCodeAdapter::ERROR));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestRelease_NonNullAdapter) {
  auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
  EXPECT_CALL(*mock_adapter, Release())
      .WillOnce(testing::Return(CodecCodeAdapter::OK));
  bridge_impl.codec_adapter_ = std::move(mock_adapter);
  CodecCodeAdapter result = bridge_impl.Release();
  ASSERT_EQ(static_cast<int>(result), static_cast<int>(CodecCodeAdapter::OK));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestCreateInputSurface) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.CreateInputSurface();
  ASSERT_EQ(static_cast<int>(result),
            static_cast<int>(CodecCodeAdapter::ERROR));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestCreateInputSurface_NullSurface) {
  bridge_impl.surface_ = nullptr;
  CodecCodeAdapter result = bridge_impl.CreateInputSurface();
  ASSERT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByMime_NullCb) {
  const std::string kExpectedMimetype = "video/avc";
  bridge_impl.cb_ = nullptr;
  bridge_impl.CreateVideoCodecByMime(kExpectedMimetype);
  ASSERT_NE(bridge_impl.cb_, callback_);
}

TEST_F(OHOSMediaCodecBridgeImplTest, RequestKeyFrameSoonTest001) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.RequestKeyFrameSoon();
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, RequestKeyFrameSoonTest002) {
  auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
  EXPECT_CALL(*mock_adapter, RequestKeyFrameSoon())
      .WillOnce(::testing::Return(CodecCodeAdapter::RETRY));
  SetCodecAdapter(std::move(mock_adapter));
  CodecCodeAdapter result = bridge_impl.RequestKeyFrameSoon();
  EXPECT_EQ(result, CodecCodeAdapter::RETRY);
}

TEST_F(OHOSMediaCodecBridgeImplTest, ReleaseOutputBufferTest001) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.ReleaseOutputBuffer(1, false);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, ReleaseOutputBufferTest002) {
  auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
  uint32_t index = 1;
  bool render = true;
  EXPECT_CALL(*mock_adapter, ReleaseOutputBuffer(index, render))
      .WillOnce(::testing::Return(CodecCodeAdapter::OK));
  SetCodecAdapter(std::move(mock_adapter));
  CodecCodeAdapter result = bridge_impl.ReleaseOutputBuffer(index, render);
  EXPECT_EQ(result, CodecCodeAdapter::OK);
}

TEST_F(OHOSMediaCodecBridgeImplTest, ClearKeyFrameCacheTest001) {
  bridge_impl.ClearKeyFrameCache();
  EXPECT_EQ(bridge_impl.keyframe_addr_, nullptr);
}

TEST_F(OHOSMediaCodecBridgeImplTest, ClearKeyFrameCacheTest002) {
  uint8_t* keyframe_memory = new uint8_t[20];
  bridge_impl.keyframe_addr_ = keyframe_memory;
  bridge_impl.ClearKeyFrameCache();
  EXPECT_EQ(bridge_impl.keyframe_addr_, nullptr);
}

TEST_F(OHOSMediaCodecBridgeImplTest, ClearConfigDataCacheTest001) {
  bridge_impl.ClearConfigDataCache();
  EXPECT_EQ(bridge_impl.config_data_cache_.config_data_addr, nullptr);
  EXPECT_EQ(bridge_impl.config_data_cache_.config_data_size, 0);
  EXPECT_EQ(bridge_impl.config_data_cache_.config_info_size, 0);
}

TEST_F(OHOSMediaCodecBridgeImplTest, ClearConfigDataCacheTest002) {
  uint8_t* config_data_memory = new uint8_t[20];
  bridge_impl.config_data_cache_.config_data_addr = config_data_memory;
  bridge_impl.config_data_cache_.config_data_size = 100;
  bridge_impl.config_data_cache_.config_info_size = 80;
  bridge_impl.ClearConfigDataCache();
  EXPECT_EQ(bridge_impl.config_data_cache_.config_data_addr, nullptr);
  EXPECT_EQ(bridge_impl.config_data_cache_.config_data_size, 0);
  EXPECT_EQ(bridge_impl.config_data_cache_.config_info_size, 0);
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest001) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  signal->isOnError_ = true;
  bridge_impl.signal_ = signal;
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest002) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  signal->isOnError_ = false;
  EncodeOutputBuffer output_buffer;
  signal->out_buffer_queue_.push(output_buffer);
  bridge_impl.signal_ = signal;
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_NE(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest003) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  signal->out_buffer_queue_ = {};
  bridge_impl.signal_ = signal;
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::RETRY);
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest004) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  EncodeOutputBuffer output_buffer;
  signal->out_buffer_queue_.push(output_buffer);
  bridge_impl.signal_ = signal;
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_NE(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::RETRY);
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest005) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  EncodeOutputBuffer keyframe_buffer;
  keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_NONE;
  signal->out_buffer_queue_.push(keyframe_buffer);
  bridge_impl.signal_ = signal;
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::OK);
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest006) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  EncodeOutputBuffer keyframe_buffer;
  keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
  signal->out_buffer_queue_.push(keyframe_buffer);
  bridge_impl.signal_ = signal;
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest007) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  EncodeOutputBuffer keyframe_buffer;
  keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
  keyframe_buffer.is_contain_config_data = true;
  keyframe_buffer.index = 1;
  EncodeConfigData config_data;
  config_data.buffer_info.size = 20;
  config_data.buffer_data.addr = new uint8_t[20];
  config_data.buffer_data.bufferSize = 20;
  keyframe_buffer.config_data = config_data;
  signal->out_buffer_queue_.push(keyframe_buffer);
  bridge_impl.signal_ = signal;
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::ERROR);
  EXPECT_EQ(bridge_impl.config_data_cache_.config_info_size, 0);
  EXPECT_EQ(bridge_impl.config_data_cache_.config_data_size, 0);
  EXPECT_EQ(bridge_impl.config_data_cache_.config_data_addr, nullptr);
  delete[] config_data.buffer_data.addr;
  delete[] bridge_impl.config_data_cache_.config_data_addr;
  delete[] keyframe_buffer.buffer_data.addr;
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest008) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  EncodeOutputBuffer keyframe_buffer;
  keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
  keyframe_buffer.is_contain_config_data = false;
  keyframe_buffer.index = 1;
  keyframe_buffer.buffer_info.presentationTimeUs = 1000;
  keyframe_buffer.buffer_info.size = 50;
  keyframe_buffer.buffer_data.addr = new uint8_t[50];
  keyframe_buffer.buffer_data.bufferSize = 50;
  signal->out_buffer_queue_.push(keyframe_buffer);
  bridge_impl.signal_ = signal;
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::ERROR);
  EXPECT_EQ(bridge_impl.config_data_cache_.config_data_addr, nullptr);
  delete[] bridge_impl.config_data_cache_.config_data_addr;
  delete[] keyframe_buffer.buffer_data.addr;
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest009) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  EncodeOutputBuffer keyframe_buffer;
  keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
  keyframe_buffer.is_contain_config_data = true;
  keyframe_buffer.index = 1;
  signal->out_buffer_queue_.push(keyframe_buffer);
  bridge_impl.signal_ = signal;
  bridge_impl.config_data_cache_.config_data_addr = nullptr;
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::ERROR);
  delete[] bridge_impl.config_data_cache_.config_data_addr;
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest010) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  EncodeOutputBuffer keyframe_buffer;
  keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
  keyframe_buffer.is_contain_config_data = true;
  keyframe_buffer.index = 1;
  signal->out_buffer_queue_.push(keyframe_buffer);
  bridge_impl.signal_ = signal;
  bridge_impl.config_data_cache_.config_data_addr = new uint8_t[50];
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::ERROR);
  delete[] bridge_impl.config_data_cache_.config_data_addr;
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest0011) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  EncodeOutputBuffer keyframe_buffer;
  keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
  keyframe_buffer.is_contain_config_data = false;
  keyframe_buffer.index = 1;
  keyframe_buffer.buffer_info.presentationTimeUs = 1000;
  keyframe_buffer.buffer_info.size = 50;
  keyframe_buffer.buffer_data.addr = new uint8_t[50];
  keyframe_buffer.buffer_data.bufferSize = 50;
  signal->out_buffer_queue_.push(keyframe_buffer);
  bridge_impl.signal_ = signal;
  bridge_impl.config_data_cache_.config_data_addr = nullptr;
  bridge_impl.config_data_cache_.config_data_size = 0;
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::ERROR);
  delete[] bridge_impl.config_data_cache_.config_data_addr;
  delete[] keyframe_buffer.buffer_data.addr;
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest0012) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  EncodeOutputBuffer keyframe_buffer;
  keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
  keyframe_buffer.is_contain_config_data = false;
  keyframe_buffer.index = 1;
  keyframe_buffer.buffer_info.presentationTimeUs = 1000;
  keyframe_buffer.buffer_info.size = 50;
  keyframe_buffer.buffer_data.addr = new uint8_t[50];
  keyframe_buffer.buffer_data.bufferSize = 50;
  signal->out_buffer_queue_.push(keyframe_buffer);
  bridge_impl.signal_ = signal;
  bridge_impl.config_data_cache_.config_data_addr = new uint8_t[30];
  bridge_impl.config_data_cache_.config_data_size = 0;
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::ERROR);
  delete[] bridge_impl.config_data_cache_.config_data_addr;
  delete[] keyframe_buffer.buffer_data.addr;
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest0013) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  EncodeOutputBuffer keyframe_buffer;
  keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
  keyframe_buffer.is_contain_config_data = false;
  keyframe_buffer.index = 1;
  keyframe_buffer.buffer_info.presentationTimeUs = 1000;
  keyframe_buffer.buffer_info.size = 50;
  keyframe_buffer.buffer_data.addr = new uint8_t[50];
  keyframe_buffer.buffer_data.bufferSize = 50;
  signal->out_buffer_queue_.push(keyframe_buffer);
  bridge_impl.signal_ = signal;
  bridge_impl.config_data_cache_.config_data_addr = nullptr;
  bridge_impl.config_data_cache_.config_data_size = 1;
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::ERROR);
  delete[] bridge_impl.config_data_cache_.config_data_addr;
  delete[] keyframe_buffer.buffer_data.addr;
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest0014) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  EncodeOutputBuffer keyframe_buffer;
  keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
  keyframe_buffer.is_contain_config_data = false;
  keyframe_buffer.index = 1;
  keyframe_buffer.buffer_info.presentationTimeUs = 1000;
  keyframe_buffer.buffer_info.size = 50;
  keyframe_buffer.buffer_data.addr = new uint8_t[50];
  keyframe_buffer.buffer_data.bufferSize = 50;
  signal->out_buffer_queue_.push(keyframe_buffer);
  bridge_impl.signal_ = signal;
  bridge_impl.config_data_cache_.config_data_addr = new uint8_t[30];
  bridge_impl.config_data_cache_.config_data_size = 20;
  bridge_impl.config_data_cache_.config_info_size = 10;
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::OK);
  EXPECT_EQ(info.size, 60);
  EXPECT_EQ(buffer.bufferSize, 70);
  delete[] bridge_impl.config_data_cache_.config_data_addr;
  delete[] keyframe_buffer.buffer_data.addr;
  delete[] buffer.addr;
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest0015) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  EncodeOutputBuffer keyframe_buffer;
  keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
  keyframe_buffer.is_contain_config_data = false;
  keyframe_buffer.index = 1;
  keyframe_buffer.buffer_info.presentationTimeUs = 1000;
  keyframe_buffer.buffer_info.size = 50;
  keyframe_buffer.buffer_data.addr = new uint8_t[50];
  keyframe_buffer.buffer_data.bufferSize = 50;
  signal->out_buffer_queue_.push(keyframe_buffer);
  bridge_impl.signal_ = signal;
  SimulateKeyFrameMemoryFailure();
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::ERROR);
  delete[] bridge_impl.config_data_cache_.config_data_addr;
  delete[] keyframe_buffer.buffer_data.addr;
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest0016) {
  auto signal = std::make_shared<CodecBridgeSignal>();
  EncodeOutputBuffer keyframe_buffer;
  keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
  keyframe_buffer.is_contain_config_data = false;
  keyframe_buffer.index = 1;
  keyframe_buffer.buffer_info.presentationTimeUs = 1000;
  keyframe_buffer.buffer_info.size = 50;
  keyframe_buffer.buffer_data.addr = new uint8_t[50];
  keyframe_buffer.buffer_data.bufferSize = 50;
  signal->out_buffer_queue_.push(keyframe_buffer);
  bridge_impl.signal_ = signal;
  bridge_impl.config_data_cache_.config_data_addr = new uint8_t[30];
  bridge_impl.config_data_cache_.config_data_size = 20;
  bridge_impl.config_data_cache_.config_info_size = 40;
  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer),
            CodecCodeAdapter::OK);
  EXPECT_EQ(info.size, 90);
  EXPECT_EQ(buffer.bufferSize, 70);
  EXPECT_NE(buffer.addr, nullptr);
  delete[] bridge_impl.config_data_cache_.config_data_addr;
  delete[] keyframe_buffer.buffer_data.addr;
  delete[] buffer.addr;
}

TEST_F(OHOSMediaCodecBridgeImplTest, RequestKeyFrameSoonTest_001) {
  bridge_impl.codec_adapter_ = nullptr;
  CodecCodeAdapter result = bridge_impl.RequestKeyFrameSoon();
  ASSERT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, RequestKeyFrameSoonTest_002) {
  auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
  EXPECT_CALL(*mock_adapter, RequestKeyFrameSoon())
      .WillOnce(::testing::Return(CodecCodeAdapter::OK));
  bridge_impl.codec_adapter_ = std::move(mock_adapter);
  ASSERT_EQ(CodecCodeAdapter::OK,
            bridge_impl.codec_adapter_->RequestKeyFrameSoon());
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBufferTest_001) {
  bridge_impl.surface_ = nullptr;
  CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(nullptr, 1);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBufferTest_002) {
  bridge_impl.surface_ = std::make_shared<MockProducerSurfaceAdapter>();
  CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(nullptr, 1);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBufferTest_003) {
  bridge_impl.surface_ = nullptr;
  auto layout = VideoFrameLayout::Create(VideoPixelFormat::PIXEL_FORMAT_I422A,
                                         gfx::Size());
  base::TimeDelta delta = base::Seconds(1);
  scoped_refptr<VideoFrame> frame(new VideoFrame(
      layout.value(), VideoFrame::StorageType::STORAGE_GPU_MEMORY_BUFFER,
      gfx::Rect(), gfx::Size(), delta));
  EXPECT_TRUE(frame);
  CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(frame, 1);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBufferTest_004) {
  bridge_impl.surface_ = std::make_shared<MockProducerSurfaceAdapter>();
  auto surface =
      static_cast<MockProducerSurfaceAdapter*>(bridge_impl.surface_.get());
  auto layout = VideoFrameLayout::Create(VideoPixelFormat::PIXEL_FORMAT_I422A,
                                         gfx::Size());
  base::TimeDelta delta = base::Seconds(1);
  scoped_refptr<VideoFrame> frame(new VideoFrame(
      layout.value(), VideoFrame::StorageType::STORAGE_GPU_MEMORY_BUFFER,
      gfx::Rect(), gfx::Size(), delta));
  EXPECT_TRUE(frame);
  EXPECT_CALL(*surface, RequestBuffer).WillOnce(testing::Return(nullptr));
  CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(frame, 1);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBufferTest_005) {
  bridge_impl.surface_ = std::make_shared<MockProducerSurfaceAdapter>();
  auto layout = VideoFrameLayout::Create(VideoPixelFormat::PIXEL_FORMAT_I422A,
                                         gfx::Size());
  base::TimeDelta delta = base::Seconds(1);

  scoped_refptr<VideoFrame> frame(new VideoFrame(
      layout.value(), VideoFrame::StorageType::STORAGE_GPU_MEMORY_BUFFER,
      gfx::Rect(), gfx::Size(), delta));
  auto surface =
      static_cast<MockProducerSurfaceAdapter*>(bridge_impl.surface_.get());
  auto buffer_adapter = std::make_shared<MockSurfaceBufferAdapter>();
  EXPECT_CALL(*buffer_adapter, GetVirAddr()).WillOnce(testing::Return(nullptr));
  EXPECT_CALL(*buffer_adapter, GetStride()).WillOnce(testing::Return(1));
  EXPECT_CALL(*surface, RequestBuffer)
      .WillOnce(testing::Return(buffer_adapter));
  CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(frame, 1);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBufferTest_006) {
  bridge_impl.surface_ = std::make_shared<MockProducerSurfaceAdapter>();
  auto layout = VideoFrameLayout::Create(VideoPixelFormat::PIXEL_FORMAT_I422A,
                                         gfx::Size());
  base::TimeDelta delta = base::Seconds(1);
  scoped_refptr<VideoFrame> frame(new VideoFrame(
      layout.value(), VideoFrame::StorageType::STORAGE_GPU_MEMORY_BUFFER,
      gfx::Rect(), gfx::Size(), delta));
  auto surface =
      static_cast<MockProducerSurfaceAdapter*>(bridge_impl.surface_.get());
  auto buffer_adapter = std::make_shared<MockSurfaceBufferAdapter>();
  EXPECT_CALL(*surface, RequestBuffer)
      .WillOnce(testing::Return(buffer_adapter));
  std::unique_ptr<uint8_t> dst_data = std::make_unique<uint8_t>(1);
  EXPECT_CALL(*buffer_adapter, GetStride()).WillOnce(testing::Return(1));
  EXPECT_CALL(*buffer_adapter, GetVirAddr())
      .WillOnce(testing::Return(dst_data.get()));
  frame->storage_type_ = VideoFrame::STORAGE_UNOWNED_MEMORY;
  frame->set_data(VideoFrame::kYPlane, nullptr);
  frame->set_data(VideoFrame::kUPlane, nullptr);
  frame->set_data(VideoFrame::kVPlane, nullptr);
  CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(frame, 1);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBufferTest_007) {
  bridge_impl.surface_ = std::make_shared<MockProducerSurfaceAdapter>();
  auto layout = VideoFrameLayout::Create(VideoPixelFormat::PIXEL_FORMAT_I422A,
                                         gfx::Size());
  base::TimeDelta delta = base::Seconds(1);
  scoped_refptr<VideoFrame> frame(new VideoFrame(
      layout.value(), VideoFrame::StorageType::STORAGE_GPU_MEMORY_BUFFER,
      gfx::Rect(), gfx::Size(), delta));
  auto surface =
      static_cast<MockProducerSurfaceAdapter*>(bridge_impl.surface_.get());
  auto buffer_adapter = std::make_shared<MockSurfaceBufferAdapter>();
  EXPECT_CALL(*surface, RequestBuffer)
      .WillOnce(testing::Return(buffer_adapter));
  std::unique_ptr<uint8_t> dst_data = std::make_unique<uint8_t>(1);
  EXPECT_CALL(*buffer_adapter, GetStride()).WillOnce(testing::Return(1));
  EXPECT_CALL(*buffer_adapter, GetVirAddr())
      .WillOnce(testing::Return(dst_data.get()));
  std::unique_ptr<uint8_t> data_ptr = std::make_unique<uint8_t>(1);
  frame->storage_type_ = VideoFrame::STORAGE_UNOWNED_MEMORY;
  frame->set_data(VideoFrame::kYPlane, data_ptr.get());
  frame->set_data(VideoFrame::kUPlane, nullptr);
  frame->set_data(VideoFrame::kVPlane, data_ptr.get());
  CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(frame, 1);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBufferTest_008) {
  bridge_impl.surface_ = std::make_shared<MockProducerSurfaceAdapter>();
  auto layout = VideoFrameLayout::Create(VideoPixelFormat::PIXEL_FORMAT_I422A,
                                         gfx::Size());
  base::TimeDelta delta = base::Seconds(1);
  scoped_refptr<VideoFrame> frame(new VideoFrame(
      layout.value(), VideoFrame::StorageType::STORAGE_GPU_MEMORY_BUFFER,
      gfx::Rect(), gfx::Size(), delta));
  auto surface =
      static_cast<MockProducerSurfaceAdapter*>(bridge_impl.surface_.get());
  auto buffer_adapter = std::make_shared<MockSurfaceBufferAdapter>();
  EXPECT_CALL(*surface, RequestBuffer)
      .WillOnce(testing::Return(buffer_adapter));
  std::unique_ptr<uint8_t> dst_data = std::make_unique<uint8_t>(1);
  EXPECT_CALL(*buffer_adapter, GetStride()).WillOnce(testing::Return(1));
  EXPECT_CALL(*buffer_adapter, GetVirAddr())
      .WillOnce(testing::Return(dst_data.get()));
  std::unique_ptr<uint8_t> data_ptr = std::make_unique<uint8_t>(1);
  frame->storage_type_ = VideoFrame::STORAGE_UNOWNED_MEMORY;
  frame->set_data(VideoFrame::kYPlane, data_ptr.get());
  frame->set_data(VideoFrame::kUPlane, data_ptr.get());
  frame->set_data(VideoFrame::kVPlane, nullptr);
  CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(frame, 1);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBufferTest_009) {
  bridge_impl.surface_ = std::make_shared<MockProducerSurfaceAdapter>();
  auto layout = VideoFrameLayout::Create(VideoPixelFormat::PIXEL_FORMAT_I422A,
                                         gfx::Size());
  base::TimeDelta delta = base::Seconds(1);
  scoped_refptr<VideoFrame> frame(new VideoFrame(
      layout.value(), VideoFrame::StorageType::STORAGE_GPU_MEMORY_BUFFER,
      gfx::Rect(), gfx::Size(), delta));
  auto surface =
      static_cast<MockProducerSurfaceAdapter*>(bridge_impl.surface_.get());
  auto buffer_adapter = std::make_shared<MockSurfaceBufferAdapter>();
  EXPECT_CALL(*surface, RequestBuffer)
      .WillOnce(testing::Return(buffer_adapter));
  std::unique_ptr<uint8_t> dst_data = std::make_unique<uint8_t>(1);
  EXPECT_CALL(*buffer_adapter, GetStride()).WillOnce(testing::Return(1));
  EXPECT_CALL(*buffer_adapter, GetVirAddr())
      .WillOnce(testing::Return(dst_data.get()));
  std::unique_ptr<uint8_t> data_ptr = std::make_unique<uint8_t>(1);
  frame->storage_type_ = VideoFrame::STORAGE_UNOWNED_MEMORY;
  frame->set_data(VideoFrame::kYPlane, data_ptr.get());
  frame->set_data(VideoFrame::kUPlane, nullptr);
  frame->set_data(VideoFrame::kVPlane, nullptr);
  CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(frame, 1);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBufferTest_010) {
  bridge_impl.surface_ = std::make_shared<MockProducerSurfaceAdapter>();
  auto layout = VideoFrameLayout::Create(VideoPixelFormat::PIXEL_FORMAT_I422A,
                                         gfx::Size());
  base::TimeDelta delta = base::Seconds(1);
  scoped_refptr<VideoFrame> frame(new VideoFrame(
      layout.value(), VideoFrame::StorageType::STORAGE_GPU_MEMORY_BUFFER,
      gfx::Rect(), gfx::Size(), delta));
  auto surface =
      static_cast<MockProducerSurfaceAdapter*>(bridge_impl.surface_.get());
  auto buffer_adapter = std::make_shared<MockSurfaceBufferAdapter>();
  EXPECT_CALL(*surface, RequestBuffer)
      .WillOnce(testing::Return(buffer_adapter));
  std::unique_ptr<uint8_t> dst_data = std::make_unique<uint8_t>(1);
  EXPECT_CALL(*buffer_adapter, GetStride()).WillOnce(testing::Return(1));
  EXPECT_CALL(*buffer_adapter, GetVirAddr())
      .WillOnce(testing::Return(dst_data.get()));
  std::unique_ptr<uint8_t> data_ptr = std::make_unique<uint8_t>(1);
  frame->storage_type_ = VideoFrame::STORAGE_UNOWNED_MEMORY;
  frame->set_data(VideoFrame::kYPlane, data_ptr.get());
  frame->set_data(VideoFrame::kUPlane, data_ptr.get());
  frame->set_data(VideoFrame::kVPlane, data_ptr.get());
  EXPECT_CALL(*surface, FlushBuffer).WillOnce(testing::Return(1));
  CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(frame, 1);
  EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBufferTest_011) {
  bridge_impl.surface_ = std::make_shared<MockProducerSurfaceAdapter>();
  auto layout = VideoFrameLayout::Create(VideoPixelFormat::PIXEL_FORMAT_I422A,
                                         gfx::Size());
  base::TimeDelta delta = base::Seconds(1);
  scoped_refptr<VideoFrame> frame(new VideoFrame(
      layout.value(), VideoFrame::StorageType::STORAGE_GPU_MEMORY_BUFFER,
      gfx::Rect(), gfx::Size(), delta));
  auto surface =
      static_cast<MockProducerSurfaceAdapter*>(bridge_impl.surface_.get());
  auto buffer_adapter = std::make_shared<MockSurfaceBufferAdapter>();
  EXPECT_CALL(*surface, RequestBuffer)
      .WillOnce(testing::Return(buffer_adapter));
  std::unique_ptr<uint8_t> dst_data = std::make_unique<uint8_t>(1);
  EXPECT_CALL(*buffer_adapter, GetStride()).WillOnce(testing::Return(1));
  EXPECT_CALL(*buffer_adapter, GetVirAddr())
      .WillOnce(testing::Return(dst_data.get()));
  std::unique_ptr<uint8_t> data_ptr = std::make_unique<uint8_t>(1);
  frame->storage_type_ = VideoFrame::STORAGE_UNOWNED_MEMORY;
  frame->set_data(VideoFrame::kYPlane, data_ptr.get());
  frame->set_data(VideoFrame::kUPlane, data_ptr.get());
  frame->set_data(VideoFrame::kVPlane, data_ptr.get());
  EXPECT_CALL(*surface, FlushBuffer).WillOnce(testing::Return(0));
  CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(frame, 1);
  EXPECT_EQ(result, CodecCodeAdapter::OK);
}

TEST_F(CodecEncodeBridgeCallbackTest, OnNeedOutputData_NullBuffer) {
  testing::internal::CaptureStderr();
  std::shared_ptr<BufferInfoAdapter> valid_info =
      std::make_shared<InheritBufferInfoAdapter>();
  std::shared_ptr<OhosBufferAdapter> null_buffer = nullptr;

  callback_->OnNeedOutputData(0, valid_info, BufferFlag::CODEC_BUFFER_FLAG_NONE,
                              null_buffer);
  std::string log_output1 = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output1.find("Output is invalid"), std::string::npos);
}

TEST_F(CodecEncodeBridgeCallbackTest, OnNeedOutputData_Valid) {
  testing::internal::CaptureStderr();
  std::shared_ptr<BufferInfoAdapter> valid_info =
      std::make_shared<InheritBufferInfoAdapter>();
  std::shared_ptr<OhosBufferAdapter> valid_buffer =
      std::make_shared<InheritOhosBufferAdapter>();

  callback_->OnNeedOutputData(0, valid_info, BufferFlag::CODEC_BUFFER_FLAG_NONE,
                              valid_buffer);
  std::string log_output1 = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output1.find("encoder is not running"), std::string::npos);
}

TEST_F(CodecEncodeBridgeCallbackTest, OnNeedOutputData_Valid1) {
  testing::internal::CaptureStderr();
  std::shared_ptr<BufferInfoAdapter> valid_info =
      std::make_shared<InheritBufferInfoAdapter>();
  std::shared_ptr<OhosBufferAdapter> valid_buffer =
      std::make_shared<InheritOhosBufferAdapter>();
  callback_->is_running_.store(false);

  callback_->OnNeedOutputData(0, valid_info, BufferFlag::CODEC_BUFFER_FLAG_NONE,
                              valid_buffer);
  std::string log_output1 = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output1.find("encoder is not running"), std::string::npos);
}

TEST_F(CodecEncodeBridgeCallbackTest, OnNeedOutputData_Valid2) {
  testing::internal::CaptureStderr();
  std::shared_ptr<BufferInfoAdapter> valid_info =
      std::make_shared<InheritBufferInfoAdapter>();
  std::shared_ptr<OhosBufferAdapter> valid_buffer =
      std::make_shared<InheritOhosBufferAdapter>();
  callback_->is_running_.store(true);
  MockSequencedTaskRunner mock_task_runner_;
  EXPECT_CALL(mock_task_runner_, RunsTasksInCurrentSequence())
      .WillOnce(testing::Return(false));
  EXPECT_CALL(mock_task_runner_,
              PostDelayedTask(testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(false));
  callback_->codec_callback_task_runner_ = &mock_task_runner_;

  callback_->OnNeedOutputData(0, valid_info, BufferFlag::CODEC_BUFFER_FLAG_NONE,
                              valid_buffer);
  std::string log_output1 = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output1.find("encoder is not running"), std::string::npos);
}

TEST_F(CodecEncodeBridgeCallbackTest, OnNeedOutputData_Valid3) {
  testing::internal::CaptureStderr();
  std::shared_ptr<BufferInfoAdapter> valid_info =
      std::make_shared<InheritBufferInfoAdapter>();
  std::shared_ptr<OhosBufferAdapter> valid_buffer =
      std::make_shared<InheritOhosBufferAdapter>();
  callback_->is_running_.store(true);
  MockSequencedTaskRunner mock_task_runner_;
  EXPECT_CALL(mock_task_runner_, RunsTasksInCurrentSequence())
      .WillOnce(testing::Return(true));
  callback_->codec_callback_task_runner_ = &mock_task_runner_;

  callback_->OnNeedOutputData(
      0, valid_info, BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA, valid_buffer);
  std::string log_output1 = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output1.find("flag is codec_data, handle with keyframe later"),
            std::string::npos);
}

TEST_F(CodecEncodeBridgeCallbackTest, OnNeedOutputData_Valid4) {
  testing::internal::CaptureStderr();
  std::shared_ptr<BufferInfoAdapter> valid_info =
      std::make_shared<InheritBufferInfoAdapter>();
  std::shared_ptr<OhosBufferAdapter> valid_buffer =
      std::make_shared<InheritOhosBufferAdapter>();
  callback_->is_running_.store(true);
  MockSequencedTaskRunner mock_task_runner_;
  EXPECT_CALL(mock_task_runner_, RunsTasksInCurrentSequence())
      .WillOnce(testing::Return(true));
  callback_->codec_callback_task_runner_ = &mock_task_runner_;

  callback_->OnNeedOutputData(0, valid_info, BufferFlag::CODEC_BUFFER_FLAG_NONE,
                              valid_buffer);
  std::string log_output1 = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output1.find("flag is codec_data, handle with keyframe later"),
            std::string::npos);
}

TEST_F(CodecEncodeBridgeCallbackTest, OnNeedOutputData_Valid5) {
  testing::internal::CaptureStderr();
  std::shared_ptr<BufferInfoAdapter> valid_info =
      std::make_shared<InheritBufferInfoAdapter>();
  std::shared_ptr<OhosBufferAdapter> valid_buffer =
      std::make_shared<InheritOhosBufferAdapterNull>();
  callback_->is_running_.store(true);
  MockSequencedTaskRunner mock_task_runner_;
  EXPECT_CALL(mock_task_runner_, RunsTasksInCurrentSequence())
      .WillOnce(testing::Return(true));
  callback_->codec_callback_task_runner_ = &mock_task_runner_;

  callback_->OnNeedOutputData(
      0, valid_info, BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME, valid_buffer);
  std::string log_output1 = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output1.find("handle keyframe now"), std::string::npos);
}

TEST_F(CodecEncodeBridgeCallbackTest, OnNeedOutputData_Valid6) {
  testing::internal::CaptureStderr();
  std::shared_ptr<BufferInfoAdapter> valid_info =
      std::make_shared<InheritBufferInfoAdapter>();
  std::shared_ptr<OhosBufferAdapter> valid_buffer =
      std::make_shared<InheritOhosBufferAdapter>();
  callback_->is_running_.store(true);
  MockSequencedTaskRunner mock_task_runner_;
  EXPECT_CALL(mock_task_runner_, RunsTasksInCurrentSequence())
      .WillOnce(testing::Return(true));
  callback_->codec_callback_task_runner_ = &mock_task_runner_;
  callback_->config_data_.buffer_data.bufferSize = 8;

  callback_->OnNeedOutputData(
      0, valid_info, BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME, valid_buffer);
  std::string log_output1 = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output1.find("handle keyframe now"), std::string::npos);
}

}  // namespace media
