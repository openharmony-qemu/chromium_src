// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "ohos_audio_decoder.h"

#include <multimedia/drm_framework/native_drm_err.h>
#include <multimedia/drm_framework/native_mediakeysession.h>
#include <multimedia/native_audio_channel_layout.h>
#include <multimedia/player_framework/native_avbuffer.h>
#include <multimedia/player_framework/native_avcapability.h>
#include <multimedia/player_framework/native_avcodec_audiocodec.h>
#include <multimedia/player_framework/native_cencinfo.h>

#include <algorithm>
#include <cmath>
#include <memory>

#include "base/android/build_info.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "base/task/bind_post_task.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"
#include "media/base/audio_timestamp_helper.h"
#include "media/base/status.h"
#include "media/base/timestamp_constants.h"
#include "media/formats/ac3/ac3_util.h"
#include "media/formats/dts/dts_util.h"
#include "media/media_buildflags.h"

#define MIME_SIZE 256

namespace media {
namespace {
constexpr base::TimeDelta kTwoSecondTimeout = base::Seconds(2);
constexpr int32_t kMaxTimeOutCount = 2;
}  // namespace
// class AudioDecoderCallback;

static const std::unordered_map<const char*, AudioMimeType> kMimeTypeMap = {
    {OH_AVCODEC_MIMETYPE_AUDIO_AAC, AudioMimeType::kMimeTypeAudioAac},
    {OH_AVCODEC_MIMETYPE_AUDIO_FLAC, AudioMimeType::kMimeTypeAudioFlac},
    {OH_AVCODEC_MIMETYPE_AUDIO_VORBIS, AudioMimeType::kMimetypeAudioVorbis},
    {OH_AVCODEC_MIMETYPE_AUDIO_MPEG, AudioMimeType::kMimetypeAudioMpeg},
    {OH_AVCODEC_MIMETYPE_AUDIO_AMR_NB, AudioMimeType::kMimetypeAudioAmrNb},
    {OH_AVCODEC_MIMETYPE_AUDIO_AMR_WB, AudioMimeType::kMimetypeAudioAmrWb},
    {OH_AVCODEC_MIMETYPE_AUDIO_G711MU, AudioMimeType::kMimeTypeAudioG711mu},
    {OH_AVCODEC_MIMETYPE_AUDIO_APE, AudioMimeType::kMimetypeAudioApe}};

OhosAudioDecoder::OhosAudioDecoder(
    scoped_refptr<base::SequencedTaskRunner> task_runner)
    : state_(kUninitialized),
      sample_format_(kSampleFormatS16),
      channel_count_(0),
      channel_layout_(CHANNEL_LAYOUT_NONE),
      sample_rate_(0),
      decoder_format_(std::make_shared<OhosAudioDecoderFormat>()),
      task_runner_(task_runner),
      pool_(base::MakeRefCounted<AudioBufferMemoryPool>()) {
  TRACE_EVENT0("media", "OhosAudioDecoder::OhosAudioDecoder");
  io_timer_.SetTaskRunner(scoped_refptr<base::SingleThreadTaskRunner>());
}

OhosAudioDecoder::~OhosAudioDecoder() {
  TRACE_EVENT0("media", "OhosAudioDecoder::~OhosAudioDecoder");
  decoder_loop_.reset();
  // Cancel previously registered callback (if any).
  if (ohos_crypto_context_) {
    ohos_crypto_context_->SetOHOSMediaCryptoReadyCB(base::NullCallback());
  }

  if (audio_decoder_created_) {
    StopDecoder();
    ReleaseDecoder();
  }

  ClearInputQueue(DecoderStatus::Codes::kAborted);
  io_timer_.Stop();

  std::unique_lock<std::mutex> lock(decoder_mutex_);

  if (decoder_ != nullptr) {
    AudioDecoderCallbackManager::DeleteAudioDecoder(decoder_);
    OH_AVErrCode err_code = OH_AudioCodec_Destroy(decoder_);
    if (err_code != AV_ERR_OK) {
      LOG(ERROR) << __func__
                 << " [AudioDecoder] destroy decoder_ fail, err_code = "
                 << static_cast<int>(err_code);
    }
    decoder_ = nullptr;
  }
}

AudioDecoderType OhosAudioDecoder::GetDecoderType() const {
  return AudioDecoderType::kOhos;
}

void OhosAudioDecoder::Initialize(const AudioDecoderConfig& config,
                                  CdmContext* cdm_context,
                                  InitCB init_cb,
                                  const OutputCB& output_cb,
                                  const WaitingCB& waiting_cb) {
  TRACE_EVENT0("media", "OhosAudioDecoder::Initialize");
  // Only the encrypted DRM audio stream goes through the openharmony system
  // decoding path
  if (!config.is_encrypted()) {
    LOG(ERROR) << " [WiseplayDRM] AudioDecoderConfig is not encrypted";
    base::BindPostTaskToCurrentDefault(std::move(init_cb))
        .Run(DecoderStatus::Codes::kUnsupportedCodec);
    return;
  }

  // Clear the input buffer and set the callback result to
  // DecoderStatus::Codes::kAborted
  ClearInputQueue(DecoderStatus::Codes::kAborted);

  if (state_ == kError) {
    LOG(ERROR) << " [AudioDecoder] OhosAudioDecoder::Initialize state error";
    base::BindPostTaskToCurrentDefault(std::move(init_cb))
        .Run(DecoderStatus::Codes::kFailed);
    return;
  }

  // Check if the audio format is supported
  mime_type_str_ = GetMimeType(config);
  sample_format_ = kSampleFormatS16;
  if (mime_type_str_.empty()) {
    LOG(ERROR)
        << "[AudioDecoder] OhosAudioDecoder::Initialize, Unsupported codec";
    base::BindPostTaskToCurrentDefault(std::move(init_cb))
        .Run(DecoderStatus::Codes::kUnsupportedCodec);
    return;
  }

  PrepareParameters(config);
  output_cb_ = base::BindPostTaskToCurrentDefault(output_cb);
  waiting_cb_ = base::BindPostTaskToCurrentDefault(waiting_cb);

  // Decryption judgment, if it is an encrypted stream and there is no
  // decryption configuration, exit directly
  if (config_.is_encrypted()) {
    if (!cdm_context || !cdm_context->GetOhosMediaCryptoContext()) {
      LOG(ERROR)
          << "[WiseplayDRM] The stream is encrypted but there is no CdmContext "
          << "or MediaCryptoContext is not supported";
      SetState(kError);
      base::BindPostTaskToCurrentDefault(std::move(init_cb))
          .Run(DecoderStatus::Codes::kUnsupportedEncryptionMode);
      return;
    }
    SetState(kWaitingForMediaCrypto);
    SetCdm(cdm_context, std::move(init_cb));
    return;
  }
  InitializeNotEncrypted(std::move(init_cb));
}

std::string OhosAudioDecoder::GetMimeType(AudioDecoderConfig config) {
  TRACE_EVENT0("media", "OhosAudioDecoder::GetMimeType");
  switch (config.codec()) {
    case AudioCodec::kAAC:
      mime_type_str_ = "audio/mp4a-latm";
      break;
    case AudioCodec::kFLAC:
      mime_type_str_ = "audio/flac";
      break;
    case AudioCodec::kVorbis:
      mime_type_str_ = "audio/vorbis";
      break;
    case AudioCodec::kMP3:
      mime_type_str_ = "audio/mpeg";
      break;
    case AudioCodec::kAMR_NB:
      mime_type_str_ = "audio/3gpp";
      break;
    case AudioCodec::kAMR_WB:
      mime_type_str_ = "audio/amr-wb";
      break;
    case AudioCodec::kUnknown:
    case AudioCodec::kOpus:
    case AudioCodec::kPCM:
    case AudioCodec::kPCM_MULAW:
    case AudioCodec::kGSM_MS:
    case AudioCodec::kPCM_S16BE:
    case AudioCodec::kPCM_S24BE:
    case AudioCodec::kPCM_ALAW:
    case AudioCodec::kALAC:
    case AudioCodec::kAC3:
    case AudioCodec::kEAC3:
    case AudioCodec::kDTS:
    case AudioCodec::kDTSXP2:
    case AudioCodec::kDTSE:
    case AudioCodec::kMpegHAudio:
    case AudioCodec::kAC4:
    case AudioCodec::kIAMF:
      break;
  }
  return mime_type_str_;
}

void OhosAudioDecoder::PrepareParameters(AudioDecoderConfig config) {
  TRACE_EVENT0("media", "OhosAudioDecoder::PrepareParameters");
  config_ = config;
  channel_layout_ = config_.channel_layout();
  channel_count_ = ChannelLayoutToChannelCount(channel_layout_);
  sample_rate_ = config_.samples_per_second();
  timestamp_helper_ = std::make_unique<AudioTimestampHelper>(sample_rate_);
}

void OhosAudioDecoder::InitializeNotEncrypted(InitCB init_cb) {
  TRACE_EVENT0("media", "OhosAudioDecoder::InitializeNotEncrypted");
  if (!InitAudioDecoder(mime_type_str_)) {
    LOG(ERROR) << "[AudioDecoder] initAudioDecoder error";
    base::BindPostTaskToCurrentDefault(std::move(init_cb))
        .Run(DecoderStatus::Codes::kFailed);
    return;
  }

  CreateOhosDecoderLoop();

  SetState(kReady);
  base::BindPostTaskToCurrentDefault(std::move(init_cb))
      .Run(DecoderStatus::Codes::kOk);
}

void OhosAudioDecoder::SetCdm(CdmContext* cdm_context, InitCB init_cb) {
  TRACE_EVENT0("media", "OhosAudioDecoder::SetCdm");
  if (!cdm_context) {
    LOG(WARNING) << "[WiseplayDRM] OhosAudioDecoder::SetCdm No CDM provided";
    base::BindPostTaskToCurrentDefault(std::move(init_cb))
        .Run(DecoderStatus::Codes::kFailed);
    return;
  }
  ohos_crypto_context_ = cdm_context->GetOhosMediaCryptoContext();

  event_cb_registration_ = cdm_context->RegisterEventCB(base::BindRepeating(
      &OhosAudioDecoder::OnCdmContextEvent, weak_factory_.GetWeakPtr()));

  ohos_crypto_context_->SetOHOSMediaCryptoReadyCB(
      base::BindPostTaskToCurrentDefault(
          base::BindOnce(&OhosAudioDecoder::OnMediaCryptoReady,
                         weak_factory_.GetWeakPtr(), std::move(init_cb))));
}

void OhosAudioDecoder::OnCdmContextEvent(CdmContext::Event event) {
  if (event != CdmContext::Event::kHasAdditionalUsableKey) {
    return;
  }

  waiting_for_key_ = false;
  SetState(kReady);
  io_timer_.Stop();
  if (decoder_loop_) {
    decoder_loop_->OnKeyAdded();
  }
}

void OhosAudioDecoder::OnMediaCryptoReady(InitCB init_cb,
                                          void* session,
                                          bool requires_secure_video_codec) {
  TRACE_EVENT0("media", "OhosAudioDecoder::OnMediaCryptoReady");
  if (session == nullptr) {
    LOG(ERROR) << "[WiseplayDRM] can't play encrypted stream";
    SetState(kUninitialized);
    std::move(init_cb).Run(DecoderStatus::Codes::kUnsupportedEncryptionMode);
    return;
  }

  media_key_session_ = std::move(session);

  if (!InitAudioDecoder(mime_type_str_)) {
    LOG(ERROR) << "[AudioDecoder] initAudioDecoder error";
    base::BindPostTaskToCurrentDefault(std::move(init_cb))
        .Run(DecoderStatus::Codes::kFailed);
    return;
  }

  // After receiving |media_key_session_| we can configure MediaCodec.
  if (!CreateOhosDecoderLoop()) {
    LOG(WARNING) << __func__ << " [AudioDecoder] CreateOhosDecoderLoop fail";
    SetState(kUninitialized);
    std::move(init_cb).Run(DecoderStatus::Codes::kFailed);
    return;
  }

  SetState(kReady);
  std::move(init_cb).Run(DecoderStatus::Codes::kOk);
}

bool OhosAudioDecoder::InitAudioDecoder(std::string mime_type) {
  TRACE_EVENT0("media", "OhosAudioDecoder::InitAudioDecoder");

  std::shared_ptr<OhosAudioDecoderFormat> audio_decoder_format =
      std::make_shared<OhosAudioDecoderFormat>();
  // Sampling rate and number of channels are required, bit rate and maximum
  // output length are optional
  audio_decoder_format->SetSampleRate(sample_rate_);
  audio_decoder_format->SetChannelCount(channel_count_);

  OhosAudioDecoderCode ret;
  if (!audio_decoder_created_) {
    ret = CreateAudioDecoderByMime(mime_type);
    if (ret != OhosAudioDecoderCode::kDecoderOk) {
      LOG(ERROR) << __func__
                 << " [AudioDecoder] CreateAudioDecoderByMime Failed, mime: "
                 << mime_type;
      return false;
    }
    audio_decoder_created_ = true;
  } else {
    LOG(INFO) << __func__
              << " [AudioDecoder] already had decoder, no need create again";
  }

  SetCallbackDec();
  // Set the decryption key. Audio currently only supports soft decryption mode.
  if (config_.is_encrypted() && media_key_session_) {
    SetDecryptionConfig(media_key_session_, false);
  }
  ConfigureDecoder(audio_decoder_format);
  PrepareDecoder();
  ret = StartDecoder();
  if (ret != OhosAudioDecoderCode::kDecoderOk) {
    LOG(ERROR) << "[AudioDecoder] StartDecoder err, ret: " << static_cast<int>(ret);
    return false;
  }
  ret = GetOutputFormatDec(decoder_format_);
  if (ret != OhosAudioDecoderCode::kDecoderOk) {
    LOG(ERROR) << "[AudioDecoder] GetOutputFormatDec err, ret: " << static_cast<int>(ret);
    return false;
  }
  LOG(INFO) << "[AudioDecoder] format: GetSampleRate: "
            << decoder_format_->GetSampleRate()
            << " GetChannelCount: " << decoder_format_->GetChannelCount();
  return true;
}

bool OhosAudioDecoder::CreateOhosDecoderLoop() {
  decoder_loop_.reset();
  decoder_loop_ = std::make_unique<OhosAudioDecoderLoop>(
      this, scoped_refptr<base::SingleThreadTaskRunner>());
  return true;
}

void OhosAudioDecoder::SetState(State new_state) {
  state_ = new_state;
}

void OhosAudioDecoder::ClearInputQueue(DecoderStatus decode_status) {
  for (auto& entry : input_queue_) {
    std::move(entry.second).Run(decode_status);
  }
  input_queue_.clear();
}

void OhosAudioDecoder::OnError(int32_t error_code) {
  if (state_ != kWaitingForMediaCrypto) {
    SetState(kWaitingForMediaCrypto);
  }
}

void OhosAudioDecoder::Decode(scoped_refptr<DecoderBuffer> buffer,
                              DecodeCB decode_cb) {
  DecodeCB cb = base::BindPostTaskToCurrentDefault(std::move(decode_cb));
  if (!DecoderBuffer::DoSubsamplesMatch(*buffer)) {
    LOG(ERROR) << "[AudioDecoder] OhosAudioDecoder::DoSubsamplesMatch error";
    std::move(cb).Run(DecoderStatus::Codes::kFailed);
    return;
  }

  if (!buffer->end_of_stream() && buffer->timestamp() == kNoTimestamp) {
    LOG(ERROR) << "[AudioDecoder] OhosAudioDecoder::Decode "
               << buffer->AsHumanReadableString()
               << ": no timestamp, skipping this buffer";
    std::move(cb).Run(DecoderStatus::Codes::kFailed);
    return;
  }

  if (state_ == kError) {
    LOG(ERROR) << "[AudioDecoder] OhosAudioDecoder::Decode "
               << buffer->AsHumanReadableString()
               << ": Error state, returning decode error for all buffers";
    ClearInputQueue(DecoderStatus::Codes::kFailed);
    std::move(cb).Run(DecoderStatus::Codes::kFailed);
    return;
  }
  if (state_ != kReady) {
    LOG(WARNING) << "[AudioDecoder] OhosAudioDecoder::Decode unexpected state"
               << state_;
  }

  input_queue_.push_back(std::make_pair(std::move(buffer), std::move(cb)));
  decoder_loop_->ExpectWork();
}

void OhosAudioDecoder::Reset(base::OnceClosure closure) {
  ClearInputQueue(DecoderStatus::Codes::kAborted);
  bool success = decoder_loop_->TryFlush();
  {
    std::unique_lock<std::mutex> lock(input_buffer_queue_mutex_);
    input_buffer_queue_.clear();
  }
  {
    std::unique_lock<std::mutex> lock(output_buffer_queue_mutex_);
    output_buffer_queue_.clear();
  }
  io_timer_.Stop();
  if (success) {
    success = CreateOhosDecoderLoop();
  }
  StartDecoder();
  timestamp_helper_->SetBaseTimestamp(kNoTimestamp);
  SetState(success ? kReady : kError);
  task_runner_->PostTask(FROM_HERE, std::move(closure));
}

bool OhosAudioDecoder::NeedsBitstreamConversion() const {
  // An AAC stream needs to be converted as ADTS stream.
  return config_.codec() == AudioCodec::kAAC;
}

void OhosAudioDecoder::OnInputDataQueued(bool success) {
  if (input_queue_.front().first->end_of_stream() && success) {
    LOG(WARNING) << "[AudioDecoder] input queue has eos";
    return;
  }
  // Queuing is successful, set the callback to ok
  std::move(input_queue_.front().second)
      .Run(success ? DecoderStatus::Codes::kOk : DecoderStatus::Codes::kFailed);
  input_queue_.pop_front();
}

bool OhosAudioDecoder::IsAnyInputPending() const {
  if (state_ != kReady) {
    return false;
  }
  return !input_queue_.empty();
}

static void SetCencInfoToInputData(OhosAudioDecoderLoop::InputData& data,
                                   const DecryptConfig* decrypt_config) {
  if (decrypt_config == nullptr) {
    return;
  }

  std::vector<uint32_t> clear_header_lens;
  std::vector<uint32_t> pay_load_lens;
  data.cenc_info = std::make_shared<OhosAudioCencInfo>();
  data.cenc_info->SetKeyId(const_cast<uint8_t*>(
      reinterpret_cast<const uint8_t*>(decrypt_config->key_id().data())));
  data.cenc_info->SetKeyIdLen(decrypt_config->key_id().size());
  data.cenc_info->SetIv(const_cast<uint8_t*>(
      reinterpret_cast<const uint8_t*>(decrypt_config->iv().data())));
  data.cenc_info->SetIvLen(decrypt_config->iv().size());

  switch (decrypt_config->encryption_scheme()) {
    case EncryptionScheme::kUnencrypted:
      data.cenc_info->SetAlgo(
          uint32_t(DrmCencAlgorithm::DRM_ALG_CENC_UNENCRYPTED));
      data.is_encrypted = false;
      break;
    case EncryptionScheme::kCenc:
      data.cenc_info->SetAlgo(uint32_t(DrmCencAlgorithm::DRM_ALG_CENC_AES_CTR));
      data.is_encrypted = true;
      break;
    case EncryptionScheme::kCbcs:
      data.cenc_info->SetAlgo(uint32_t(DrmCencAlgorithm::DRM_ALG_CENC_AES_CBC));
      data.is_encrypted = true;
      break;
    default:
      data.is_encrypted = false;
      // Currently the kernel only supports AES-CTR and AES-CBC encryption
      // algorithm modes
      data.cenc_info->SetAlgo(
          uint32_t(DrmCencAlgorithm::DRM_ALG_CENC_UNENCRYPTED));
  }

  if (decrypt_config->encryption_pattern()) {
    data.cenc_info->SetEncryptedBlockCount(
        decrypt_config->encryption_pattern()->crypt_byte_block());
    data.cenc_info->SetSkippedBlockCount(
        decrypt_config->encryption_pattern()->skip_byte_block());
  }

  // The kernel does not involve offset, the default setting is 0
  data.cenc_info->SetFirstEncryptedOffset(0);
  for (size_t i = 0; i < decrypt_config->subsamples().size(); i++) {
    clear_header_lens.push_back(decrypt_config->subsamples()[i].clear_bytes);
    pay_load_lens.push_back(decrypt_config->subsamples()[i].cypher_bytes);
  }

  data.cenc_info->SetClearHeaderLens(clear_header_lens);
  data.cenc_info->SetPayLoadLens(pay_load_lens);
  // The web kernel sets keyid and iv by default, so
  // DRM_CENC_INFO_KEY_IV_SUBSAMPLES_SET is selected by default here
  data.cenc_info->SetMode(
      uint32_t(DrmCencInfoMode::DRM_CENC_INFO_KEY_IV_SUBSAMPLES_SET));
}

OhosAudioDecoderLoop::InputData OhosAudioDecoder::ProvideInputData() {
  const DecoderBuffer* decoder_buffer = input_queue_.front().first.get();
  OhosAudioDecoderLoop::InputData data;

  if (decoder_buffer == nullptr) {
    LOG(WARNING) << "[AudioDecoder] decoder_buffer is null";
    data.is_valid = false;
    return data;
  }

  if (decoder_buffer->end_of_stream()) {
    LOG(WARNING) << "[AudioDecoder] get eos";
    data.memory = const_cast<uint8_t*>(decoder_buffer->data());
    data.is_eos = true;
    data.length = 0;
    data.presentation_time = decoder_buffer->timestamp();
  } else {
    data.memory = const_cast<uint8_t*>(decoder_buffer->data());
    data.length = decoder_buffer->size();

    const DecryptConfig* decrypt_config = decoder_buffer->decrypt_config();
    if (decrypt_config) {
      SetCencInfoToInputData(data, decrypt_config);
    }

    data.presentation_time = decoder_buffer->timestamp();
  }
  return data;
}

bool OhosAudioDecoder::OnDecodedEos(const OutputBufferData& out) {
  if (!input_queue_.size() || !input_queue_.front().first->end_of_stream()) {
    LOG(WARNING) << "[AudioDecoder] received unexpected eos";
    return false;
  }
  // EOS frame direct callback ok
  std::move(input_queue_.front()).second.Run(DecoderStatus::Codes::kOk);
  input_queue_.pop_front();
  std::unique_lock<std::mutex> lock(output_buffer_queue_mutex_);
  output_buffer_queue_.pop_front();
  return true;
}

bool OhosAudioDecoder::OnDecodedFrame(const OutputBufferData& out) {
  if (out.size_ == 0U || out.index_ == -1U || decoder_loop_ == nullptr ||
      channel_count_ == 0) {
    LOG(ERROR) << "[AudioDecoder] OhosAudioDecoder::OnDecodedFrame buffer data "
                  "is invalid";
    return false;
  }

  size_t frame_count = 1;
  scoped_refptr<AudioBuffer> audio_buffer;

  const int bytes_per_frame = sizeof(uint16_t) * channel_count_;
  frame_count = out.size_ / bytes_per_frame;

  audio_buffer =
      AudioBuffer::CreateBuffer(sample_format_, channel_layout_, channel_count_,
                                sample_rate_, frame_count, pool_);

  // channel_data is a AudioBufferMemoryPool and memcpy should not have problem
  memcpy(audio_buffer->channel_data()[0], out.data_.data(), out.size_);

  ReleaseOutputBufferDec(out.index_);
  {
    std::unique_lock<std::mutex> lock(output_buffer_queue_mutex_);
    output_buffer_queue_.pop_front();
  }

  if (!timestamp_helper_->base_timestamp() ||
      timestamp_helper_->base_timestamp() == kNoTimestamp) {
    // Seek, baseTimestamp needs to be set to the time of the first frame,
    // the pts unit given by the oh system audio measurement is ms
    timestamp_helper_->SetBaseTimestamp(
        std::max(base::TimeDelta(), base::Milliseconds(out.pts_)));
  }
  // Set the timestamp of the audio buffer.
  // timestamp_helper_->GetTimestamp() Calculates the current timestamp based on
  // the base timestamp and the number of frames added.
  audio_buffer->set_timestamp(timestamp_helper_->GetTimestamp());
  timestamp_helper_->AddFrames(frame_count);
  output_cb_.Run(audio_buffer);
  return true;
}

void OhosAudioDecoder::OnCodecLoopError() {
  SetState(kError);
  ClearInputQueue(DecoderStatus::Codes::kFailed);
}

int32_t OhosAudioDecoder::DequeueInputBuffer(int64_t& buffer_index) {
  std::unique_lock<std::mutex> lock(input_buffer_queue_mutex_);
  if (!input_buffer_queue_.empty()) {
    buffer_index = input_buffer_queue_.front();
    input_buffer_queue_.pop_front();
    return buffer_index;
  }
  return -1;
}

void OhosAudioDecoder::EnqueueInputBuffer(int64_t buffer_index) {
  std::unique_lock<std::mutex> lock(input_buffer_queue_mutex_);
  input_buffer_queue_.push_back(buffer_index);
}

int32_t OhosAudioDecoder::DequeueOutputBuffer(OutputBufferData& out) {
  std::unique_lock<std::mutex> lock(output_buffer_queue_mutex_);
  if (!output_buffer_queue_.empty()) {
    out = output_buffer_queue_.front();
    // Output buffer queue will be popped after decoding is successful
    return 0;
  }
  return -1;
}

void OhosAudioDecoder::WaitingForLicence() {
  // Define a timer to check the license status, The timer will check the
  // license status after 2 seconds, and if the license is still invalid, it
  // will retry once more. After 2 attempts, if the license is still invalid,
  // the timer will stop.
  LOG(INFO) << __func__
            << " [WiseplayDRM] time_out_count_ = " << time_out_count_;
  if (!waiting_for_key_) {
    LOG(WARNING) << __func__ << " [WiseplayDRM] get key and stop timer";
    io_timer_.Stop();
    time_out_count_ = 0;
    return;
  }
  if (time_out_count_ > kMaxTimeOutCount) {
    io_timer_.Stop();
    time_out_count_ = 0;
    LOG(WARNING) << __func__ << " [WiseplayDRM] not get key and stop timer";
    SetState(kError);
    return;
  }
  time_out_count_++;
  if (!io_timer_.IsRunning()) {
    LOG(INFO) << __func__ << " [WiseplayDRM] start timer";
    io_timer_.Start(FROM_HERE, kTwoSecondTimeout, this,
                    &OhosAudioDecoder::WaitingForLicence);
  }
}

OhosAudioDecoderCode OhosAudioDecoder::QueueInputBufferDec(
    uint32_t index,
    int64_t presentation_time_us,
    uint8_t* buffer_data,
    int32_t buffer_size,
    std::shared_ptr<OhosAudioCencInfo> cenc_info,
    bool is_encrypted,
    BufferFlag flag) {
  if (state_ == kWaitingForMediaCrypto) {
    LOG(INFO) << __func__
              << " [AudioDecoder] error, state = kWaitingForMediaCrypto";
    return OhosAudioDecoderCode::kDecoderRetry;
  }
  OhosAudioDecoderCode ret =
      QueueInputBufferDecOhos(index, presentation_time_us, buffer_data,
                              buffer_size, cenc_info, is_encrypted, flag);
  if (ret == OhosAudioDecoderCode::kDecoderNoKey && waiting_for_key_) {
    SetState(kWaitingForMediaCrypto);
    LOG(WARNING) << __func__ << " [WiseplayDRM] error, wait for key";
    waiting_cb_.Run(WaitingReason::kNoDecryptionKey);
    if (!io_timer_.IsRunning()) {
      WaitingForLicence();
    }
    return OhosAudioDecoderCode::kDecoderNoKey;
  }
  return ret;
}

void OhosAudioDecoder::AddInputBuffer(uint32_t index) {
  std::unique_lock<std::mutex> lock(input_buffer_queue_mutex_);
  input_buffer_queue_.push_back(index);
}

void OhosAudioDecoder::AddOutputBuffer(uint32_t index,
                                       uint8_t* buffer_data,
                                       uint32_t size,
                                       int64_t pts,
                                       BufferFlag flag) {
  OutputBufferData data(index, buffer_data, size, pts, flag);
  std::unique_lock<std::mutex> lock(output_buffer_queue_mutex_);
  output_buffer_queue_.push_back(data);
}

void OhosAudioDecoder::UpdateOutputFormat() {
  OhosAudioDecoderCode ret = GetOutputFormatDec(decoder_format_);
  if (ret != OhosAudioDecoderCode::kDecoderOk) {
    LOG(ERROR) << "[AudioDecoder] OhosAudioDecoder::UpdateOutputFormat err";
  }
}

std::mutex OhosAudioDecoder::decoder_mutex_;

std::mutex& OhosAudioDecoder::GetDecoderMutex() {
  return decoder_mutex_;
}

OH_AVCodec* OhosAudioDecoder::GetAVCodec() {
  return decoder_;
}

void OhosAudioDecoder::SetInputBuffer(uint32_t index, OH_AVBuffer* buffer) {
  std::unique_lock<std::mutex> lock(in_mutex_);
  if (input_buffers_.find(index) != input_buffers_.end()) {
    return;
  }
  input_buffers_.insert(std::make_pair(index, buffer));
}

void OhosAudioDecoder::SetOutputBuffer(uint32_t index, OH_AVBuffer* buffer) {
  std::unique_lock<std::mutex> lock(out_mutex_);
  if (output_buffers_.find(index) != output_buffers_.end()) {
    return;
  }
  output_buffers_.insert(std::make_pair(index, buffer));
}

OH_AVBuffer* OhosAudioDecoder::GetInputBuffer(uint32_t index) {
  std::unique_lock<std::mutex> lock(in_mutex_);
  if (input_buffers_.find(index) == input_buffers_.end()) {
    return nullptr;
  }
  return input_buffers_.at(index);
}

OH_AVBuffer* OhosAudioDecoder::GetOutputBuffer(uint32_t index) {
  std::unique_lock<std::mutex> lock(out_mutex_);
  if (output_buffers_.find(index) == output_buffers_.end()) {
    return nullptr;
  }
  return output_buffers_.at(index);
}

void OhosAudioDecoder::GetMimeType() {
  if (decoder_ == nullptr) {
    return;
  }
  mime_type_ = AudioMimeType::kMimetypeUnknow;
  OH_AVFormat* av_format = OH_AudioCodec_GetOutputDescription(decoder_);
  if (av_format == nullptr) {
    return;
  }

  char mime_array[MIME_SIZE] = {0};
  const char* mime = mime_array;
  bool ret = OH_AVFormat_GetStringValue(av_format, OH_MD_KEY_CODEC_MIME, &mime);
  if (!ret) {
    LOG(ERROR) << __func__
               << " [AudioDecoder] AudioCodecDecoder Fail to get mime_type";
    return;
  }
  OH_AVFormat_Destroy(av_format);
  mime_type_ = AudioMimeType::kMimetypeUnknow;
  for (auto it = kMimeTypeMap.begin(); it != kMimeTypeMap.end(); it++) {
    if (strcmp(it->first, mime) == 0) {
      mime_type_ = it->second;
    }
  }
}

OhosAudioDecoderCode OhosAudioDecoder::CreateAudioDecoderByMime(
    const std::string& mime_type) {
  if (decoder_ != nullptr) {
    ReleaseDecoder();
  }
  decoder_ = OH_AudioCodec_CreateByMime(mime_type.c_str(), false);
  if (decoder_ == nullptr) {
    LOG(ERROR) << __func__
               << " [AudioDecoder] create decoder by min failed.mime_type: "
               << mime_type.c_str();
    return OhosAudioDecoderCode::kDecoderError;
  }
  mime_type_ = AudioMimeType::kMimetypeUnknow;
  for (auto it = kMimeTypeMap.begin(); it != kMimeTypeMap.end(); it++) {
    if (strcmp(it->first, mime_type.c_str()) == 0) {
      mime_type_ = it->second;
    }
  }

  AudioDecoderCallbackManager::AddAudioDecoder(this);
  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::CreateAudioDecoderByName(
    const std::string& name) {
  if (decoder_ != nullptr) {
    LOG(ERROR) << __func__
               << " [AudioDecoder] create decoder by name failed.name: "
               << name.c_str();
    ReleaseDecoder();
  }

  decoder_ = OH_AudioCodec_CreateByName(name.c_str());
  if (decoder_ == nullptr) {
    LOG(ERROR) << __func__
               << " [AudioDecoder] AudioCodecDecoder create decoder failed.";
    return OhosAudioDecoderCode::kDecoderError;
  }
  GetMimeType();
  AudioDecoderCallbackManager::AddAudioDecoder(this);
  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::ConfigureDecoder(
    const std::shared_ptr<OhosAudioDecoderFormat> format) {
  if (decoder_ == nullptr) {
    LOG(ERROR) << __func__ << " [AudioDecoder] AudioCodecDecoder is nullptr.";
    return OhosAudioDecoderCode::kDecoderError;
  }

  if (format == nullptr) {
    LOG(ERROR) << __func__
               << " [AudioDecoder] AudioCodecDecoder format is nullptr.";
    return OhosAudioDecoderCode::kDecoderError;
  }

  OH_AVFormat* av_format = OH_AVFormat_Create();
  if (av_format == nullptr) {
    LOG(ERROR) << __func__
               << " [AudioDecoder] AudioCodecDecoder create avformat fail.";
    return OhosAudioDecoderCode::kDecoderError;
  }

  OH_AVFormat_SetIntValue(av_format, OH_MD_KEY_AUD_SAMPLE_RATE,
                          format->GetSampleRate());
  OH_AVFormat_SetIntValue(av_format, OH_MD_KEY_AUD_CHANNEL_COUNT,
                          format->GetChannelCount());
  OH_AVErrCode err_code = OH_AudioCodec_Configure(decoder_, av_format);
  OH_AVFormat_Destroy(av_format);
  av_format = nullptr;
  if (err_code != AV_ERR_OK) {
    LOG(ERROR)
        << __func__
        << " [AudioDecoder] AudioCodecDecoder set config fail, err_code = "
        << static_cast<int>(err_code);
    return OhosAudioDecoderCode::kDecoderError;
  }
  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::SetParameterDecoder(
    const std::shared_ptr<OhosAudioDecoderFormat> format) {
  if (decoder_ == nullptr) {
    LOG(ERROR) << __func__ << "[AudioDecoder] AudioCodecDecoder is nullptr.";
    return OhosAudioDecoderCode::kDecoderError;
  }
  if (format == nullptr) {
    LOG(ERROR) << __func__ << "[AudioDecoder] format is nullptr.";
    return OhosAudioDecoderCode::kDecoderError;
  }

  OH_AVFormat* av_format = OH_AVFormat_Create();
  if (av_format == nullptr) {
    LOG(ERROR) << __func__
               << "[AudioDecoder] AudioCodecDecoder create avformat fail.";
    return OhosAudioDecoderCode::kDecoderError;
  }

  OH_AVFormat_SetIntValue(av_format, OH_MD_KEY_AUD_SAMPLE_RATE,
                          format->GetSampleRate());
  OH_AVFormat_SetIntValue(av_format, OH_MD_KEY_AUD_CHANNEL_COUNT,
                          format->GetChannelCount());
  OH_AVErrCode err_code = OH_AudioCodec_SetParameter(decoder_, av_format);
  OH_AVFormat_Destroy(av_format);
  av_format = nullptr;
  if (err_code != AV_ERR_OK) {
    LOG(ERROR) << __func__
               << "[AudioDecoder] AudioCodecDecoder set config fail, err_code = "
               << static_cast<int>(err_code);
    return OhosAudioDecoderCode::kDecoderError;
  }
  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::PrepareDecoder() {
  if (decoder_ == nullptr) {
    LOG(ERROR) << __func__ << "[AudioDecoder] AudioCodecDecoder is nullptr.";
    return OhosAudioDecoderCode::kDecoderError;
  }

  OH_AVErrCode err_code = OH_AudioCodec_Prepare(decoder_);
  if (err_code != AV_ERR_OK) {
    LOG(ERROR) << __func__ << "[AudioDecoder] prepare decoder fail, err_code = "
               << static_cast<int>(err_code);
    return OhosAudioDecoderCode::kDecoderError;
  }
  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::StartDecoder() {
  if (decoder_ == nullptr) {
    LOG(ERROR) << __func__ << "[AudioDecoder] AudioCodecDecoder is nullptr.";
    return OhosAudioDecoderCode::kDecoderError;
  }

  OH_AVErrCode err_code = OH_AudioCodec_Start(decoder_);
  if (err_code != AV_ERR_OK) {
    LOG(ERROR)
        << __func__
        << "[AudioDecoder] AudioCodecDecoder start decoder fail, err_code = "
        << static_cast<int>(err_code);
    return OhosAudioDecoderCode::kDecoderError;
  }
  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::StopDecoder() {
  if (decoder_ == nullptr) {
    LOG(ERROR) << __func__ << "[AudioDecoder] AudioCodecDecoder is nullptr.";
    return OhosAudioDecoderCode::kDecoderError;
  }

  OH_AVErrCode err_code = OH_AudioCodec_Stop(decoder_);
  if (err_code != AV_ERR_OK) {
    LOG(ERROR)
        << __func__
        << "[AudioDecoder] AudioCodecDecoder stop decoder fail, err_code = "
        << static_cast<int>(err_code);
    return OhosAudioDecoderCode::kDecoderError;
  }
  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::FlushDecoder() {
  if (decoder_ == nullptr) {
    LOG(ERROR) << __func__ << "[AudioDecoder] AudioCodecDecoder is nullptr.";
    return OhosAudioDecoderCode::kDecoderError;
  }

  OH_AVErrCode err_code = OH_AudioCodec_Flush(decoder_);
  if (err_code != AV_ERR_OK) {
    LOG(ERROR)
        << __func__
        << "[AudioDecoder] AudioCodecDecoder flush decoder fail, err_code = "
        << static_cast<int>(err_code);
    return OhosAudioDecoderCode::kDecoderError;
  }

  // clear input and output buffers
  {
    std::unique_lock<std::mutex> lock(in_mutex_);
    input_buffers_.clear();
  }
  {
    std::unique_lock<std::mutex> lock(out_mutex_);
    output_buffers_.clear();
  }
  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::ResetDecoder() {
  if (decoder_ == nullptr) {
    LOG(ERROR) << __func__ << "[AudioDecoder] AudioCodecDecoder is nullptr.";
    return OhosAudioDecoderCode::kDecoderError;
  }

  OH_AVErrCode err_code = OH_AudioCodec_Reset(decoder_);
  if (err_code != AV_ERR_OK) {
    LOG(ERROR)
        << __func__
        << "[AudioDecoder] AudioCodecDecoder start reset fail, err_code = "
        << static_cast<int>(err_code);
    return OhosAudioDecoderCode::kDecoderError;
  }

  // clear input and output buffers
  {
    std::unique_lock<std::mutex> lock(in_mutex_);
    input_buffers_.clear();
  }
  {
    std::unique_lock<std::mutex> lock(out_mutex_);
    output_buffers_.clear();
  }
  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::ReleaseDecoder() {
  if (decoder_ == nullptr) {
    return OhosAudioDecoderCode::kDecoderOk;
  }

  AudioDecoderCallbackManager::DeleteAudioDecoder(decoder_);
  OH_AVErrCode err_code = OH_AudioCodec_Destroy(decoder_);
  if (err_code != AV_ERR_OK) {
    LOG(ERROR) << __func__ << "[AudioDecoder] destroy decoder_ fail, err_code = "
               << static_cast<int>(err_code);
    return OhosAudioDecoderCode::kDecoderError;
  }
  decoder_ = nullptr;

  // clear input and output buffers
  {
    std::unique_lock<std::mutex> lock(in_mutex_);
    input_buffers_.clear();
  }
  {
    std::unique_lock<std::mutex> lock(out_mutex_);
    output_buffers_.clear();
  }

  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::SetAVCencInfo(
    OH_AVCencInfo* av_cenc_info,
    std::shared_ptr<OhosAudioCencInfo> cenc_info) {
  OH_AVErrCode err_no = OH_AVCencInfo_SetAlgorithm(
      av_cenc_info, static_cast<DrmCencAlgorithm>(cenc_info->GetAlgo()));
  if (err_no != AV_ERR_OK) {
    LOG(ERROR) << __func__
               << "[WiseplayDRM] set AVCencInfo Algorithm fail, err_no = "
               << static_cast<uint32_t>(err_no);
    return OhosAudioDecoderCode::kDecoderError;
  }

  err_no = OH_AVCencInfo_SetKeyIdAndIv(
      av_cenc_info, cenc_info->GetKeyId(), cenc_info->GetKeyIdLen(),
      cenc_info->GetIv(), cenc_info->GetIvLen());
  if (err_no != AV_ERR_OK) {
    LOG(ERROR) << __func__
               << "[WiseplayDRM] AudioCodecDecoder set AVCencInfo keyid and iv "
                  "fail, err_no = "
               << static_cast<uint32_t>(err_no);
    return OhosAudioDecoderCode::kDecoderError;
  }

  DrmSubsample sub_samples[cenc_info->GetClearHeaderLens().size()];
  for (uint32_t i = 0; i < cenc_info->GetClearHeaderLens().size(); i++) {
    sub_samples[i].clearHeaderLen = cenc_info->GetClearHeaderLens()[i];
    sub_samples[i].payLoadLen = cenc_info->GetPayLoadLens()[i];
  }
  err_no = OH_AVCencInfo_SetSubsampleInfo(
      av_cenc_info, cenc_info->GetEncryptedBlockCount(),
      cenc_info->GetSkippedBlockCount(), cenc_info->GetFirstEncryptedOffset(),
      cenc_info->GetClearHeaderLens().size(), sub_samples);
  if (err_no != AV_ERR_OK) {
    LOG(ERROR) << __func__
               << "[WiseplayDRM] AudioCodecDecoder set AVCencInfo "
                  "subsampleInfo fail, err_no = "
               << static_cast<uint32_t>(err_no);
    return OhosAudioDecoderCode::kDecoderError;
  }

  err_no = OH_AVCencInfo_SetMode(
      av_cenc_info, DrmCencInfoMode::DRM_CENC_INFO_KEY_IV_SUBSAMPLES_SET);
  if (err_no != AV_ERR_OK) {
    LOG(ERROR)
        << __func__
        << "[WiseplayDRM] AudioCodecDecoder set AVCencInfo mode fail, err_no = "
        << static_cast<uint32_t>(err_no);
    return OhosAudioDecoderCode::kDecoderError;
  }
  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::SetBufferCencInfo(
    uint32_t index,
    std::shared_ptr<OhosAudioCencInfo> cenc_info) {
  if (cenc_info == nullptr) {
    LOG(ERROR) << __func__
               << "[WiseplayDRM] AudioCodecDecoder cenc_info is nullptr.";
    return OhosAudioDecoderCode::kDecoderError;
  }
  OH_AVCencInfo* av_cenc_info = OH_AVCencInfo_Create();
  if (av_cenc_info == nullptr) {
    LOG(ERROR) << __func__
               << "[WiseplayDRM] AudioCodecDecoder create AVCencInfo fail.";
    return OhosAudioDecoderCode::kDecoderError;
  }

  OhosAudioDecoderCode ret = SetAVCencInfo(av_cenc_info, cenc_info);
  if (ret != OhosAudioDecoderCode::kDecoderOk) {
    LOG(ERROR)
        << __func__
        << "[WiseplayDRM] AudioCodecDecoder SetAVCencInfo result is not ok.";
    return ret;
  }
  // set CencInfo to AVBuffer
  OH_AVBuffer* av_buffer = GetInputBuffer(index);
  if (av_buffer == nullptr) {
    LOG(ERROR) << __func__
               << "[WiseplayDRM] set AVCencInfo fail, not find inputbuffererr, "
                  "index = "
               << index;
    return OhosAudioDecoderCode::kDecoderError;
  }
  OH_AVErrCode err_no = OH_AVCencInfo_SetAVBuffer(av_cenc_info, av_buffer);
  if (err_no != AV_ERR_OK) {
    LOG(ERROR) << __func__ << "[WiseplayDRM] set AVCencInfo fail, err_no = "
               << static_cast<uint32_t>(err_no);
    return OhosAudioDecoderCode::kDecoderError;
  }
  err_no = OH_AVCencInfo_Destroy(av_cenc_info);
  if (err_no != AV_ERR_OK) {
    LOG(ERROR) << __func__ << "[WiseplayDRM] destroy cenc_info fail, err_no = "
               << static_cast<uint32_t>(err_no);
    return OhosAudioDecoderCode::kDecoderError;
  }
  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::QueueInputBufferDecOhos(
    uint32_t index,
    int64_t presentation_time_us,
    uint8_t* buffer_data,
    int32_t buffer_size,
    std::shared_ptr<OhosAudioCencInfo> cenc_info,
    bool is_encrypted,
    BufferFlag flag) {
  if (decoder_ == nullptr) {
    LOG(ERROR) << __func__
               << "[AudioDecoder] AudioCodecDecoder decoder_ is nullptr.";
    return OhosAudioDecoderCode::kDecoderError;
  }

  if (is_encrypted &&
      SetBufferCencInfo(index, cenc_info) != OhosAudioDecoderCode::kDecoderOk) {
    LOG(ERROR) << __func__
               << "[WiseplayDRM] AudioCodecDecoder SetBufferCencInfo failed";
    return OhosAudioDecoderCode::kDecoderError;
  }

  OH_AVBuffer* av_buffer = GetInputBuffer(index);
  if (av_buffer == nullptr) {
    LOG(ERROR) << __func__ << " [AudioDecoder]  inputbuffer not find." << index;
    return OhosAudioDecoderCode::kDecoderError;
  }
  uint8_t* addr = OH_AVBuffer_GetAddr(av_buffer);
  if (flag != BufferFlag::kCodecBufferFlagEos) {
    if (buffer_data == nullptr) {
      LOG(ERROR)
          << __func__
          << " [AudioDecoder] AudioCodecDecoder index buffer_data is nullptr."
          << index;
      return OhosAudioDecoderCode::kDecoderError;
    }
    // size of addr is OH_AVBuffer_GetCapacity(av_buffer) and is ensured >=
    // buffer_size, so memcpy should not have problem
    if (OH_AVBuffer_GetCapacity(av_buffer) >= buffer_size) {
      memcpy(addr, buffer_data, buffer_size);
    } else {
      LOG(ERROR) << __func__ << "[AudioDecoder] memcpy copy fail source size: "
                 << buffer_size
                 << " dest size: " << OH_AVBuffer_GetCapacity(av_buffer);
      return OhosAudioDecoderCode::kDecoderError;
    }
  }

  // The size is the length of each frame of data to be decoded. The pts is the
  // timestamp for each frame, indicating when the audio should be played. The
  // values of size and pts are obtained from the audio/video resource file or
  // the data stream to be decoded.
  OH_AVCodecBufferAttr attr = {0};
  if (flag == BufferFlag::kCodecBufferFlagEos) {
    attr.size = 0;
    attr.flags = static_cast<uint32_t>(BufferFlag::kCodecBufferFlagEos);
  } else {
    attr.size = buffer_size;
    attr.flags = static_cast<uint32_t>(BufferFlag::kCodecBufferFlagNone);
  }
  attr.pts = presentation_time_us;
  OH_AVBuffer_SetBufferAttr(av_buffer, &attr);
  OH_AVErrCode err_code = OH_AudioCodec_PushInputBuffer(decoder_, index);
  if (err_code == AV_ERR_DRM_DECRYPT_FAILED) {
    // workaround until 6.1, treat DRM_DECRYPT_FAILED as NO_KEY
    LOG(WARNING)
        << __func__
        << "[WiseplayDRM] OH_AudioCodec_PushInputBuffer failed, "
           "result: AV_ERR_DRM_DECRYPT_FAILED.  and return as kDecoderNoKey";
    return OhosAudioDecoderCode::kDecoderNoKey;
  }

  if (err_code != AV_ERR_OK) {
    LOG(ERROR) << __func__ << "[AudioDecoder] err_code =" << static_cast<int>(err_code);
    return OhosAudioDecoderCode::kDecoderError;
  }
  std::unique_lock<std::mutex> lock(in_mutex_);
  input_buffers_.erase(index);
  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::GetOutputFormatDec(
    std::shared_ptr<OhosAudioDecoderFormat> format) {
  if (decoder_ == nullptr) {
    LOG(ERROR) << __func__ << "[AudioDecoder] AudioCodecDecoder is nullptr.";
    return OhosAudioDecoderCode::kDecoderError;
  }

  OH_AVFormat* av_format = OH_AudioCodec_GetOutputDescription(decoder_);
  if (av_format == nullptr) {
    LOG(ERROR)
        << __func__
        << "[AudioDecoder] AudioCodecDecoder get output description fail.";
    return OhosAudioDecoderCode::kDecoderError;
  }
  GetParamFromAVFormat(av_format, format);
  OH_AVFormat_Destroy(av_format);
  av_format = nullptr;
  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::ReleaseOutputBufferDec(uint32_t index) {
  if (decoder_ == nullptr) {
    LOG(ERROR) << __func__
               << "[AudioDecoder] AudioCodecDecoder index is nullptr. index:"
               << index;
    return OhosAudioDecoderCode::kDecoderError;
  }

  OH_AVBuffer* av_buffer = GetOutputBuffer(index);
  if (av_buffer == nullptr) {
    LOG(ERROR) << __func__
               << "[AudioDecoder] ReleaseOutputBufferDec fail, outputbuffer not "
                  "find. index:"
               << index;
    return OhosAudioDecoderCode::kDecoderError;
  }

  OH_AVCodecBufferAttr attr = {0};
  OH_AVErrCode err_code = OH_AVBuffer_GetBufferAttr(av_buffer, &attr);
  if (err_code != AV_ERR_OK) {
    LOG(ERROR) << __func__
               << "[AudioDecoder] AudioCodecDecoder index get output buffer "
                  "attr fail. index:"
               << index << ",error_code: " << static_cast<int>(err_code);
    return OhosAudioDecoderCode::kDecoderError;
  }

  err_code = OH_AudioCodec_FreeOutputBuffer(decoder_, index);
  if (err_code != AV_ERR_OK) {
    LOG(ERROR) << __func__
               << "[AudioDecoder] AudioCodecDecoder index get output buffer "
                  "attr fail. index:"
               << index << ",error_code: " << static_cast<int>(err_code);
    return OhosAudioDecoderCode::kDecoderError;
  }
  {
    std::unique_lock<std::mutex> lock(out_mutex_);
    output_buffers_.erase(index);
  }

  if (attr.flags == static_cast<uint32_t>(BufferFlag::kCodecBufferFlagEos)) {
    LOG(WARNING)
        << __func__
        << "[AudioDecoder] free output buffer, buffer flag is eos. index:"
        << index;
  }
  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::SetCallbackDec() {
  if (decoder_ == nullptr) {
    LOG(ERROR) << __func__
               << "[AudioDecoder] AudioCodecDecoder decoder is nullptr.";
    return OhosAudioDecoderCode::kDecoderError;
  }

  struct OH_AVCodecCallback cb = {
      &AudioDecoderCallbackManager::OnError,
      &AudioDecoderCallbackManager::OnOutputFormatChanged,
      &AudioDecoderCallbackManager::OnInputBufferAvailable,
      &AudioDecoderCallbackManager::OnOutputBufferAvailable};

  // Instead of using the ADecBufferSignal structure recommended by the media
  // framework for buffer rotation, implement it in the chromium kernel to
  // reduce the business logic at the webview layer.
  OH_AVErrCode err_code = OH_AudioCodec_RegisterCallback(decoder_, cb, nullptr);
  if (err_code != AV_ERR_OK) {
    LOG(ERROR) << __func__
               << "[AudioDecoder] register callback fail, err_code = "
               << static_cast<int>(err_code);
    return OhosAudioDecoderCode::kDecoderError;
  }

  return OhosAudioDecoderCode::kDecoderOk;
}

OhosAudioDecoderCode OhosAudioDecoder::SetDecryptionConfig(void* session,
                                                           bool secure_audio) {
  if (session == nullptr) {
    LOG(ERROR) << __func__
               << "[WiseplayDRM] AudioCodecDecoder session is nullptr.";
    return OhosAudioDecoderCode::kDecoderOk;
  }

  // The media audio codec module currently only supports non-secure decoding
  // mode.
  secure_audio = false;

  MediaKeySession* media_key_session = static_cast<MediaKeySession*>(session);
  OH_AVErrCode err_code = OH_AudioCodec_SetDecryptionConfig(
      decoder_, media_key_session, secure_audio);
  if (err_code != AV_ERR_OK) {
    LOG(ERROR) << __func__
               << "[WiseplayDRM] set decryption config fail, err_code = "
               << static_cast<int>(err_code);
    return OhosAudioDecoderCode::kDecoderOk;
  }

  return OhosAudioDecoderCode::kDecoderOk;
}

void OhosAudioDecoder::GetParamFromAVFormat(
    OH_AVFormat* av_format,
    std::shared_ptr<OhosAudioDecoderFormat> format) {
  if (av_format == nullptr || format == nullptr) {
    LOG(ERROR) << __func__
               << "[AudioDecoder] AudioCodecDecoder av_format or format is "
                  "nullptr, av_format is "
               << int32_t(av_format == nullptr) << ", format is."
               << int32_t(format == nullptr);
    return;
  }

  int32_t sample_rate = 0;
  int32_t channels = 0;
  int32_t max_input_size = 0;
  int32_t aac_is_adts = 0;
  int32_t sample_format = 0;
  int64_t bit_rate = 0;
  int32_t id_header = 0;
  int32_t setup_header = 0;
  uint8_t* codec_config = nullptr;
  size_t codec_config_size = 0;
  OH_AVFormat_GetIntValue(av_format, OH_MD_KEY_AUD_SAMPLE_RATE, &sample_rate);
  OH_AVFormat_GetIntValue(av_format, OH_MD_KEY_AUD_CHANNEL_COUNT, &channels);
  OH_AVFormat_GetIntValue(av_format, OH_MD_KEY_MAX_INPUT_SIZE, &max_input_size);
  OH_AVFormat_GetIntValue(av_format, OH_MD_KEY_AAC_IS_ADTS, &aac_is_adts);
  OH_AVFormat_GetIntValue(av_format, OH_MD_KEY_AUDIO_SAMPLE_FORMAT,
                          &sample_format);
  OH_AVFormat_GetLongValue(av_format, OH_MD_KEY_BITRATE, &bit_rate);
  OH_AVFormat_GetIntValue(av_format, OH_MD_KEY_IDENTIFICATION_HEADER,
                          &id_header);
  OH_AVFormat_GetIntValue(av_format, OH_MD_KEY_SETUP_HEADER, &setup_header);
  OH_AVFormat_GetBuffer(av_format, OH_MD_KEY_CODEC_CONFIG, &codec_config,
                        &codec_config_size);

  format->SetSampleRate(sample_rate);
  format->SetChannelCount(channels);
  format->SetMaxInputSize(max_input_size);
  if (aac_is_adts != 0) {
    format->SetAACIsAdts(true);
  } else {
    format->SetAACIsAdts(false);
  }
  format->SetAudioSampleFormat(sample_format);
  format->SetBitRate(bit_rate);
  format->SetIdentificationHeader(id_header);
  format->SetSetupHeader(setup_header);
  format->SetCodecConfig(codec_config);
  format->SetCodecConfigSize(uint32_t(codec_config_size));
}

// AudioDecoderCallbackManager

std::map<OH_AVCodec*, media::OhosAudioDecoder*>
    AudioDecoderCallbackManager::decoders_;

std::mutex AudioDecoderCallbackManager::decoders_map_mutex_;

media::OhosAudioDecoder* AudioDecoderCallbackManager::FindAudioDecoder(
    OH_AVCodec* codec) {
  std::unique_lock<std::mutex> lock(decoders_map_mutex_);
  if (decoders_.find(codec) != decoders_.end()) {
    return decoders_.at(codec);
  }
  return nullptr;
}

void AudioDecoderCallbackManager::AddAudioDecoder(
    media::OhosAudioDecoder* decoder) {
  if (decoder == nullptr || decoder->GetAVCodec() == nullptr) {
    return;
  }
  std::unique_lock<std::mutex> lock(decoders_map_mutex_);
  if (decoders_.find(decoder->GetAVCodec()) != decoders_.end()) {
    return;
  }
  decoders_[decoder->GetAVCodec()] = decoder;
}

void AudioDecoderCallbackManager::DeleteAudioDecoder(OH_AVCodec* codec) {
  if (codec == nullptr) {
    return;
  }
  std::unique_lock<std::mutex> lock(decoders_map_mutex_);
  auto it = decoders_.find(codec);
  if (it != decoders_.end()) {
    decoders_.erase(codec);
  }
}

void AudioDecoderCallbackManager::OnError(OH_AVCodec* codec,
                                          int32_t error_code,
                                          void* user_data) {
  (void)user_data;
  if (codec == nullptr) {
    LOG(ERROR)
        << __func__
        << "[AudioDecoder] AudioDecoderCallbackManager avcodec is nullptr.";
    return;
  }
  std::unique_lock<std::mutex> lock(OhosAudioDecoder::GetDecoderMutex());
  media::OhosAudioDecoder* audio_decoder = FindAudioDecoder(codec);
  if (audio_decoder == nullptr) {
    LOG(ERROR) << __func__
               << "[AudioDecoder] AudioDecoderCallbackManager not find decoder.";
    return;
  }

  audio_decoder->OnError(error_code);
}

void AudioDecoderCallbackManager::OnOutputFormatChanged(OH_AVCodec* codec,
                                                        OH_AVFormat* format,
                                                        void* user_data) {
  if (codec == nullptr) {
    LOG(ERROR)
        << __func__
        << "[AudioDecoder] AudioDecoderCallbackManager avcodec is nullptr.";
    return;
  }
  std::unique_lock<std::mutex> lock(OhosAudioDecoder::GetDecoderMutex());
  media::OhosAudioDecoder* audio_decoder = FindAudioDecoder(codec);
  if (audio_decoder == nullptr) {
    LOG(ERROR) << __func__
               << "[AudioDecoder] AudioDecoderCallbackManager not find decoder.";
    return;
  }

  audio_decoder->UpdateOutputFormat();
}

void AudioDecoderCallbackManager::OnInputBufferAvailable(OH_AVCodec* codec,
                                                         uint32_t index,
                                                         OH_AVBuffer* data,
                                                         void* user_data) {
  if (codec == nullptr) {
    LOG(ERROR) << __func__ << "[AudioDecoder] avcodec is nullptr.";
    return;
  }

  if (data == nullptr) {
    LOG(ERROR) << __func__ << "[AudioDecoder] avbuffer is nullptr.";
    return;
  }
  std::unique_lock<std::mutex> lock(OhosAudioDecoder::GetDecoderMutex());
  media::OhosAudioDecoder* audio_decoder = FindAudioDecoder(codec);
  if (audio_decoder == nullptr) {
    LOG(ERROR) << __func__ << " [AudioDecoder] not find decoder.";
    return;
  }

  audio_decoder->SetInputBuffer(index, data);
  audio_decoder->AddInputBuffer(index);
}

void AudioDecoderCallbackManager::OnOutputBufferAvailable(OH_AVCodec* codec,
                                                          uint32_t index,
                                                          OH_AVBuffer* data,
                                                          void* user_data) {
  (void)user_data;
  if (codec == nullptr) {
    LOG(ERROR)
        << __func__
        << " [AudioDecoder] AudioDecoderCallbackManager avcodec is nullptr.";
    return;
  }
  std::unique_lock<std::mutex> lock(OhosAudioDecoder::GetDecoderMutex());
  media::OhosAudioDecoder* audio_decoder = FindAudioDecoder(codec);
  if (audio_decoder == nullptr) {
    LOG(ERROR)
        << __func__
        << " [AudioDecoder] AudioDecoderCallbackManager not find decoder.";
    return;
  }

  audio_decoder->SetOutputBuffer(index, data);

  OH_AVCodecBufferAttr attr = {0};
  OH_AVErrCode err_code = OH_AVBuffer_GetBufferAttr(data, &attr);
  if (err_code != AV_ERR_OK || attr.size < 0) {
    LOG(ERROR)
        << __func__
        << " [AudioDecoder] AudioCodecDecoder get buffer attr fail. ret: "
        << static_cast<int>(err_code);
    return;
  }

  uint8_t buffer_data[attr.size];
  // memcpy size should be less or euqal to source size and destination size
  if (sizeof(buffer_data) >= static_cast<size_t>(attr.size)) {
    memcpy(buffer_data, reinterpret_cast<uint8_t*>(OH_AVBuffer_GetAddr(data)),
           attr.size);
  } else {
    LOG(ERROR) << "[AudioDecoder] memcpy copy fail" << __func__
               << " source size: " << static_cast<size_t>(attr.size)
               << " dest size: " << sizeof(buffer_data);
    return;
  }

  audio_decoder->AddOutputBuffer(index, buffer_data, attr.size, attr.pts,
                                 static_cast<BufferFlag>(attr.flags));
}

}  // namespace media
