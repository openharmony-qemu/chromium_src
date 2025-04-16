// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cmath>
#include <memory>
#include <algorithm>
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
#include "third_party/bounds_checking_function/include/securec.h"

#include "ohos_adapter_helper.h"
#include "ohos_audio_decoder.h"

namespace media {
namespace {
  constexpr base::TimeDelta TwoSecondTimeout = base::Seconds(2);
  constexpr int32_t MaxTimeOutCount = 2;
}  // namespace

class AudioDecoderCallback;

int32_t OHOSAudioDecoderFormat::GetSampleRate() {
  return sample_rate_;
}

int32_t OHOSAudioDecoderFormat::GetChannelCount() {
  return channel_count_;
}

int64_t OHOSAudioDecoderFormat::GetBitRate() {
  return bit_rate_;
}

int32_t OHOSAudioDecoderFormat::GetMaxInputSize() {
  return max_input_size_;
}
bool OHOSAudioDecoderFormat::GetAACIsAdts() {
  return is_adts_;
}

int32_t OHOSAudioDecoderFormat::GetAudioSampleFormat() {
  return audio_sample_format_;
}

int32_t OHOSAudioDecoderFormat::GetIdentificationHeader() {
  return id_header_;
}

int32_t OHOSAudioDecoderFormat::GetSetupHeader() {
  return setup_header_;
}

uint8_t* OHOSAudioDecoderFormat::GetCodecConfig() {
  return codec_config_;
}

uint32_t OHOSAudioDecoderFormat::GetCodecConfigSize() {
  return codec_config_size_;
}

void OHOSAudioDecoderFormat::SetSampleRate(int32_t sampleRate) {
  sample_rate_ = sampleRate;
}

void OHOSAudioDecoderFormat::SetChannelCount(int32_t channelCount) {
  channel_count_ = channelCount;
}

void OHOSAudioDecoderFormat::SetBitRate(int64_t bitRate) {
  bit_rate_ = bitRate;
}

void OHOSAudioDecoderFormat::SetMaxInputSize(int32_t maxInputSize) {
  max_input_size_ = maxInputSize;
}

void OHOSAudioDecoderFormat::SetAACIsAdts(bool isAdts) {
  is_adts_ = isAdts;
}

void OHOSAudioDecoderFormat::SetAudioSampleFormat(int32_t audioSampleFormat) {
  audio_sample_format_ = audioSampleFormat;
}

void OHOSAudioDecoderFormat::SetIdentificationHeader(int32_t idHeader) {
  id_header_ = idHeader;
}

void OHOSAudioDecoderFormat::SetSetupHeader(int32_t setupHeader) {
  setup_header_ = setupHeader;
}

void OHOSAudioDecoderFormat::SetCodecConfig(uint8_t* codecConfig) {
  codec_config_ = codecConfig;
}

void OHOSAudioDecoderFormat::SetCodecConfigSize(uint32_t size) {
  codec_config_size_ = size;
}

AudioDecoderCallback::AudioDecoderCallback(Client* client) : client_(client) {}

AudioDecoderCallback::~AudioDecoderCallback() {}

void AudioDecoderCallback::OnError(int32_t errorCode) {
  LOG(ERROR) << "OHOSAudioDecoder::AudioDecoderCallback::OnError, errorCode: " << errorCode;
  if (client_) {
    client_->OnError(errorCode);
  }
}

void AudioDecoderCallback::OnOutputFormatChanged() {
  LOG(DEBUG) << "OHOSAudioDecoder::AudioDecoderCallback::OnOutputFormatChanged";
  client_->UpdateOutputFormat();
}

void AudioDecoderCallback::OnInputBufferAvailable(uint32_t index) {
  LOG(DEBUG) << "OHOSAudioDecoder::AudioDecoderCallback::OnInputBufferAvailable inputbuffer available index " << index;
  client_->AddInputBuffer(index);
}

void AudioDecoderCallback::OnOutputBufferAvailable(uint32_t index, uint8_t* bufferData,
    int32_t size, int64_t pts, int32_t offset, uint32_t flags) {
  LOG(DEBUG) << "OHOSAudioDecoder outputbuffer available index " << index << " size " << size;
  client_->AddOutputBuffer(index, bufferData, size, pts, static_cast<BufferFlag>(flags));
}

OHOSAudioCencInfo::~OHOSAudioCencInfo() {}

OHOSAudioDecoder::OHOSAudioDecoder(scoped_refptr<base::SequencedTaskRunner> task_runner) : state_(UNINITIALIZED),
    sample_format_(kSampleFormatS16),
    channel_count_(0),
    channel_layout_(CHANNEL_LAYOUT_NONE),
    sample_rate_(0),
    decoder_format_(std::make_shared<OHOSAudioDecoderFormat>()),
    task_runner_(task_runner),
    pool_(base::MakeRefCounted<AudioBufferMemoryPool>()) {
  LOG(INFO) << "OHOSAudioDecoder::OHOSAudioDecoder initialize";
  TRACE_EVENT0("media", "OHOSAudioDecoder::OHOSAudioDecoder");
  io_timer_.SetTaskRunner(scoped_refptr<base::SingleThreadTaskRunner>());
}

OHOSAudioDecoder::~OHOSAudioDecoder() {
  TRACE_EVENT0("media", "OHOSAudioDecoder::~OHOSAudioDecoder");
  decoder_loop_.reset();
  // Cancel previously registered callback (if any).
  if (ohos_crypto_context_) {
    ohos_crypto_context_->SetOHOSMediaCryptoReadyCB(base::NullCallback());
  }

  LOG(INFO) << "OHOSAudioDecoder::~OHOSAudioDecoder ReleaseDecoder";
  if (audio_decoder_ != nullptr) {
    audio_decoder_->StopDecoder();
    audio_decoder_->ReleaseDecoder();
    audio_decoder_ = nullptr;
    audio_decoder_created_ = false;
  }

  ClearInputQueue(DecoderStatus::Codes::kAborted);
  io_timer_.Stop();
}

AudioDecoderType OHOSAudioDecoder::GetDecoderType() const {
  return AudioDecoderType::kOhos;
}

void OHOSAudioDecoder::Initialize(const AudioDecoderConfig& config,
    CdmContext* cdm_context,
    InitCB init_cb,
    const OutputCB& output_cb,
    const WaitingCB& waiting_cb) {
  LOG(INFO) << "OHOSAudioDecoder::Initialize";
  TRACE_EVENT0("media", "OHOSAudioDecoder::Initialize");
  // Only the encrypted DRM audio stream goes through the openharmony system decoding path
  if (!config.is_encrypted()) {
    LOG(ERROR) << "OHOSAudioDecoder::Initialize AudioDecoderConfig is not encrypted";
    base::BindPostTaskToCurrentDefault(std::move(init_cb))
      .Run(DecoderStatus::Codes::kUnsupportedCodec);
    return;
  }

  // Clear the input buffer and set the callback result to DecoderStatus::Codes::kAborted
  ClearInputQueue(DecoderStatus::Codes::kAborted);

  if (state_ == ERROR) {
    LOG(INFO) << "OHOSAudioDecoder::Initialize state error";
    base::BindPostTaskToCurrentDefault(std::move(init_cb)).Run(DecoderStatus::Codes::kFailed);
    return;
  }

  // Check if the audio format is supported
  mime_type_ = GetMimeType(config);
  sample_format_ = kSampleFormatS16;
  if (mime_type_.empty()) {
    LOG(ERROR) << "OHOSAudioDecoder::Initialize, Unsupported codec";
    base::BindPostTaskToCurrentDefault(std::move(init_cb))
      .Run(DecoderStatus::Codes::kUnsupportedCodec);
    return;
  }
  LOG(INFO) << "OHOSAudioDecoder::Initialize mime type: " << mime_type_;

  PrepareParameters(config);
  output_cb_ = base::BindPostTaskToCurrentDefault(output_cb);
  waiting_cb_ = base::BindPostTaskToCurrentDefault(waiting_cb);

  // Decryption judgment, if it is an encrypted stream and there is no decryption configuration, exit directly
  if (config_.is_encrypted()) {
    LOG(INFO) << "OHOSAudioDecoder::Initialize config_.is_encrypted ";
    if (!cdm_context || !cdm_context->GetOHOSMediaCryptoContext()) {
      LOG(ERROR) << "OHOSAudioDecoder::Initialize The stream is encrypted but there is no CdmContext "
        << "or MediaCryptoContext is not supported";
      SetState(ERROR);
      base::BindPostTaskToCurrentDefault(std::move(init_cb))
        .Run(DecoderStatus::Codes::kUnsupportedEncryptionMode);
      return;
    }
    SetState(WAITING_FOR_MEDIA_CRYPTO);
    SetCdm(cdm_context, std::move(init_cb));
    return;
  }
  InitializeNotEncrypted(std::move(init_cb));
}

std::string OHOSAudioDecoder::GetMimeType(AudioDecoderConfig config) {
  TRACE_EVENT0("media", "OHOSAudioDecoder::GetMimeType");
  switch (config.codec()) {
    case AudioCodec::kAAC:
      mime_type_ = "audio/mp4a-latm";
      break;
    case AudioCodec::kFLAC:
      mime_type_ = "audio/flac";
      break;
    case AudioCodec::kVorbis:
      mime_type_ = "audio/vorbis";
      break;
    case AudioCodec::kMP3:
      mime_type_ = "audio/mpeg";
      break;
    case AudioCodec::kAMR_NB:
      mime_type_ = "audio/3gpp";
      break;
    case AudioCodec::kAMR_WB:
      mime_type_ = "audio/amr-wb";
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
      break;
  }
  return mime_type_;
}

void OHOSAudioDecoder::PrepareParameters(AudioDecoderConfig config) {
  TRACE_EVENT0("media", "OHOSAudioDecoder::PrepareParameters");
  config_ = config;
  channel_layout_ = config_.channel_layout();
  channel_count_ = ChannelLayoutToChannelCount(channel_layout_);
  sample_rate_ = config_.samples_per_second();
  timestamp_helper_ = std::make_unique<AudioTimestampHelper>(sample_rate_);
}

void OHOSAudioDecoder::InitializeNotEncrypted(InitCB init_cb) {
  TRACE_EVENT0("media", "OHOSAudioDecoder::InitializeNotEncrypted");
  if (!InitAudioDecoder(mime_type_)) {
    LOG(ERROR) << "OHOSAudioDecoder::Initialize initAudioDecoder error";
    base::BindPostTaskToCurrentDefault(std::move(init_cb))
      .Run(DecoderStatus::Codes::kFailed);
    return;
  }
  LOG(INFO) << "OHOSAudioDecoder::Initialize start create loop ";

  CreateOhosDecoderLoop();

  SetState(READY);
  LOG(INFO) << "OHOSAudioDecoder::Initialize loop ready ";
  base::BindPostTaskToCurrentDefault(std::move(init_cb))
    .Run(DecoderStatus::Codes::kOk);
}

void OHOSAudioDecoder::SetCdm(CdmContext* cdm_context, InitCB init_cb) {
  TRACE_EVENT0("media", "OHOSAudioDecoder::SetCdm");
  LOG(INFO) << "OHOSAudioDecoder::SetCdm enter";
  if (!cdm_context) {
    LOG(DEBUG) << "OHOSAudioDecoder::SetCdm No CDM provided";
    base::BindPostTaskToCurrentDefault(std::move(init_cb)).Run(DecoderStatus::Codes::kFailed);
    return;
  }
  ohos_crypto_context_ = cdm_context->GetOHOSMediaCryptoContext();

  event_cb_registration_ = cdm_context->RegisterEventCB(base::BindRepeating(
      &OHOSAudioDecoder::OnCdmContextEvent, weak_factory_.GetWeakPtr()));

  ohos_crypto_context_->SetOHOSMediaCryptoReadyCB(
      base::BindPostTaskToCurrentDefault(
          base::BindOnce(&OHOSAudioDecoder::OnMediaCryptoReady,
                         weak_factory_.GetWeakPtr(), std::move(init_cb))));
}

void OHOSAudioDecoder::OnCdmContextEvent(CdmContext::Event event) {
  LOG(INFO) << "OHOSAudioDecoder::OnCdmContextEvent enter";
  if (event != CdmContext::Event::kHasAdditionalUsableKey) {
    return;
  }

  waiting_for_key_ = false;
  SetState(READY);
  io_timer_.Stop();
  if (decoder_loop_) {
    decoder_loop_->OnKeyAdded();
  }
}

void OHOSAudioDecoder::OnMediaCryptoReady(InitCB init_cb, void* session, bool requires_secure_video_codec) {
  TRACE_EVENT0("media", "OHOSAudioDecoder::OnMediaCryptoReady");
  LOG(INFO) << "OHOSAudioDecoder::OnMediaCryptoReady enter";
  if (session == nullptr) {
    LOG(ERROR) << "OHOSAudioDecoder::OnMediaCryptoReady can't play encrypted stream";
    SetState(UNINITIALIZED);
    std::move(init_cb).Run(DecoderStatus::Codes::kUnsupportedEncryptionMode);
    return;
  }

  mediaKeySession_ = std::move(session);

  if (!InitAudioDecoder(mime_type_)) {
    LOG(ERROR) << "OHOSAudioDecoder::OnMediaCryptoReady initAudioDecoder error";
    base::BindPostTaskToCurrentDefault(std::move(init_cb))
      .Run(DecoderStatus::Codes::kFailed);
    return;
  }

  // After receiving |mediaKeySession_| we can configure MediaCodec.
  if (!CreateOhosDecoderLoop()) {
    SetState(UNINITIALIZED);
    std::move(init_cb).Run(DecoderStatus::Codes::kFailed);
    return;
  }

  SetState(READY);
  std::move(init_cb).Run(DecoderStatus::Codes::kOk);
}

bool OHOSAudioDecoder::InitAudioDecoder(std::string mime_type) {
  TRACE_EVENT0("media", "OHOSAudioDecoder::InitAudioDecoder");
  LOG(INFO) << "OHOSAudioDecoder::InitAudioDecoder";
  audio_decoder_ = OhosAdapterHelper::GetInstance().CreateAudioCodecDecoderAdapter();
  if (!audio_decoder_) {
    LOG(ERROR) << "OHOSAudioDecoder::Initialize audio_decoder_ is null ";
  }

  std::shared_ptr<OHOSAudioDecoderFormat> audioDecoderFormat = std::make_shared<OHOSAudioDecoderFormat>();
  // Sampling rate and number of channels are required, bit rate and maximum output length are optional
  audioDecoderFormat->SetSampleRate(sample_rate_);
  audioDecoderFormat->SetChannelCount(channel_count_);

  AudioDecoderAdapterCode ret;
  if (!audio_decoder_created_) {
    ret = audio_decoder_->CreateAudioDecoderByMime(mime_type);
    if (ret != AudioDecoderAdapterCode::DECODER_OK) {
      LOG(ERROR) << "OHOSAudioDecoder::InitAudioDecoder CreateAudioDecoderByMime Failed mime: " << mime_type;
      return false;
    }
    audio_decoder_created_ = true;
  }

  decoder_callback_ = std::make_unique<AudioDecoderCallback>(this);
  if (!decoder_callback_) {
    LOG(ERROR) << "OHOSAudioDecoder::InitAudioDecoder Get decoderCallback failed.";
    return false;
  }
  audio_decoder_->SetCallbackDec(decoder_callback_);
  // Set the decryption key. Audio currently only supports soft decryption mode.
  if (config_.is_encrypted() && mediaKeySession_) {
    audio_decoder_->SetDecryptionConfig(mediaKeySession_, false);
  }
  audio_decoder_->ConfigureDecoder(audioDecoderFormat);
  audio_decoder_->PrepareDecoder();
  ret = audio_decoder_->StartDecoder();
  if (ret != AudioDecoderAdapterCode::DECODER_OK) {
    LOG(ERROR) << "OHOSAudioDecoder::InitAudioDecoder StartDecoder err";
    return false;
  }
  ret = audio_decoder_->GetOutputFormatDec(decoder_format_);
  if (ret != AudioDecoderAdapterCode::DECODER_OK) {
    LOG(ERROR) << "OHOSAudioDecoder::InitAudioDecoder GetOutputFormatDec err";
    return false;
  }
  LOG(DEBUG) << "OHOSAudioDecoder::InitAudioDecoder format: GetSampleRate: " << decoder_format_->GetSampleRate()
    << " GetChannelCount: " << decoder_format_->GetChannelCount();
  return true;
}

bool OHOSAudioDecoder::CreateOhosDecoderLoop() {
  decoder_loop_.reset();
  decoder_loop_ = std::make_unique<OHOSAudioDecoderLoop>(this, scoped_refptr<base::SingleThreadTaskRunner>());
  return true;
}

void OHOSAudioDecoder::SetState(State new_state) {
  LOG(INFO)<< "OHOSAudioDecoder::SetState state_: " << static_cast<int32_t>(state_)
    << " new_state: " << static_cast<int32_t>(new_state);
  state_ = new_state;
}

void OHOSAudioDecoder::ClearInputQueue(DecoderStatus decode_status) {
  LOG(INFO) << "OHOSAudioDecoder::ClearInputQueue";
  for (auto& entry : input_queue_) {
    std::move(entry.second).Run(decode_status);
  }
  input_queue_.clear();
}

void OHOSAudioDecoder::OnError(int32_t errorCode) {
  if (state_ != WAITING_FOR_MEDIA_CRYPTO) {
    SetState(WAITING_FOR_MEDIA_CRYPTO);
  }
}

void OHOSAudioDecoder::Decode(scoped_refptr<DecoderBuffer> buffer, DecodeCB decode_cb) {
  LOG(DEBUG) << "OHOSAudioDecoder::Decode";
  DecodeCB cb = base::BindPostTaskToCurrentDefault(std::move(decode_cb));
  if (!DecoderBuffer::DoSubsamplesMatch(*buffer)) {
    LOG(ERROR) << "OHOSAudioDecoder::DoSubsamplesMatch error";
    std::move(cb).Run(DecoderStatus::Codes::kFailed);
    return;
  }

  if (!buffer->end_of_stream() && buffer->timestamp() == kNoTimestamp) {
    LOG(ERROR) <<"OHOSAudioDecoder::Decode "<< buffer->AsHumanReadableString()
      << ": no timestamp, skipping this buffer";
    std::move(cb).Run(DecoderStatus::Codes::kFailed);
    return;
  }

  if (state_ == ERROR) {
    LOG(ERROR) << "OHOSAudioDecoder::Decode "<< buffer->AsHumanReadableString()
      << ": Error state, returning decode error for all buffers";
    ClearInputQueue(DecoderStatus::Codes::kFailed);
    std::move(cb).Run(DecoderStatus::Codes::kFailed);
    return;
  }
  if (state_ != READY) {
    LOG(ERROR) << "OHOSAudioDecoder::Decode unexpected state"<< state_;
  }

  input_queue_.push_back(std::make_pair(std::move(buffer), std::move(cb)));
  LOG(DEBUG) << "OHOSAudioDecoder::Decode add input_queue_ success";
  decoder_loop_->ExpectWork();
  LOG(DEBUG) << "OHOSAudioDecoder::Decode loop start success";
}

void OHOSAudioDecoder::Reset(base::OnceClosure closure) {
  LOG(INFO) << "OHOSAudioDecoder::Reset";
  ClearInputQueue(DecoderStatus::Codes::kAborted);
  bool success = decoder_loop_->TryFlush();
  input_buffer_queue_.clear();
  output_buffer_queue_.clear();
  if (success) {
    success = CreateOhosDecoderLoop();
  }
  io_timer_.Stop();
  audio_decoder_->StartDecoder();
  timestamp_helper_->SetBaseTimestamp(kNoTimestamp);
  SetState(success ? READY : ERROR);
  task_runner_->PostTask(FROM_HERE, std::move(closure));
}

bool OHOSAudioDecoder::NeedsBitstreamConversion() const {
  // An AAC stream needs to be converted as ADTS stream.
  LOG(INFO) << "OHOSAudioDecoder::NeedsBitstreamConversion";
  return config_.codec() == AudioCodec::kAAC;
}

void OHOSAudioDecoder::OnInputDataQueued(bool success) {
  if (input_queue_.front().first->end_of_stream() && success) {
    LOG(DEBUG) << "OHOSAudioDecoder::OnInputDataQueued input queue has eos";
    return;
  }
  // Queuing is successful, set the callback to ok
  std::move(input_queue_.front().second)
    .Run(success ? DecoderStatus::Codes::kOk : DecoderStatus::Codes::kFailed);
  LOG(DEBUG) << "OHOSAudioDecoder::OnInputDataQueued remove success";
  input_queue_.pop_front();
}

bool OHOSAudioDecoder::IsAnyInputPending() const {
  if (state_ != READY) {
    return false;
  }
  return !input_queue_.empty();
}

static void SetCencInfoToInputData(OHOSAudioDecoderLoop::InputData& data, const DecryptConfig* decrypt_config)
{
  if (decrypt_config == nullptr) {
    return;
  }

  std::vector<uint32_t> clearHeaderLens;
  std::vector<uint32_t> payLoadLens;
  data.cenc_info = std::make_shared<OHOSAudioCencInfo>();
  data.cenc_info->SetKeyId(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(decrypt_config->key_id().data())));
  data.cenc_info->SetKeyIdLen(decrypt_config->key_id().size());
  data.cenc_info->SetIv(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(decrypt_config->iv().data())));
  data.cenc_info->SetIvLen(decrypt_config->iv().size());

  switch (decrypt_config->encryption_scheme()) {
    case EncryptionScheme::kUnencrypted:
      data.cenc_info->SetAlgo(uint32_t(DrmCencAlgorithmAdapter::DRM_ALG_CENC_UNENCRYPTED));
      data.is_encrypted = false;
      break;
    case EncryptionScheme::kCenc:
      data.cenc_info->SetAlgo(uint32_t(DrmCencAlgorithmAdapter::DRM_ALG_CENC_AES_CTR));
      data.is_encrypted = true;
      break;
    case EncryptionScheme::kCbcs:
      data.cenc_info->SetAlgo(uint32_t(DrmCencAlgorithmAdapter::DRM_ALG_CENC_AES_CBC));
      data.is_encrypted = true;
      break;
    default:
      data.is_encrypted = false;
      // Currently the kernel only supports AES-CTR and AES-CBC encryption algorithm modes
      data.cenc_info->SetAlgo(uint32_t(DrmCencAlgorithmAdapter::DRM_ALG_CENC_UNENCRYPTED));
  }

  if (decrypt_config->encryption_pattern()) {
    data.cenc_info->SetEncryptedBlockCount(decrypt_config->encryption_pattern()->crypt_byte_block());
    data.cenc_info->SetSkippedBlockCount(decrypt_config->encryption_pattern()->skip_byte_block());
  }

  // The kernel does not involve offset, the default setting is 0
  data.cenc_info->SetFirstEncryptedOffset(0);
  for (size_t i = 0; i < decrypt_config->subsamples().size(); i++) {
    clearHeaderLens.push_back(decrypt_config->subsamples()[i].clear_bytes);
    payLoadLens.push_back(decrypt_config->subsamples()[i].cypher_bytes);
  }

  data.cenc_info->SetClearHeaderLens(clearHeaderLens);
  data.cenc_info->SetPayLoadLens(payLoadLens);
  // The web kernel sets keyid and iv by default, so DRM_CENC_INFO_KEY_IV_SUBSAMPLES_SET is selected by default here
  data.cenc_info->SetMode(uint32_t(DrmCencInfoModeAdapter::DRM_CENC_INFO_KEY_IV_SUBSAMPLES_SET));
}

OHOSAudioDecoderLoop::InputData OHOSAudioDecoder::ProvideInputData() {
  LOG(DEBUG) << "OHOSAudioDecoder::ProvideInputData";
  const DecoderBuffer* decoder_buffer = input_queue_.front().first.get();
  OHOSAudioDecoderLoop::InputData data;

  if (decoder_buffer == nullptr) {
    LOG(WARNING) << "OHOSAudioDecoder::ProvideInputData , decoder_buffer is null";
    data.is_valid = false;
    return data;
  }

  if (decoder_buffer->end_of_stream()) {
    LOG(DEBUG) << "OHOSAudioDecoder::ProvideInputData get eos";
    data.memory = const_cast<uint8_t*>(decoder_buffer->data());
    data.is_eos = true;
    data.length = 0;
    data.presentation_time = decoder_buffer->timestamp();
  } else {
    data.memory = const_cast<uint8_t*>(decoder_buffer->data());
    data.length = decoder_buffer->data_size();
    LOG(DEBUG) << "OHOSAudioDecoder::ProvideInputData get normal data length: " << data.length;

    const DecryptConfig* decrypt_config = decoder_buffer->decrypt_config();
    if (decrypt_config) {
      SetCencInfoToInputData(data, decrypt_config);
    }

    data.presentation_time = decoder_buffer->timestamp();
  }
  return data;
}

bool OHOSAudioDecoder::OnDecodedEos(const OutputBufferData& out) {
  LOG(INFO) << "OHOSAudioDecoder::OnDecodedEos";
  if (!input_queue_.size() || !input_queue_.front().first->end_of_stream()) {
    LOG(WARNING) << "OHOSAudioDecoder::OnDecodedEos: received unexpected eos";
    return false;
  }
  // EOS frame direct callback ok
  std::move(input_queue_.front()).second.Run(DecoderStatus::Codes::kOk);
  input_queue_.pop_front();
  output_buffer_queue_.pop_front();
  LOG(DEBUG) << "OHOSAudioDecoder::OnDecodedEos output_buffer_queue_ pop front ok";
  return true;
}

bool OHOSAudioDecoder::OnDecodedFrame(const OutputBufferData& out) {
  if (out.size_ == 0U || out.index_ == OHOSAudioDecoderLoop::kInvalidBufferIndex
    || decoder_loop_ == nullptr || channel_count_ == 0) {
      LOG(ERROR) << "OHOSAudioDecoder::OnDecodedFrame buffer data is invalid";
      return false;
  }

  LOG(DEBUG) << "OHOSAudioDecoder::OnDecodedFrame";
  size_t frame_count = 1;
  scoped_refptr<AudioBuffer> audio_buffer;

  const int bytes_per_frame = sizeof(uint16_t) * channel_count_;
  frame_count = out.size_ / bytes_per_frame;

  LOG(DEBUG) << "OHOSAudioDecoder::AudioBuffer sample_format_: " << sample_format_
    << " channel_layout_: " << channel_layout_ << " channel_count_: " << channel_count_
    << " sample_rate_: " << sample_rate_<< " frame_count: " << frame_count;

  audio_buffer = AudioBuffer::CreateBuffer(sample_format_, channel_layout_,
            channel_count_, sample_rate_,
            frame_count, pool_);

  if (memcpy_s(audio_buffer->channel_data()[0], out.size_, out.data_.data(), out.size_) != EOK) {
    LOG(ERROR) << "OHOSAudioDecoder::OnDecodedFrame copy frame data error";
    audio_decoder_->ReleaseOutputBufferDec(out.index_);
    return false;
  }

  LOG(DEBUG) << "OHOSAudioDecoder::OnDecodedFrame audio_buffer release output buffer index: " << out.index_;
  audio_decoder_->ReleaseOutputBufferDec(out.index_);
  output_buffer_queue_.pop_front();

  const bool first_buffer = timestamp_helper_->base_timestamp() == kNoTimestamp;
  if (first_buffer) {
    // Seek, baseTimestamp needs to be set to the time of the first frame,
    // the pts unit given by the oh system audio measurement is ms
    timestamp_helper_->SetBaseTimestamp(std::max(base::TimeDelta(), base::Milliseconds(out.pts_)));
  }
  // Set the timestamp of the audio buffer.
  // timestamp_helper_->GetTimestamp() Calculates the current timestamp based on the base timestamp
  // and the number of frames added.
  audio_buffer->set_timestamp(timestamp_helper_->GetTimestamp());
  LOG(DEBUG) << "OHOSAudioDecoder::OnDecodedFrame timestamp_helper_->GetTimestamp(): "
    << timestamp_helper_->GetTimestamp();

  timestamp_helper_->AddFrames(frame_count);
  output_cb_.Run(audio_buffer);
  LOG(DEBUG) << "OHOSAudioDecoder::OnDecodedFrame callback data ok";
  return true;
}

void OHOSAudioDecoder::OnCodecLoopError() {
  LOG(ERROR) << "OHOSAudioDecoder::OnCodecLoopError";
  SetState(ERROR);
  ClearInputQueue(DecoderStatus::Codes::kFailed);
}

int32_t OHOSAudioDecoder::DequeueInputBuffer(int64_t& buffer_index) {
  LOG(DEBUG) << "OHOSAudioDecoder::DequeueInputBuffer";
  if (!input_buffer_queue_.empty()) {
    buffer_index = input_buffer_queue_.front();
    input_buffer_queue_.pop_front();
    LOG(DEBUG) << "OHOSAudioDecoder::DequeueInputBuffer return index: " << buffer_index;
    return buffer_index;
  }
  LOG(DEBUG) << "OHOSAudioDecoder::DequeueInputBuffer return index: -1";
  return -1;
}

void OHOSAudioDecoder::EnqueueInputBuffer(int64_t buffer_index) {
  LOG(DEBUG) << "OHOSAudioDecoder::EnqueueInputBuffer index: " << buffer_index;
  input_buffer_queue_.push_back(buffer_index);
}

int32_t OHOSAudioDecoder::DequeueOutputBuffer(OutputBufferData& out) {
  LOG(DEBUG) << "OHOSAudioDecoder::DequeueOutputBuffer";
  if (!output_buffer_queue_.empty()) {
    out = output_buffer_queue_.front();
    // Output buffer queue will be popped after decoding is successful
    LOG(DEBUG) << "OHOSAudioDecoder::DequeueOutputBuffer ok index: " << out.index_ << " size:" << out.size_;
    return 0;
  }
  LOG(DEBUG) << "OHOSAudioDecoder::DequeueOutputBuffer get fail";
  return -1;
}

AudioDecoderAdapterCode OHOSAudioDecoder::FlushDecoder() {
  return audio_decoder_->FlushDecoder();
}

void OHOSAudioDecoder::WaitingForLicence()
{
  // Define a timer to check the license status, The timer will check the license status after 2 seconds,
  // and if the license is still invalid, it will retry once more. After 2 attempts, if the license is
  // still invalid, the timer will stop.
  LOG(INFO) << "OHOSAudioDecoderLoop::WaitingForLicence time_out_count_ = " << time_out_count_;
  if (!waiting_for_key_) {
    LOG(WARNING) << "OHOSAudioDecoder::WaitingForLicence get key and stop timer";
    io_timer_.Stop();
    time_out_count_ = 0;
    return;
  }
  if (time_out_count_ > MaxTimeOutCount) {
    io_timer_.Stop();
    time_out_count_ = 0;
    LOG(WARNING) << "OHOSAudioDecoder::WaitingForLicence not get key and stop timer";
    SetState(ERROR);
    return;
  }
  time_out_count_++;
  if (!io_timer_.IsRunning()) {
    LOG(INFO) << "OHOSAudioDecoder::WaitingForLicence start timer";
    io_timer_.Start(FROM_HERE, TwoSecondTimeout, this, &OHOSAudioDecoder::WaitingForLicence);
  }
}
 
AudioDecoderAdapterCode OHOSAudioDecoder::QueueInputBufferDec(uint32_t index, int64_t presentationTimeUs,
  uint8_t* bufferData, int32_t bufferSize, std::shared_ptr<AudioCencInfoAdapter> cencInfo, bool isEncrypted,
  BufferFlag flag) {
  if (state_ == WAITING_FOR_MEDIA_CRYPTO) {
    LOG(DEBUG) << "OHOSAudioDecoder::QueueInputBufferDec error, state = WAITING_FOR_MEDIA_CRYPTO";
    return AudioDecoderAdapterCode::DECODER_RETRY;
  }
  AudioDecoderAdapterCode ret = audio_decoder_->QueueInputBufferDec(index, presentationTimeUs, bufferData,
    bufferSize, cencInfo, isEncrypted, flag);
  if (ret == AudioDecoderAdapterCode::DECODER_ERROR && waiting_for_key_) {
    SetState(WAITING_FOR_MEDIA_CRYPTO);
    LOG(WARNING) << "OHOSAudioDecoder::QueueInputBufferDec error, wait for key";
    if (!io_timer_.IsRunning()) {
      WaitingForLicence();
    }
    return AudioDecoderAdapterCode::DECODER_RETRY;
  }
  return ret;
}

AudioDecoderAdapterCode OHOSAudioDecoder::ReleaseOutputBufferDec(uint32_t index) {
  return audio_decoder_->ReleaseOutputBufferDec(index);
}

void OHOSAudioDecoder::AddInputBuffer(uint32_t index) {
  LOG(DEBUG) << "OHOSAudioDecoder::AddInputBuffer index: " << index;
  input_buffer_queue_.push_back(index);
}

void OHOSAudioDecoder::AddOutputBuffer(uint32_t index, uint8_t* bufferData, uint32_t size,
    int64_t pts, BufferFlag flag) {
  LOG(DEBUG) << "OHOSAudioDecoder::AddOutputBuffer index: " << index << " size: " << size << " pts: " << pts;
  OutputBufferData data(index, bufferData, size, pts, flag);
  output_buffer_queue_.push_back(data);
}

void OHOSAudioDecoder::UpdateOutputFormat() {
  AudioDecoderAdapterCode ret = audio_decoder_->GetOutputFormatDec(decoder_format_);
  if (ret != AudioDecoderAdapterCode::DECODER_OK) {
    LOG(ERROR) << "OHOSAudioDecoder::UpdateOutputFormat err";
  }
}

} // namespace media
