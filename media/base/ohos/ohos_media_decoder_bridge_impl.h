/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
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

#ifndef MEDIA_BASE_OHOS_MEDIA_CODEC_BRIDGE_IMPL_H_
#define MEDIA_BASE_OHOS_MEDIA_CODEC_BRIDGE_IMPL_H_

#include "base/task/sequenced_task_runner.h"
#include "media/base/video_decoder.h"
#include "media/base/video_decoder_config.h"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <iterator>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include <multimedia/player_framework/native_avcapability.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_avcodec_videodecoder.h>
#include <native_window/external_window.h>
namespace media {

struct BufferInfo {
  int64_t presentation_time_us = 0;
  int32_t size = 0;
  int32_t offset = 0;
};

struct OhosBuffer {
  raw_ptr<uint8_t> addr;
  uint32_t buffer_size;
};

struct CodecConfigPara {
  int32_t width;
  int32_t height;
  int64_t bit_rate;
  double frame_rate;
};

struct DecoderFormat {
  int32_t width;
  int32_t height;
  double frame_rate;
};

enum class DecoderAdapterCode : int32_t {
  DECODER_OK = 0,
  DECODER_ERROR = 1,
  DECODER_RETRY = 2
};

class MEDIA_EXPORT VideoBridgeCodecConfig {
 public:
  VideoBridgeCodecConfig();

  VideoBridgeCodecConfig(const VideoBridgeCodecConfig&) = delete;
  VideoBridgeCodecConfig& operator=(const VideoBridgeCodecConfig&) = delete;

  ~VideoBridgeCodecConfig();

  media::VideoCodec codec = media::VideoCodec::kUnknown;
  std::string codec_type;
  base::RepeatingClosure on_buffers_available_cb;
};

struct VideoBridgeDecoderInputBuffer {
  uint32_t input_buffer_index;
  OhosBuffer input_buffer;
};

struct VideoBridgeDecoderOutputBuffer {
  uint32_t output_buffer_index;
  uint32_t output_buffer_flags;
  BufferInfo output_buffer_info;
};

class DecoderBridgeSignal {
 public:
  std::queue<VideoBridgeDecoderInputBuffer> input_queue_;
  std::queue<VideoBridgeDecoderOutputBuffer> output_queue_;
  std::atomic<bool> is_decoder_flushing_ = false;
  std::atomic<bool> is_on_error_ = false;
};

class CodecBridgeCallback : public std::enable_shared_from_this<CodecBridgeCallback> {
 public:
  explicit CodecBridgeCallback(std::shared_ptr<DecoderBridgeSignal> signal)
      : signal_(signal) {}
  virtual ~CodecBridgeCallback() = default;

  void OnError(int32_t error_code);
  void OnStreamChanged(OH_AVFormat* format);
  void OnNeedInputData(uint32_t index, OhosBuffer buffer);
  void OnNewOutputData(uint32_t index, BufferInfo info, uint32_t flags);

  base::RepeatingClosure on_buffers_available_cb_;

  scoped_refptr<base::SequencedTaskRunner> decoder_callback_task_runner_;

 private:
  std::shared_ptr<DecoderBridgeSignal> signal_;
};

class MediaCodecDecoderBridgeImpl {
 public:
  static std::unique_ptr<MediaCodecDecoderBridgeImpl> CreateVideoDecoder(
      const VideoBridgeCodecConfig& config);

  MediaCodecDecoderBridgeImpl(const MediaCodecDecoderBridgeImpl&) = delete;
  MediaCodecDecoderBridgeImpl& operator=(const MediaCodecDecoderBridgeImpl&) =
      delete;

  ~MediaCodecDecoderBridgeImpl();

  DecoderAdapterCode CreateVideoBridgeDecoderByMime(std::string mimetype);
  DecoderAdapterCode CreateVideoBridgeDecoderByName(std::string name);

  DecoderAdapterCode ConfigureBridgeDecoder(
      const DecoderFormat& format,
      scoped_refptr<base::SequencedTaskRunner> decoder_task_runner);
  DecoderAdapterCode SetBridgeParameterDecoder(const DecoderFormat& format);
  DecoderAdapterCode SetBridgeOutputSurface(void* window);
  DecoderAdapterCode GetOutputFormatBridgeDecoder(DecoderFormat& format);
  DecoderAdapterCode PrepareBridgeDecoder();
  DecoderAdapterCode StartBridgeDecoder();
  DecoderAdapterCode StopBridgeDecoder();
  DecoderAdapterCode FlushBridgeDecoder();
  DecoderAdapterCode ResetBridgeDecoder();
  DecoderAdapterCode ReleaseBridgeDecoder();
  DecoderAdapterCode ReleaseOutputBuffer(uint32_t index, bool render);
  DecoderAdapterCode QueueInputBuffer(const uint8_t* data,
                                      size_t data_size,
                                      int64_t presentation_time);
  DecoderAdapterCode QueueInputBufferEOS();
  DecoderAdapterCode DequeueOutputBuffer(base::TimeDelta* presentation_time,
                                         uint32_t& index,
                                         bool& eos);
  static void DestoryNativeWindow(void* window);
  bool CheckHasCreated() { return has_created_; }
  int32_t GetConfigWidth() const { return width_; }
  int32_t GetConfigHeight() const { return height_; }

 private:
  explicit MediaCodecDecoderBridgeImpl(const std::string codec_type,
                              base::RepeatingClosure on_buffers_available_cb =
                                  base::RepeatingClosure());
  void PopInqueueDec();
  void PopOutqueueDec();
  DecoderAdapterCode PushInbufferDec(const uint32_t index,
                                     const uint32_t& buffer_size,
                                     const int64_t& time);
  DecoderAdapterCode PushInbufferDecEos(const uint32_t index);
  void UpdateFlushToFalse() {
    signal_->is_decoder_flushing_ = false;
    return;
  }

  base::WeakPtrFactory<MediaCodecDecoderBridgeImpl> weak_factory_{this};
  std::atomic<bool> is_running_ = false;
  bool is_first_decFrame_ = true;
  std::shared_ptr<DecoderBridgeSignal> signal_ = nullptr;
  std::shared_ptr<CodecBridgeCallback> cb_ = nullptr;
  RAW_PTR_EXCLUSION OH_AVCodec* video_decoder_ = nullptr;
  bool has_created_ = false;
  scoped_refptr<base::SequencedTaskRunner> decoder_task_runner_ = nullptr;
  int32_t width_;
  int32_t height_;
};

}  // namespace media

#endif  // MEDIA_BASE_OHOS_MEDIA_CODEC_BRIDGE_IMPL_H_
