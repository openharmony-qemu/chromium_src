// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_AUDIO_DECODER_H
#define OHOS_AUDIO_DECODER_H
#include <memory>
#include <utility>
#include <vector>
#include <cstring>
#include <mutex>
#include "base/containers/circular_deque.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/audio_codec_decoder_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/audio_cenc_info_adapter.h"
#include "base/logging.h"
#include "media/base/audio_buffer.h"
#include "media/base/audio_decoder.h"
#include "media/base/audio_decoder_config.h"
#include "media/base/callback_registry.h"
#include "media/base/cdm_context.h"
#include "media/base/audio_timestamp_helper.h"
#include "media/base/media_export.h"
#include "media/base/ohos/ohos_media_crypto_context.h"
#include "ohos_audio_decoder_loop.h"

namespace media {
using namespace OHOS::NWeb;
class AudioTimestampHelper;

class OHOSAudioCencInfo : public AudioCencInfoAdapter {
public:
  OHOSAudioCencInfo() = default;

  ~OHOSAudioCencInfo();

  uint8_t* GetKeyId() override { return keyId_; }

  uint32_t GetKeyIdLen() override { return keyIdLen_; }

  uint8_t* GetIv() override { return iv_; }

  uint32_t GetIvLen() override { return ivLen_; }

  uint32_t GetEncryptedBlockCount() override { return encryptedBlockCount_; }

  uint32_t GetAlgo() override { return algo_; }

  uint32_t GetSkippedBlockCount() override { return skippedBlockCount_; }

  uint32_t GetFirstEncryptedOffset() override { return firstEncryptedOffset_; }

  std::vector<uint32_t> GetClearHeaderLens() override { return clearHeaderLens_; }

  std::vector<uint32_t> GetPayLoadLens() override { return payLoadLens_; }

  uint32_t GetMode() override { return mode_; }

  void SetKeyId(uint8_t* keyId) override { keyId_ = keyId; }

  void SetKeyIdLen(uint32_t keyIdLen) override { keyIdLen_ = keyIdLen; }

  void SetIv(uint8_t* iv) override { iv_ = iv; }

  void SetIvLen(uint32_t ivLen) override { ivLen_ = ivLen; }

  void SetAlgo(uint32_t algo) override { algo_ = algo; }

  void SetEncryptedBlockCount(uint32_t encryptedBlockCount) override { encryptedBlockCount_ = encryptedBlockCount; }

  void SetSkippedBlockCount(uint32_t skippedBlockCount) override { skippedBlockCount_ = skippedBlockCount; }

  void SetFirstEncryptedOffset(uint32_t firstEncryptedOffset) override { firstEncryptedOffset_ = firstEncryptedOffset; }

  void SetClearHeaderLens(const std::vector<uint32_t>& clearHeaderLens) override { clearHeaderLens_ = clearHeaderLens; }

  void SetPayLoadLens(const std::vector<uint32_t>& payLoadLens) override { payLoadLens_ = payLoadLens; }

  void SetMode(uint32_t mode) override { mode_ = mode; }

private:
  uint8_t* keyId_;

  uint32_t keyIdLen_;

  uint8_t* iv_;

  uint32_t ivLen_;

  // DRM encryption algorithm
  uint32_t algo_;

  // Number of encrypted blocks
  uint32_t encryptedBlockCount_;

  // Number of unencrypted blocks
  uint32_t skippedBlockCount_;

  uint32_t firstEncryptedOffset_;

  // The length of the head clear flow data
  std::vector<uint32_t> clearHeaderLens_;

  // The length of encrypted data
  std::vector<uint32_t> payLoadLens_;

  // Include set or non-set [keyId/iv/subsample] mode
  uint32_t mode_;
};

class OHOSAudioDecoderFormat : public AudioDecoderFormatAdapter {
public:
  OHOSAudioDecoderFormat() = default;

  int32_t GetSampleRate() override;

  int32_t GetChannelCount() override;

  int64_t GetBitRate() override;

  int32_t GetMaxInputSize() override;

  bool GetAACIsAdts() override;

  int32_t GetAudioSampleFormat() override;

  int32_t GetIdentificationHeader() override;

  int32_t GetSetupHeader() override;

  uint8_t* GetCodecConfig() override;

  uint32_t GetCodecConfigSize() override;

  void SetSampleRate(int32_t sampleRate) override;

  void SetChannelCount(int32_t channelCount) override;

  void SetBitRate(int64_t bitRate) override;

  void SetMaxInputSize(int32_t maxInputSize) override;

  void SetAACIsAdts(bool isAdts) override;

  void SetAudioSampleFormat(int32_t audioSampleFormat) override;

  void SetIdentificationHeader(int32_t idHeader) override;

  void SetSetupHeader(int32_t setupHeader) override;

  void SetCodecConfig(uint8_t* codecConfig) override;

  void SetCodecConfigSize(uint32_t size) override;

 private:
  int32_t sample_rate_;

  int32_t channel_count_;

  int64_t bit_rate_;

  int32_t max_input_size_;

  bool is_adts_;

  int32_t audio_sample_format_;

  int32_t id_header_;

  int32_t setup_header_;

  uint8_t* codec_config_;

  uint32_t codec_config_size_;
};

class AudioDecoderCallback : public AudioDecoderCallbackAdapter {
 public:
  class Client {
   public:
    virtual void AddInputBuffer(uint32_t index) = 0;

    virtual void AddOutputBuffer(uint32_t index, uint8_t* bufferData, uint32_t size, int64_t pts, BufferFlag flag) = 0;

    virtual void UpdateOutputFormat() = 0;

    virtual void OnError(int32_t errorCode) = 0;

   protected:
    virtual ~Client() {}
  };

  AudioDecoderCallback() = default;

  explicit AudioDecoderCallback(Client* client);

  ~AudioDecoderCallback();

  void OnError(int32_t errorCode) override;

  void OnOutputFormatChanged() override;

  void OnInputBufferAvailable(uint32_t index) override;

  void OnOutputBufferAvailable(uint32_t index, uint8_t* bufferData, int32_t size,
    int64_t pts, int32_t offset, uint32_t flags) override;

 private:
  raw_ptr<Client> client_;
};

class OHOSAudioDecoder : public AudioDecoder,
    public OHOSAudioDecoderLoop::Client,
    public AudioDecoderCallback::Client {
 public:

  explicit OHOSAudioDecoder(scoped_refptr<base::SequencedTaskRunner> task_runner);

  OHOSAudioDecoder(const OHOSAudioDecoder&) = delete;

  OHOSAudioDecoder& operator=(const OHOSAudioDecoder&) = delete;

  ~OHOSAudioDecoder() override;

  enum State {
    UNINITIALIZED,

    WAITING_FOR_MEDIA_CRYPTO,

    READY,

    ERROR,
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

  // OHOSAudioDecoderLoop::Client implementation
  bool IsAnyInputPending() const override;

  OHOSAudioDecoderLoop::InputData ProvideInputData() override;

  void OnInputDataQueued(bool success) override;

  bool OnDecodedEos(const OutputBufferData& out) override;

  bool OnDecodedFrame(const OutputBufferData& out) override;

  void OnCodecLoopError() override;

  int32_t DequeueInputBuffer(int64_t& buffer_index) override;

  void EnqueueInputBuffer(int64_t buffer_index) override;

  int32_t DequeueOutputBuffer(OutputBufferData& out) override;

  AudioDecoderAdapterCode FlushDecoder() override;

  AudioDecoderAdapterCode QueueInputBufferDec(uint32_t index, int64_t presentationTimeUs, uint8_t* bufferData,
      int32_t bufferSize, std::shared_ptr<AudioCencInfoAdapter> cencInfo, bool isEncrypted, BufferFlag flag) override;

  AudioDecoderAdapterCode ReleaseOutputBufferDec(uint32_t index) override;

  // AudioDecoderCallback::Client implementation
  void AddInputBuffer(uint32_t index) override;

  void AddOutputBuffer(uint32_t index, uint8_t* bufferData, uint32_t size, int64_t pts, BufferFlag flag) override;

  void OnError(int32_t errorCode) override;

  void UpdateOutputFormat() override;

  void OnMediaCryptoReady(InitCB init_cb, void* session, bool requires_secure_video_codec);

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

  void ReportDrmAudioPlayErrorInfo(const std::string& errorDesc);

 private:
  std::string mime_type_;

  State state_ = State::UNINITIALIZED;

  std::unique_ptr<AudioCodecDecoderAdapter> audio_decoder_ = nullptr;

  std::shared_ptr<AudioDecoderCallback> decoder_callback_ = nullptr;

  std::deque<uint32_t> input_buffer_queue_;

  std::deque<OutputBufferData> output_buffer_queue_;

  std::mutex input_mtx_;

  std::mutex output_mtx_;

  AudioDecoderConfig config_;

  SampleFormat sample_format_;

  int channel_count_;

  ChannelLayout channel_layout_;

  int sample_rate_;

  // Audio decode format from ohos audio decoder
  std::shared_ptr<OHOSAudioDecoderFormat> decoder_format_;

  // Callback that delivers output frames.
  OutputCB output_cb_;

  WaitingCB waiting_cb_;

  std::unique_ptr<OHOSAudioDecoderLoop> decoder_loop_;

  scoped_refptr<base::SequencedTaskRunner> task_runner_;

  using BufferCBPair = std::pair<scoped_refptr<DecoderBuffer>, DecodeCB>;

  using InputQueue = base::circular_deque<BufferCBPair>;

  InputQueue input_queue_;

  std::unique_ptr<AudioTimestampHelper> timestamp_helper_;

  std::unique_ptr<CallbackRegistration> event_cb_registration_;

  scoped_refptr<AudioBufferMemoryPool> pool_;

  // ohos cdm object
  raw_ptr<OHOSMediaCryptoContext> ohos_crypto_context_;

  void* mediaKeySession_ = nullptr;

  base::WeakPtrFactory<OHOSAudioDecoder> weak_factory_{this};

  bool waiting_for_key_ = true;

  int32_t time_out_count_ = 0;

  // define a timer for waiting licence
  base::RepeatingTimer io_timer_;

  bool audio_decoder_created_ = false;
};

}

#endif //OHOS_AUDIO_DECODER_H
