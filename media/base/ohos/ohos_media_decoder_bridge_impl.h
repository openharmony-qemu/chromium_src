// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_MEDIA_CODEC_BRIDGE_IMPL_H_
#define MEDIA_BASE_OHOS_MEDIA_CODEC_BRIDGE_IMPL_H_

#include <multimedia/player_framework/native_avcapability.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_avcodec_videodecoder.h>
#include <native_window/external_window.h>

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

#include "base/containers/span.h"
#include "base/task/sequenced_task_runner.h"
#include "media/base/video_decoder.h"
#include "media/base/video_decoder_config.h"
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
  DECODER_RETRY = 2,
  DECODER_DECRYPT_FAILED_NO_KEY = 3
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
  void* media_key_session = nullptr;
};

struct VideoBridgeDecoderInputBuffer {
  uint32_t input_buffer_index;
  OH_AVBuffer* av_buffer;
  OhosBuffer input_buffer;
};

struct VideoBridgeDecoderOutputBuffer {
  uint32_t output_buffer_index;
  uint32_t output_buffer_flags;
  BufferInfo output_buffer_info;
  OH_AVBuffer* av_buffer;
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
  void OnNeedInputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer);
  void OnNewOutputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer);

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

  DecoderAdapterCode CreateVideoBridgeDecoderByMime(std::string mime_type);
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
                                      int64_t presentation_time,
                                      const DecryptConfig* decrypt_config);
  DecoderAdapterCode QueueInputBufferEOS();
  DecoderAdapterCode DequeueOutputBuffer(base::TimeDelta* presentation_time,
                                         uint32_t& index,
                                         bool& eos);
  static void DestroyNativeWindow(void* window);
  bool CheckHasCreated() { return has_created_; }
  int32_t GetConfigWidth() const { return width_; }
  int32_t GetConfigHeight() const { return height_; }
#if BUILDFLAG(ENABLE_WISEPLAY)
  void SetAVCencInfo(OH_AVBuffer* av_buffer, const DecryptConfig* decrypt_config);
  DecoderAdapterCode SetDecryptionConfig(void* session, bool is_secure);
#endif  // BUILDFLAG(ENABLE_WISEPLAY)
  static void OnInputBufferAvailable(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *data, void *userData);

 private:
  explicit MediaCodecDecoderBridgeImpl(
      const std::string codec_type,
      base::RepeatingClosure on_buffers_available_cb = base::RepeatingClosure(),
      void* media_key_session = nullptr);
  void PopInqueueDec();
  void PopOutqueueDec();
  DecoderAdapterCode PushInbufferDec(OH_AVBuffer* av_buffer,
                                     const uint32_t index,
                                     const uint32_t bufferSize,
                                     const int64_t time);
  DecoderAdapterCode PushInbufferDecEos(OH_AVBuffer* av_buffer, const uint32_t index);
  void UpdateFlushToFalse() {
    signal_->is_decoder_flushing_ = false;
    return;
  }
#if BUILDFLAG(ENABLE_WISEPLAY)
  void CopyBytes(const std::string& src, base::span<uint8_t> dest);
#endif  // BUILDFLAG(ENABLE_WISEPLAY)

  std::atomic<bool> is_running_ = false;
  bool is_first_decFrame_ = true;
  std::shared_ptr<DecoderBridgeSignal> signal_ = nullptr;
  std::shared_ptr<CodecBridgeCallback> cb_ = nullptr;
  RAW_PTR_EXCLUSION OH_AVCodec* video_decoder_ = nullptr;
  bool has_created_ = false;
  scoped_refptr<base::SequencedTaskRunner> decoder_task_runner_ = nullptr;
  int32_t width_;
  int32_t height_;
  MediaKeySession*  media_key_session_ = nullptr;

  base::WeakPtrFactory<MediaCodecDecoderBridgeImpl> weak_factory_{this};
};

}  // namespace media

#endif  // MEDIA_BASE_OHOS_MEDIA_CODEC_BRIDGE_IMPL_H_
