/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * mock ohos_sdk api and struct used in current unit test
 */
#define NATIVE_AVCODEC_VIDEODECODER_H
#define NATIVE_AVFORMAT_H
#define NATIVE_AVERRORS_H
#define NATIVE_AVBUFFER_H
#define NATIVE_AVCODEC_BASE_H
#define NDK_INCLUDE_EXTERNAL_NATIVE_WINDOW_H_

#include <cstdint>
#include <cstdio>

extern "C" {
struct OH_AVCodec {
  int32_t id;
};

struct MediaKeySession {
  int32_t id;
};

struct OH_AVFormat {
  int32_t id;
};

struct OHNativeWindow {
  int32_t id;
};

struct OH_AVCodecBufferAttr {
  int32_t id;
};

struct OH_AVBuffer {
  int32_t id;
};
enum OH_BitrateMode {
  /** Constant Bit rate mode. */
  BITRATE_MODE_CBR = 0,
  /** Variable Bit rate mode. */
  BITRATE_MODE_VBR = 1,
  /** Constant Quality mode. */
  BITRATE_MODE_CQ = 2
};

enum OH_AVErrCode {
  /**
   * @error the operation completed successfully.
   */
  AV_ERR_OK = 0,
};

OH_AVCodec* OH_VideoDecoder_CreateByMime(const char* mime) {
  OH_AVCodec* codecPtr = new OH_AVCodec();
  return reinterpret_cast<OH_AVCodec*>(codecPtr);
}

OH_AVCodec* OH_VideoDecoder_CreateByName(const char* name) {
  OH_AVCodec* codecPtr = new OH_AVCodec();
  return reinterpret_cast<OH_AVCodec*>(codecPtr);
}

OH_AVFormat* OH_AVFormat_Create(void) {
  OH_AVFormat* formatPtr = new OH_AVFormat();
  return reinterpret_cast<OH_AVFormat*>(formatPtr);
}

bool OH_AVFormat_SetIntValue(struct OH_AVFormat* format,
                             const char* key,
                             int32_t value) {
  return true;
}

OH_AVErrCode OH_VideoDecoder_Configure(OH_AVCodec* codec, OH_AVFormat* format) {
  return OH_AVErrCode::AV_ERR_OK;
}

void OH_AVFormat_Destroy(struct OH_AVFormat* format) {
  // no nothing
}

OH_AVErrCode OH_VideoDecoder_SetParameter(OH_AVCodec* codec,
                                          OH_AVFormat* format) {
  return OH_AVErrCode::AV_ERR_OK;
}

OH_AVErrCode OH_VideoDecoder_SetSurface(OH_AVCodec* codec,
                                        OHNativeWindow* window) {
  return OH_AVErrCode::AV_ERR_OK;
}

OH_AVFormat* OH_VideoDecoder_GetOutputDescription(OH_AVCodec* codec) {
  OH_AVFormat* formatPtr = new OH_AVFormat();
  return reinterpret_cast<OH_AVFormat*>(formatPtr);
}

bool OH_AVFormat_GetIntValue(struct OH_AVFormat* format,
                             const char* key,
                             int32_t* out) {
  return true;
}

OH_AVErrCode OH_VideoDecoder_Prepare(OH_AVCodec* codec) {
  return OH_AVErrCode::AV_ERR_OK;
}

OH_AVErrCode OH_VideoDecoder_Start(OH_AVCodec* codec) {
  return OH_AVErrCode::AV_ERR_OK;
}

OH_AVErrCode OH_VideoDecoder_Stop(OH_AVCodec* codec) {
  return OH_AVErrCode::AV_ERR_OK;
}

OH_AVErrCode OH_VideoDecoder_Destroy(OH_AVCodec* codec) {
  return OH_AVErrCode::AV_ERR_OK;
}

OH_AVErrCode OH_AVBuffer_SetBufferAttr(OH_AVBuffer* buffer,
                                       const OH_AVCodecBufferAttr* attr) {
  return OH_AVErrCode::AV_ERR_OK;
}

OH_AVErrCode OH_VideoDecoder_PushInputBuffer(OH_AVCodec* codec,
                                             uint32_t index) {
  return OH_AVErrCode::AV_ERR_OK;
}

OH_AVErrCode OH_VideoDecoder_RenderOutputBuffer(OH_AVCodec* codec,
                                                uint32_t index) {
  return OH_AVErrCode::AV_ERR_OK;
}

OH_AVErrCode OH_VideoDecoder_FreeOutputBuffer(OH_AVCodec* codec,
                                              uint32_t index) {
  return OH_AVErrCode::AV_ERR_OK;
}
}

#include "base/functional/callback_helpers.h"
#define private public
#include "ohos_media_decoder_bridge_impl.h"
#undef private
#include <memory>
#include "base/logging.h"
#include "base/task/single_thread_task_executor.h"
#include "base/task/task_runner.h"
#include "base/trace_event/trace_event.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#include "base/base_paths.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/location.h"
#include "base/threading/thread.h"
#include "base/threading/thread_restrictions.h"
#include "base/time/time.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::MockFunction;
using ::testing::NiceMock;
using ::testing::Ref;
using ::testing::Return;

using namespace std;

namespace testing {}  // namespace testing

namespace media {

class MockDecoderBridgeSignal : public DecoderBridgeSignal {
 public:
  MOCK_METHOD(void, pop, (), ());
  MOCK_METHOD(size_t, size, (), ());
  MOCK_METHOD(uint32_t, front_inputBufferIndex, (), ());
  MOCK_METHOD(OhosBuffer, front_inputBuffer, (), ());
  MOCK_METHOD(bool, empty, (), ());
  MOCK_METHOD(int64_t, front_outputBufferInfo_presentationTimeUs, (), ());
  MOCK_METHOD(uint32_t, front_outputBufferIndex, (), ());
  MOCK_METHOD(void,
              swap_inputQueue_,
              (std::queue<VideoBridgeDecoderInputBuffer>&),
              ());
  MOCK_METHOD(void,
              swap_outputQueue_,
              (std::queue<VideoBridgeDecoderOutputBuffer>&),
              ());
};

class MockSequencedTaskRunner : public base::SequencedTaskRunner {
 public:
  MOCK_METHOD(bool,
              PostNonNestableTask,
              (const base::Location& location, base::OnceClosure onceClosure),
              ());
  MOCK_METHOD(bool,
              PostNonNestableDelayedTask,
              (const base::Location& location,
               base::OnceClosure onceClosure,
               base::TimeDelta timeDelta),
              (override));
  MOCK_METHOD(base::DelayedTaskHandle,
              PostCancelableDelayedTask,
              (base::subtle::PostDelayedTaskPassKey postDelayedTaskPassKey,
               const base::Location& location,
               base::OnceClosure onceClosure,
               base::TimeDelta timeDelta),
              (override));
  MOCK_METHOD(base::DelayedTaskHandle,
              PostCancelableDelayedTaskAt,
              (base::subtle::PostDelayedTaskPassKey postDelayedTaskPassKey,
               const base::Location& location,
               base::OnceClosure onceClosure,
               base::TimeTicks timeTicks,
               base::subtle::DelayPolicy delayPolicy),
              (override));
  MOCK_METHOD(bool,
              PostDelayedTaskAt,
              (base::subtle::PostDelayedTaskPassKey postDelayedTaskPassKey,
               const base::Location& location,
               base::OnceClosure onceClosure,
               base::TimeTicks timeTicks,
               base::subtle::DelayPolicy delayPolicy),
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

class MediaCodecDecoderBridgeImplTest : public ::testing::Test {
 protected:
  void SetUp() override {
    signal_ = std::make_shared<NiceMock<MockDecoderBridgeSignal>>();

    bridge_ = std::make_unique<MediaCodecDecoderBridgeImpl>("video/avc");
    std::string decoder_name = "mock";
    bridge_->video_decoder_ =
        OH_VideoDecoder_CreateByName(decoder_name.c_str());

    bridge_->signal_ = signal_;
    callback_ = std::make_shared<CodecBridgeCallback>(signal_);

    mock_signal_ = std::make_shared<NiceMock<DecoderBridgeSignal>>();
    callback_c = std::make_shared<CodecBridgeCallback>(mock_signal_);

    // video_decoder_ = new testing::NiceMock<MockMediaCodecDecoderAdapter>;
  }

  void TearDown() override {
    bridge_.reset();
    signal_.reset();
    callback_.reset();
  }

  std::shared_ptr<CodecBridgeCallback> callback_;
  std::shared_ptr<NiceMock<MockDecoderBridgeSignal>> signal_;

  std::unique_ptr<MediaCodecDecoderBridgeImpl> bridge_;

  std::shared_ptr<NiceMock<DecoderBridgeSignal>> mock_signal_;
  std::shared_ptr<CodecBridgeCallback> callback_c;

  bool is_first_decFrame_ = true;
};

/**
 * test CreateVideoDecoder should success for H264
 */
TEST_F(MediaCodecDecoderBridgeImplTest, CreateVideoDecoder) {
  VideoBridgeCodecConfig config;
  config.codec = media::VideoCodec::kH264;
  config.on_buffers_available_cb = base::DoNothing();
  auto bridge_impl = MediaCodecDecoderBridgeImpl::CreateVideoDecoder(config);
  ASSERT_NE(bridge_impl, nullptr);
}

/**
 * test CreateVideoDecoder should success for HEVC
 */
TEST_F(MediaCodecDecoderBridgeImplTest, CreateVideoDecoder1) {
  VideoBridgeCodecConfig config;
  config.codec = media::VideoCodec::kHEVC;
  config.on_buffers_available_cb = base::DoNothing();
  auto bridge_impl = MediaCodecDecoderBridgeImpl::CreateVideoDecoder(config);
  ASSERT_NE(bridge_impl, nullptr);
}

/**
 * test CreateVideoDecoder should fail for AV1
 */
TEST_F(MediaCodecDecoderBridgeImplTest, CreateVideoDecoder2) {
  VideoBridgeCodecConfig config;
  config.codec = media::VideoCodec::kAV1;
  config.on_buffers_available_cb = base::DoNothing();
  auto bridge_impl = MediaCodecDecoderBridgeImpl::CreateVideoDecoder(config);
  ASSERT_EQ(bridge_impl, nullptr);
}

/**
 * test CreateVideoBridgeDecoderByMime should result in ERROR when name
 * mis-match
 */
TEST_F(MediaCodecDecoderBridgeImplTest, CreateVideoBridgeDecoderByMime) {
  bridge_ = std::make_unique<MediaCodecDecoderBridgeImpl>("video/avc");
  std::string codec_name = "video/h264";
  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByMime(codec_name);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test CreateVideoBridgeDecoderByMime should result in ERROR when decoder is
 * null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, CreateVideoBridgeDecoderByMime1) {
  bridge_->video_decoder_ = nullptr;
  std::string codec_name = "video/h264";
  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByMime(codec_name);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test CreateVideoBridgeDecoderByName should result in ERROR when name
 * mis-match
 */
TEST_F(MediaCodecDecoderBridgeImplTest, CreateVideoBridgeDecoderByName) {
  bridge_ = std::make_unique<MediaCodecDecoderBridgeImpl>("video/avc");
  std::string codec_name = "video/h264";
  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByName(codec_name);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test CreateVideoBridgeDecoderByName should result in ERROR when decoder is
 * null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, CreateVideoBridgeDecoderByName1) {
  bridge_->video_decoder_ = nullptr;
  std::string codec_name = "video/h264";
  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByName(codec_name);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test ConfigureBridgeDecoder should result in ERROR when decoder is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, ConfigureBridgeDecoder) {
  DecoderFormat format;
  format.width = 640;
  format.height = 480;
  format.frame_rate = 30.0;
  scoped_refptr<base::SequencedTaskRunner> decoder_task_runner = nullptr;
  auto signal = std::make_shared<MockDecoderBridgeSignal>();
  bridge_->cb_ = std::make_shared<CodecBridgeCallback>(signal);
  bridge_->video_decoder_ = nullptr;
  DecoderAdapterCode result =
      bridge_->ConfigureBridgeDecoder(format, decoder_task_runner);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test ConfigureBridgeDecoder should success
 */
TEST_F(MediaCodecDecoderBridgeImplTest, ConfigureBridgeDecoder1) {
  DecoderFormat format;
  format.width = 640;
  format.height = 480;
  format.frame_rate = 30.0;
  scoped_refptr<base::SequencedTaskRunner> decoder_task_runner = nullptr;
  auto signal = std::make_shared<MockDecoderBridgeSignal>();
  bridge_->cb_ = std::make_shared<CodecBridgeCallback>(signal);

  DecoderAdapterCode result =
      bridge_->ConfigureBridgeDecoder(format, decoder_task_runner);

  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

/**
 * test MediaCodecDecoderBridgeImpl should be created
 */
TEST_F(MediaCodecDecoderBridgeImplTest, Constructor) {
  base::RepeatingClosure on_buffers_available_cb = base::DoNothing();
  ASSERT_FALSE(bridge_->CheckHasCreated());
}

/**
 * test SetBridgeParameterDecoder should success
 */
TEST_F(MediaCodecDecoderBridgeImplTest, SetBridgeParameterDecoder) {
  DecoderFormat format;
  format.width = 640;
  format.height = 480;
  format.frame_rate = 30.0;
  DecoderAdapterCode result = bridge_->SetBridgeParameterDecoder(format);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
  ASSERT_EQ(format.width, 640);
  ASSERT_EQ(format.height, 480);
  ASSERT_DOUBLE_EQ(format.frame_rate, 30.0);
}

/**
 * test SetBridgeParameterDecoder should result in ERROR when decoder is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, SetBridgeParameterDecoder1) {
  DecoderFormat format;
  format.width = 640;
  format.height = 480;
  format.frame_rate = 30.0;
  bridge_->video_decoder_ = nullptr;
  DecoderAdapterCode result = bridge_->SetBridgeParameterDecoder(format);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test SetBridgeOutputSurface should result in ERROR when window is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, SetBridgeOutputSurface) {
  void* window = nullptr;
  DecoderAdapterCode result = bridge_->SetBridgeOutputSurface(window);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test SetBridgeOutputSurface should success
 */
TEST_F(MediaCodecDecoderBridgeImplTest, SetBridgeOutputSurface1) {
  int32_t value = 1;
  intptr_t int_as_ptr = reinterpret_cast<intptr_t>(&value);
  void* window = reinterpret_cast<void*>(int_as_ptr);

  DecoderAdapterCode result = bridge_->SetBridgeOutputSurface(window);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

/**
 * test SetBridgeOutputSurface should result in ERROR when window and decoder
 * are null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, SetBridgeOutputSurface2) {
  bridge_->video_decoder_ = nullptr;
  void* window = nullptr;
  DecoderAdapterCode result = bridge_->SetBridgeOutputSurface(window);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test SetBridgeOutputSurface should result in ERROR when decoder is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, SetBridgeOutputSurface3) {
  bridge_->video_decoder_ = nullptr;
  int32_t value = 1;
  intptr_t int_as_ptr = reinterpret_cast<intptr_t>(&value);
  void* window = reinterpret_cast<void*>(int_as_ptr);
  DecoderAdapterCode result = bridge_->SetBridgeOutputSurface(window);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test GetOutputFormatBridgeDecoder should result success
 */
TEST_F(MediaCodecDecoderBridgeImplTest, GetOutputFormatBridgeDecoder) {
  DecoderFormat format;
  format.width = 0;
  format.height = 0;
  format.frame_rate = 0.0;

  DecoderAdapterCode result = bridge_->GetOutputFormatBridgeDecoder(format);

  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
  ASSERT_EQ(format.width, 0);
  ASSERT_EQ(format.height, 0);
  ASSERT_DOUBLE_EQ(format.frame_rate, 0.0);
}

/**
 * test GetOutputFormatBridgeDecoder should result in ERROR when decoder is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, GetOutputFormatBridgeDecoder1) {
  DecoderFormat format;
  format.width = 0;
  format.height = 0;
  format.frame_rate = 0.0;
  bridge_->video_decoder_ = nullptr;

  DecoderAdapterCode result = bridge_->GetOutputFormatBridgeDecoder(format);

  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test PrepareBridgeDecoder should result in ERROR when decoder is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, PrepareBridgeDecoder) {
  bridge_->video_decoder_ = nullptr;
  DecoderAdapterCode result = bridge_->PrepareBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test PrepareBridgeDecoder should success
 */
TEST_F(MediaCodecDecoderBridgeImplTest, PrepareBridgeDecoder1) {
  DecoderAdapterCode result = bridge_->PrepareBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

/**
 * test StartBridgeDecoder should result in ERROR when decoder is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, StartBridgeDecoder) {
  bridge_->video_decoder_ = nullptr;
  DecoderAdapterCode result = bridge_->StartBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test StartBridgeDecoder should success
 */
TEST_F(MediaCodecDecoderBridgeImplTest, StartBridgeDecoder1) {
  DecoderAdapterCode result = bridge_->StartBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

/**
 * test StopBridgeDecoder should result in ERROR when decoder is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, StopBridgeDecoder) {
  bridge_->video_decoder_ = nullptr;
  DecoderAdapterCode result = bridge_->StopBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test ResetBridgeDecoder should success
 */
TEST_F(MediaCodecDecoderBridgeImplTest, StopBridgeDecoder1) {
  DecoderAdapterCode result = bridge_->StopBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

/**
 * test ResetBridgeDecoder should result in ERROR when decoder is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, FlushBridgeDecoderReturns) {
  bridge_->video_decoder_ = nullptr;
  auto result = bridge_->FlushBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test ResetBridgeDecoder should result in ERROR when signal is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, FlushBridgeDecoderReturns1) {
  bridge_->signal_ = nullptr;
  auto result = bridge_->FlushBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test ResetBridgeDecoder should result in ERROR when decoder is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, ResetBridgeDecoderReturns) {
  bridge_->video_decoder_ = nullptr;
  auto result = bridge_->ResetBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test ResetBridgeDecoder should result in ERROR when signal is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, ResetBridgeDecoderReturns1) {
  bridge_->signal_ = nullptr;
  auto result = bridge_->ResetBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test ReleaseBridgeDecoder should result in ERROR when decoder is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, ReleaseBridgeDecoderReturns) {
  bridge_->video_decoder_ = nullptr;
  auto result = bridge_->ReleaseBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test ReleaseBridgeDecoder can success
 */
TEST_F(MediaCodecDecoderBridgeImplTest, ReleaseBridgeDecoderReturns1) {
  auto result = bridge_->ReleaseBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

/**
 * test PopInqueueDec can execute when signal is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, PopInqueueDec) {
  bridge_->signal_ = nullptr;
  bridge_->PopInqueueDec();
}

/**
 * test PopInqueueDec can execute when signal is on_error
 */
TEST_F(MediaCodecDecoderBridgeImplTest, PopInqueueDec1) {
  bridge_->signal_->is_on_error_ = true;
  ASSERT_NE(bridge_->signal_, nullptr);
  bridge_->PopInqueueDec();
}

/**
 * test PopInqueueDec can execute when singnal is not null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, PopInqueueDec2) {
  ASSERT_NE(bridge_->signal_, nullptr);
  bridge_->PopInqueueDec();
}

/**
 * test PopInqueueDec can consume input_queue
 */
TEST_F(MediaCodecDecoderBridgeImplTest, PopInqueueDec3) {
  ASSERT_NE(bridge_->signal_, nullptr);
  uint8_t test = 1;
  uint8_t* addr = &test;
  OH_AVBuffer* av_buffer = nullptr;
  VideoBridgeDecoderInputBuffer buffer = {10, av_buffer, {addr, 10}};
  bridge_->signal_->input_queue_.push(buffer);
  bridge_->PopInqueueDec();
  ASSERT_TRUE(bridge_->signal_->input_queue_.empty());
}

/**
 * test PushInbufferDec can be execute
 */
TEST_F(MediaCodecDecoderBridgeImplTest, PushInbufferDec) {
  OH_AVBuffer* av_buffer = nullptr;
  uint32_t index = 0;
  uint32_t bufferSize = 1024;
  int64_t time = 100000000;

  auto result = bridge_->PushInbufferDec(av_buffer, index, bufferSize, time);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

/**
 * test PushInbufferDecEos can be execute
 */
TEST_F(MediaCodecDecoderBridgeImplTest, PushInbufferDecEos) {
  uint32_t index = 1;

  OH_AVBuffer* av_buffer = nullptr;
  auto result = bridge_->PushInbufferDecEos(av_buffer, index);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

/**
 * test QueueInputBuffer should result in RETRY when input_queue is empty
 */
TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBuffer) {
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  auto result =
      bridge_->QueueInputBuffer(data, data_size, presentation_time, nullptr);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_RETRY);
}

/**
 * test QueueInputBuffer should result in ERROR when signal is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBuffer1) {
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  bridge_->signal_ = nullptr;

  auto result =
      bridge_->QueueInputBuffer(data, data_size, presentation_time, nullptr);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test QueueInputBuffer should result in ERROR when signal is on_error
 */
TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBuffer2) {
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  bridge_->signal_->is_on_error_ = true;

  auto result =
      bridge_->QueueInputBuffer(data, data_size, presentation_time, nullptr);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test QueueInputBuffer should result in RETRY when input_queue is empty
 */
TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBuffer3) {
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  bridge_->signal_->is_decoder_flushing_.store(true);

  auto result =
      bridge_->QueueInputBuffer(data, data_size, presentation_time, nullptr);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_RETRY);
}

/**
 * test QueueInputBuffer should result in RETRY when is_decoder_flushing_ is
 * true
 */
TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBuffer4) {
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  bridge_->signal_->is_decoder_flushing_.store(true);
  uint8_t test = 1;
  uint8_t* addr = &test;
  OH_AVBuffer* av_buffer = nullptr;
  VideoBridgeDecoderInputBuffer buffer = {10, av_buffer, {addr, 10}};
  bridge_->signal_->input_queue_.push(buffer);

  auto result =
      bridge_->QueueInputBuffer(data, data_size, presentation_time, nullptr);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_RETRY);
}

/**
 * test QueueInputBuffer should result in ERROR when decoder is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBuffer5) {
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  uint8_t test = 1;
  uint8_t* addr = &test;
  OH_AVBuffer* av_buffer = nullptr;
  VideoBridgeDecoderInputBuffer buffer = {10, av_buffer, {addr, 10}};
  bridge_->signal_->input_queue_.push(buffer);
  bridge_->video_decoder_ = nullptr;

  auto result =
      bridge_->QueueInputBuffer(data, data_size, presentation_time, nullptr);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test QueueInputBuffer should be execution success
 */
TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBuffer6) {
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  auto test = std::make_unique<uint8_t>();
  uint8_t* addr = test.get();
  OH_AVBuffer* av_buffer = nullptr;
  VideoBridgeDecoderInputBuffer buffer = {10, av_buffer, {addr, 8}};
  bridge_->signal_->input_queue_.push(buffer);

  auto result =
      bridge_->QueueInputBuffer(data, data_size, presentation_time, nullptr);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

/**
 * test QueueInputBufferEOS should result in RETRY when input queue is empty
 */
TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBufferEOS) {
  auto result = bridge_->QueueInputBufferEOS();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_RETRY);
}

/**
 * test QueueInputBufferEOS should result in ERROR when signal is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBufferEOS1) {
  bridge_->signal_ = nullptr;

  auto result = bridge_->QueueInputBufferEOS();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test QueueInputBufferEOS should result in ERROR when signal is on_error
 */
TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBufferEOS2) {
  bridge_->signal_->is_on_error_ = true;

  auto result = bridge_->QueueInputBufferEOS();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test QueueInputBufferEOS should result in RETRY when is_decoder_flushing_ is
 * true
 */
TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBufferEOS3) {
  bridge_->signal_->is_decoder_flushing_.store(true);
  bridge_->is_running_.store(true);
  auto result = bridge_->QueueInputBufferEOS();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_RETRY);
}

/**
 * test QueueInputBufferEOS should result in RETRY when is_running_ is fales
 */
TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBufferEOS4) {
  BufferInfo outputBufferInfo = {0, 0, 0};
  VideoBridgeDecoderOutputBuffer buffer = {10, 1 << 3, outputBufferInfo};
  bridge_->signal_->output_queue_.push(buffer);
  bridge_->is_running_.store(false);

  auto result = bridge_->QueueInputBufferEOS();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_RETRY);
}

/**
 * test QueueInputBufferEOS should result in ERROR when decoder is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBufferEOS5) {
  auto test = std::make_unique<uint8_t>();
  uint8_t* addr = test.get();
  OH_AVBuffer* av_buffer = nullptr;
  VideoBridgeDecoderInputBuffer buffer = {10, av_buffer, {addr, 8}};
  bridge_->signal_->input_queue_.push(buffer);
  bridge_->is_running_.store(true);
  bridge_->video_decoder_ = nullptr;

  auto result = bridge_->QueueInputBufferEOS();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test QueueInputBufferEOS should result in success for buffer
 */
TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBufferEOS6) {
  // auto video_decoder_s = std::make_unique<MockMediaCodecDecoderAdapter>();
  auto test = std::make_unique<uint8_t>();
  uint8_t* addr = test.get();
  OH_AVBuffer* av_buffer = nullptr;
  VideoBridgeDecoderInputBuffer buffer = {10, av_buffer, {addr, 8}};
  bridge_->signal_->input_queue_.push(buffer);

  bridge_->is_running_.store(true);
  auto result = bridge_->QueueInputBufferEOS();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

/**
 * test ReleaseOutputBuffer should execution success
 */
TEST_F(MediaCodecDecoderBridgeImplTest, ReleaseOutputBuffer) {
  uint32_t index = 0;
  bool render = true;
  auto result = bridge_->ReleaseOutputBuffer(index, render);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

/**
 * test ReleaseOutputBuffer should result is ERROR when decoder is null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, ReleaseOutputBuffer1) {
  uint32_t index = 0;
  bool render = true;
  bridge_->video_decoder_ = nullptr;
  auto result = bridge_->ReleaseOutputBuffer(index, render);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test PopOutqueueDec execution should success
 */
TEST_F(MediaCodecDecoderBridgeImplTest, PopOutqueueDec) {
  bridge_->signal_ = nullptr;
  bridge_->PopOutqueueDec();
}

/**
 * test output queue should be empty when last buffer is poped
 */
TEST_F(MediaCodecDecoderBridgeImplTest, PopOutqueueDec1) {
  BufferInfo outputBufferInfo = {0, 0, 0};
  VideoBridgeDecoderOutputBuffer buffer = {10, 1 << 3, outputBufferInfo};
  bridge_->signal_->output_queue_.push(buffer);

  bridge_->PopOutqueueDec();
  ASSERT_TRUE(bridge_->signal_->output_queue_.empty());
}

/**
 * test DequeueOutputBuffer should result in RETRY when empty queue
 */
TEST_F(MediaCodecDecoderBridgeImplTest, DequeueOutputBuffer) {
  base::TimeDelta presentation_time;
  uint32_t index = 1;
  bool eos;
  auto result = bridge_->DequeueOutputBuffer(&presentation_time, index, eos);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_RETRY);
}

/**
 * test DequeueOutputBuffer should return DECODER_ERROR when signal is not
 * defined
 */
TEST_F(MediaCodecDecoderBridgeImplTest, DequeueOutputBuffer1) {
  base::TimeDelta presentation_time;
  uint32_t index = 1;
  bool eos;
  bridge_->signal_ = nullptr;

  auto result = bridge_->DequeueOutputBuffer(&presentation_time, index, eos);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test DequeueOutputBuffer should return DECODER_ERROR when there is error
 * signal
 */
TEST_F(MediaCodecDecoderBridgeImplTest, DequeueOutputBuffer2) {
  base::TimeDelta presentation_time;
  uint32_t index = 1;
  bool eos;
  bridge_->signal_->is_on_error_ = true;

  auto result = bridge_->DequeueOutputBuffer(&presentation_time, index, eos);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test DequeueOutputBuffer should return DECODER_RETRY when there is no buffer
 */
TEST_F(MediaCodecDecoderBridgeImplTest, DequeueOutputBuffer3) {
  base::TimeDelta presentation_time;
  uint32_t index = 1;
  bool eos;
  bridge_->signal_->is_decoder_flushing_.store(true);

  auto result = bridge_->DequeueOutputBuffer(&presentation_time, index, eos);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_RETRY);
}

/**
 * test DequeueOutputBuffer should be success
 */
TEST_F(MediaCodecDecoderBridgeImplTest, DequeueOutputBuffer4) {
  base::TimeDelta presentation_time;
  uint32_t index = 1;
  bool eos;

  BufferInfo outputBufferInfo = {0, 0, 0};
  VideoBridgeDecoderOutputBuffer buffer = {10, 1 << 3, outputBufferInfo};
  bridge_->signal_->output_queue_.push(buffer);
  bridge_->video_decoder_ = nullptr;

  auto result = bridge_->DequeueOutputBuffer(&presentation_time, index, eos);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

/**
 * test DequeueOutputBuffer should be success
 */
TEST_F(MediaCodecDecoderBridgeImplTest, DequeueOutputBuffer5) {
  base::TimeDelta presentation_time;
  uint32_t index = 1;
  bool eos;

  BufferInfo outputBufferInfo = {0, 0, 0};
  VideoBridgeDecoderOutputBuffer buffer = {10, 1 << 3, outputBufferInfo};
  bridge_->signal_->output_queue_.push(buffer);

  auto result = bridge_->DequeueOutputBuffer(&presentation_time, index, eos);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

/**
 * test destroy empty window will not cause crash
 */
TEST_F(MediaCodecDecoderBridgeImplTest, DestroyNativeWindow) {
  void* window = nullptr;
  bridge_->DestroyNativeWindow(window);
  ASSERT_EQ(window, nullptr);
}

/**
 * test signal will not be set if it's set to null
 */
TEST_F(MediaCodecDecoderBridgeImplTest, OnError) {
  base::Thread thread("ExampleThread");
  CHECK(thread.Start());
  scoped_refptr<base::SequencedTaskRunner> task_runner = thread.task_runner();
  callback_->decoder_callback_task_runner_ = task_runner;
  callback_->on_buffers_available_cb_ = base::DoNothing();

  int32_t errorCode = 123;
  callback_->signal_ = nullptr;
  callback_->OnError(errorCode);
  ASSERT_EQ(callback_->signal_, nullptr);
}

/**
 * test signal's on_error state is set when OnError is called
 */
TEST_F(MediaCodecDecoderBridgeImplTest, OnError1) {
  base::Thread thread("ExampleThread");
  CHECK(thread.Start());
  scoped_refptr<base::SequencedTaskRunner> task_runner = thread.task_runner();
  callback_->decoder_callback_task_runner_ = task_runner;
  callback_->on_buffers_available_cb_ = base::DoNothing();
  int32_t errorCode = 123;
  callback_->signal_->is_on_error_ = false;
  callback_->OnError(errorCode);

  // ensure tasks on thread are finished before assertion
  thread.FlushForTesting();
  ASSERT_TRUE(callback_->signal_->is_on_error_);
}

/**
 * test OnStreamChanged should not fail execution
 */
TEST_F(MediaCodecDecoderBridgeImplTest, OnStreamChanged) {
  OH_AVFormat* format = nullptr;
  callback_ = std::make_shared<CodecBridgeCallback>(signal_);
  callback_->OnStreamChanged(format);
}

}  // namespace media
