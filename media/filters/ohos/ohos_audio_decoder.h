// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_AUDIO_DECODER_H
#define OHOS_AUDIO_DECODER_H

#include <multimedia/player_framework/native_avcodec_audiodecoder.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_averrors.h>
#include <multimedia/player_framework/native_avformat.h>
#include <multimedia/player_framework/native_cencinfo.h>

#include <cstring>
#include <memory>
#include <queue>
#include <utility>
#include <vector>

#include "base/containers/circular_deque.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "media/base/audio_buffer.h"
#include "media/base/audio_decoder.h"
#include "media/base/audio_decoder_config.h"
#include "media/base/audio_timestamp_helper.h"
#include "media/base/callback_registry.h"
#include "media/base/cdm_context.h"
#include "media/base/media_export.h"
#include "media/base/media_log.h"
#include "media/base/ohos/ohos_media_crypto_context.h"
#include "ohos_audio_decoder_loop.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace media {
using namespace media;
class AudioTimestampHelper;

enum class AudioMimeType {
  kMimetypeUnknow = 0,
  kMimeTypeAudioAac = 1,     // "audio/mp4a-latm"
  kMimeTypeAudioFlac = 2,    // "audio/flac"
  kMimetypeAudioVorbis = 3,  // "audio/vorbis"
  kMimetypeAudioMpeg = 4,    // "audio/mpeg"
  kMimetypeAudioAmrNb = 5,  // "audio/3gpp"
  kMimetypeAudioAmrWb = 6,  // "audio/amr-wb"
  kMimeTypeAudioG711mu = 7,  // "audio/g711mu"
  kMimetypeAudioApe = 8      // "audio/x-ape"
};

class OhosAudioDecoder : public AudioDecoder,
                         public OhosAudioDecoderLoop::Client {
 public:
  explicit OhosAudioDecoder(
      scoped_refptr<base::SequencedTaskRunner> task_runner,
      std::unique_ptr<MediaLog> media_log);

  OhosAudioDecoder(const OhosAudioDecoder&) = delete;

  OhosAudioDecoder& operator=(const OhosAudioDecoder&) = delete;

  ~OhosAudioDecoder() override;

  enum State {
    kUninitialized,

    kWaitingForMediaCrypto,

    kReady,

    kError,
  };

  // AudioDecoder implementation.
  AudioDecoderType GetDecoderType() const override;

  void Initialize(const AudioDecoderConfig& config,
                  CdmContext* cdm_context,
                  InitCB init_cb,
                  const OutputCB& output_cb,
                  const WaitingCB& waiting_cb) override;

  void Decode(scoped_refptr<DecoderBuffer> buffer, DecodeCB decode_cb) override;

  void Reset(base::OnceClosure closure) override;

  bool NeedsBitstreamConversion() const override;

  bool IsAnyInputPending() const override;

  OhosAudioDecoderLoop::InputData ProvideInputData() override;

  void OnInputDataQueued(bool success) override;

  bool OnDecodedEos(const OutputBufferData& out) override;

  bool OnDecodedFrame(const OutputBufferData& out) override;

  void OnCodecLoopError() override;

  int32_t DequeueInputBuffer(int64_t& buffer_index) override;

  void EnqueueInputBuffer(int64_t buffer_index) override;

  int32_t DequeueOutputBuffer(OutputBufferData& out) override;

  OhosAudioDecoderCode FlushDecoder() override;

  OhosAudioDecoderCode QueueInputBufferDec(
      uint32_t index,
      int64_t presentation_time_us,
      uint8_t* buffer_data,
      int32_t buffer_size,
      std::shared_ptr<OhosAudioCencInfo> cenc_info,
      bool is_encrypted,
      BufferFlag flag) override;

  OhosAudioDecoderCode ReleaseOutputBufferDec(uint32_t index) override;

  void AddInputBuffer(uint32_t index);

  void AddOutputBuffer(uint32_t index,
                       uint8_t* buffer_data,
                       uint32_t size,
                       int64_t pts,
                       BufferFlag flag);

  void OnError(int32_t error_code);

  void UpdateOutputFormat(int32_t sample_format,
                          int32_t channel_count,
                          int32_t sample_rate);

  void OnMediaCryptoReady(InitCB init_cb,
                          void* session,
                          bool requires_secure_video_codec);

  void OnCdmContextEvent(CdmContext::Event event);

  void ClearInputQueue(DecoderStatus decode_status);

  void SetState(State new_state);

  void SetCdm(CdmContext* cdm_context, InitCB init_cb);

  bool InitAudioDecoder(std::string mime_type);

  bool CreateOhosDecoderLoop();

  bool SupportsDecryption() const override { return true; }

  std::string GetMimeType(AudioDecoderConfig config);

  void PrepareParameters(AudioDecoderConfig config);

  void InitializeNotEncrypted(InitCB init_cb);

  void WaitingForLicence();

  OhosAudioDecoderCode CreateAudioDecoderByMime(const std::string& mime_type);

  OhosAudioDecoderCode CreateAudioDecoderByName(const std::string& name);

  OhosAudioDecoderCode ConfigureDecoder(
      const std::shared_ptr<OhosAudioDecoderFormat> format);

  OhosAudioDecoderCode SetParameterDecoder(
      const std::shared_ptr<OhosAudioDecoderFormat> format);

  OhosAudioDecoderCode PrepareDecoder();

  OhosAudioDecoderCode StartDecoder();

  OhosAudioDecoderCode StopDecoder();

  OhosAudioDecoderCode ResetDecoder();

  OhosAudioDecoderCode ReleaseDecoder();

  OhosAudioDecoderCode QueueInputBufferDecOhos(
      uint32_t index,
      int64_t presentation_time_us,
      uint8_t* buffer_data,
      int32_t buffer_size,
      std::shared_ptr<OhosAudioCencInfo> cenc_info,
      bool is_encrypted,
      BufferFlag flag);

  OhosAudioDecoderCode GetOutputFormatDec(
      std::shared_ptr<OhosAudioDecoderFormat> format);

  OhosAudioDecoderCode SetCallbackDec();

  OhosAudioDecoderCode SetDecryptionConfig(void* session, bool secure_audio);

  OH_AVCodec* GetAVCodec();

  void SetInputBuffer(uint32_t index, OH_AVBuffer* buffer);

  void SetOutputBuffer(uint32_t index, OH_AVBuffer* buffer);

  static std::mutex& GetDecoderMutex();

  void GetParamFromAVFormat(OH_AVFormat* av_format,
                            std::shared_ptr<OhosAudioDecoderFormat> format);

  OhosAudioDecoderCode SetBufferCencInfo(
      uint32_t index,
      std::shared_ptr<OhosAudioCencInfo> cenc_info);

  void GetMimeType();

  OhosAudioDecoderCode SetAVCencInfo(
      OH_AVCencInfo* av_cenc_info,
      std::shared_ptr<OhosAudioCencInfo> cenc_info);

  OH_AVBuffer* GetInputBuffer(uint32_t index);

  OH_AVBuffer* GetOutputBuffer(uint32_t index);

 private:
  AudioMimeType mime_type_ = AudioMimeType::kMimetypeUnknow;

  OH_AVCodec* decoder_ = nullptr;

  std::mutex in_mutex_;

  std::mutex out_mutex_;

  std::map<uint32_t, OH_AVBuffer*> input_buffers_;

  std::map<uint32_t, OH_AVBuffer*> output_buffers_;

  static std::mutex decoder_mutex_;

  std::string mime_type_str_;

  State state_ = State::kUninitialized;

  std::mutex input_buffer_queue_mutex_;

  std::mutex output_buffer_queue_mutex_;

  std::deque<uint32_t> input_buffer_queue_;

  std::deque<OutputBufferData> output_buffer_queue_;

  AudioDecoderConfig config_;

  SampleFormat sample_format_;

  int channel_count_;

  ChannelLayout channel_layout_;

  int sample_rate_;

  // Audio decode format from ohos audio decoder
  std::shared_ptr<OhosAudioDecoderFormat> decoder_format_;

  // Callback that delivers output frames.
  OutputCB output_cb_;

  WaitingCB waiting_cb_;

  std::unique_ptr<OhosAudioDecoderLoop> decoder_loop_;

  scoped_refptr<base::SequencedTaskRunner> task_runner_;

  using BufferCBPair = std::pair<scoped_refptr<DecoderBuffer>, DecodeCB>;

  using InputQueue = base::circular_deque<BufferCBPair>;

  InputQueue input_queue_;

  std::unique_ptr<AudioTimestampHelper> timestamp_helper_;

  std::unique_ptr<CallbackRegistration> event_cb_registration_;

  scoped_refptr<AudioBufferMemoryPool> pool_;

  // ohos cdm object
  raw_ptr<OhosMediaCryptoContext> ohos_crypto_context_ = nullptr;

  void* media_key_session_ = nullptr;

  bool waiting_for_key_ = true;

  int32_t time_out_count_ = 0;

  // define a timer for waiting licence
  base::RepeatingTimer io_timer_;

  bool audio_decoder_created_ = false;

  std::unique_ptr<MediaLog> media_log_;

  base::WeakPtrFactory<OhosAudioDecoder> weak_factory_{this};
};

/**
 * mangage a map of decoders for different media mime type
 */
class AudioDecoderCallbackManager {
 public:
  static void OnError(OH_AVCodec* codec, int32_t error_code, void* user_data);

  static void OnOutputFormatChanged(OH_AVCodec* codec,
                                    OH_AVFormat* format,
                                    void* user_data);

  static void OnInputBufferAvailable(OH_AVCodec* codec,
                                     uint32_t index,
                                     OH_AVBuffer* data,
                                     void* user_data);

  static void OnOutputBufferAvailable(OH_AVCodec* codec,
                                      uint32_t index,
                                      OH_AVBuffer* data,
                                      void* user_data);

  static media::OhosAudioDecoder* FindAudioDecoder(OH_AVCodec* decoder);

  static void DeleteAudioDecoder(OH_AVCodec* decoder);

  static void AddAudioDecoder(media::OhosAudioDecoder* decoder);

 private:
  static std::map<OH_AVCodec*, media::OhosAudioDecoder*> decoders_;

  static std::mutex decoders_map_mutex_;
};

}  // namespace media

#endif  // OHOS_AUDIO_DECODER_H
