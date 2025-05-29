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
#include "third_party/bounds_checking_function/include/securec.h"

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
  if (videoDecoder_ == nullptr) {
    LOG(ERROR)
        << "MediaCodecDecoderBridgeImpl::StopBridgeDecoder decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  return videoDecoder_->StopDecoder();
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::FlushBridgeDecoder() {
  LOG(INFO)
      << " MediaCodecDecoderBridgeImpl::FlushBridgeDecoder flush decoder.";
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
  if (videoDecoder_ == nullptr) {
    LOG(ERROR)
        << "MediaCodecDecoderBridgeImpl::ResetBridgeDecoder decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  if (signal_ == nullptr) {
    return DecoderAdapterCode::DECODER_ERROR;
  }

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

DecoderAdapterCode MediaCodecDecoderBridgeImpl::QueueInputBuffer(
    const uint8_t* data,
    size_t data_size,
    int64_t presentation_time) {
  LOG(DEBUG) << "MediaCodecDecoderBridgeImpl::QueueInputBuffer";
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
  if (memcpy_s(buffer.addr, bufferSize, data, inputSize) != EOK) {
    LOG(ERROR) << "MediaCodecDecoderBridgeImpl::QueueInputBuffer memcpy failed.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  DecoderAdapterCode ret = PushInbufferDec(index, inputSize, presentation_time);

  TRACE_EVENT0("media", "PushInbufferDec End");
  PopInqueueDec();
  return ret;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::QueueInputBufferEOS() {
  LOG(INFO) << "MediaCodecDecoderBridgeImpl::QueueInputBufferEOS";
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
    OhosAdapterHelper::GetInstance()
        .GetWindowAdapterInstance()
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
 