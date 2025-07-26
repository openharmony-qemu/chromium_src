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

#include "buffer_request_config_adapter_impl.h"
#include "third_party/libc++/src/include/__ranges/lazy_split_view.h"
#define private public
#include "ohos_media_codec_bridge_impl.h"
#undef private

#include <cstddef>
#include <memory>
#include <utility>
#include <string>

#include "media/base/video_frame.h"
#include "graphic_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/media_codec_adapter.h"
#include "ohos_adapter_helper.h"
#include "buffer_flush_config_adapter_impl.h"
#include "codec_config_para_adapter_impl.h"
#include "third_party/bounds_checking_function/include/securec.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

std::string test = "video/hevc";
std::string &refToMyString = test;

namespace media {
using namespace OHOS::NWeb;

class MockProducerSurfaceAdapter : public ProducerSurfaceAdapter {
public:
    MOCK_METHOD(std::shared_ptr<SurfaceBufferAdapter>, RequestBuffer,
        (int32_t &, std::shared_ptr<BufferRequestConfigAdapter>), (override));
    MOCK_METHOD(int32_t, FlushBuffer,
        (std::shared_ptr<SurfaceBufferAdapter>, int32_t, std::shared_ptr<BufferFlushConfigAdapter>), (override));
};

class MockSurfaceBufferAdapter : public SurfaceBufferAdapter {
public:
    MOCK_METHOD(int32_t, GetFileDescriptor, (), (override));
    MOCK_METHOD(int32_t, GetWidth, (), (override));
    MOCK_METHOD(int32_t, GetHeight, (), (override));
    MOCK_METHOD(int32_t, GetStride, (), (override));
    MOCK_METHOD(int32_t, GetFormat, (), (override));
    MOCK_METHOD(uint32_t, GetSize, (), (override));
    MOCK_METHOD(void *, GetVirAddr, (), (override));
};

class MockMediaCodecAdapter : public MediaCodecAdapter {
public:
    MOCK_METHOD(CodecCodeAdapter, CreateVideoCodecByMime, (const std::string), (override));
    MOCK_METHOD(CodecCodeAdapter, CreateVideoCodecByName, (const std::string), (override));
    MOCK_METHOD(CodecCodeAdapter, Configure, (const std::shared_ptr<CodecConfigParaAdapter>), (override));
    MOCK_METHOD(CodecCodeAdapter, Prepare, (), (override));
    MOCK_METHOD(CodecCodeAdapter, Start, (), (override));
    MOCK_METHOD(CodecCodeAdapter, Stop, (), (override));
    MOCK_METHOD(CodecCodeAdapter, RequestKeyFrameSoon, (), (override));
    MOCK_METHOD(CodecCodeAdapter, Reset, (), (override));
    MOCK_METHOD(CodecCodeAdapter, Release, (), (override));
    MOCK_METHOD(CodecCodeAdapter, ReleaseOutputBuffer, (uint32_t, bool), (override));
    MOCK_METHOD(CodecCodeAdapter, SetCodecCallback, (const std::shared_ptr<CodecCallbackAdapter>), (override));
    MOCK_METHOD(std::shared_ptr<ProducerSurfaceAdapter>, CreateInputSurface, (), (override));
};

class InheritBufferInfoAdapter : public BufferInfoAdapter {
public:
    int64_t GetPresentationTimeUs() override
    {
        return 100;
    }
    int32_t GetSize() override
    {
        return 1;
    }
    int32_t GetOffset() override
    {
        return 0;
    }
};

class MockOhosBufferAdapter : public OhosBufferAdapter {
public:
    MOCK_METHOD(uint8_t *, GetAddr, (), (override));
    MOCK_METHOD(uint32_t, GetBufferSize, (), (override));
};

class InheritOhosBufferAdapter : public OhosBufferAdapter {
public:
    uint8_t *GetAddr() override
    {
        return nullptr;
    }
    uint32_t GetBufferSize() override
    {
        return 8;
    }
};

class MockSequencedTaskRunner : public base::SequencedTaskRunner {
public:
    MOCK_METHOD(bool, PostNonNestableTask, (const base::Location &location, base::OnceClosure onceClosure), ());
    MOCK_METHOD(bool, PostNonNestableDelayedTask,
        (const base::Location &location, base::OnceClosure onceClosure, base::TimeDelta time), (override));
    MOCK_METHOD(base::DelayedTaskHandle, PostCancelableDelayedTask,
        (base::subtle::PostDelayedTaskPassKey key, const base::Location &location, base::OnceClosure onceClosure,
            base::TimeDelta time),
        (override));
    MOCK_METHOD(base::DelayedTaskHandle, PostCancelableDelayedTaskAt,
        (base::subtle::PostDelayedTaskPassKey key, const base::Location &location, base::OnceClosure onceClosure,
            base::TimeTicks ticks, base::subtle::DelayPolicy policy),
        (override));
    MOCK_METHOD(bool, PostDelayedTaskAt,
        (base::subtle::PostDelayedTaskPassKey key, const base::Location &location, base::OnceClosure onceClosure,
            base::TimeTicks ticks, base::subtle::DelayPolicy policy),
        (override));
    MOCK_METHOD(bool, RunsTasksInCurrentSequence, (), (const, override));
    MOCK_METHOD(
        bool, PostDelayedTask, (const base::Location &location, base::OnceClosure, base::TimeDelta), (override));
    MOCK_METHOD(bool, DeleteOrReleaseSoonInternal,
        (const base::Location &location, void (*)(const void *), const void *), (override));
};

class OHOSMediaCodecBridgeImplTest : public ::testing::Test {
public:
    void SimulateKeyFrameMemoryFailure()
    {
        bridge_impl.keyframe_addr_ = nullptr;
    }
    void UpdateStatusAndClearCache(bool is_running)
    {
        bridge_impl.UpdateStatusAndClearCache(is_running);
    }
    void ClearConfigDataCache()
    {
        bridge_impl.ClearConfigDataCache();
    }

    MockMediaCodecAdapter mock_codec_adapter;
    std::unique_ptr<MockMediaCodecAdapter> mock_codec_adapter_;
    OHOSMediaCodecBridgeImpl bridge_impl = OHOSMediaCodecBridgeImpl(refToMyString);
    std::optional<VideoFrameLayout> layout =
    VideoFrameLayout::Create(VideoPixelFormat::PIXEL_FORMAT_I422A, gfx::Size());
    scoped_refptr<VideoFrame> video_frame = new VideoFrame(
        layout.value(), VideoFrame::StorageType::STORAGE_GPU_MEMORY_BUFFER, 
        gfx::Rect(), gfx::Size(), base::Seconds(1));
protected:
    void SetUp() override
    {
        signal_ = std::make_shared<CodecBridgeSignal>();
        callback_ = std::make_shared<CodecEncodeBridgeCallback>(signal_);
    }
    void SetCodecAdapter(std::unique_ptr<MediaCodecAdapter> adapter)
    {
        bridge_impl.codec_adapter_ = std::move(adapter);
    }
    void SetSignal(std::shared_ptr<CodecBridgeSignal> signal)
    {
        bridge_impl.signal_ = signal;
    }
    void SetIsRunning(bool is_running)
    {
        bridge_impl.is_running_.store(is_running);
    }
    void SetCodecEncodeBridgeCallback(std::shared_ptr<CodecEncodeBridgeCallback> cb) {
        bridge_impl.cb_ = cb;
    }
    void SetKeyFrameAddr(uint8_t* addr)
    {
        bridge_impl.keyframe_addr_ = addr;
    }
    void SetSurface(std::shared_ptr<ProducerSurfaceAdapter> surface)
    {
        bridge_impl.surface_ = surface;
    }
    void SetConfigDataCache(EncodeConfigDataCache config_data_cache)
    {
        bridge_impl.config_data_cache_ = config_data_cache;
    }
    void SetBufferAdapter(std::shared_ptr<SurfaceBufferAdapter> buffer_adapter)
    {
        bridge_impl.buffer_adapter_ = buffer_adapter;
    }
    void SetData(size_t plane, const uint8_t* ptr) {
        video_frame->data_[plane] = ptr;
    }
    void SetStorageType(VideoFrame::StorageType storage_type) {
        video_frame->storage_type_ = storage_type;
    }

    std::unique_ptr<MediaCodecAdapter> GetCodecAdapter()
    {
        return std::move(bridge_impl.codec_adapter_);
    }
    std::shared_ptr<CodecBridgeSignal> GetSignal()
    {
        return bridge_impl.signal_;
    }
    bool GetIsRunning()
    {
        return bridge_impl.is_running_.load();
    }
    std::shared_ptr<CodecEncodeBridgeCallback> GetCodecEncodeBridgeCallback()
    {
        return bridge_impl.cb_;
    }
    scoped_refptr<base::SequencedTaskRunner> GetTaskRunner() {
        return bridge_impl.codec_task_runner_;
    }
    uint8_t* GetKeyFrameAddr()
    {
        return bridge_impl.keyframe_addr_;
    }
    std::shared_ptr<ProducerSurfaceAdapter> GetSurface()
    {
        return bridge_impl.surface_;
    }
    EncodeConfigDataCache GetConfigDataCache()
    {
        return bridge_impl.config_data_cache_;
    }
    std::shared_ptr<SurfaceBufferAdapter> GetBufferAdapter()
    {
        return bridge_impl.buffer_adapter_;
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
    void SetUp() override
    {
        signal_ = std::make_shared<CodecBridgeSignal>();
        callback_ = std::make_shared<CodecEncodeBridgeCallback>(signal_);
    }
    void SetConfigDataBufferSize(const int32_t buffer_size)
    {
        callback_->config_data_.buffer_data.bufferSize = buffer_size;
    }
    std::shared_ptr<CodecBridgeSignal> signal_;
    std::shared_ptr<CodecEncodeBridgeCallback> callback_;
    std::shared_ptr<CodecEncodeBridgeCallback> cb_ = nullptr;
    BufferInfo buffer_info_;
    OhosBuffer buffer_data_;
    EncodeOutputBuffer output_buffer_;
};

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByMime001)
{
    SetCodecAdapter(nullptr);
    CodecCodeAdapter result = bridge_impl.CreateVideoCodecByMime("kExpectedMimetype");
    EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByMime002)
{
    const std::string kExpectedMimetype = "video/avc";
    CodecCodeAdapter result = bridge_impl.CreateVideoCodecByMime(kExpectedMimetype);
    EXPECT_EQ(result, CodecCodeAdapter::OK);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByMime003)
{
    const std::string kExpectedMimetype = "codec_bridge";
    CodecCodeAdapter result = bridge_impl.CreateVideoCodecByMime(kExpectedMimetype);
    EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByMime004)
{
    const std::string kExpectedMimetype = "video/avc";
    auto mockCodecAdapter = std::make_unique<MockMediaCodecAdapter>();
    EXPECT_CALL(*mockCodecAdapter, CreateVideoCodecByMime(kExpectedMimetype))
        .WillOnce(testing::Return(CodecCodeAdapter::OK));
    EXPECT_CALL(*mockCodecAdapter, SetCodecCallback(testing::_)).WillOnce(testing::Return(CodecCodeAdapter::OK));
    SetCodecAdapter(std::move(mockCodecAdapter));
    CodecCodeAdapter result = bridge_impl.CreateVideoCodecByMime(kExpectedMimetype);
    ASSERT_EQ(result, CodecCodeAdapter::OK);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByMime005)
{
    const std::string kExpectedMimetype = "video/avc";
    auto mockCodecAdapter = std::make_unique<MockMediaCodecAdapter>();
    EXPECT_CALL(*mockCodecAdapter, CreateVideoCodecByMime(kExpectedMimetype))
        .WillOnce(testing::Return(CodecCodeAdapter::OK));
    EXPECT_CALL(*mockCodecAdapter, SetCodecCallback(testing::_)).WillOnce(testing::Return(CodecCodeAdapter::OK));
    SetSignal(std::make_shared<CodecBridgeSignal>());
    SetCodecAdapter(std::move(mockCodecAdapter));
    (void)bridge_impl.CreateVideoCodecByMime(kExpectedMimetype);
    ASSERT_NE(GetSignal(), nullptr);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByName_NullAdapter)
{
    SetCodecAdapter(nullptr);
    CodecCodeAdapter ret = bridge_impl.CreateVideoCodecByName("codec_nullptr");
    EXPECT_EQ(ret, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByName_CreateFailed)
{
    auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
    EXPECT_NE(mock_adapter, nullptr);
    EXPECT_CALL(*mock_adapter, CreateVideoCodecByName("codec_failed"))
        .WillOnce(testing::Return(CodecCodeAdapter::ERROR));
    SetCodecAdapter(std::move(mock_adapter));
    CodecCodeAdapter ret = bridge_impl.CreateVideoCodecByName("codec_failed");
    EXPECT_EQ(ret, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, UpdateStatusAndClearCache_Isrunning)
{
    bool Is_running = true;
    SetIsRunning(Is_running);
    UpdateStatusAndClearCache(Is_running);
    EXPECT_EQ(GetIsRunning(), Is_running);
}

TEST_F(OHOSMediaCodecBridgeImplTest, UpdateStatusAndClearCache_Cb)
{
    bool Is_running = true;
    auto mock_adapter = std::make_shared<CodecEncodeBridgeCallback>(signal_);
    SetCodecEncodeBridgeCallback(mock_adapter);
    UpdateStatusAndClearCache(Is_running);
    EXPECT_EQ(mock_adapter->is_running_.load(), Is_running);
}

TEST_F(OHOSMediaCodecBridgeImplTest, UpdateStatusAndClearCache001)
{
    const bool kExpected = true;
    UpdateStatusAndClearCache(kExpected);
    EXPECT_TRUE(kExpected);
}

TEST_F(OHOSMediaCodecBridgeImplTest, UpdateStatusAndClearCache002)
{
    const bool kExpected = false;
    auto mock_adapter = std::make_shared<CodecEncodeBridgeCallback>(signal_);
    SetIsRunning(true);
    SetSignal(std::make_shared<CodecBridgeSignal>());
    UpdateStatusAndClearCache(kExpected);
    EXPECT_FALSE(kExpected);
}

TEST_F(OHOSMediaCodecBridgeImplTest, UpdateStatusAndClearCache003)
{
    const bool kExpected = false;
    auto mock_adapter = std::make_shared<CodecEncodeBridgeCallback>(signal_);
    SetIsRunning(true);
    SetSignal(std::make_shared<CodecBridgeSignal>());
    SetCodecEncodeBridgeCallback(mock_adapter);
    UpdateStatusAndClearCache(kExpected);
    EXPECT_FALSE(kExpected);
}

TEST_F(OHOSMediaCodecBridgeImplTest, UpdateStatusAndClearCache004)
{
    const bool kExpected = false;
    SetIsRunning(true);
    SetSignal(std::make_shared<CodecBridgeSignal>());
    UpdateStatusAndClearCache(kExpected);
    EXPECT_FALSE(kExpected);
}

TEST_F(OHOSMediaCodecBridgeImplTest, Configure001)
{
    auto mock_codec_adapter = std::make_unique<MockMediaCodecAdapter>();
    SetCodecAdapter(std::move(mock_codec_adapter));
    auto mock_callback = std::make_shared<CodecEncodeBridgeCallback>(signal_);
    SetCodecEncodeBridgeCallback(mock_callback);
    CodecConfigPara kExpectedConfigure = {1280, 720, 1000000, 30.0};
    scoped_refptr<base::SequencedTaskRunner> codec_task_runner = GetTaskRunner();
    CodecCodeAdapter result = bridge_impl.Configure(kExpectedConfigure, codec_task_runner);
    EXPECT_EQ(result, CodecCodeAdapter::OK);
}

TEST_F(OHOSMediaCodecBridgeImplTest, Configure002)
{
    CodecConfigPara config{.width = 640, .height = 480, .bitRate = 1000000, .frameRate = 30};
    scoped_refptr<base::SequencedTaskRunner> codec_task_runner = GetTaskRunner();
    SetCodecEncodeBridgeCallback(nullptr);
    CodecCodeAdapter result = bridge_impl.Configure(config, codec_task_runner);
    EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, Configure003)
{
    CodecConfigPara config{.width = 640, .height = 480, .bitRate = 1000000, .frameRate = 30};
    scoped_refptr<base::SequencedTaskRunner> codec_task_runner = GetTaskRunner();
    SetCodecAdapter(nullptr);
    CodecCodeAdapter result = bridge_impl.Configure(config, codec_task_runner);
    EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestPrepare_NonNullAdapter)
{
    auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
    EXPECT_CALL(*mock_adapter, Prepare()).WillOnce(testing::Return(CodecCodeAdapter::OK));
    SetCodecAdapter(std::move(mock_adapter));
    CodecCodeAdapter result = bridge_impl.Prepare();
    ASSERT_EQ(static_cast<int>(result), static_cast<int>(CodecCodeAdapter::OK));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestStart)
{
    SetCodecAdapter(nullptr);
    CodecCodeAdapter result = bridge_impl.Start();
    ASSERT_EQ(static_cast<int>(result), static_cast<int>(CodecCodeAdapter::ERROR));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestStart_NonNullAdapter)
{
    auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
    EXPECT_CALL(*mock_adapter, Start()).WillOnce(testing::Return(CodecCodeAdapter::OK));
    SetCodecAdapter(std::move(mock_adapter));
    CodecCodeAdapter result = bridge_impl.Start();
    ASSERT_EQ(static_cast<int>(result), static_cast<int>(CodecCodeAdapter::OK));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestStop)
{
    SetCodecAdapter(nullptr);
    CodecCodeAdapter result = bridge_impl.Stop();
    ASSERT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestStop_NonNullAdapter)
{
    auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
    EXPECT_CALL(*mock_adapter, Stop()).WillOnce(testing::Return(CodecCodeAdapter::OK));
    SetCodecAdapter(std::move(mock_adapter));
    CodecCodeAdapter result = bridge_impl.Stop();
    ASSERT_EQ(result, CodecCodeAdapter::OK);
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestReset)
{
    SetCodecAdapter(nullptr);
    CodecCodeAdapter result = bridge_impl.Reset();
    ASSERT_EQ(static_cast<int>(result), static_cast<int>(CodecCodeAdapter::ERROR));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestReset_NonNullAdapter)
{
    auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
    EXPECT_CALL(*mock_adapter, Reset()).WillOnce(testing::Return(CodecCodeAdapter::OK));
    SetCodecAdapter(std::move(mock_adapter));
    CodecCodeAdapter result = bridge_impl.Reset();
    ASSERT_EQ(static_cast<int>(result), static_cast<int>(CodecCodeAdapter::OK));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestRelease)
{
    SetCodecAdapter(nullptr);
    CodecCodeAdapter result = bridge_impl.Release();
    ASSERT_EQ(static_cast<int>(result), static_cast<int>(CodecCodeAdapter::ERROR));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestRelease_NonNullAdapter)
{
    auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
    EXPECT_CALL(*mock_adapter, Release()).WillOnce(testing::Return(CodecCodeAdapter::OK));
    SetCodecAdapter(std::move(mock_adapter));
    CodecCodeAdapter result = bridge_impl.Release();
    ASSERT_EQ(static_cast<int>(result), static_cast<int>(CodecCodeAdapter::OK));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestCreateInputSurface)
{
    SetCodecAdapter(nullptr);
    CodecCodeAdapter result = bridge_impl.CreateInputSurface();
    ASSERT_EQ(static_cast<int>(result), static_cast<int>(CodecCodeAdapter::ERROR));
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestCreateInputSurface_NullSurface)
{
    SetSurface(nullptr);
    CodecCodeAdapter result = bridge_impl.CreateInputSurface();
    ASSERT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, TestCreateInputSurface_NonNullSurface)
{
    auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
    EXPECT_CALL(*mock_adapter, CreateInputSurface())
        .WillOnce(::testing::Return(std::make_shared<MockProducerSurfaceAdapter>()));
    SetCodecAdapter(std::move(mock_adapter));
    CodecCodeAdapter result = bridge_impl.CreateInputSurface();
    ASSERT_EQ(result, CodecCodeAdapter::OK);
}

TEST_F(OHOSMediaCodecBridgeImplTest, CreateVideoCodecByMime_NullCb)
{
    const std::string kExpectedMimetype = "video/avc";
    SetCodecEncodeBridgeCallback(nullptr);
    bridge_impl.CreateVideoCodecByMime(kExpectedMimetype);
    ASSERT_NE(GetCodecEncodeBridgeCallback(), callback_);
}

TEST_F(OHOSMediaCodecBridgeImplTest, RequestKeyFrameSoonTest001)
{
    SetCodecAdapter(nullptr);
    CodecCodeAdapter result = bridge_impl.RequestKeyFrameSoon();
    EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, RequestKeyFrameSoonTest002)
{
    auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
    EXPECT_CALL(*mock_adapter, RequestKeyFrameSoon()).WillOnce(::testing::Return(CodecCodeAdapter::RETRY));
    SetCodecAdapter(std::move(mock_adapter));
    CodecCodeAdapter result = bridge_impl.RequestKeyFrameSoon();
    EXPECT_EQ(result, CodecCodeAdapter::RETRY);
}

TEST_F(OHOSMediaCodecBridgeImplTest, ReleaseOutputBufferTest001)
{
    SetCodecAdapter(nullptr);
    CodecCodeAdapter result = bridge_impl.ReleaseOutputBuffer(1, false);
    EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, ReleaseOutputBufferTest002)
{
    auto mock_adapter = std::make_unique<MockMediaCodecAdapter>();
    uint32_t index = 1;
    bool render = true;
    EXPECT_CALL(*mock_adapter, ReleaseOutputBuffer(index, render)).WillOnce(::testing::Return(CodecCodeAdapter::OK));
    SetCodecAdapter(std::move(mock_adapter));
    CodecCodeAdapter result = bridge_impl.ReleaseOutputBuffer(index, render);
    EXPECT_EQ(result, CodecCodeAdapter::OK);
}

TEST_F(OHOSMediaCodecBridgeImplTest, ClearKeyFrameCacheTest001)
{
    bridge_impl.ClearKeyFrameCache();
    EXPECT_EQ(GetKeyFrameAddr(), nullptr);
}

TEST_F(OHOSMediaCodecBridgeImplTest, ClearKeyFrameCacheTest002)
{
    uint8_t *keyframe_memory = new uint8_t[20];
    SetKeyFrameAddr(keyframe_memory);
    bridge_impl.ClearKeyFrameCache();
    EXPECT_EQ(GetKeyFrameAddr(), nullptr);
}

TEST_F(OHOSMediaCodecBridgeImplTest, ClearConfigDataCacheTest001)
{
    ClearConfigDataCache();
    EXPECT_EQ(GetConfigDataCache().config_data_addr, nullptr);
    EXPECT_EQ(GetConfigDataCache().config_data_size, 0);
    EXPECT_EQ(GetConfigDataCache().config_info_size, 0);
}

TEST_F(OHOSMediaCodecBridgeImplTest, ClearConfigDataCacheTest002)
{
    uint8_t *config_data_memory = new uint8_t[20];
    EncodeConfigDataCache config_data_cache;
    config_data_cache.config_data_addr = config_data_memory;
    config_data_cache.config_data_size = 100;
    config_data_cache.config_info_size = 80;
    SetConfigDataCache(config_data_cache);
    ClearConfigDataCache();
    EXPECT_EQ(GetConfigDataCache().config_data_addr, nullptr);
    EXPECT_EQ(GetConfigDataCache().config_data_size, 0);
    EXPECT_EQ(GetConfigDataCache().config_info_size, 0);
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest001)
{
    auto signal = std::make_shared<CodecBridgeSignal>();
    signal->isOnError_ = true;
    SetSignal(signal);
    uint32_t index;
    BufferInfo info;
    BufferFlag flag;
    OhosBuffer buffer;
    EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer), CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest002)
{
    auto signal = std::make_shared<CodecBridgeSignal>();
    signal->isOnError_ = false;
    signal->out_buffer_queue_ = {};
    SetSignal(signal);
    uint32_t index;
    BufferInfo info;
    BufferFlag flag;
    OhosBuffer buffer;
    EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer), CodecCodeAdapter::RETRY);
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest003)
{
    auto signal = std::make_shared<CodecBridgeSignal>();
    EncodeOutputBuffer keyframe_buffer;
    keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_NONE;
    signal->out_buffer_queue_.push(keyframe_buffer);
    SetSignal(signal);
    uint32_t index;
    BufferInfo info;
    BufferFlag flag;
    OhosBuffer buffer;
    EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer), CodecCodeAdapter::OK);
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest004)
{
    auto signal = std::make_shared<CodecBridgeSignal>();
    EncodeOutputBuffer keyframe_buffer;
    keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
    signal->out_buffer_queue_.push(keyframe_buffer);
    SetSignal(signal);
    uint32_t index;
    BufferInfo info;
    BufferFlag flag;
    OhosBuffer buffer;
    EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer), CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest005)
{
    auto signal = std::make_shared<CodecBridgeSignal>();
    EncodeOutputBuffer keyframe_buffer;
    keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
    keyframe_buffer.is_contain_config_data = false;
    keyframe_buffer.index = 1;
    keyframe_buffer.is_contain_config_data = false;  
    keyframe_buffer.buffer_info.size = 50;
    keyframe_buffer.buffer_data.addr = new uint8_t[50];
    keyframe_buffer.buffer_info.presentationTimeUs = 1000;
    keyframe_buffer.buffer_data.bufferSize = 50;
    signal->out_buffer_queue_.push(keyframe_buffer);
    SetSignal(signal);
    uint32_t index;
    BufferInfo info;
    BufferFlag flag;
    OhosBuffer buffer;
    EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer), CodecCodeAdapter::ERROR);
    delete[] bridge_impl.config_data_cache_.config_data_addr;
    delete[] keyframe_buffer.buffer_data.addr;
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest006)
{
    auto signal = std::make_shared<CodecBridgeSignal>();
    EncodeOutputBuffer keyframe_buffer;
    keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
    keyframe_buffer.buffer_info.presentationTimeUs = 1000;
    keyframe_buffer.buffer_info.size = 50;
    keyframe_buffer.is_contain_config_data = false;
    keyframe_buffer.index = 1;
    keyframe_buffer.buffer_data.addr = new uint8_t[50];
    keyframe_buffer.buffer_data.bufferSize = 50;
    signal->out_buffer_queue_.push(keyframe_buffer);
    SetSignal(signal);
    bridge_impl.config_data_cache_.config_data_addr = nullptr;
    bridge_impl.config_data_cache_.config_data_size = 0;
    uint32_t index;
    BufferInfo info;
    BufferFlag flag;
    OhosBuffer buffer;
    EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer), CodecCodeAdapter::ERROR);
    delete[] bridge_impl.config_data_cache_.config_data_addr;
    delete[] keyframe_buffer.buffer_data.addr;
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest007)
{
    auto signal = std::make_shared<CodecBridgeSignal>();
    EncodeOutputBuffer keyframe_buffer;
    keyframe_buffer.is_contain_config_data = false;
    keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
    keyframe_buffer.buffer_data.addr = new uint8_t[50];  
    keyframe_buffer.index = 1;
    keyframe_buffer.buffer_info.size = 50;
    keyframe_buffer.buffer_info.presentationTimeUs = 1000;
    keyframe_buffer.buffer_data.bufferSize = 50;
    bridge_impl.config_data_cache_.config_data_addr = nullptr;
    signal->out_buffer_queue_.push(keyframe_buffer);
    bridge_impl.config_data_cache_.config_data_size = 1;
    SetSignal(signal);
    uint32_t index;
    BufferInfo info;
    BufferFlag flag;
    OhosBuffer buffer;
    EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer), CodecCodeAdapter::ERROR);
    delete[] bridge_impl.config_data_cache_.config_data_addr;
    delete[] keyframe_buffer.buffer_data.addr;
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest008)
{
    auto signal = std::make_shared<CodecBridgeSignal>();
    EncodeOutputBuffer keyframe_buffer;
    keyframe_buffer.index = 1;
    keyframe_buffer.buffer_info.size = 50;
    keyframe_buffer.buffer_data.bufferSize = 50;
    keyframe_buffer.is_contain_config_data = false;
    keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
    keyframe_buffer.buffer_info.presentationTimeUs = 1000;
    keyframe_buffer.buffer_data.addr = new uint8_t[50];
    signal->out_buffer_queue_.push(keyframe_buffer);
    SetSignal(signal);
    SimulateKeyFrameMemoryFailure();
    uint32_t index;
    BufferInfo info;
    BufferFlag flag;
    OhosBuffer buffer;
    EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer), CodecCodeAdapter::ERROR);
    delete[] bridge_impl.config_data_cache_.config_data_addr;
    delete[] keyframe_buffer.buffer_data.addr;
}

TEST_F(OHOSMediaCodecBridgeImplTest, DequeueOutputBufferTest009)
{
    auto signal = std::make_shared<CodecBridgeSignal>();
    EncodeOutputBuffer keyframe_buffer;
    keyframe_buffer.index = 1;
    keyframe_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME;
    keyframe_buffer.buffer_data.addr = new uint8_t[50];
    keyframe_buffer.is_contain_config_data = false;
    keyframe_buffer.buffer_data.bufferSize = 50;
    keyframe_buffer.buffer_info.presentationTimeUs = 1000;
    keyframe_buffer.buffer_info.size = 50;
    bridge_impl.config_data_cache_.config_data_addr = new uint8_t[30];
    bridge_impl.config_data_cache_.config_data_size = 20;
    bridge_impl.config_data_cache_.config_info_size = 40;
    signal->out_buffer_queue_.push(keyframe_buffer);
    SetSignal(signal);
    uint32_t index;
    BufferInfo info;
    BufferFlag flag;
    OhosBuffer buffer;
    EXPECT_EQ(bridge_impl.DequeueOutputBuffer(index, info, flag, buffer), CodecCodeAdapter::OK);
    delete[] bridge_impl.config_data_cache_.config_data_addr;
    delete[] keyframe_buffer.buffer_data.addr;
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBuffer_ShouldReturnError_WhenSurfaceIsNull)
{
    SetSurface(nullptr);
    CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(nullptr, 1);
    EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBuffer_ShouldReturnError_WhenFrameIsNull)
{
    SetSurface(std::make_shared<MockProducerSurfaceAdapter>());
    CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(nullptr, 1);
    EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBuffer_ShouldReturnError_WhenRequestBufferFails)
{
    std::shared_ptr<MockProducerSurfaceAdapter> mock_producer_surface_adapter =
    std::make_shared<MockProducerSurfaceAdapter>();
    EXPECT_CALL(*mock_producer_surface_adapter, RequestBuffer).WillOnce(testing::Return(nullptr));
    SetSurface(mock_producer_surface_adapter);
    CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(video_frame, 1);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBuffer_ShouldReturnError_WhenDstIsNull)
{
    std::shared_ptr<MockProducerSurfaceAdapter> mock_producer_surface_adapter =
    std::make_shared<MockProducerSurfaceAdapter>();
    auto buffer_adapter = std::make_shared<MockSurfaceBufferAdapter>();
    EXPECT_CALL(*mock_producer_surface_adapter, RequestBuffer).WillOnce(testing::Return(buffer_adapter));
    EXPECT_CALL(*buffer_adapter, GetVirAddr()).WillOnce(testing::Return(nullptr));
    EXPECT_CALL(*buffer_adapter, GetStride()).WillOnce(testing::Return(1));
    SetSurface(mock_producer_surface_adapter);
    CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(video_frame, 1);
    EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBuffer_ShouldReturnError_WhenYSrcIsNull)
{
    std::shared_ptr<MockProducerSurfaceAdapter> mock_producer_surface_adapter =
    std::make_shared<MockProducerSurfaceAdapter>();
    auto buffer_adapter = std::make_shared<MockSurfaceBufferAdapter>();
    EXPECT_CALL(*mock_producer_surface_adapter, RequestBuffer).WillOnce(testing::Return(buffer_adapter));
    std::unique_ptr<uint8_t> dst_data = std::make_unique<uint8_t>(1);
    EXPECT_CALL(*buffer_adapter, GetVirAddr()).WillOnce(testing::Return(dst_data.get()));
    EXPECT_CALL(*buffer_adapter, GetStride()).WillOnce(testing::Return(1));
    SetSurface(mock_producer_surface_adapter);
    std::unique_ptr<uint8_t> data_ptr = std::make_unique<uint8_t>(1);
    SetStorageType(VideoFrame::STORAGE_UNOWNED_MEMORY);
    SetData(VideoFrame::kYPlane, nullptr);
    SetData(VideoFrame::kUPlane, data_ptr.get());
    SetData(VideoFrame::kVPlane, data_ptr.get());
    CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(video_frame, 1);
    EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBuffer_ShouldReturnError_WhenUSrcIsNull)
{
    std::shared_ptr<MockProducerSurfaceAdapter> mock_producer_surface_adapter =
    std::make_shared<MockProducerSurfaceAdapter>();
    auto buffer_adapter = std::make_shared<MockSurfaceBufferAdapter>();
    EXPECT_CALL(*mock_producer_surface_adapter, RequestBuffer).WillOnce(testing::Return(buffer_adapter));
    std::unique_ptr<uint8_t> dst_data = std::make_unique<uint8_t>(1);
    EXPECT_CALL(*buffer_adapter, GetVirAddr()).WillOnce(testing::Return(dst_data.get()));
    EXPECT_CALL(*buffer_adapter, GetStride()).WillOnce(testing::Return(1));
    SetSurface(mock_producer_surface_adapter);
    std::unique_ptr<uint8_t> data_ptr = std::make_unique<uint8_t>(1);
    SetStorageType(VideoFrame::STORAGE_UNOWNED_MEMORY);
    SetData(VideoFrame::kYPlane, data_ptr.get());
    SetData(VideoFrame::kUPlane, nullptr);
    SetData(VideoFrame::kVPlane, data_ptr.get());
    CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(video_frame, 1);
    EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBuffer_ShouldReturnError_WhenVSrcIsNull)
{
    std::shared_ptr<MockProducerSurfaceAdapter> mock_producer_surface_adapter =
    std::make_shared<MockProducerSurfaceAdapter>();
    auto buffer_adapter = std::make_shared<MockSurfaceBufferAdapter>();
    EXPECT_CALL(*mock_producer_surface_adapter, RequestBuffer).WillOnce(testing::Return(buffer_adapter));
    std::unique_ptr<uint8_t> dst_data = std::make_unique<uint8_t>(1);
    EXPECT_CALL(*buffer_adapter, GetVirAddr()).WillOnce(testing::Return(dst_data.get()));
    EXPECT_CALL(*buffer_adapter, GetStride()).WillOnce(testing::Return(1));
    SetSurface(mock_producer_surface_adapter);
    std::unique_ptr<uint8_t> data_ptr = std::make_unique<uint8_t>(1);
    SetStorageType(VideoFrame::STORAGE_UNOWNED_MEMORY);
    SetData(VideoFrame::kYPlane, data_ptr.get());
    SetData(VideoFrame::kUPlane, data_ptr.get());
    SetData(VideoFrame::kVPlane, nullptr);
    CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(video_frame, 1);
    EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBuffer_ShouldReturnError_WhenSrcIsNotNull)
{
    std::shared_ptr<MockProducerSurfaceAdapter> mock_producer_surface_adapter =
    std::make_shared<MockProducerSurfaceAdapter>();
    auto buffer_adapter = std::make_shared<MockSurfaceBufferAdapter>();
    EXPECT_CALL(*mock_producer_surface_adapter, RequestBuffer).WillOnce(testing::Return(buffer_adapter));
    std::unique_ptr<uint8_t> dst_data = std::make_unique<uint8_t>(1);
    EXPECT_CALL(*buffer_adapter, GetVirAddr()).WillOnce(testing::Return(dst_data.get()));
    EXPECT_CALL(*buffer_adapter, GetStride()).WillOnce(testing::Return(1));
    SetSurface(mock_producer_surface_adapter);
    std::unique_ptr<uint8_t> data_ptr = std::make_unique<uint8_t>(1);
    SetStorageType(VideoFrame::STORAGE_UNOWNED_MEMORY);
    SetData(VideoFrame::kYPlane, data_ptr.get());
    SetData(VideoFrame::kUPlane, data_ptr.get());
    SetData(VideoFrame::kVPlane, data_ptr.get());
    CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(video_frame, 1);
    EXPECT_EQ(result, CodecCodeAdapter::OK);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBuffer_ShouldReturnError_WhenFlushBufferIsError)
{
    std::shared_ptr<MockProducerSurfaceAdapter> mock_producer_surface_adapter =
    std::make_shared<MockProducerSurfaceAdapter>();
    auto buffer_adapter = std::make_shared<MockSurfaceBufferAdapter>();
    EXPECT_CALL(*mock_producer_surface_adapter, RequestBuffer).WillOnce(testing::Return(buffer_adapter));
    std::unique_ptr<uint8_t> dst_data = std::make_unique<uint8_t>(1);
    EXPECT_CALL(*buffer_adapter, GetVirAddr()).WillOnce(testing::Return(dst_data.get()));
    EXPECT_CALL(*buffer_adapter, GetStride()).WillOnce(testing::Return(1));
    SetSurface(mock_producer_surface_adapter);
    std::unique_ptr<uint8_t> data_ptr = std::make_unique<uint8_t>(1);
    SetStorageType(VideoFrame::STORAGE_UNOWNED_MEMORY);
    SetData(VideoFrame::kYPlane, data_ptr.get());
    SetData(VideoFrame::kUPlane, data_ptr.get());
    SetData(VideoFrame::kVPlane, data_ptr.get());
    EXPECT_CALL(*mock_producer_surface_adapter, FlushBuffer).WillOnce(testing::Return(1));
    CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(video_frame, 1);
    EXPECT_EQ(result, CodecCodeAdapter::ERROR);
}

TEST_F(OHOSMediaCodecBridgeImplTest, FillSurfaceBuffer_ShouldReturnError_WhenFlushBufferIsOk)
{
    std::shared_ptr<MockProducerSurfaceAdapter> mock_producer_surface_adapter =
    std::make_shared<MockProducerSurfaceAdapter>();
    auto buffer_adapter = std::make_shared<MockSurfaceBufferAdapter>();
    EXPECT_CALL(*mock_producer_surface_adapter, RequestBuffer).WillOnce(testing::Return(buffer_adapter));
    std::unique_ptr<uint8_t> dst_data = std::make_unique<uint8_t>(1);
    EXPECT_CALL(*buffer_adapter, GetVirAddr()).WillOnce(testing::Return(dst_data.get()));
    EXPECT_CALL(*buffer_adapter, GetStride()).WillOnce(testing::Return(1));
    SetSurface(mock_producer_surface_adapter);
    std::unique_ptr<uint8_t> data_ptr = std::make_unique<uint8_t>(1);
    SetStorageType(VideoFrame::STORAGE_UNOWNED_MEMORY);
    SetData(VideoFrame::kYPlane, data_ptr.get());
    SetData(VideoFrame::kUPlane, data_ptr.get());
    SetData(VideoFrame::kVPlane, data_ptr.get());
    EXPECT_CALL(*mock_producer_surface_adapter, FlushBuffer).WillOnce(testing::Return(0));
    CodecCodeAdapter result = bridge_impl.FillSurfaceBuffer(video_frame, 1);
    EXPECT_EQ(result, CodecCodeAdapter::OK);
}

TEST_F(CodecEncodeBridgeCallbackTest, OnNeedOutputData_NullBuffer)
{
    testing::internal::CaptureStderr();
    std::shared_ptr<BufferInfoAdapter> valid_info = std::make_shared<InheritBufferInfoAdapter>();
    std::shared_ptr<OhosBufferAdapter> null_buffer = nullptr;
    callback_->is_running_.store(false);
    callback_->OnNeedOutputData(0, valid_info, BufferFlag::CODEC_BUFFER_FLAG_NONE, null_buffer);
    std::string log_output1 = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output1.find("Output is invalid"), std::string::npos);
}

TEST_F(CodecEncodeBridgeCallbackTest, OnNeedOutputData_ShouldReturn_WhenIsRunningIsFalse)
{
    testing::internal::CaptureStderr();
    std::shared_ptr<BufferInfoAdapter> valid_info = std::make_shared<InheritBufferInfoAdapter>();
    std::shared_ptr<OhosBufferAdapter> valid_buffer = std::make_shared<InheritOhosBufferAdapter>();
    callback_->is_running_.store(false);
    callback_->OnNeedOutputData(0, valid_info, BufferFlag::CODEC_BUFFER_FLAG_NONE, valid_buffer);
    std::string log_output1 = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output1.find("encoder is not running"), std::string::npos);
}

TEST_F(CodecEncodeBridgeCallbackTest, OnNeedOutputData_ShouldReturn_WhenRunsTasksInCurrentSequenceFalse)
{
    testing::internal::CaptureStderr();
    std::shared_ptr<BufferInfoAdapter> valid_info = std::make_shared<InheritBufferInfoAdapter>();
    std::shared_ptr<OhosBufferAdapter> valid_buffer = std::make_shared<InheritOhosBufferAdapter>();
    callback_->is_running_.store(true);
    MockSequencedTaskRunner mock_task_runner_;
    EXPECT_CALL(mock_task_runner_, RunsTasksInCurrentSequence())
      .WillOnce(testing::Return(false));
    EXPECT_CALL(mock_task_runner_,
              PostDelayedTask(testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(false));
    callback_->codec_callback_task_runner_ = &mock_task_runner_;
    callback_->OnNeedOutputData(0, valid_info, BufferFlag::CODEC_BUFFER_FLAG_NONE, valid_buffer);
    std::string log_output1 = testing::internal::GetCapturedStderr();
    EXPECT_EQ(log_output1.find("encoder is not running"),
            std::string::npos);
}

TEST_F(CodecEncodeBridgeCallbackTest, OnNeedOutputData_ShouldReturn_WhenFlagIsCodecData)
{
    testing::internal::CaptureStderr();
    std::shared_ptr<BufferInfoAdapter> valid_info = std::make_shared<InheritBufferInfoAdapter>();
    std::shared_ptr<OhosBufferAdapter> valid_buffer = std::make_shared<InheritOhosBufferAdapter>();
    callback_->is_running_.store(true);
    MockSequencedTaskRunner mock_task_runner_;
    EXPECT_CALL(mock_task_runner_, RunsTasksInCurrentSequence())
      .WillOnce(testing::Return(true));
    callback_->codec_callback_task_runner_ = &mock_task_runner_;
    callback_->OnNeedOutputData(0, valid_info, BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA, valid_buffer);
    std::string log_output1 = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output1.find("flag is codec_data, handle with keyframe later"),
            std::string::npos);
}

TEST_F(CodecEncodeBridgeCallbackTest, OnNeedOutputData_ShouldReturn_WhenFlagIsSyncFrame)
{
    testing::internal::CaptureStderr();
    std::shared_ptr<BufferInfoAdapter> valid_info = std::make_shared<InheritBufferInfoAdapter>();
    std::shared_ptr<OhosBufferAdapter> valid_buffer = std::make_shared<InheritOhosBufferAdapter>();
    callback_->is_running_.store(true);
    MockSequencedTaskRunner mock_task_runner_;
    EXPECT_CALL(mock_task_runner_, RunsTasksInCurrentSequence())
      .WillOnce(testing::Return(true));
    callback_->codec_callback_task_runner_ = &mock_task_runner_;
    callback_->OnNeedOutputData(0, valid_info, BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME, valid_buffer);
    std::string log_output1 = testing::internal::GetCapturedStderr();
    EXPECT_EQ(log_output1.find("handle keyframe now"), std::string::npos);
}

TEST_F(CodecEncodeBridgeCallbackTest, OnNeedOutputData_ShouldReturn_WhenFlagIsSyncFrame2)
{
    testing::internal::CaptureStderr();
    std::shared_ptr<BufferInfoAdapter> valid_info = std::make_shared<InheritBufferInfoAdapter>();
    std::shared_ptr<OhosBufferAdapter> valid_buffer = std::make_shared<InheritOhosBufferAdapter>();
    callback_->is_running_.store(true);
    MockSequencedTaskRunner mock_task_runner_;
    EXPECT_CALL(mock_task_runner_, RunsTasksInCurrentSequence())
      .WillOnce(testing::Return(true));
    callback_->codec_callback_task_runner_ = &mock_task_runner_;
    callback_->config_data_.buffer_data.bufferSize = 8;
    callback_->OnNeedOutputData(0, valid_info, BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME, valid_buffer);
    std::string log_output1 = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output1.find("handle keyframe now"), std::string::npos);
}
}  // namespace media
