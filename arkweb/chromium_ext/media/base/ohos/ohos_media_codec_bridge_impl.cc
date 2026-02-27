// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "buffer_request_config_adapter_impl.h"
#include "ohos_media_codec_bridge_impl.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <cmath>

#include "base/logging.h"
#include "buffer_flush_config_adapter_impl.h"
#include "codec_config_para_adapter_impl.h"
#include "ohos_adapter_helper.h"
#include "third_party/bounds_checking_function/include/securec.h"

namespace media {
using namespace OHOS::NWeb;

namespace {
constexpr int32_t SAMPLE_RATIO = 2;
constexpr int32_t DEFAULT_STRIDE = 16;
constexpr int32_t MAXPLANES = 3;

struct YUVMemcpyData {
  int32_t stride;
  int32_t width;
  int32_t height;
  int32_t planes_cnt[MAXPLANES];
  int32_t planes_stride[MAXPLANES];
  raw_ptr<uint8_t> dst_addr;
  uint32_t dst_size = 0;
  const uint8_t* src_addr[MAXPLANES];
  size_t src_size[MAXPLANES];
};

CodecCodeAdapter FillSurfaceBufferDataCheck(
    scoped_refptr<VideoFrame> frame, std::shared_ptr<BufferRequestConfigAdapterImpl> configAdapter, YUVMemcpyData &data)
{
  LOG(DEBUG) << __FUNCTION__ << " enter";
  std::vector<size_t> frame_planes_size = frame->GetPlaneSize();
  if (frame_planes_size.size() < MAXPLANES) {
    LOG(ERROR) << "frame planes cnt < " << MAXPLANES;
    return CodecCodeAdapter::ERROR;
  }
  data.width = configAdapter->GetWidth();
  data.height = configAdapter->GetHeight();
  data.planes_cnt[VideoFrame::kYPlane] = data.height;
  data.planes_cnt[VideoFrame::kUPlane] = data.height / SAMPLE_RATIO;
  data.planes_cnt[VideoFrame::kVPlane] = data.height / SAMPLE_RATIO;
  data.planes_stride[VideoFrame::kYPlane] = frame->stride(VideoFrame::kYPlane);
  data.planes_stride[VideoFrame::kUPlane] = frame->stride(VideoFrame::kUPlane);
  data.planes_stride[VideoFrame::kVPlane] = frame->stride(VideoFrame::kVPlane);
  data.src_addr[VideoFrame::kYPlane] = frame->data(VideoFrame::kYPlane);
  data.src_addr[VideoFrame::kUPlane] = frame->data(VideoFrame::kUPlane);
  data.src_addr[VideoFrame::kVPlane] = frame->data(VideoFrame::kVPlane);
  data.src_size[VideoFrame::kYPlane] = frame_planes_size[VideoFrame::kYPlane];
  data.src_size[VideoFrame::kUPlane] = frame_planes_size[VideoFrame::kUPlane];
  data.src_size[VideoFrame::kVPlane] = frame_planes_size[VideoFrame::kVPlane];
  // check addr.
  if (data.dst_addr == nullptr || nullptr == data.src_addr[VideoFrame::kYPlane] ||
      nullptr == data.src_addr[VideoFrame::kUPlane] || nullptr == data.src_addr[VideoFrame::kVPlane]) {
    LOG(ERROR) << "addr is nullptr";
    return CodecCodeAdapter::ERROR;
  }
  // check stride and width
  if (data.width > data.planes_stride[VideoFrame::kYPlane] ||
      data.width / SAMPLE_RATIO > data.planes_stride[VideoFrame::kUPlane] ||
      data.width / SAMPLE_RATIO > data.planes_stride[VideoFrame::kVPlane]) {
      LOG(ERROR) << "width less than stride";
      return CodecCodeAdapter::ERROR;
  }
  // check addr size.
  uint64_t required_dst_space = data.planes_cnt[VideoFrame::kYPlane] * data.stride +
      data.planes_cnt[VideoFrame::kUPlane] * data.stride / SAMPLE_RATIO +
      data.planes_cnt[VideoFrame::kVPlane] * data.stride / SAMPLE_RATIO;
  uint64_t required_y_src_space = data.planes_cnt[VideoFrame::kYPlane] * data.planes_stride[VideoFrame::kYPlane];
  uint64_t required_u_src_space = data.planes_cnt[VideoFrame::kUPlane] * data.planes_stride[VideoFrame::kUPlane];
  uint64_t required_v_src_space = data.planes_cnt[VideoFrame::kVPlane] * data.planes_stride[VideoFrame::kVPlane];
  if (required_dst_space > data.dst_size || required_y_src_space > data.src_size[VideoFrame::kYPlane] ||
      required_u_src_space > data.src_size[VideoFrame::kUPlane] ||
      required_v_src_space > data.src_size[VideoFrame::kVPlane]) {
    LOG(ERROR) << __FUNCTION__ << "failed, plane size error";
    return CodecCodeAdapter::ERROR;
  }
  auto offset = std::abs(data.src_addr[VideoFrame::kVPlane] - data.src_addr[VideoFrame::kYPlane]);
  uint64_t required_all_plane_space = data.src_size[VideoFrame::kVPlane] + offset;
  uint64_t real_all_plane_space = frame->layout().planes()[VideoFrame::kYPlane].size + 
      frame->layout().planes()[VideoFrame::kUPlane].size + frame->layout().planes()[VideoFrame::kVPlane].size;
  if (required_all_plane_space > real_all_plane_space) {
    LOG(ERROR) << "plane size error, required_all_plane_space > real_all_plane_space!";
    return CodecCodeAdapter::ERROR; 
  }
  return CodecCodeAdapter::OK;
}

CodecCodeAdapter CopyYUVData(const YUVMemcpyData &data, int32_t plane, uint8_t **dst) {
  LOG(DEBUG) << "CopyYUVData enter";
  if (plane < 0 || plane >= MAXPLANES) {
      LOG(ERROR) << "Invalid plane index: " << plane;
      return CodecCodeAdapter::ERROR;
  }
  int32_t width = data.width;
  int32_t stride = data.stride;
  const uint8_t *src = data.src_addr[plane];
  if (plane == VideoFrame::kUPlane || plane == VideoFrame::kVPlane) {
    width = width / SAMPLE_RATIO;
    stride = stride / SAMPLE_RATIO;
  }
  if (data.planes_cnt[plane] < 0) {
    LOG(ERROR) << "CopyYUVData check data.planes_cnt[plane] < 0";
    return CodecCodeAdapter::ERROR;
  }
  for (int32_t i = 0; i < data.planes_cnt[plane]; i++) {
    if (stride < width || (memcpy_s(*dst, stride, src, width) != EOK)) {
      LOG(ERROR) << "memcpy_s failed";
      return CodecCodeAdapter::ERROR;
    }
    *dst += stride;
    src += data.planes_stride[plane];
  }

  return CodecCodeAdapter::OK;
}
}

void ClearOutputCache(std::queue<EncodeOutputBuffer>& q) {
  std::queue<EncodeOutputBuffer> empty;
  std::swap(empty, q);
}
// static
std::unique_ptr<OHOSMediaCodecBridge>
OHOSMediaCodecBridgeImpl::CreateVideoEncoder(std::string& mime) {
  LOG(INFO) << "OHOSMediaCodecBridgeImpl::CreateVideoEncoder.";
  return absl::WrapUnique(new OHOSMediaCodecBridgeImpl(mime));
}

OHOSMediaCodecBridgeImpl::OHOSMediaCodecBridgeImpl(std::string& mime) {
  codec_adapter_ =
      OhosAdapterHelper::GetInstance().CreateMediaCodecEncoderAdapter();
  CreateVideoCodecByMime(mime);
}

OHOSMediaCodecBridgeImpl::~OHOSMediaCodecBridgeImpl() {
  LOG(INFO) << "~OHOSMediaCodecBridgeImpl.";
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::CreateVideoCodecByMime(
    std::string mimetype) {
  LOG(INFO) << "OHOSMediaCodecBridgeImpl::CreateVideoCodecByMime.";
  if (codec_adapter_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ERROR;
  }
  if (mimetype == "video/avc") {
    LOG(INFO) << "create video codec by mime, type : " << mimetype;
  }
  CodecCodeAdapter ret = codec_adapter_->CreateVideoCodecByMime(mimetype);
  if (ret == CodecCodeAdapter::ERROR) {
    LOG(INFO) << "create codec failed.";
    return ret;
  }

  if (signal_ == nullptr) {
    signal_ = std::make_shared<CodecBridgeSignal>();
  }

  if (cb_ == nullptr) {
    cb_ = std::make_shared<CodecEncodeBridgeCallback>(signal_);
  }

  return codec_adapter_->SetCodecCallback(cb_);
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::CreateVideoCodecByName(
    std::string name) {
  LOG(INFO) << "create video codec by name, type : " << name.c_str();
  if (codec_adapter_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ERROR;
  }

  CodecCodeAdapter ret = codec_adapter_->CreateVideoCodecByName(name);
  if (ret == CodecCodeAdapter::ERROR) {
    LOG(ERROR) << "create codec failed.";
    return ret;
  }

  return ret;
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
    ClearConfigDataCache();
    ClearOutputCache(signal_->out_buffer_queue_);
    if (cb_) {
      cb_->ClearCache();
    }
  }
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::Configure(
    const CodecConfigPara& config,
    scoped_refptr<base::SequencedTaskRunner> codec_task_runner) {
  LOG(INFO) << "configure codec bitRate:" << config.bitRate;
  if ((codec_adapter_ == nullptr) || (cb_ == nullptr)) {
    LOG(ERROR) << "codec or callback is NULL.";
    return CodecCodeAdapter::ERROR;
  }
  codec_task_runner_ = codec_task_runner;
  cb_->codec_callback_task_runner_ = codec_task_runner;

  std::shared_ptr<CodecConfigParaAdapterImpl> configAdapter =
      std::make_shared<CodecConfigParaAdapterImpl>();
  if (!configAdapter) {
    LOG(ERROR)
        << "OHOSMediaCodecBridgeImpl::Configure error configAdapter is NULL";
    return CodecCodeAdapter::ERROR;
  }

  configAdapter->SetWidth(config.width);
  configAdapter->SetHeight(config.height);
  configAdapter->SetBitRate(config.bitRate);
  configAdapter->SetFrameRate(config.frameRate);

  return codec_adapter_->Configure(configAdapter);
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::Prepare() {
  LOG(INFO) << "prepare codec.";
  if (codec_adapter_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ERROR;
  }
  return codec_adapter_->Prepare();
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::Start() {
  LOG(INFO) << "start.";
  UpdateStatusAndClearCache(true);
  if (codec_adapter_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ERROR;
  }

  return codec_adapter_->Start();
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::Stop() {
  UpdateStatusAndClearCache(false);
  LOG(INFO) << "stop codec.";
  if (codec_adapter_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ERROR;
  }

  return codec_adapter_->Stop();
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::Reset() {
  LOG(INFO) << "Reset codec.";
  UpdateStatusAndClearCache(false);
  if (codec_adapter_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ERROR;
  }
  return codec_adapter_->Reset();
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::Release() {
  LOG(INFO) << "Release codec.";
  UpdateStatusAndClearCache(false);
  if (codec_adapter_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ERROR;
  }
  return codec_adapter_->Release();
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::CreateInputSurface() {
  LOG(INFO) << "CreateInputSurface.";
  if (codec_adapter_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ERROR;
  }

  surface_ = codec_adapter_->CreateInputSurface();
  if (surface_ == nullptr) {
    LOG(ERROR) << "create input surface failed";
    return CodecCodeAdapter::ERROR;
  }
  return CodecCodeAdapter::OK;
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::FillSurfaceBuffer(
    scoped_refptr<VideoFrame> frame, const int64_t timestamp_ms) {
  LOG(DEBUG) << __FUNCTION__ << " enter";
  if ((surface_ == nullptr) || (frame == nullptr)) {
    LOG(DEBUG) << "surface_ is null when QueueInput";
    return CodecCodeAdapter::ERROR;
  }
  YUVMemcpyData data;
  (void)memset_s(&data, sizeof(data), 0, sizeof(data)); 
  int32_t fence;
  std::shared_ptr<BufferRequestConfigAdapterImpl> configAdapter = std::make_shared<BufferRequestConfigAdapterImpl>();
  configAdapter->SetWidth(frame->coded_size().width());
  configAdapter->SetHeight(frame->coded_size().height());
  configAdapter->SetStrideAlignment(DEFAULT_STRIDE);
  configAdapter->SetTimestamp(timestamp_ms);
  buffer_adapter_ = surface_->RequestBuffer(fence, configAdapter);
  if (buffer_adapter_ == nullptr) {
    LOG(DEBUG) << "fail to RequestBuffer";
    return CodecCodeAdapter::ERROR;
  }
  data.stride = buffer_adapter_->GetStride();
  data.dst_addr = reinterpret_cast<uint8_t*>(buffer_adapter_->GetVirAddr());
  data.dst_size = buffer_adapter_->GetSize();
  if (FillSurfaceBufferDataCheck(frame, configAdapter, data) != CodecCodeAdapter::OK) {
    buffer_adapter_ = nullptr;
    return CodecCodeAdapter::ERROR;
  }
  uint8_t *dst = data.dst_addr.get();
  if ((CopyYUVData(data, VideoFrame::kYPlane, &dst) != CodecCodeAdapter::OK) ||
      (CopyYUVData(data, VideoFrame::kUPlane, &dst) != CodecCodeAdapter::OK) ||
      (CopyYUVData(data, VideoFrame::kVPlane, &dst) != CodecCodeAdapter::OK)) {
    buffer_adapter_ = nullptr;
    return CodecCodeAdapter::ERROR;
  }
  std::shared_ptr<BufferFlushConfigAdapterImpl> flush_config_adapter =
      std::make_shared<BufferFlushConfigAdapterImpl>();
  flush_config_adapter->SetX(0);
  flush_config_adapter->SetY(0);
  flush_config_adapter->SetW(configAdapter->GetWidth());
  flush_config_adapter->SetH(configAdapter->GetHeight());
  flush_config_adapter->SetTimestamp(timestamp_ms);
  LOG(DEBUG) << "flush_config_adapter x " << flush_config_adapter->GetX()
             << ", y " << flush_config_adapter->GetY() << ", w "
             << flush_config_adapter->GetW() << ", h "
             << flush_config_adapter->GetH() << ", timestamp "
             << flush_config_adapter->GetTimestamp()
             << ", intput real frame time stamp: "
             << frame->timestamp().InMicroseconds();
  if (surface_->FlushBuffer(buffer_adapter_, fence, flush_config_adapter) != 0) {
    buffer_adapter_ = nullptr;
    LOG(DEBUG) << "fail to FlushBuffer";
    return CodecCodeAdapter::ERROR;
  }
  return CodecCodeAdapter::OK;
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::RequestKeyFrameSoon() {
  LOG(INFO) << "RequestKeyFrameSoon.";
  if (codec_adapter_ == nullptr) {
    LOG(ERROR) << "codec is NULL.";
    return CodecCodeAdapter::ERROR;
  }
  return codec_adapter_->RequestKeyFrameSoon();
}

void OHOSMediaCodecBridgeImpl::PopOutQueue() {
  signal_->out_buffer_queue_.pop();
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::DequeueOutputBuffer(
    uint32_t& index,
    BufferInfo& info,
    BufferFlag& flag,
    OhosBuffer& buffer) {
  if (signal_->isOnError_) {
    LOG(DEBUG) << "OHOSMediaCodecBridgeImpl::isOnError_";
    return CodecCodeAdapter::ERROR;
  }

  if (signal_->out_buffer_queue_.empty()) {
    LOG(DEBUG) << "OHOSMediaCodecBridgeImpl::RETRY";
    return CodecCodeAdapter::RETRY;
  }

  EncodeOutputBuffer ouput_buffer = signal_->out_buffer_queue_.front();
  index = ouput_buffer.index;
  flag = ouput_buffer.flag;
  info = ouput_buffer.buffer_info;
  buffer = ouput_buffer.buffer_data;
  bool is_key_frame = (flag == BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME);
  LOG(DEBUG) << "DequeueOutputBuffer is_key_frame " << is_key_frame
             << ", is_contain_config_data: "
             << ouput_buffer.is_contain_config_data;
  if (is_key_frame) {
    if (ouput_buffer.is_contain_config_data) {
      LOG(DEBUG) << "update config data cache";
      ClearConfigDataCache();
      EncodeConfigData config_data = ouput_buffer.config_data;
      config_data_cache_.config_info_size = config_data.buffer_info.size;
      config_data_cache_.config_data_size = config_data.buffer_data.bufferSize;
      config_data_cache_.config_data_addr =
          new uint8_t[config_data_cache_.config_data_size];
      if (config_data_cache_.config_data_addr == nullptr) {
        LOG(ERROR) << "new config data failed";
        ReleaseOutputBuffer(config_data.index, false);
        return CodecCodeAdapter::ERROR;
      }
      if (memcpy_s(config_data_cache_.config_data_addr,
                   config_data_cache_.config_data_size,
                   config_data.buffer_data.addr,
                   config_data.buffer_data.bufferSize) != EOK) {
        LOG(ERROR) << "config data memcpy_s failed";
        ReleaseOutputBuffer(config_data.index, false);
        return CodecCodeAdapter::ERROR;
      }
      ReleaseOutputBuffer(config_data.index, false);
    }
    uint32_t config_data_size = config_data_cache_.config_data_size;
    uint32_t config_info_size = config_data_cache_.config_info_size;
    uint8_t* config_data_addr = config_data_cache_.config_data_addr;
    if ((config_data_addr == nullptr) || (config_data_size == 0)) {
      LOG(DEBUG) << "config data is invalid when handle key frame";
      PopOutQueue();
      return CodecCodeAdapter::ERROR;
    }
    BufferInfo merge_frame_info;
    OhosBuffer merge_frame_data;
    merge_frame_info.presentationTimeUs = info.presentationTimeUs;
    merge_frame_info.size = info.size + config_info_size;
    merge_frame_data.bufferSize = config_data_size + buffer.bufferSize;
    merge_frame_info.offset = 0;
    keyframe_addr_ = new uint8_t[merge_frame_data.bufferSize];
    if (!keyframe_addr_) {
      LOG(ERROR) << "new key frame failed";
      ClearConfigDataCache();
      PopOutQueue();
      return CodecCodeAdapter::ERROR;
    }
    LOG(DEBUG) << "DequeueOutputBuffer handle keyframe : configSize: "
               << config_data_size << ", buffersize: " << buffer.bufferSize
               << ", mergeFrame Size: " << merge_frame_data.bufferSize;
    if (memcpy_s(keyframe_addr_.get(), merge_frame_data.bufferSize, config_data_addr,
                 config_data_size) != EOK) {
      LOG(ERROR) << "keyframe_addr_ memcpy_s failed";
      ClearKeyFrameCache();
      ClearConfigDataCache();
      PopOutQueue();
      return CodecCodeAdapter::ERROR;
    }
    if (memcpy_s(keyframe_addr_.get() + config_data_size, buffer.bufferSize,
                 buffer.addr, buffer.bufferSize) != EOK) {
      LOG(ERROR) << "keyframe_addr_ + config_data_size memcpy_s failed";
      ClearKeyFrameCache();
      ClearConfigDataCache();
      PopOutQueue();
      return CodecCodeAdapter::ERROR;
    }
    merge_frame_data.addr = keyframe_addr_;
    info = merge_frame_info;
    buffer = merge_frame_data;
  }

  LOG(DEBUG) << "OHOSMediaCodecBridgeImpl::index " << index
             << ", presentationTimeUs. " << info.presentationTimeUs
             << ", size. " << info.size << ", offset. " << info.offset
             << ", flag. " << (int32_t)flag << ", buffersize. "
             << buffer.bufferSize;
  PopOutQueue();
  return CodecCodeAdapter::OK;
}

CodecCodeAdapter OHOSMediaCodecBridgeImpl::ReleaseOutputBuffer(uint32_t index,
                                                               bool render) {
  LOG(DEBUG) << "OHOSMediaCodecBridgeImpl::ReleaseOutputBuffer. index: "
             << index;
  if (codec_adapter_ == nullptr) {
    LOG(DEBUG) << "OHOSMediaCodecBridgeImpl::ReleaseOutputBuffer "
                  "codec_adapter_ is NULL.";
    return CodecCodeAdapter::ERROR;
  }
  return codec_adapter_->ReleaseOutputBuffer(index, render);
}

void OHOSMediaCodecBridgeImpl::ClearKeyFrameCache() {
  if (keyframe_addr_) {
    LOG(DEBUG) << "OHOSMediaCodecBridgeImpl::ClearKeyFrameCache enter.";
    delete[] keyframe_addr_.get();
    keyframe_addr_ = nullptr;
  }
}

void OHOSMediaCodecBridgeImpl::ClearConfigDataCache() {
  if (config_data_cache_.config_data_addr) {
    LOG(DEBUG) << "clear config first";
    delete[] config_data_cache_.config_data_addr;
    config_data_cache_.config_data_addr = nullptr;
  }
  config_data_cache_.config_data_size = 0;
  config_data_cache_.config_info_size = 0;
}

CodecEncodeBridgeCallback::CodecEncodeBridgeCallback(
    std::shared_ptr<CodecBridgeSignal> signal)
    : signal_(signal) {}

void CodecEncodeBridgeCallback::OnError(ErrorType errorType,
                                        int32_t errorCode) {
  LOG(DEBUG) << "Error errorCode=" << errorCode;
  if (!codec_callback_task_runner_->RunsTasksInCurrentSequence()) {
    codec_callback_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&CodecEncodeBridgeCallback::OnError,
                       shared_from_this(), errorType, errorCode));
    return;
  }
  signal_->isOnError_ = true;
  ClearOutputCache(signal_->out_buffer_queue_);
}

void CodecEncodeBridgeCallback::OnStreamChanged(
    const std::shared_ptr<CodecFormatAdapter> format) {
  LOG(DEBUG) << "Output Format Changed.";
}

void CodecEncodeBridgeCallback::OnNeedInputData(
    uint32_t index,
    std::shared_ptr<OhosBufferAdapter> buffer) {
  LOG(DEBUG) << "CodecBridgeCallback::OnNeedInputData";
}

void CodecEncodeBridgeCallback::ClearBufferInfoCache(BufferInfo& buffer_info) {
  buffer_info.presentationTimeUs = 0;
  buffer_info.size = 0;
  buffer_info.offset = 0;
}

void CodecEncodeBridgeCallback::ClearBufferDataCache(OhosBuffer& buffer_data) {
  buffer_data.addr = nullptr;
  buffer_data.bufferSize = 0;
}

void CodecEncodeBridgeCallback::ClearConfigCache() {
  ClearBufferInfoCache(config_data_.buffer_info);
  ClearBufferDataCache(config_data_.buffer_data);
}

void CodecEncodeBridgeCallback::InitEncodeOuputBuffer(
    EncodeOutputBuffer& output_buffer) {
  output_buffer.is_contain_config_data = false;
  output_buffer.config_data.buffer_info.presentationTimeUs = 0;
  output_buffer.config_data.buffer_info.size = 0;
  output_buffer.config_data.buffer_info.offset = 0;
  output_buffer.config_data.buffer_data.addr = nullptr;
  output_buffer.config_data.buffer_data.bufferSize = 0;

  output_buffer.buffer_info.presentationTimeUs = 0;
  output_buffer.buffer_info.size = 0;
  output_buffer.buffer_info.offset = 0;

  output_buffer.buffer_data.addr = nullptr;
  output_buffer.buffer_data.bufferSize = 0;

  output_buffer.flag = BufferFlag::CODEC_BUFFER_FLAG_NONE;
}

void CodecEncodeBridgeCallback::OnNeedOutputData(
    uint32_t index,
    std::shared_ptr<BufferInfoAdapter> info,
    BufferFlag flag,
    std::shared_ptr<OhosBufferAdapter> buffer) {
  if (!info || !buffer) {
    LOG(ERROR) << "Output is invalid";
  }

  LOG(DEBUG) << "Output Buffer Available, queue size, "
             << signal_->out_buffer_queue_.size() << ", index =" << index
             << ", timestamp = " << info->GetPresentationTimeUs()
             << ", isrunning = " << is_running_.load();
  if (!is_running_.load()) {
    LOG(DEBUG) << "encoder is not running";
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

  LOG(DEBUG) << "OnNeedOutputData::index " << index << ", presentationTimeUs. "
             << info->GetPresentationTimeUs() << ", size. " << info->GetSize()
             << ", offset. " << info->GetOffset() << ", flag. " << (int32_t)flag
             << ", buffersize. " << buffer->GetBufferSize();

  if (flag == BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA) {
    LOG(DEBUG) << "flag is codec_data, handle with keyframe later";
    config_data_.index = index;
    config_data_.buffer_info.presentationTimeUs = info->GetPresentationTimeUs();
    config_data_.buffer_info.size = info->GetSize();
    config_data_.buffer_info.offset = info->GetOffset();
    config_data_.buffer_data.addr = buffer->GetAddr();
    config_data_.buffer_data.bufferSize = buffer->GetBufferSize();
    return;
  }
  EncodeOutputBuffer output_buffer;
  InitEncodeOuputBuffer(output_buffer);
  bool is_key_frame = (flag == BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME);
  if (is_key_frame && (config_data_.buffer_data.bufferSize > 0)) {
    LOG(DEBUG) << "handle keyframe now";
    output_buffer.is_contain_config_data = true;
    output_buffer.config_data = config_data_;
  }

  output_buffer.index = index;
  output_buffer.buffer_info.presentationTimeUs = info->GetPresentationTimeUs();
  output_buffer.buffer_info.size = info->GetSize();
  output_buffer.buffer_info.offset = info->GetOffset();
  output_buffer.buffer_data.addr = buffer->GetAddr();
  output_buffer.buffer_data.bufferSize = buffer->GetBufferSize();
  output_buffer.flag = flag;
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
