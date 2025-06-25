// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_media_decoder_bridge_impl.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>

#include "base/logging.h"
#include "base/task/task_runner.h"
#include "base/trace_event/trace_event.h"
#include "decoder_format_adapter_impl.h"
#include "audio_cenc_info_adapter.h"
#include "media/filters/ohos/ohos_audio_decoder.h"
#include "ohos_glue/base/include/ark_web_errno.h"
#include "third_party/bounds_checking_function/include/securec.h"
#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "gpu/ipc/common/nweb_native_window_tracker.h"
#endif // ARKWEB_VIDEO_ASSISTANT

#include "media/filters/ohos/ohos_audio_decoder.h"
#include "ohos_glue/base/include/ark_web_errno.h"

using namespace media;
using namespace OHOS::NWeb;
using namespace std;

const std::string PRODUCT_MODEL_EMULATOR = "emulator";

void clearInputQueue(std::queue<VideoBridgeDecoderInputBuffer>& q) {
  std::queue<VideoBridgeDecoderInputBuffer> empty;
  std::swap(empty, q);
}

void clearOutputQueue(std::queue<VideoBridgeDecoderOutputBuffer>& q) {
  std::queue<VideoBridgeDecoderOutputBuffer> empty;
  std::swap(empty, q);
}

VideoBridgeCodecConfig::VideoBridgeCodecConfig() = default;
VideoBridgeCodecConfig::~VideoBridgeCodecConfig() = default;

// static
std::unique_ptr<MediaCodecDecoderBridgeImpl>
MediaCodecDecoderBridgeImpl::CreateVideoDecoder(
    const VideoBridgeCodecConfig& config) {
  LOG(INFO) << "MediaCodecDecoderBridgeImpl::CreateVideoDecoder.";
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance();
  std::string product_model =
      system_properties_adapter.GetDeviceInfoProductModel();
  if (product_model == PRODUCT_MODEL_EMULATOR) {
    return nullptr;
  }
  std::string codec_type;
  if (config.codec == media::VideoCodec::kH264) {
    LOG(INFO) << "OhosVideoDecoder::CreateCodec video/avc";
    codec_type = "video/avc";
  } else if (config.codec == media::VideoCodec::kHEVC) {
    LOG(INFO) << "OhosVideoDecoder::CreateCodec video/hevc";
    codec_type = "video/hevc";
  } else {
    LOG(ERROR) << "OhosVideoDecoder::CreateCodec not supported type.";
    return nullptr;
  }
  return absl::WrapUnique(new MediaCodecDecoderBridgeImpl(
      codec_type, config.on_buffers_available_cb));
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::PrepareForCallback() {
  if (signal_ == nullptr) {
    signal_ = make_shared<DecoderBridgeSignal>();
  }

  if (cb_ == nullptr) {
    cb_ = make_shared<CodecBridgeCallback>(signal_);
  }
  return videoDecoder_->SetCallbackDec(cb_);
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::CreateVideoBridgeDecoderByMime(
    std::string mimetype) {
  LOG(INFO) << "MediaCodecDecoderBridgeImpl::CreateVideoBridgeDecoderByMime.";
  if (videoDecoder_ == nullptr) {
    LOG(ERROR) << "decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  DecoderAdapterCode ret = videoDecoder_->CreateVideoDecoderByMime(mimetype);
  if (ret == DecoderAdapterCode::DECODER_ERROR) {
    LOG(ERROR) << "create decoder by mime failed.";
    return ret;
  }
  hasCreated_ = true;

  return PrepareForCallback();
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::CreateVideoBridgeDecoderByName(
    std::string name) {
  LOG(INFO) << "create video decoder by name, type : " << name.c_str();
  if (videoDecoder_ == nullptr) {
    LOG(ERROR) << "decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  DecoderAdapterCode ret =
      videoDecoder_->CreateVideoDecoderByName(name.c_str());
  if (ret == DecoderAdapterCode::DECODER_ERROR) {
    LOG(ERROR) << "create decoder by name failed.";
    return ret;
  }
  hasCreated_ = true;

  return PrepareForCallback();
}

MediaCodecDecoderBridgeImpl::MediaCodecDecoderBridgeImpl(
    std::string codec_type,
    base::RepeatingClosure on_buffers_available_cb) {
  LOG(INFO) << "MediaCodecDecoderBridgeImpl::MediaCodecDecoderBridgeImpl.";
  if (!on_buffers_available_cb) {
    return;
  }
  videoDecoder_ =
      OhosAdapterHelper::GetInstance().CreateMediaCodecDecoderAdapter();
  DecoderAdapterCode ret = CreateVideoBridgeDecoderByMime(codec_type);
  if (ret == DecoderAdapterCode::DECODER_ERROR) {
    LOG(ERROR) << "create decoder failed.";
    return;
  }
  cb_->on_buffers_available_cb_ = on_buffers_available_cb;
}

MediaCodecDecoderBridgeImpl::~MediaCodecDecoderBridgeImpl() {
  LOG(INFO) << "MediaCodecDecoderBridgeImpl::~MediaCodecDecoderBridgeImpl.";
  ReleaseBridgeDecoder();
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::ConfigureBridgeDecoder(
    const DecoderFormat& format,
    scoped_refptr<base::SequencedTaskRunner> decoder_task_runner) {
  LOG(INFO) << "MediaCodecDecoderBridgeImpl::ConfigureBridgeDecoder configure "
               "decoder.";
  width_ = format.width;
  height_ = format.height;
  decoder_task_runner_ = decoder_task_runner;
  cb_->decoder_callback_task_runner_ = decoder_task_runner;
  if (videoDecoder_ == nullptr) {
    LOG(ERROR) << "MediaCodecDecoderBridgeImpl::ConfigureBridgeDecoder decoder "
                  "is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

    std::shared_ptr<DecoderFormatAdapterImpl> formatAdapter =
      std::make_shared<DecoderFormatAdapterImpl>();
  if (!formatAdapter) {
    LOG(ERROR) << "MediaCodecDecoderBridgeImpl::ConfigureBridgeDecoder "
                  "DecoderFormatAdapterImpl is NULL";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  formatAdapter->SetWidth(format.width);
  formatAdapter->SetHeight(format.height);
  formatAdapter->SetFrameRate(format.frameRate);

  return videoDecoder_->ConfigureDecoder(formatAdapter);
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::SetBridgeParameterDecoder(
    const DecoderFormat& format) {
  LOG(INFO) << " MediaCodecDecoderBridgeImpl::SetBridgeParameterDecoder set "
               "decoder parameter.";
  if (videoDecoder_ == nullptr) {
    LOG(ERROR) << "MediaCodecDecoderBridgeImpl::SetBridgeParameterDecoder "
                  "decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  std::shared_ptr<DecoderFormatAdapterImpl> formatAdapter =
      std::make_shared<DecoderFormatAdapterImpl>();
  if (!formatAdapter) {
    LOG(ERROR) << "MediaCodecDecoderBridgeImpl::SetBridgeParameterDecoder "
                  "DecoderFormatAdapterImpl is NULL";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  formatAdapter->SetWidth(format.width);
  formatAdapter->SetHeight(format.height);
  formatAdapter->SetFrameRate(format.frameRate);

  return videoDecoder_->SetParameterDecoder(formatAdapter);
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::SetBridgeOutputSurface(
    void* window) {
  LOG(INFO) << "MediaCodecDecoderBridgeImpl::SetBridgeOutputSurface set "
               "decoder outputsurface.";
  if (videoDecoder_ == nullptr || window == nullptr) {
    LOG(ERROR) << "MediaCodecDecoderBridgeImpl::SetBridgeOutputSurface decoder "
                  "is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  window_from_surface_ = window;
#endif // ARKWEB_VIDEO_ASSISTANT
  return videoDecoder_->SetOutputSurface(window);
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::GetOutputFormatBridgeDecoder(
    DecoderFormat& format) {
  LOG(DEBUG) << "MediaCodecDecoderBridgeImpl::GetOutputFormatBridgeDecoder get "
                "decoder outputformat.";
  if (videoDecoder_ == nullptr) {
    LOG(ERROR) << "MediaCodecDecoderBridgeImpl::GetOutputFormatBridgeDecoder "
                  "decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  std::shared_ptr<DecoderFormatAdapterImpl> formatAdapter =
      std::make_shared<DecoderFormatAdapterImpl>();
  if (!formatAdapter) {
    LOG(ERROR) << "MediaCodecDecoderBridgeImpl::GetOutputFormatBridgeDecoder "
                  "DecoderFormatAdapterImpl is NULL";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  DecoderAdapterCode result = videoDecoder_->GetOutputFormatDec(formatAdapter);
  format.width = formatAdapter->GetWidth();
  format.height = formatAdapter->GetHeight();
  format.frameRate = formatAdapter->GetFrameRate();

  return result;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::PrepareBridgeDecoder() {
  LOG(INFO)
      << "MediaCodecDecoderBridgeImpl::PrepareBridgeDecoder prepare decoder.";
  if (videoDecoder_ == nullptr) {
    LOG(ERROR) << " MediaCodecDecoderBridgeImpl::PrepareBridgeDecoder decoder "
                  "is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  return videoDecoder_->PrepareDecoder();
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::StartBridgeDecoder() {
  LOG(INFO) << "MediaCodecDecoderBridgeImpl::StartBridgeDecoder start decoder.";
  std::lock_guard<std::recursive_mutex> lock(decoderMutex_);
  isRunning_.store(true);

  if (videoDecoder_ == nullptr) {
    LOG(ERROR)
        << "MediaCodecDecoderBridgeImpl::StartBridgeDecoder decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  return videoDecoder_->StartDecoder();
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::StopBridgeDecoder() {
  LOG(INFO) << "MediaCodecDecoderBridgeImpl::StopBridgeDecoder stop decoder.";
  std::lock_guard<std::recursive_mutex> lock(decoderMutex_);
  if (videoDecoder_ == nullptr) {
    LOG(ERROR)
        << "MediaCodecDecoderBridgeImpl::StopBridgeDecoder decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  isRunning_.store(false);
  return videoDecoder_->StopDecoder();
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::FlushBridgeDecoder() {
  LOG(INFO)
      << " MediaCodecDecoderBridgeImpl::FlushBridgeDecoder flush decoder.";
  std::lock_guard<std::recursive_mutex> lock(decoderMutex_);
  if (videoDecoder_ == nullptr) {
    LOG(ERROR)
        << "MediaCodecDecoderBridgeImpl::FlushBridgeDecoder decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  if (signal_ == nullptr) {
    return DecoderAdapterCode::DECODER_ERROR;
  }

  signal_->isDecoderFlushing_.store(true);

  DecoderAdapterCode ret = videoDecoder_->FlushDecoder();
  if (ret != DecoderAdapterCode::DECODER_OK) {
    LOG(ERROR) << " MediaCodecDecoderBridgeImpl::FlushBridgeDecoder flush "
                  "decoder failed.";
    return ret;
  }

  clearInputQueue(signal_->inputQueue_);
  clearOutputQueue(signal_->outputQueue_);

  decoder_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&MediaCodecDecoderBridgeImpl::UpdateFlushToFalse,
                     base::Unretained(this)));
  return StartBridgeDecoder();
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::ResetBridgeDecoder() {
  LOG(INFO) << "MediaCodecDecoderBridgeImpl::ResetBridgeDecoder reset decoder.";
  std::lock_guard<std::recursive_mutex> lock(decoderMutex_);
  if (videoDecoder_ == nullptr) {
    LOG(ERROR)
        << "MediaCodecDecoderBridgeImpl::ResetBridgeDecoder decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  if (signal_ == nullptr) {
    return DecoderAdapterCode::DECODER_ERROR;
  }

  isRunning_.store(false);
  signal_->isDecoderFlushing_.store(true);
  DecoderAdapterCode ret = videoDecoder_->ResetDecoder();
  if (ret != DecoderAdapterCode::DECODER_OK) {
    LOG(ERROR) << " MediaCodecDecoderBridgeImpl::ResetBridgeDecoder reset "
                  "decoder failed.";
    return ret;
  }

  clearInputQueue(signal_->inputQueue_);
  clearOutputQueue(signal_->outputQueue_);

  decoder_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&MediaCodecDecoderBridgeImpl::UpdateFlushToFalse,
                     base::Unretained(this)));
  return ret;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::ReleaseBridgeDecoder() {
  LOG(INFO)
      << "MediaCodecDecoderBridgeImpl::ReleaseBridgeDecoder release decoder.";
  std::lock_guard<std::recursive_mutex> lock(decoderMutex_);
  if (videoDecoder_ == nullptr) {
    LOG(ERROR)
        << "MediaCodecDecoderBridgeImpl::ReleaseBridgeDecoder decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  isRunning_.store(false);

  DecoderAdapterCode ret = videoDecoder_->ReleaseDecoder();

  return ret;
}

void MediaCodecDecoderBridgeImpl::PopInqueueDec() {
  LOG(DEBUG) << "MediaCodecDecoderBridgeImpl::PopInqueueDec";
  if (signal_ == nullptr || signal_->isOnError_ ||
      signal_->inputQueue_.empty()) {
    return;
  }
  signal_->inputQueue_.pop();
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::PushInbufferDec(
    const uint32_t index,
    const uint32_t& bufferSize,
    const int64_t& time) {
  BufferFlag bufferFlag;
  if (isFirstDecFrame_) {
    bufferFlag = BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA;
    isFirstDecFrame_ = false;
  } else {
    bufferFlag = BufferFlag::CODEC_BUFFER_FLAG_NONE;
  }
  LOG(DEBUG) << "PushInbufferDec index:" << index
             << ", buffersize:" << bufferSize;
  return videoDecoder_->QueueInputBufferDec(index, time, bufferSize, 0,
                                            bufferFlag);
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::PushInbufferDecEos(
    const uint32_t index) {
  BufferFlag bufferFlag;
  bufferFlag = BufferFlag::CODEC_BUFFER_FLAG_EOS;
  return videoDecoder_->QueueInputBufferDec(index, 0, 0, 0, bufferFlag);
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::SetAVCencInfo(uint32_t index, const DecryptConfig* decrypt_config)
{
  if (videoDecoder_ == nullptr) {
    LOG(ERROR) << "MediaCodecDecoderBridgeImpl::SetAVCencInfo decoder is NULL";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  if (decrypt_config == nullptr) {
    return DecoderAdapterCode::DECODER_ERROR;
  }

  std::vector<uint32_t> clearHeaderLens;
  std::vector<uint32_t> payLoadLens;
  std::shared_ptr<OHOSAudioCencInfo> cenc_info = std::make_shared<OHOSAudioCencInfo>();
  cenc_info->SetKeyId(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(decrypt_config->key_id().data())));
  cenc_info->SetKeyIdLen(decrypt_config->key_id().size());
  cenc_info->SetIv(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(decrypt_config->iv().data())));
  cenc_info->SetIvLen(decrypt_config->iv().size());

  switch (decrypt_config->encryption_scheme()) {
    case EncryptionScheme::kUnencrypted:
      cenc_info->SetAlgo(uint32_t(DrmCencAlgorithmAdapter::DRM_ALG_CENC_UNENCRYPTED));
      break;
    case EncryptionScheme::kCenc:
      cenc_info->SetAlgo(uint32_t(DrmCencAlgorithmAdapter::DRM_ALG_CENC_AES_CTR));
      break;
    case EncryptionScheme::kCbcs:
      cenc_info->SetAlgo(uint32_t(DrmCencAlgorithmAdapter::DRM_ALG_CENC_AES_CBC));
      break;
    default:
      // Currently the kernel only supports AES-CTR and AES-CBC encryption algorithm modes
      cenc_info->SetAlgo(uint32_t(DrmCencAlgorithmAdapter::DRM_ALG_CENC_UNENCRYPTED));
  }

  if (decrypt_config->encryption_pattern()) {
    cenc_info->SetEncryptedBlockCount(decrypt_config->encryption_pattern()->crypt_byte_block());
    cenc_info->SetSkippedBlockCount(decrypt_config->encryption_pattern()->skip_byte_block());
  }

  // The kernel does not involve offset, the default setting is 0
  cenc_info->SetFirstEncryptedOffset(0);
  for (size_t i = 0; i < decrypt_config->subsamples().size(); i++) {
    clearHeaderLens.push_back(decrypt_config->subsamples()[i].clear_bytes);
    payLoadLens.push_back(decrypt_config->subsamples()[i].cypher_bytes);
  }

  cenc_info->SetClearHeaderLens(clearHeaderLens);
  cenc_info->SetPayLoadLens(payLoadLens);
  // The web kernel sets keyid and iv by default, so DRM_CENC_INFO_KEY_IV_SUBSAMPLES_SET is selected by default here
  cenc_info->SetMode(uint32_t(DrmCencInfoModeAdapter::DRM_CENC_INFO_KEY_IV_SUBSAMPLES_SET));
  DecoderAdapterCode ret = videoDecoder_->SetAVCencInfo(index, cenc_info);
  if (ArkWebGetErrno() != ArkWebInterfaceResult::RESULT_OK) {
    LOG(ERROR) << "SetAVCencInfo api version not support";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  return ret;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::QueueInputBuffer(
    const uint8_t* data,
    size_t data_size,
    int64_t presentation_time,
    const DecryptConfig* decrypt_config) {
  LOG(DEBUG) << "MediaCodecDecoderBridgeImpl::QueueInputBuffer";
  std::lock_guard<std::recursive_mutex> lock(decoderMutex_);
  if (!isRunning_.load()) {
    LOG(ERROR)
        << "MediaCodecDecoderBridgeImpl::QueueInputBuffer decoder is stopped.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  if (data == nullptr || data_size == 0) {
    return DecoderAdapterCode::DECODER_ERROR;
  }
  if (signal_ == nullptr || signal_->isOnError_) {
    return DecoderAdapterCode::DECODER_ERROR;
  }
  if (signal_->isDecoderFlushing_.load() || signal_->inputQueue_.empty()) {
    return DecoderAdapterCode::DECODER_RETRY;
  }
  if (videoDecoder_ == nullptr) {
    LOG(ERROR)
        << "MediaCodecDecoderBridgeImpl::QueueInputBuffer decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  uint32_t index = signal_->inputQueue_.front().inputBufferIndex;
  OhosBuffer buffer = signal_->inputQueue_.front().inputBuffer;
  uint32_t bufferSize = buffer.bufferSize;

  size_t inputSize = bufferSize >= data_size ? data_size : bufferSize;
  LOG(DEBUG) << "MediaCodecDecoderBridgeImpl::QueueInputBuffer bufferSize: "
             << bufferSize << " " << data_size;
  if (buffer.addr == nullptr) {
    LOG(ERROR) << "MediaCodecDecoderBridgeImpl::QueueInputBuffer buffer.addr"
               << "is nullptr."
    return DecoderAdapterCode::DECODER_ERROR;
  }
  if (memcpy_s(buffer.addr, bufferSize, data, inputSize) != EOK) {
    LOG(ERROR)
      << "MediaCodecDecoderBridgeImpl::QueueInputBuffer memcpy failed.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  if (decrypt_config && SetAVCencInfo(index, decrypt_config) ==
    DecoderAdapterCode::DECODER_ERROR) {
    return DecoderAdapterCode::DECODER_ERROR;
  }
  DecoderAdapterCode ret = PushInbufferDec(index, inputSize, presentation_time);

  TRACE_EVENT0("media", "PushInbufferDec End");
  PopInqueueDec();
  return ret;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::QueueInputBufferEOS() {
  LOG(INFO) << "MediaCodecDecoderBridgeImpl::QueueInputBufferEOS";
  std::lock_guard<std::recursive_mutex> lock(decoderMutex_);
  if (!isRunning_.load()) {
    LOG(ERROR)
        << "MediaCodecDecoderBridgeImpl::QueueInputBufferEOS decoder is stopped.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  if (signal_ == nullptr || signal_->isOnError_) {
    return DecoderAdapterCode::DECODER_ERROR;
  }
  if (signal_->isDecoderFlushing_.load() || signal_->inputQueue_.empty() ||
      !isRunning_.load()) {
    return DecoderAdapterCode::DECODER_RETRY;
  }
  if (videoDecoder_ == nullptr) {
    LOG(ERROR)
        << "MediaCodecDecoderBridgeImpl::QueueInputBufferEOS decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  uint32_t index = signal_->inputQueue_.front().inputBufferIndex;
  DecoderAdapterCode ret = PushInbufferDecEos(index);

  TRACE_EVENT0("media", "PushInbufferDecEos End");
  PopInqueueDec();
  isRunning_.store(false);
  return ret;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::ReleaseOutputBuffer(
    uint32_t index,
    bool render) {
  LOG(DEBUG) << "MediaCodecDecoderBridgeImpl::ReleaseOutputBuffer.";
  if (videoDecoder_ == nullptr) {
    LOG(ERROR)
        << "MediaCodecDecoderBridgeImpl::ReleaseOutputBuffer decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  return videoDecoder_->ReleaseOutputBufferDec(index, render);
}
void MediaCodecDecoderBridgeImpl::PopOutqueueDec() {
  LOG(DEBUG) << "MediaCodecDecoderBridgeImpl::PopOutqueueDec.";
  if (signal_ == nullptr) {
    return;
  }

  signal_->outputQueue_.pop();
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::DequeueOutputBuffer(
    base::TimeDelta* presentation_time,
    uint32_t& index,
    bool& eos) {
  LOG(DEBUG) << "MediaCodecDecoderBridgeImpl::DequeueOutputBuffer.";
  if (signal_ == nullptr || signal_->isOnError_) {
    return DecoderAdapterCode::DECODER_ERROR;
  }
  if (signal_->isDecoderFlushing_.load() || signal_->outputQueue_.empty()) {
    LOG(DEBUG) << "CodecBridgeCallback::OnNeedOutputData Decoder is flushing.";
    return DecoderAdapterCode::DECODER_RETRY;
  }
  if (videoDecoder_ == nullptr) {
    LOG(ERROR)
        << "MediaCodecDecoderBridgeImpl::DequeueOutputBuffer decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  index = signal_->outputQueue_.front().outputBufferIndex;
  eos = signal_->outputQueue_.front().outputBufferFlag ==
        BufferFlag::CODEC_BUFFER_FLAG_EOS;
  *presentation_time = base::Microseconds(
      signal_->outputQueue_.front().outputBufferInfo.presentationTimeUs);
  PopOutqueueDec();
  return DecoderAdapterCode::DECODER_OK;
}

void MediaCodecDecoderBridgeImpl::DestoryNativeWindow(void* window) {
  if (window) {
    OHOS::NWeb::OhosAdapterHelperExt::GetWindowAdapterNdkInstance()
        .DestroyNativeWindow(window);
  }
}

void CodecBridgeCallback::OnError(ErrorType errorType, int32_t errorCode) {
  LOG(ERROR) << "CodecBridgeCallback::OnError Error errorCode=" << errorCode;

  if (signal_ == nullptr) {
    return;
  }

  signal_->isOnError_ = true;
  clearInputQueue(signal_->inputQueue_);
  clearOutputQueue(signal_->outputQueue_);
}

void CodecBridgeCallback::OnStreamChanged(int32_t width,
                                          int32_t height,
                                          double frameRate) {
  LOG(INFO) << "CodecBridgeCallback::OnStreamChanged Output Format Changed.";
}

void CodecBridgeCallback::OnNeedInputData(
    uint32_t index,
    std::shared_ptr<OhosBufferAdapter> buffer) {
  LOG(DEBUG) << "CodecBridgeCallback::OnNeedInputData";

  if (!decoder_callback_task_runner_->RunsTasksInCurrentSequence()) {
    decoder_callback_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&CodecBridgeCallback::OnNeedInputData,
                                  shared_from_this(), std::move(index),
                                  std::move(buffer)));
    return;
  }

  if (signal_ == nullptr) {
    return;
  }

  TRACE_EVENT0("media", "CodecBridgeCallback::OnNeedInputData");
  LOG(DEBUG)
      << "CodecBridgeCallback::OnNeedInputData Input Buffer Available, index = "
      << index;
  if (signal_->isDecoderFlushing_.load()) {
    LOG(DEBUG) << "CodecBridgeCallback::OnNeedInputData Decoder is flushing.";
    return;
  }

  if (!buffer) {
    LOG(ERROR) << "CodecBridgeCallback::OnNeedInputData buffer is NULL";
    return;
  }

  VideoBridgeDecoderInputBuffer inputBuffer;
  inputBuffer.inputBufferIndex = index;
  inputBuffer.inputBuffer.addr = buffer->GetAddr();
  inputBuffer.inputBuffer.bufferSize = buffer->GetBufferSize();
  signal_->inputQueue_.push(inputBuffer);
  on_buffers_available_cb_.Run();
}

void CodecBridgeCallback::OnNeedOutputData(
    uint32_t index,
    std::shared_ptr<BufferInfoAdapter> info,
    BufferFlag flag) {
  LOG(DEBUG) << "CodecBridgeCallback::OnNeedOutputData";

  if (!decoder_callback_task_runner_->RunsTasksInCurrentSequence()) {
    decoder_callback_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&CodecBridgeCallback::OnNeedOutputData,
                                  shared_from_this(), std::move(index),
                                  std::move(info), std::move(flag)));
    return;
  }

  if (!info) {
    LOG(ERROR) << "CodecBridgeCallback::OnNeedOutputData info is NULLL";
    return;
  }

  if (signal_ == nullptr) {
    return;
  }

  TRACE_EVENT0("media", "CodecBridgeCallback::OnNeedOutputData");
  LOG(DEBUG) << "CodecBridgeCallback::OnNeedOutputData Output Buffer "
                "Available, index ="
             << index << ", timestamp = " << info->GetPresentationTimeUs();
  if (signal_->isDecoderFlushing_.load()) {
    LOG(DEBUG) << "CodecBridgeCallback::OnNeedOutputData Decoder is flushing.";
    return;
  }

  if (!info) {
    LOG(ERROR) << "CodecBridgeCallback::OnNeedOutputData info is NULL";
    return;
  }

  VideoBridgeDecoderOutputBuffer outputBuffer;
  outputBuffer.outputBufferIndex = index;
  outputBuffer.outputBufferFlag = flag;
  outputBuffer.outputBufferInfo.presentationTimeUs =
      info->GetPresentationTimeUs();
  outputBuffer.outputBufferInfo.size = info->GetSize();
  outputBuffer.outputBufferInfo.offset = info->GetOffset();
  signal_->outputQueue_.push(outputBuffer);
  on_buffers_available_cb_.Run();
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
DecoderAdapterCode MediaCodecDecoderBridgeImpl::SetVideoSurface(
        int32_t widget_id) {
    LOG(INFO) << "MediaCodecDecoderBridgeImpl::SetVideoSurface(" << widget_id << ")";
    
    if (video_surface_id_ == widget_id) {
        return DecoderAdapterCode::DECODER_OK;
    }
    if (videoDecoder_ == nullptr) {
        LOG(ERROR) << "MediaCodecDecoderBridgeImpl::SetVideoSurface decoder is NULL";
        return DecoderAdapterCode::DECODER_ERROR;
    }
    if (widget_id < 0) {
        if (window_from_surface_) {
            return videoDecoder_->SetOutputSurface(window_from_surface_);
        }
        return DecoderAdapterCode::DECODER_ERROR;
    }
    video_surface_id_ = widget_id;
    return videoDecoder_->SetOutputSurface(
        NWebNativeWindowTracker::Get()->GetNativeWindow(video_surface_id_));
}
#endif // ARKWEB_VIDEO_ASSISTANT

DecoderAdapterCode MediaCodecDecoderBridgeImpl::SetDecryptionConfig(void *session, bool isSecure)
{
  if (videoDecoder_ == nullptr) {
    LOG(ERROR) << "MediaCodecDecoderBridgeImpl::SetDecryptionConfig decoder is NULL";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  DecoderAdapterCode ret = videoDecoder_->SetDecryptionConfig(session, isSecure);
  if (ArkWebGetErrno() != ArkWebInterfaceResult::RESULT_OK) {
    LOG(ERROR) << "SetDecryptionConfig api version not support";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  return ret;
}
