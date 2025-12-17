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

#include "base/task/sequenced_task_runner.h"
#include "base/test/mock_callback.h"
#include "base/test/task_environment.h"
#include "base/threading/thread.h"
#include "base/time/time.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#if BUILDFLAG(ARKWEB_TEST)
#define private public
#include "arkweb/chromium_ext/media/gpu/ohos/codec_wrapper.h"
#undef private
#else
#include "arkweb/chromium_ext/media/gpu/ohos/codec_wrapper.h"
#endif //ARKWEB_TEST

using testing::_;
using testing::DoAll;
using testing::NiceMock;
using testing::Return;

namespace media {

class MockMediaCodecDecoderAdapter : public MediaCodecDecoderAdapter {
 public:
  MOCK_METHOD(DecoderAdapterCode,
              CreateVideoDecoderByMime,
              (const std::string& mime),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              CreateVideoDecoderByName,
              (const std::string& name),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              ConfigureDecoder,
              (std::shared_ptr<DecoderFormatAdapter> adapter),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              SetParameterDecoder,
              (std::shared_ptr<DecoderFormatAdapter> adapter),
              (override));
  MOCK_METHOD(DecoderAdapterCode, SetOutputSurface, (void* window), (override));
  MOCK_METHOD(DecoderAdapterCode, PrepareDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, StartDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, StopDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, FlushDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, ReleaseDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, ResetDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode,
              QueueInputBufferDec,
              (uint32_t, int64_t, int32_t, int32_t, BufferFlag),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              GetOutputFormatDec,
              (std::shared_ptr<DecoderFormatAdapter> format),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              ReleaseOutputBufferDec,
              (uint32_t, bool),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              SetCallbackDec,
              (std::shared_ptr<DecoderCallbackAdapter> adapter),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              SetDecryptionConfig,
              (void*, bool),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              SetAVCencInfo,
              (uint32_t, const std::shared_ptr<AudioCencInfoAdapter>),
              (override));
};

class MockDecoderBridgeSignal : public DecoderBridgeSignal {
 public:
  MOCK_METHOD(void, pop, (), ());
  MOCK_METHOD(bool, isOnError, (), ());
  MOCK_METHOD(bool, isDecoderFlushing, (), ());
  MOCK_METHOD(size_t, size, (), ());
  MOCK_METHOD(uint32_t, front_inputBufferIndex, (), ());
  MOCK_METHOD(OhosBuffer, front_inputBuffer, (), ());
  MOCK_METHOD(bool, empty, (), ());
  MOCK_METHOD(int64_t, front_outputBufferInfo_presentationTimeUs, (), ());
  MOCK_METHOD(uint32_t, front_outputBufferIndex, (), ());
  MOCK_METHOD(BufferFlag, front_outputBufferFlag, (), ());
  MOCK_METHOD(void,
              swap_inputQueue_,
              (std::queue<VideoBridgeDecoderInputBuffer>&),
              ());
  MOCK_METHOD(void,
              swap_outputQueue_,
              (std::queue<VideoBridgeDecoderOutputBuffer>&),
              ());
};

class MockCodecSurfaceBundle : public CodecSurfaceBundle {
 public:
  MockCodecSurfaceBundle() = default;
  ~MockCodecSurfaceBundle() = default;
  void* GetOHOSNativeWindow()  { return nullptr; }
};

class CodecWrapperTest : public testing::Test {

public:
  void SetUp() override {
    task_runner_ = base::SequencedTaskRunner::GetCurrentDefault();
    mock_adapter_ = std::make_unique<NiceMock<MockMediaCodecDecoderAdapter>>();
    signal_ = std::make_shared<NiceMock<MockDecoderBridgeSignal>>();
    VideoBridgeCodecConfig config;
    config.codec = media::VideoCodec::kH264;
    config.on_buffers_available_cb = base::DoNothing();
    bridge_ = MediaCodecDecoderBridgeImpl::CreateVideoDecoder(config);
    adapter_ = mock_adapter_.get();
    bridge_->videoDecoder_ = std::move(mock_adapter_);
    bridge_->signal_ = signal_;
    DecoderFormat format;
    format.width = 640;
    format.height = 480;
    format.frameRate = 30.0;
    bridge_->ConfigureBridgeDecoder(format, base::SequencedTaskRunner::GetCurrentDefault());
    surface_bundle_ = base::MakeRefCounted<CodecSurfaceBundle>();
    CodecSurfacePair codec_surface_pair = {
      std::move(bridge_),
      surface_bundle_
    };
    wrapper_ = std::make_unique<CodecWrapper>(std::move(codec_surface_pair),
    output_buffer_release_cb_.Get(), task_runner_);
    uint8_t data = 0;
    fake_decoder_buffer_ = DecoderBuffer::CopyFrom(base::span_from_ref(data));
  }

  void TearDown() override {
    if (wrapper_) {
        wrapper_.reset();
    }
    mock_adapter_.reset();
    signal_.reset();
    bridge_.reset();
    surface_bundle_.reset();
  }

  std::unique_ptr<CodecOutputBuffer> DequeueCodecOutputBuffer() {
    std::unique_ptr<CodecOutputBuffer> codec_buffer;
    wrapper_->DequeueOutputBuffer(nullptr, nullptr, &codec_buffer);
    return codec_buffer;
  }

protected:
  scoped_refptr<base::SequencedTaskRunner> task_runner_;
  base::test::TaskEnvironment task_environment_;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> bridge_;
  scoped_refptr<CodecSurfaceBundle> surface_bundle_;
  testing::NiceMock<base::MockCallback<CodecWrapper::OutputReleasedCB>> output_buffer_release_cb_;
  std::unique_ptr<CodecWrapper> wrapper_;
  std::shared_ptr<NiceMock<MockDecoderBridgeSignal>> signal_;
  std::unique_ptr<NiceMock<MockMediaCodecDecoderAdapter>> mock_adapter_;
  scoped_refptr<DecoderBuffer> fake_decoder_buffer_;
  MockMediaCodecDecoderAdapter* adapter_;
};

TEST_F(CodecWrapperTest, TakeCodecSurfacePair)
{
  auto pair1 = wrapper_->TakeCodecSurfacePair();
  ASSERT_TRUE(pair1.first != nullptr);

  auto pair2 = wrapper_->TakeCodecSurfacePair();
  ASSERT_TRUE(pair2.first == nullptr);
}

TEST_F(CodecWrapperTest, HasUnreleasedOutputBuffers)
{
  ASSERT_FALSE(wrapper_->HasUnreleasedOutputBuffers());
}

TEST_F(CodecWrapperTest, FlushInvalidatesCodecOutputBuffers001) {
  auto codec_buffer = DequeueCodecOutputBuffer();
  EXPECT_CALL(*adapter_, FlushDecoder())
    .WillOnce(Return(DecoderAdapterCode::DECODER_ERROR));
  ASSERT_FALSE(wrapper_->Flush());
}

TEST_F(CodecWrapperTest, FlushInvalidatesCodecOutputBuffers002) {
  EXPECT_CALL(*adapter_, FlushDecoder())
    .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*adapter_, StartDecoder())
    .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  auto codec_buffer = DequeueCodecOutputBuffer();
  ASSERT_TRUE(wrapper_->Flush());
  ASSERT_TRUE(wrapper_->IsFlushed());
}

TEST_F(CodecWrapperTest, QueueInputBuffer) {
  auto eos = DecoderBuffer::CreateEOSBuffer();
  ASSERT_TRUE(wrapper_->QueueInputBuffer(*eos) == CodecWrapper::QueueStatus::kOk);
}

TEST_F(CodecWrapperTest, DequeueOutputBuffer_ElidedEosPending) {
  auto eos = DecoderBuffer::CreateEOSBuffer();
  ASSERT_EQ(wrapper_->QueueInputBuffer(*eos), CodecWrapper::QueueStatus::kOk);

  base::TimeDelta presentation_time;
  bool end_of_stream = false;
  std::unique_ptr<CodecOutputBuffer> codec_buffer;
  auto result = wrapper_->DequeueOutputBuffer(&presentation_time, &end_of_stream, &codec_buffer);

  ASSERT_EQ(result, CodecWrapper::DequeueStatus::kOk);
  ASSERT_TRUE(end_of_stream);
  ASSERT_TRUE(wrapper_->IsDrained());
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
TEST_F(CodecWrapperTest, SetSurfaceInvalidatesCodecOutputBuffers) {
  ASSERT_FALSE(wrapper_->SetSurface(base::MakeRefCounted<CodecSurfaceBundle>()));

  int32_t value = 1;
  intptr_t int_as_ptr = reinterpret_cast<intptr_t>(&value);
  void* window = reinterpret_cast<void*>(int_as_ptr);
  auto sorce_bundle = base::MakeRefCounted<CodecSurfaceBundle>();
  sorce_bundle->ohos_native_window_ = window;

  ON_CALL(*adapter_, SetOutputSurface(_))
      .WillByDefault(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*adapter_, ReleaseDecoder()).Times(1);
  ASSERT_TRUE(sorce_bundle != nullptr);
  ASSERT_TRUE(wrapper_->SetSurface(sorce_bundle));
}

TEST_F(CodecWrapperTest, SetVideoSurface) {
  int32_t widget_id = -1;
  ASSERT_NO_FATAL_FAILURE(wrapper_->SetVideoSurface(widget_id));
}
#endif // ARKWEB_VIDEO_ASSISTANT

TEST_F(CodecWrapperTest, DiscardOutputBuffers) {
  ASSERT_NO_FATAL_FAILURE(wrapper_->DiscardOutputBuffers());
}

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
TEST_F(CodecWrapperTest, RecycleDmaBuffer) {
  ASSERT_NO_FATAL_FAILURE(wrapper_->RecycleDmaBuffer());
}

TEST_F(CodecWrapperTest, ResumeDmaBuffer) {
  ASSERT_NO_FATAL_FAILURE(wrapper_->ResumeDmaBuffer());
}
#endif  // ARKWEB_MEDIA_DMABUF

TEST_F(CodecWrapperTest, SurfaceBundle) {
  auto surface_bundle = wrapper_->SurfaceBundle();
  EXPECT_NE(surface_bundle, nullptr);
}

TEST_F(CodecWrapperTest, SetDecryptionConfig) {
  void *session = nullptr;
  bool isSecure = true;
  EXPECT_CALL(*adapter_, SetDecryptionConfig(_, _))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  auto result = wrapper_->SetDecryptionConfig(session, isSecure);
  EXPECT_TRUE(result);
}
}  // namespace media
