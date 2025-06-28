// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_MEDIA_CODEC_BRIDGE_IMPL_H_
#define MEDIA_BASE_OHOS_MEDIA_CODEC_BRIDGE_IMPL_H_

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

#include "base/task/sequenced_task_runner.h"
#include "media/base/video_decoder.h"
#include "media/base/video_decoder_config.h"
#include "ohos_media_codec_bridge.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/media_codec_decoder_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace media {
// using namespace OHOS::NWeb;

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
  uint32_t inputBufferIndex;
  OhosBuffer inputBuffer;
};

struct VideoBridgeDecoderOutputBuffer {
  uint32_t outputBufferIndex;
  BufferFlag outputBufferFlag;
  BufferInfo outputBufferInfo;
};

class DecoderBridgeSignal {
 public:
  std::queue<VideoBridgeDecoderInputBuffer> inputQueue_;
  std::queue<VideoBridgeDecoderOutputBuffer> outputQueue_;
  std::atomic<bool> isDecoderFlushing_ = false;
  std::atomic<bool> isOnError_ = false;
};

class CodecBridgeCallback
    : public DecoderCallbackAdapter,
      public std::enable_shared_from_this<CodecBridgeCallback> {
 public:
  explicit CodecBridgeCallback(std::shared_ptr<DecoderBridgeSignal> signal)
      : signal_(signal) {}
  virtual ~CodecBridgeCallback() = default;

  void OnError(ErrorType errorType, int32_t errorCode) override;
  void OnStreamChanged(int32_t width,
                       int32_t height,
                       double frameRate) override;
  void OnNeedInputData(uint32_t index,
                       std::shared_ptr<OhosBufferAdapter> buffer) override;
  void OnNeedOutputData(uint32_t index,
                        std::shared_ptr<BufferInfoAdapter> info,
                        BufferFlag flag) override;

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
                                      int64_t presentation_time,
                                      const DecryptConfig* decrypt_config);
  DecoderAdapterCode QueueInputBufferEOS();
  DecoderAdapterCode DequeueOutputBuffer(base::TimeDelta* presentation_time,
                                         uint32_t& index,
                                         bool& eos);
  DecoderAdapterCode SetDecryptionConfig(void *session, bool isSecure);
  static void DestoryNativeWindow(void* window);
  bool CheckHasCreated() { return hasCreated_; }
  int32_t GetConfigWidth() const { return width_; }
  int32_t GetConfigHeight() const { return height_; }

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  DecoderAdapterCode SetVideoSurface(int32_t widget_id);
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
  DecoderAdapterCode RecycleDmaBuffer();
  DecoderAdapterCode ResumeDmaBuffer();
#endif  // ARKWEB_MEDIA_DMABUF

 private:
  MediaCodecDecoderBridgeImpl(const std::string codec_type,
                              base::RepeatingClosure on_buffers_available_cb =
                                  base::RepeatingClosure());
  void PopInqueueDec();
  void PopOutqueueDec();
  DecoderAdapterCode PushInbufferDec(const uint32_t index,
                                     const uint32_t& bufferSize,
                                     const int64_t& time);
  DecoderAdapterCode PushInbufferDecEos(const uint32_t index);
  DecoderAdapterCode PrepareForCallback();
  void UpdateFlushToFalse() {
    signal_->isDecoderFlushing_ = false;
    return;
  }
  DecoderAdapterCode SetAVCencInfo(uint32_t index, const DecryptConfig* decrypt_config);

  std::atomic<bool> isRunning_ = false;
  bool isFirstDecFrame_ = true;
  std::shared_ptr<DecoderBridgeSignal> signal_ = nullptr;
  std::shared_ptr<CodecBridgeCallback> cb_ = nullptr;
  std::unique_ptr<MediaCodecDecoderAdapter> videoDecoder_ = nullptr;
  bool hasCreated_ = false;
  scoped_refptr<base::SequencedTaskRunner> decoder_task_runner_ = nullptr;
  int32_t width_;
  int32_t height_;

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void* window_from_surface_ = nullptr;
  int32_t video_surface_id_ = -1;
#endif // ARKWEB_VIDEO_ASSISTANT
  base::WeakPtrFactory<MediaCodecDecoderBridgeImpl> weak_factory_{this};
};

}  // namespace media

#endif  // MEDIA_BASE_OHOS_MEDIA_CODEC_BRIDGE_IMPL_H_
