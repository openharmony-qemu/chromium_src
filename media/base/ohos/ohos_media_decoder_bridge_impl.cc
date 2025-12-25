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

#include "ohos_media_decoder_bridge_impl.h"

#include <multimedia/player_framework/native_avbuffer.h>
#include <multimedia/player_framework/native_avcapability.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_avcodec_videodecoder.h>
#include <multimedia/player_framework/native_avformat.h>
#include <multimedia/player_framework/native_cencinfo.h>
#include <native_buffer/native_buffer.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <thread>

#include "base/debug/debugger.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/task/task_runner.h"
#include "base/trace_event/trace_event.h"

using namespace media;
using namespace std;

void OnError(OH_AVCodec* codec, int32_t error_code, void* user_data) {
  if (user_data) {
    ((CodecBridgeCallback*)(user_data))->OnError(error_code);
  }
}
void OnStreamChanged(OH_AVCodec* codec, OH_AVFormat* format, void* user_data) {
  if (user_data) {
    ((CodecBridgeCallback*)(user_data))->OnStreamChanged(format);
  }
}

void OnNeedInputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *user_data) {
  if (user_data) {
    ((CodecBridgeCallback*)(user_data))->OnNeedInputBuffer(codec, index, buffer);
  }
}

void OnNewOutputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *user_data) {
  if (user_data) {
    ((CodecBridgeCallback*)(user_data))->OnNewOutputBuffer(codec, index, buffer);
  }
}

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
  std::string codec_type;
  if (config.codec == media::VideoCodec::kH264) {
    LOG(INFO) << "MediaCodecDecoderBridgeImpl::CreateVideoDecoder video/avc";
    codec_type = "video/avc";
  } else if (config.codec == media::VideoCodec::kHEVC) {
    LOG(INFO) << "MediaCodecDecoderBridgeImpl::CreateVideoDecoder video/hevc";
    codec_type = "video/hevc";
  } else {
    LOG(ERROR) << "MediaCodecDecoderBridgeImpl::CreateVideoDecoder not supported type.";
    return nullptr;
  }
  return absl::WrapUnique(new MediaCodecDecoderBridgeImpl(
      codec_type, config.on_buffers_available_cb, config.media_key_session));
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::CreateVideoBridgeDecoderByMime(
    std::string mime_type) {
  LOG(INFO) << __FUNCTION__ << " [VideoDecoder] mime_type: " << mime_type;

  if (video_decoder_ != nullptr) {
    LOG(ERROR) << __FUNCTION__  << " [VideoDecoder] decoder is not NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  video_decoder_ = OH_VideoDecoder_CreateByMime(mime_type.c_str());
  if (video_decoder_ == nullptr) {
    LOG(ERROR) << __FUNCTION__ << " [VideoDecoder] create decoder failed.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  has_created_ = true;

  if (signal_ == nullptr) {
    signal_ = make_shared<DecoderBridgeSignal>();
  }

  if (cb_ == nullptr) {
    cb_ = make_shared<CodecBridgeCallback>(signal_);
  }

  OH_AVCodecCallback codec_callback = {&OnError, &OnStreamChanged, &OnNeedInputBuffer, &OnNewOutputBuffer};
  int32_t ret = OH_VideoDecoder_RegisterCallback(video_decoder_, codec_callback, cb_.get());
  LOG(INFO) << __FUNCTION__ << " [VideoDecoder] OH_VideoDecoder_RegisterCallback: " << ret;
  if (media_key_session_) {
    OH_VideoDecoder_SetDecryptionConfig(video_decoder_, media_key_session_, false);
  }
  return ret == AV_ERR_OK ? DecoderAdapterCode::DECODER_OK
                          : DecoderAdapterCode::DECODER_ERROR;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::CreateVideoBridgeDecoderByName(
    std::string name) {
  LOG(INFO) << __FUNCTION__ << " [VideoDecoder] create video decoder by name, type : " << name.c_str();

  if (video_decoder_ != nullptr) {
    LOG(ERROR) << __FUNCTION__ << "[VideoDecoder] decoder is not NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  video_decoder_ = OH_VideoDecoder_CreateByName(name.c_str());
  if (video_decoder_ == nullptr) {
    LOG(ERROR) << __FUNCTION__ << " [VideoDecoder] create decoder failed.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  has_created_ = true;

  if (signal_ == nullptr) {
    signal_ = make_shared<DecoderBridgeSignal>();
  }

  if (cb_ == nullptr) {
    cb_ = make_shared<CodecBridgeCallback>(signal_);
  }

  OH_AVCodecCallback codec_callback = {&OnError, &OnStreamChanged, &OnNeedInputBuffer, &OnNewOutputBuffer};
  int32_t ret = OH_VideoDecoder_RegisterCallback(video_decoder_, codec_callback, cb_.get());
  if (media_key_session_) {
    OH_VideoDecoder_SetDecryptionConfig(video_decoder_, media_key_session_,
                                        false);
  }

  return ret == AV_ERR_OK ? DecoderAdapterCode::DECODER_OK
                          : DecoderAdapterCode::DECODER_ERROR;
}

MediaCodecDecoderBridgeImpl::MediaCodecDecoderBridgeImpl(
    std::string codec_type,
    base::RepeatingClosure on_buffers_available_cb,
    void* media_key_session) {
  LOG(INFO) << __FUNCTION__ << " [VideoDecoder] codec_type: " << codec_type
            << ", media_key_session: " << media_key_session;
  if (!on_buffers_available_cb) {
    return;
  }
  if (media_key_session) {
    media_key_session_ = (MediaKeySession*)media_key_session;
  }
  DecoderAdapterCode ret = CreateVideoBridgeDecoderByMime(codec_type);
  if (ret == DecoderAdapterCode::DECODER_ERROR) {
    LOG(ERROR) << __FUNCTION__ << "[VideoDecoder] create decoder failed.";
    return;
  }
  cb_->on_buffers_available_cb_ = on_buffers_available_cb;
}

MediaCodecDecoderBridgeImpl::~MediaCodecDecoderBridgeImpl() {
  LOG(INFO) << __FUNCTION__ << "[VideoDecoder] ";
  ReleaseBridgeDecoder();
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::ConfigureBridgeDecoder(
    const DecoderFormat& format,
    scoped_refptr<base::SequencedTaskRunner> decoder_task_runner) {
  LOG(INFO) << __FUNCTION__ << " [VideoDecoder] ";
  width_ = format.width;
  height_ = format.height;
  decoder_task_runner_ = decoder_task_runner;
  cb_->decoder_callback_task_runner_ = decoder_task_runner;
  if (video_decoder_ == nullptr) {
    LOG(ERROR) << __FUNCTION__ << " [VideoDecoder] decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  OH_AVFormat* av_format = OH_AVFormat_Create();
  OH_AVFormat_SetIntValue(av_format, OH_MD_KEY_WIDTH, format.width);
  OH_AVFormat_SetIntValue(av_format, OH_MD_KEY_HEIGHT, format.height);

  OH_AVErrCode ret = OH_VideoDecoder_Configure(video_decoder_, av_format);

  OH_AVFormat_Destroy(av_format);
  return ret == AV_ERR_OK ? DecoderAdapterCode::DECODER_OK
                          : DecoderAdapterCode::DECODER_ERROR;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::SetBridgeParameterDecoder(
    const DecoderFormat& format) {
  LOG(INFO) << __FUNCTION__ << "[VideoDecoder] ";
  if (video_decoder_ == nullptr) {
    LOG(ERROR) << __FUNCTION__ << "[VideoDecoder] decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  OH_AVFormat* av_format = OH_AVFormat_Create();
  OH_AVFormat_SetIntValue(av_format, OH_MD_KEY_WIDTH, format.width);
  OH_AVFormat_SetIntValue(av_format, OH_MD_KEY_HEIGHT, format.height);

  OH_AVErrCode ret = OH_VideoDecoder_SetParameter(video_decoder_, av_format);
  OH_AVFormat_Destroy(av_format);
  return ret == AV_ERR_OK ? DecoderAdapterCode::DECODER_OK
                          : DecoderAdapterCode::DECODER_ERROR;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::SetBridgeOutputSurface(
    void* window) {
  LOG(INFO) << __FUNCTION__ << " [VideoDecoder]  set decoder outputsurface.";
  if (video_decoder_ == nullptr) {
    LOG(ERROR) << __FUNCTION__ << "[VideoDecoder] decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  if (window == nullptr) {
    LOG(ERROR) << __FUNCTION__ << "[VideoDecoder] window is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  OH_AVErrCode ret =
      OH_VideoDecoder_SetSurface(video_decoder_, (OHNativeWindow*)window);
  return ret == AV_ERR_OK ? DecoderAdapterCode::DECODER_OK
                          : DecoderAdapterCode::DECODER_ERROR;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::GetOutputFormatBridgeDecoder(
    DecoderFormat& format) {
  if (video_decoder_ == nullptr) {
    LOG(ERROR) << __FUNCTION__ << "[VideoDecoder] decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  OH_AVFormat* av_format = OH_VideoDecoder_GetOutputDescription(video_decoder_);
  if (av_format) {
    OH_AVFormat_GetIntValue(av_format, OH_MD_KEY_VIDEO_PIC_WIDTH, &format.width);
    OH_AVFormat_GetIntValue(av_format, OH_MD_KEY_VIDEO_PIC_HEIGHT, &format.height);
    OH_AVFormat_Destroy(av_format);
    return DecoderAdapterCode::DECODER_OK;
  }
  return DecoderAdapterCode::DECODER_ERROR;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::PrepareBridgeDecoder() {
  if (video_decoder_ == nullptr) {
    LOG(ERROR) << __FUNCTION__ << "[VideoDecoder] decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  OH_AVErrCode ret = OH_VideoDecoder_Prepare(video_decoder_);
  return ret == AV_ERR_OK ? DecoderAdapterCode::DECODER_OK
                          : DecoderAdapterCode::DECODER_ERROR;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::StartBridgeDecoder() {
  LOG(INFO) << __FUNCTION__ << "[VideoDecoder] start decoder.";
  is_running_.store(true);

  if (video_decoder_ == nullptr) {
    LOG(ERROR) << __FUNCTION__ << "[VideoDecoder] decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  OH_AVErrCode ret = OH_VideoDecoder_Start(video_decoder_);
  return ret == AV_ERR_OK ? DecoderAdapterCode::DECODER_OK
                          : DecoderAdapterCode::DECODER_ERROR;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::StopBridgeDecoder() {
  LOG(INFO) << __FUNCTION__ << "[VideoDecoder] stop decoder.";
  if (video_decoder_ == nullptr) {
    LOG(ERROR) << __FUNCTION__ << "[VideoDecoder] decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  OH_AVErrCode ret = OH_VideoDecoder_Stop(video_decoder_);
  return ret == AV_ERR_OK ? DecoderAdapterCode::DECODER_OK
                          : DecoderAdapterCode::DECODER_ERROR;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::FlushBridgeDecoder() {
  if (video_decoder_ == nullptr) {
    LOG(ERROR) << __FUNCTION__ << "[VideoDecoder] decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  if (signal_ == nullptr) {
    LOG(ERROR) << __func__ << "[VideoDecoder] signal_ is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  signal_->is_decoder_flushing_.store(true);

  OH_AVErrCode ret = OH_VideoDecoder_Flush(video_decoder_);
  if (ret != AV_ERR_OK) {
    LOG(ERROR) << __FUNCTION__
               << "[VideoDecoder] flush decoder failed. errcode:" << ret;
    return DecoderAdapterCode::DECODER_ERROR;
  }

  clearInputQueue(signal_->input_queue_);
  clearOutputQueue(signal_->output_queue_);

  decoder_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&MediaCodecDecoderBridgeImpl::UpdateFlushToFalse,
                     base::Unretained(this)));
  return StartBridgeDecoder();
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::ResetBridgeDecoder() {
  if (video_decoder_ == nullptr) {
    LOG(ERROR) << __FUNCTION__ << "[VideoDecoder] decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  if (signal_ == nullptr) {
    LOG(ERROR) << __func__ << "[VideoDecoder] signal_ is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  signal_->is_decoder_flushing_.store(true);
  OH_AVErrCode ret = OH_VideoDecoder_Reset(video_decoder_);
  if (ret != AV_ERR_OK) {
    LOG(ERROR) << __FUNCTION__
               << "[VideoDecoder] reset decoder failed. errcode:" << ret;
    return DecoderAdapterCode::DECODER_ERROR;
  }

  clearInputQueue(signal_->input_queue_);
  clearOutputQueue(signal_->output_queue_);

  decoder_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&MediaCodecDecoderBridgeImpl::UpdateFlushToFalse,
                     base::Unretained(this)));
  return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::ReleaseBridgeDecoder() {
  LOG(INFO) << __FUNCTION__ << "[VideoDecoder]  release decoder.";
  if (video_decoder_ == nullptr) {
    LOG(ERROR) << __FUNCTION__ << "[VideoDecoder]  decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  is_running_.store(false);

  OH_AVErrCode ret = OH_VideoDecoder_Destroy(video_decoder_);
  video_decoder_ = nullptr;
  return ret == AV_ERR_OK ? DecoderAdapterCode::DECODER_OK
                          : DecoderAdapterCode::DECODER_ERROR;
}

void MediaCodecDecoderBridgeImpl::PopInqueueDec() {
  if (signal_ == nullptr || signal_->is_on_error_ ||
      signal_->input_queue_.empty()) {
    LOG(ERROR) << __func__ << "[VideoDecoder] signal_ is not available.";
    return;
  }
  signal_->input_queue_.pop();
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::PushInbufferDec(
    OH_AVBuffer* av_buffer,
    const uint32_t index,
    const uint32_t buffer_size,
    const int64_t time) {
  OH_AVCodecBufferFlags buffer_flag;
  if (is_first_decFrame_) {
    buffer_flag = AVCODEC_BUFFER_FLAGS_CODEC_DATA;
    is_first_decFrame_ = false;
  } else {
    buffer_flag = AVCODEC_BUFFER_FLAGS_NONE;
  }
  DVLOG(3) << "PushInbufferDec index:" << index
           << ", buffer_size:" << buffer_size;

  OH_AVCodecBufferAttr attr;
  attr.size = buffer_size;
  attr.offset = 0;
  attr.pts = time;
  attr.flags = buffer_flag;
  OH_AVErrCode ret = OH_AVBuffer_SetBufferAttr(av_buffer, &attr);
  if (ret != AV_ERR_OK) {
    LOG(WARNING) << __FUNCTION__ << " [VideoDecoder] OH_AVBuffer_SetBufferAttr index: " << index << ", err: " << ret;
  }

  ret = OH_VideoDecoder_PushInputBuffer(video_decoder_, index);
  if (ret == AV_ERR_DRM_DECRYPT_FAILED) {
    // workaround until 6.1, treat DRM_DECRYPT_FAILED as NO_KEY
    LOG(WARNING) << __func__
                 << "[WiseplayDRM] OH_VideoDecoder_PushInputBuffer failed as "
                    "AV_ERR_DRM_DECRYPT_FAILED. and return as "
                    "DECODER_DECRYPT_FAILED_NO_KEY";
    return DecoderAdapterCode::DECODER_DECRYPT_FAILED_NO_KEY;
  }
  if (ret != AV_ERR_OK) {
    LOG(WARNING)
        << __FUNCTION__
        << " [OHOSVideoDecoder] OH_VideoDecoder_PushInputBuffer failed. error code: "
        << ret;
    return DecoderAdapterCode::DECODER_ERROR;
  }
  return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::PushInbufferDecEos(
    OH_AVBuffer* av_buffer,
    const uint32_t index) {
  OH_AVCodecBufferAttr attr;
  attr.size = 0;
  attr.offset = 0;
  attr.pts = 0;
  attr.flags = AVCODEC_BUFFER_FLAGS_EOS;

  OH_AVErrCode ret = OH_AVBuffer_SetBufferAttr(av_buffer, &attr);
  if (ret != AV_ERR_OK) {
    LOG(WARNING) << __FUNCTION__ << " [VideoDecoder] OH_AVBuffer_SetBufferAttr index: " << index << ", err: " << ret;
  }

  ret = OH_VideoDecoder_PushInputBuffer(video_decoder_, index);
  if (ret != AV_ERR_OK) {
    LOG(ERROR) << __FUNCTION__
               << " [VideoDecoder] OH_VideoDecoder_PushInputBuffer, ret: "
               << ret;
    return DecoderAdapterCode::DECODER_ERROR;
  }
  return DecoderAdapterCode::DECODER_OK;
}

#if BUILDFLAG(ENABLE_WISEPLAY)
void MediaCodecDecoderBridgeImpl::CopyBytes(const std::string& src,
                                            base::span<uint8_t> dest) {
  memcpy(dest.data(), src.data(), std::min(src.size(), dest.size_bytes()));
}

void MediaCodecDecoderBridgeImpl::SetAVCencInfo(
    OH_AVBuffer* av_buffer,
    const DecryptConfig* decrypt_config) {
  if (decrypt_config == nullptr) {
    LOG(WARNING) << __FUNCTION__ << "[WiseplayDRM] decrypt_config is null.";
    return;
  }
  OH_AVCencInfo *av_cenc_info = OH_AVCencInfo_Create();
  if (av_cenc_info == nullptr) {
      LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] OH_AVCencInfo_Create failed.";
      return;
  }
  OH_AVErrCode err = AV_ERR_OK;
    switch (decrypt_config->encryption_scheme()) {
        case EncryptionScheme::kUnencrypted:
            err = OH_AVCencInfo_SetAlgorithm(av_cenc_info, DRM_ALG_CENC_UNENCRYPTED);
            return;
        case EncryptionScheme::kCenc:
            err = OH_AVCencInfo_SetAlgorithm(av_cenc_info, DRM_ALG_CENC_AES_CTR);
            break;
        case EncryptionScheme::kCbcs:
            err = OH_AVCencInfo_SetAlgorithm(av_cenc_info, DRM_ALG_CENC_AES_CBC);
            break;
        default:
            // Currently the kernel only supports AES-CTR and AES-CBC encryption algorithm modes
            LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] Encrypt algorithm not supported: "
                         << static_cast<int>(decrypt_config->encryption_scheme());
            err = OH_AVCencInfo_SetAlgorithm(av_cenc_info, DRM_ALG_CENC_UNENCRYPTED);
    }

  std::string key_id_str = decrypt_config->key_id();
  std::vector<uint8_t> key_id(key_id_str.size(), 0);
  CopyBytes(key_id_str, key_id);

  std::string iv_str = decrypt_config->iv();
  std::vector<uint8_t> iv(iv_str.size(), 0);
  CopyBytes(iv_str, iv);

  err = OH_AVCencInfo_SetKeyIdAndIv(av_cenc_info, key_id.data(), key_id.size(), iv.data(), iv.size());
  if (err != AV_ERR_OK) {
      LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] OH_AVCencInfo_SetKeyIdAndIv error: " << err;
      return;
  }

  uint32_t encrypted_block_count = 0;
  uint32_t skipped_block_count = 0;
  if (decrypt_config->encryption_pattern()) {
      encrypted_block_count =
          decrypt_config->encryption_pattern()->crypt_byte_block();
      skipped_block_count =
          decrypt_config->encryption_pattern()->skip_byte_block();
      LOG(WARNING) << __FUNCTION__
                   << "[WiseplayDRM] Set encrypted_block_count = "
                   << encrypted_block_count
                   << ", skipped_block_count = " << skipped_block_count;
  }

  uint32_t first_encrypted_offset = 0;

  size_t samplesCnt = decrypt_config->subsamples().size();
  DrmSubsample sub_samples[samplesCnt];
  for (uint32_t i = 0; i < samplesCnt; i++) {
      sub_samples[i].clearHeaderLen =
          decrypt_config->subsamples()[i].clear_bytes;
      sub_samples[i].payLoadLen = decrypt_config->subsamples()[i].cypher_bytes;
  }
  err = OH_AVCencInfo_SetSubsampleInfo(
      av_cenc_info, encrypted_block_count, skipped_block_count,
      first_encrypted_offset, samplesCnt, sub_samples);
  if (err != AV_ERR_OK) {
      LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] OH_AVCencInfo_SetSubsampleInfo error: " << err;
      return;
  }
  
  err = OH_AVCencInfo_SetMode(av_cenc_info, DRM_CENC_INFO_KEY_IV_SUBSAMPLES_SET);
  if (err != AV_ERR_OK) {
      LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] OH_AVCencInfo_SetMode error: " << err;
      return;
  }

  err = OH_AVCencInfo_SetAVBuffer(av_cenc_info, av_buffer);
  if (err != AV_ERR_OK) {
      LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] OH_AVCencInfo_SetAVBuffer error: " << err;
      return;
  }

  err = OH_AVCencInfo_Destroy(av_cenc_info);
  if (err != AV_ERR_OK) {
      LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] OH_AVCencInfo_Destroy error: " << err;
      return;
  }
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::SetDecryptionConfig(
    void* session,
    bool is_secure) {
  if (video_decoder_ == nullptr) {
      LOG(ERROR) << __FUNCTION__ << " [WiseplayDRM] decoder is NULL";
      return DecoderAdapterCode::DECODER_ERROR;
  }

  if (session == nullptr) {
      return DecoderAdapterCode::DECODER_OK;
  }
  OH_AVErrCode errCode = OH_VideoDecoder_SetDecryptionConfig(
      video_decoder_, static_cast<MediaKeySession*>(session), is_secure);
  if (errCode != AV_ERR_OK) {
      LOG(ERROR) << __FUNCTION__
                 << " [WiseplayDRM]  OH_VideoDecoder_SetDecryptionConfig fail, "
                    "errCode = "
                 << static_cast<int>(errCode);
      return DecoderAdapterCode::DECODER_ERROR;
  }
  return DecoderAdapterCode::DECODER_OK;
}
#endif  // BUILDFLAG(ENABLE_WISEPLAY)

DecoderAdapterCode MediaCodecDecoderBridgeImpl::QueueInputBuffer(
    const uint8_t* data,
    size_t data_size,
    int64_t presentation_time,
    const DecryptConfig* decrypt_config) {
  if (signal_ == nullptr || signal_->is_on_error_) {
    LOG(ERROR) << __func__ << "[VideoDecoder] signal_ is not available.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  if (signal_->is_decoder_flushing_.load() || signal_->input_queue_.empty()) {
    LOG(WARNING) << __FUNCTION__ << "[VideoDecoder] is_decoder_flushing_: "
                 << signal_->is_decoder_flushing_.load()
                 << ", output_queue_.empty(): "
                 << signal_->output_queue_.empty()
                 << " return DecoderAdapterCode::DECODER_RETRY";
    return DecoderAdapterCode::DECODER_RETRY;
  }
  if (video_decoder_ == nullptr) {
    LOG(ERROR) << __FUNCTION__ << "[VideoDecoder] decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  VideoBridgeDecoderInputBuffer& input_buffer = signal_->input_queue_.front();
  uint32_t index = input_buffer.input_buffer_index;
  OH_AVBuffer* av_buffer = input_buffer.av_buffer;
  uint8_t* buffer_addr = input_buffer.input_buffer.addr;
  uint32_t buffer_size = input_buffer.input_buffer.buffer_size;
  DVLOG(3) << "MediaCodecDecoderBridgeImpl::QueueInputBuffer buffer_size: "
           << buffer_size << " " << data_size;
  size_t input_size = buffer_size >= data_size ? data_size : buffer_size;
  // buffer_addr.size is ensured by the min value of buffer_size and data_size,
  // so memcpy should not have problem
  memcpy(buffer_addr, data, input_size);
#if BUILDFLAG(ENABLE_WISEPLAY)
  if (decrypt_config) {
    SetAVCencInfo(av_buffer, decrypt_config);
  }
#endif  // BUILDFLAG(ENABLE_WISEPLAY)
  DecoderAdapterCode ret =
      PushInbufferDec(av_buffer, index, input_size, presentation_time);
  if (ret == DecoderAdapterCode::DECODER_OK) {
    PopInqueueDec();
  }
  return ret;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::QueueInputBufferEOS() {
  if (signal_ == nullptr || signal_->is_on_error_) {
    LOG(ERROR) << __func__ << "[VideoDecoder] signal_ is not available.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  if (signal_->is_decoder_flushing_.load() || signal_->input_queue_.empty() ||
      !is_running_.load()) {
    return DecoderAdapterCode::DECODER_RETRY;
  }
  if (video_decoder_ == nullptr) {
    LOG(ERROR) << __func__ << "[VideoDecoder] video_decoder_ is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  VideoBridgeDecoderInputBuffer& input_buffer = signal_->input_queue_.front();
  uint32_t index = input_buffer.input_buffer_index;
  OH_AVBuffer* av_buffer = input_buffer.av_buffer;
  DecoderAdapterCode ret = PushInbufferDecEos(av_buffer, index);
  if (ret == DecoderAdapterCode::DECODER_OK) {
    PopInqueueDec();
  }
  is_running_.store(false);
  return ret;
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::ReleaseOutputBuffer(
    uint32_t index,
    bool render) {
  if (video_decoder_ == nullptr) {
    LOG(ERROR)
        << " [VideoDecoder] MediaCodecDecoderBridgeImpl::ReleaseOutputBuffer decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }
  OH_AVErrCode ret = AV_ERR_OK;
  if (render) {
    // Use RenderOutputData in surface mode and FreeOutputData in buffer mode.
    ret = OH_VideoDecoder_RenderOutputBuffer(video_decoder_, index);
  } else {
    ret = OH_VideoDecoder_FreeOutputBuffer(video_decoder_, index);
  }
  return ret == AV_ERR_OK ? DecoderAdapterCode::DECODER_OK
                          : DecoderAdapterCode::DECODER_ERROR;
}

void MediaCodecDecoderBridgeImpl::PopOutqueueDec() {
  if (signal_ == nullptr || signal_->is_on_error_ ||
      signal_->output_queue_.empty()) {
    LOG(ERROR) << __func__ << "[VideoDecoder] signal_ is not available.";
    return;
  }
  signal_->output_queue_.pop();
}

DecoderAdapterCode MediaCodecDecoderBridgeImpl::DequeueOutputBuffer(
    base::TimeDelta* presentation_time,
    uint32_t& index,
    bool& eos) {
  if (signal_ == nullptr || signal_->is_on_error_) {
    LOG(ERROR) << __FUNCTION__ << "[VideoDecoder] signal_ is nullptr or is on error: " << signal_;
    return DecoderAdapterCode::DECODER_ERROR;
  }
  if (signal_->is_decoder_flushing_.load() || signal_->output_queue_.empty()) {
    return DecoderAdapterCode::DECODER_RETRY;
  }
  if (video_decoder_ == nullptr) {
    LOG(ERROR) << __FUNCTION__ << "[VideoDecoder] decoder is NULL.";
    return DecoderAdapterCode::DECODER_ERROR;
  }

  VideoBridgeDecoderOutputBuffer& output_buffer = signal_->output_queue_.front();

  eos = (output_buffer.output_buffer_flags & AVCODEC_BUFFER_FLAGS_EOS) == AVCODEC_BUFFER_FLAGS_EOS;
  *presentation_time = base::Microseconds(output_buffer.output_buffer_info.presentation_time_us);
  index = output_buffer.output_buffer_index;

  PopOutqueueDec();
  return DecoderAdapterCode::DECODER_OK;
}

void MediaCodecDecoderBridgeImpl::DestroyNativeWindow(void* window) {
  LOG(INFO) << "[VideoDecoder] MediaCodecDecoderBridgeImpl::DestroyNativeWindow";
  if (window) {
    OH_NativeWindow_DestroyNativeWindow((OHNativeWindow*)window);
  }
}

void CodecBridgeCallback::OnError(int32_t error_code) {
  if (!decoder_callback_task_runner_->RunsTasksInCurrentSequence()) {
    decoder_callback_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&CodecBridgeCallback::OnError,
                                  shared_from_this(), error_code));
    return;
  }
  LOG(ERROR) << __func__ << "[VideoDecoder] error_code: " << error_code;
  if (signal_ == nullptr) {
    LOG(ERROR) << __func__ << "[VideoDecoder] signal_ is NULL.";
    return ;
  }
  signal_->is_on_error_ = true;
  clearInputQueue(signal_->input_queue_);
  clearOutputQueue(signal_->output_queue_);
}

void CodecBridgeCallback::OnStreamChanged(OH_AVFormat* format) {
  LOG(INFO) << "CodecBridgeCallback::OnStreamChanged Output Format Changed.";
}

void CodecBridgeCallback::OnNeedInputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *av_buffer) {
  if (!decoder_callback_task_runner_->RunsTasksInCurrentSequence()) {
    decoder_callback_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&CodecBridgeCallback::OnNeedInputBuffer,
                                  shared_from_this(), std::move(codec), std::move(index),
                                  std::move(av_buffer)));
    return;
  }
  if (signal_ == nullptr) {
    LOG(ERROR) << __func__ << "[VideoDecoder] signal_ is NULL.";
    return;
  }
  if (signal_->is_decoder_flushing_.load()) {
    LOG(WARNING) << "[VideoDecoder] | CodecBridgeCallback::" << __FUNCTION__ << " | Decoder is flushing.";
    return;
  }

  if (!av_buffer) {
    return;
  }

  uint8_t* av_buffer_size = OH_AVBuffer_GetAddr(av_buffer);
  if (!av_buffer_size) {
    return;
  }
  uint32_t av_buffer_capacity = OH_AVBuffer_GetCapacity(av_buffer);

  OhosBuffer ohos_buffer = {av_buffer_size, av_buffer_capacity};
  VideoBridgeDecoderInputBuffer input_buffer;
  input_buffer.input_buffer_index = index;
  input_buffer.av_buffer = av_buffer;
  input_buffer.input_buffer = ohos_buffer;
  signal_->input_queue_.push(input_buffer);
  on_buffers_available_cb_.Run();
}

void CodecBridgeCallback::OnNewOutputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *av_buffer) {
  if (!decoder_callback_task_runner_->RunsTasksInCurrentSequence()) {
    decoder_callback_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&CodecBridgeCallback::OnNewOutputBuffer,
                                  shared_from_this(), std::move(codec), std::move(index),
                                  std::move(av_buffer)));
    return;
  }

  if (signal_ == nullptr) {
    LOG(ERROR) << __func__ << "[VideoDecoder] signal_ is NULL.";
    return;
  }
  if (signal_->is_decoder_flushing_.load()) {
    LOG(WARNING) << "[VideoDecoder] | CodecBridgeCallback::" << __FUNCTION__ << " | Decoder is flushing.";
    return;
  }

  if (!av_buffer) {
    return;
  }

  OH_AVCodecBufferAttr attr;
  OH_AVErrCode ret = OH_AVBuffer_GetBufferAttr(av_buffer, &attr);
  if (ret != AV_ERR_OK) {
    LOG(WARNING) << __FUNCTION__ << " [VideoDecoder] OH_AVBuffer_GetBufferAttr: " << ret;
  }
  BufferInfo info = {attr.pts, attr.size, attr.offset};
  
  VideoBridgeDecoderOutputBuffer output_buffer;
  output_buffer.output_buffer_index = index;
  output_buffer.output_buffer_flags = attr.flags;
  output_buffer.output_buffer_info = info;
  output_buffer.av_buffer = av_buffer;
  signal_->output_queue_.push(output_buffer);
  on_buffers_available_cb_.Run();
}
