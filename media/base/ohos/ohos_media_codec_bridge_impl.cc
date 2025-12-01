// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_media_codec_bridge_impl.h"

#include <multimedia/player_framework/native_avcodec_videoencoder.h>
#include <multimedia/player_framework/native_avformat.h>
#include <native_buffer/native_buffer.h>
#include <sys/mman.h>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "base/logging.h"

namespace media {

void OnError(OH_AVCodec* codec, int32_t error_code, void* user_data) {
  if (user_data) {
    ((CodecEncodeBridgeCallback*)(user_data))->OnError(error_code);
  }
}
void OnStreamChanged(OH_AVCodec* codec, OH_AVFormat* format, void* user_data) {
  if (user_data) {
    ((CodecEncodeBridgeCallback*)(user_data))->OnStreamChanged(format);
  }
}
void OnNeedInputBuffer(OH_AVCodec* codec,
                       uint32_t index,
                       OH_AVBuffer* buffer,
                       void* user_data) {
  if (user_data == nullptr) {
    return;
  }
  if (buffer != nullptr && OH_AVBuffer_GetAddr(buffer) != nullptr) {
    OhosBuffer ohos_buffer;
    ohos_buffer.addr = OH_AVBuffer_GetAddr(buffer);
    ohos_buffer.buffer_size = OH_AVBuffer_GetCapacity(buffer);
    ohos_buffer.buffer = buffer;
    ((CodecEncodeBridgeCallback*)(user_data))
        ->OnNeedInputData(index, ohos_buffer);
  }
}
void OnNewOutputBuffer(OH_AVCodec* codec,
                       uint32_t index,
                       OH_AVBuffer* buffer,
                       void* user_data) {
  if (user_data == nullptr) {
    return;
  }
  if (buffer != nullptr && OH_AVBuffer_GetAddr(buffer) != nullptr) {
    OH_AVCodecBufferAttr attr;
    OH_AVErrCode ret = OH_AVBuffer_GetBufferAttr(buffer, &attr);
    if (ret != AV_ERR_OK) {
      LOG(ERROR) << "OnNewOutputBuffer:OH_AVBuffer_GetBufferAttr ERROR.";
      return;
    }
    BufferInfo info;
    info.presentation_time_us = attr.pts;
    info.size = attr.size;
    info.offset = attr.offset;
    OhosBuffer ohos_buffer;
    ohos_buffer.addr = OH_AVBuffer_GetAddr(buffer);
    ohos_buffer.buffer_size = attr.size;
    ohos_buffer.buffer = buffer;
    ((CodecEncodeBridgeCallback*)(user_data))
        ->OnNeedOutputData(index, info, attr.flags, ohos_buffer);
  }
}

void ClearOutputCache(std::queue<EncodeOutputBuffer>& q) {
  std::queue<EncodeOutputBuffer> empty;
  std::swap(empty, q);
}
// static
std::unique_ptr<OHOSMediaCodecBridge> OHOSMediaCodecBridgeImpl::CreateVideoEncoder(std::string& mime) {
  LOG(INFO) << "OHOSMediaCodecBridgeImpl::CreateVideoEncoder.";
  return absl::WrapUnique(new OHOSMediaCodecBridgeImpl(mime));
}

OHOSMediaCodecBridgeImpl::OHOSMediaCodecBridgeImpl(std::string& mime) {
  CodecCodeAdapter ret = CreateVideoCodecByMime(mime.c_str());
  if (ret == CodecCodeAdapter::ENCODER_ERROR) {
    LOG(ERROR) << "create VideoCodec failed.";
    return;
  }
}

OHOSMediaCodecBridgeImpl::~OHOSMediaCodecBridgeImpl() {
  LOG(INFO) << "~OHOSMediaCodecBridgeImpl.";
  is_running_.store(false);
  OH_VideoEncoder_Destroy(video_encoder_);
  video_encoder_ = nullptr;
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::CreateVideoCodecByMime(
    const std::string mime_type) {
  LOG(INFO) << "OHOSMediaCodecBridgeImpl::CreateVideoCodecByMime.";
  if (mime_type == "video/avc") {
    LOG(INFO) << "create video codec by mime, type : " << mime_type;
  }
  video_encoder_ = OH_VideoEncoder_CreateByMime(mime_type.c_str());
  if (video_encoder_ == nullptr) {
    LOG(ERROR) << "create codec failed.";
    return CodecCodeAdapter::ENCODER_ERROR;
  }

  if (signal_ == nullptr) {
    signal_ = std::make_shared<CodecBridgeSignal>();
  }

  if (cb_ == nullptr) {
    cb_ = std::make_shared<CodecEncodeBridgeCallback>(signal_);
  }

  OH_AVCodecCallback oh_cb = {&OnError, &OnStreamChanged, &OnNeedInputBuffer,
                              &OnNewOutputBuffer};
  OH_AVErrCode ret =
      OH_VideoEncoder_RegisterCallback(video_encoder_, oh_cb, cb_.get());
  return ret == AV_ERR_OK ? CodecCodeAdapter::ENCODER_OK
                          : CodecCodeAdapter::ENCODER_ERROR;
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::CreateVideoCodecByName(
    const std::string name) {
  LOG(INFO) << "create video codec by name, type : " << name.c_str();
  video_encoder_ = OH_VideoEncoder_CreateByName(name.c_str());
  if (video_encoder_ == nullptr) {
    LOG(ERROR) << "create codec failed.";
    return CodecCodeAdapter::ENCODER_ERROR;
  }

  if (signal_ == nullptr) {
    signal_ = std::make_shared<CodecBridgeSignal>();
  }

  if (cb_ == nullptr) {
    cb_ = std::make_shared<CodecEncodeBridgeCallback>(signal_);
  }

  OH_AVCodecCallback oh_cb = {&OnError, &OnStreamChanged, &OnNeedInputBuffer,
                              &OnNewOutputBuffer};
  OH_AVErrCode ret =
      OH_VideoEncoder_RegisterCallback(video_encoder_, oh_cb, cb_.get());
  return ret == AV_ERR_OK ? CodecCodeAdapter::ENCODER_OK
                          : CodecCodeAdapter::ENCODER_ERROR;
}

void OHOSMediaCodecBridgeImpl::UpdateStatusAndClearCache(bool is_running) {
  LOG(INFO) << "current encode status is " << is_running;
  if (is_running_.load() == is_running) {
    LOG(INFO) << "status not change, do nothing.";
    return;
  }
  is_running_.store(is_running);
  if (cb_) {
    cb_->is_running_.store(is_running);
  }

  // clear cache when encode is not running
  if (!is_running) {
    ClearOutputCache(signal_->out_buffer_queue_);
    if (cb_) {
      cb_->ClearCache();
    }
  }
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::Configure(
    OH_AVFormat& config_format,
    scoped_refptr<base::SequencedTaskRunner> codec_task_runner) {
  LOG(INFO) << "configure codec.";
  if ((video_encoder_ == nullptr) || (cb_ == nullptr)) {
    LOG(ERROR) << "codec or callback is NULL.";
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  codec_task_runner_ = codec_task_runner;
  cb_->codec_callback_task_runner_ = codec_task_runner;
  OH_AVErrCode ret = OH_VideoEncoder_Configure(video_encoder_, &config_format);
  return ret == AV_ERR_OK ? CodecCodeAdapter::ENCODER_OK
                          : CodecCodeAdapter::ENCODER_ERROR;
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::Prepare() {
  LOG(INFO) << "prepare codec.";
  if (video_encoder_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  OH_AVErrCode ret = OH_VideoEncoder_Prepare(video_encoder_);
  return ret == AV_ERR_OK ? CodecCodeAdapter::ENCODER_OK
                          : CodecCodeAdapter::ENCODER_ERROR;
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::Start() {
  LOG(INFO) << "start.";
  UpdateStatusAndClearCache(true);
  if (video_encoder_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ENCODER_ERROR;
  }

  OH_AVErrCode ret = OH_VideoEncoder_Start(video_encoder_);
  if (ret != AV_ERR_OK) {
    LOG(ERROR) << "OH_VideoEncoder_Start error.";
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  return CodecCodeAdapter::ENCODER_OK;
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::Stop() {
  UpdateStatusAndClearCache(false);
  LOG(INFO) << "stop codec.";
  if (video_encoder_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  OH_AVErrCode ret = OH_VideoEncoder_NotifyEndOfStream(video_encoder_);
  if (ret != AV_ERR_OK) {
    LOG(ERROR) << "OH_VideoEncoder_NotifyEndOfStream error.";
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  ret = OH_VideoEncoder_Stop(video_encoder_);
  return ret == AV_ERR_OK ? CodecCodeAdapter::ENCODER_OK
                          : CodecCodeAdapter::ENCODER_ERROR;
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::Reset() {
  LOG(INFO) << "Reset codec.";
  UpdateStatusAndClearCache(false);
  if (video_encoder_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  OH_AVErrCode ret = OH_VideoEncoder_Reset(video_encoder_);
  return ret == AV_ERR_OK ? CodecCodeAdapter::ENCODER_OK
                          : CodecCodeAdapter::ENCODER_ERROR;
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::Release() {
  LOG(INFO) << "Release codec.";
  UpdateStatusAndClearCache(false);
  if (video_encoder_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  OH_AVErrCode ret = OH_VideoEncoder_Destroy(video_encoder_);
  if (ret != AV_ERR_OK) {
    LOG(ERROR) << "OH_VideoEncoder_Destroy error.";
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  video_encoder_ = nullptr;
  return ret == AV_ERR_OK ? CodecCodeAdapter::ENCODER_OK
                          : CodecCodeAdapter::ENCODER_ERROR;
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::CreateInputSurface() {
  LOG(INFO) << "CreateInputSurface.";
  if (video_encoder_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ENCODER_ERROR;
  }

  OH_AVErrCode ret = AV_ERR_OK;
  if (native_window_ == nullptr) {
    ret = OH_VideoEncoder_GetSurface(video_encoder_, &native_window_);
  }
  if (ret != AV_ERR_OK) {
    LOG(ERROR) << "OH_VideoEncoder_GetSurface failed." << ret;
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  return CodecCodeAdapter::ENCODER_OK;
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::FillSurfaceBuffer(
    scoped_refptr<VideoFrame> frame,
    const int64_t timestamp_ms) {
  if (native_window_ == nullptr) {
    LOG(ERROR) << "native_window_ is null when QueueInput";
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  if (frame == nullptr) {
    LOG(ERROR) << "frame is null when QueueInput";
    return CodecCodeAdapter::ENCODER_ERROR;
  }

  int fenceFd;
  int32_t height = frame->coded_size().height();
  int32_t width = frame->coded_size().width();
  constexpr int32_t SAMPLE_RATIO = 2;
  constexpr int32_t DEFAULT_STRIDE = 16;
  OHNativeWindowBuffer* window_buffer = nullptr;
  OH_NativeWindow_NativeWindowHandleOpt(native_window_, SET_BUFFER_GEOMETRY,
                                        width, height);
  OH_NativeWindow_NativeWindowHandleOpt(native_window_, SET_STRIDE,
                                        DEFAULT_STRIDE);
  OH_NativeWindow_NativeWindowHandleOpt(native_window_, SET_UI_TIMESTAMP,
                                        timestamp_ms);
  OH_NativeWindow_NativeWindowHandleOpt(native_window_, SET_FORMAT,
                                        NATIVEBUFFER_PIXEL_FMT_YCBCR_420_P);
  if (OH_NativeWindow_NativeWindowRequestBuffer(native_window_, &window_buffer,
                                                &fenceFd) != AV_ERR_OK) {
    LOG(ERROR) << "OH_NativeWindow_NativeWindowRequestBuffer failed";
  }

  BufferHandle* bufferHandle =
      OH_NativeWindow_GetBufferHandleFromNative(window_buffer);
  if (bufferHandle == nullptr) {
    LOG(ERROR) << "fail to RequestBuffer";
    return CodecCodeAdapter::ENCODER_ERROR;
  }

  int32_t stride = bufferHandle->stride;
  void* mappedAddr =
      mmap(bufferHandle->virAddr, bufferHandle->size, PROT_READ | PROT_WRITE,
           MAP_SHARED, bufferHandle->fd, 0);
  if (mappedAddr == MAP_FAILED) {
    LOG(ERROR) << "mmap failed";
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  // mappedAddr is created refer to the size of original bufferHandle, so memcpy
  // should not have problem
  uint8_t* dst = reinterpret_cast<uint8_t*>(mappedAddr);
  const uint8_t* y_src = frame->data(VideoFrame::kYPlane);
  const uint8_t* u_src = frame->data(VideoFrame::kUPlane);
  const uint8_t* v_src = frame->data(VideoFrame::kVPlane);
  const int32_t src_y_stride = frame->stride(VideoFrame::kYPlane);
  const int32_t src_u_stride = frame->stride(VideoFrame::kUPlane);
  const int32_t src_v_stride = frame->stride(VideoFrame::kVPlane);

  if ((y_src == nullptr) || (u_src == nullptr) || (v_src == nullptr)) {
    LOG(ERROR) << "src is nullptr";
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  int32_t y_cnt = height;
  int32_t u_cnt = height / SAMPLE_RATIO;
  int32_t v_cnt = height / SAMPLE_RATIO;
  // copy Y
  for (int32_t i = 0; i < y_cnt; i++) {
    memcpy(dst, y_src, width);
    dst += stride;
    y_src += src_y_stride;
  }
  // copy U
  for (int32_t i = 0; i < u_cnt; i++) {
    memcpy(dst, u_src, width / SAMPLE_RATIO);
    dst += stride / SAMPLE_RATIO;
    u_src += src_u_stride;
  }
  // copy V
  for (int32_t i = 0; i < v_cnt; i++) {
    memcpy(dst, v_src, width / SAMPLE_RATIO);
    dst += stride / SAMPLE_RATIO;
    v_src += src_v_stride;
  }

  Region region{nullptr, 0};
  if (OH_NativeWindow_NativeWindowFlushBuffer(native_window_, window_buffer,
                                              fenceFd, region) != AV_ERR_OK) {
    LOG(ERROR) << "fail to FlushBuffer";
    return CodecCodeAdapter::ENCODER_ERROR;
  }

  if (munmap(mappedAddr, bufferHandle->size) < 0) {
    LOG(ERROR) << "munmap failed";
  }
  return CodecCodeAdapter::ENCODER_OK;
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::RequestKeyFrameSoon() {
  LOG(INFO) << "RequestKeyFrameSoon.";
  if (video_encoder_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  OH_AVFormat* format = OH_AVFormat_Create();
  OH_AVFormat_SetIntValue(format, OH_MD_KEY_REQUEST_I_FRAME, true);
  if (OH_VideoEncoder_SetParameter(video_encoder_, format) != 0) {
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  return CodecCodeAdapter::ENCODER_OK;
}

void OHOSMediaCodecBridgeImpl::PopOutQueue() {
  signal_->out_buffer_queue_.pop();
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::DequeueOutputBuffer(
    uint32_t& index,
    BufferInfo& info,
    OH_AVCodecBufferFlags& flag,
    OhosBuffer& buffer) {
  if (signal_->isOnError_) {
    LOG(WARNING) << "OHOSMediaCodecBridgeImpl::DequeueOutputBuffer error";
    return CodecCodeAdapter::ENCODER_ERROR;
  }

  if (signal_->out_buffer_queue_.empty()) {
    return CodecCodeAdapter::ENCODER_RETRY;
  }

  EncodeOutputBuffer ouput_buffer = signal_->out_buffer_queue_.front();
  index = ouput_buffer.index;
  flag = ouput_buffer.flag;
  info = ouput_buffer.buffer_info;
  buffer = ouput_buffer.buffer_data;
  bool is_key_frame =
      (flag == OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_SYNC_FRAME);
  if (is_key_frame && ouput_buffer.is_contain_config_data) {
    EncodeConfigData config_data = ouput_buffer.config_data;
    BufferInfo merge_frame_info;
    OhosBuffer merge_frame_data;
    merge_frame_info.presentation_time_us = info.presentation_time_us;
    merge_frame_info.size = info.size + config_data.buffer_info.size;
    merge_frame_data.buffer_size =
        config_data.buffer_data.buffer_size + buffer.buffer_size;
    merge_frame_info.offset = 0;
    keyframe_addr_ = new uint8_t[merge_frame_data.buffer_size];
    if (keyframe_addr_ == nullptr) {
      LOG(ERROR) << "DequeueOutputBuffer malloc failed";
      return CodecCodeAdapter::ENCODER_ERROR;
    }
    // size of keyframe_addr_ is defined as config_data.buffer_data.buffer_size
    // + buffer.buffer_size, so two memcpy should no have problem
    memcpy(keyframe_addr_, config_data.buffer_data.addr,
           config_data.buffer_data.buffer_size);

    memcpy(keyframe_addr_ + config_data.buffer_data.buffer_size, buffer.addr,
           buffer.buffer_size);
    merge_frame_data.addr = keyframe_addr_;
    info = merge_frame_info;
    buffer = merge_frame_data;
    ReleaseOutputBuffer(config_data.index, false);
  }

  PopOutQueue();
  return CodecCodeAdapter::ENCODER_OK;
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::ReleaseOutputBuffer(uint32_t index,
                                                               bool render) {
  if (video_encoder_ == nullptr) {
    LOG(ERROR) << "OHOSMediaCodecBridgeImpl::ReleaseOutputBuffer "
                  "video_encoder_ is NULL.";
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  if (OH_VideoEncoder_FreeOutputBuffer(video_encoder_, index) != AV_ERR_OK) {
    LOG(ERROR)
        << "OHOSMediaCodecBridgeImpl::OH_VideoEncoder_FreeOutputBuffer error";
    return CodecCodeAdapter::ENCODER_ERROR;
  }
  return CodecCodeAdapter::ENCODER_OK;
}

void OHOSMediaCodecBridgeImpl::ClearKeyFrameCache() {
  if (keyframe_addr_) {
    delete[] keyframe_addr_;
    keyframe_addr_ = nullptr;
  }
}

CodecEncodeBridgeCallback::CodecEncodeBridgeCallback(
    std::shared_ptr<CodecBridgeSignal> signal)
    : signal_(signal) {}

void CodecEncodeBridgeCallback::OnError(int32_t error_code) {
  LOG(ERROR) << "Error errorCode=" << error_code;
  signal_->isOnError_ = true;
  ClearOutputCache(signal_->out_buffer_queue_);
}

void CodecEncodeBridgeCallback::OnStreamChanged(OH_AVFormat* format) {
  LOG(INFO) << "Output Format Changed.";
}

void CodecEncodeBridgeCallback::OnNeedInputData(uint32_t index,
                                                OhosBuffer buffer) {
  LOG(INFO) << "CodecBridgeCallback::OnNeedInputData";
}

void CodecEncodeBridgeCallback::ClearBufferInfoCache(BufferInfo& buffer_info) {
  buffer_info.presentation_time_us = 0;
  buffer_info.size = 0;
  buffer_info.offset = 0;
}

void CodecEncodeBridgeCallback::ClearBufferDataCache(OhosBuffer& buffer_data) {
  buffer_data.addr = nullptr;
  buffer_data.buffer_size = 0;
  buffer_data.buffer = nullptr;
}

void CodecEncodeBridgeCallback::ClearConfigCache() {
  ClearBufferInfoCache(config_data_.buffer_info);
  ClearBufferDataCache(config_data_.buffer_data);
}

void CodecEncodeBridgeCallback::InitEncodeOuputBuffer(
    EncodeOutputBuffer& output_buffer) {
  output_buffer.is_contain_config_data = false;
  output_buffer.config_data.buffer_info.presentation_time_us = 0;
  output_buffer.config_data.buffer_info.size = 0;
  output_buffer.config_data.buffer_info.offset = 0;
  output_buffer.config_data.buffer_data.addr = nullptr;
  output_buffer.config_data.buffer_data.buffer_size = 0;
  output_buffer.config_data.buffer_data.buffer = nullptr;

  output_buffer.buffer_info.presentation_time_us = 0;
  output_buffer.buffer_info.size = 0;
  output_buffer.buffer_info.offset = 0;

  output_buffer.buffer_data.addr = nullptr;
  output_buffer.buffer_data.buffer_size = 0;
  output_buffer.buffer_data.buffer = nullptr;

  output_buffer.flag = OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_NONE;
}

void CodecEncodeBridgeCallback::OnNeedOutputData(uint32_t index,
                                                 BufferInfo info,
                                                 uint32_t flag,
                                                 OhosBuffer buffer) {
  if (!is_running_.load()) {
    LOG(WARNING) << "encoder is not running";
    return;
  }
  if (!codec_callback_task_runner_->RunsTasksInCurrentSequence()) {
    codec_callback_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&CodecEncodeBridgeCallback::OnNeedOutputData,
                       shared_from_this(), std::move(index), std::move(info),
                       std::move(flag), std::move(buffer)));
    return;
  }

  if (flag == OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_CODEC_DATA) {
    config_data_.index = index;
    config_data_.buffer_info = info;
    config_data_.buffer_data = buffer;
    return;
  }
  EncodeOutputBuffer output_buffer;
  InitEncodeOuputBuffer(output_buffer);
  bool is_key_frame =
      (flag == OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_SYNC_FRAME);
  if (is_key_frame && (config_data_.buffer_data.buffer_size > 0)) {
    output_buffer.is_contain_config_data = true;
    output_buffer.config_data = config_data_;
  }

  output_buffer.index = index;
  output_buffer.buffer_info = info;
  output_buffer.buffer_data = buffer;
  output_buffer.flag = (OH_AVCodecBufferFlags)flag;
  signal_->out_buffer_queue_.push(output_buffer);
  ClearConfigCache();
}

void CodecEncodeBridgeCallback::ClearCache() {
  ClearConfigCache();
  ClearBufferInfoCache(config_buffer_info_);
  ClearBufferDataCache(config_buffer_data_);
  ClearOutputCache(signal_->out_buffer_queue_);
}
}  // namespace media
