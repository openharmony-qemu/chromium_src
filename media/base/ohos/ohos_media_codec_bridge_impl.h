// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_MEDIA_CODEC_BRIDGE_IMPL_H_
#define MEDIA_BASE_OHOS_MEDIA_CODEC_BRIDGE_IMPL_H_

#include <native_window/external_window.h>
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
#include "media/base/video_encoder.h"
#include "ohos_media_codec_bridge.h"

namespace media {

struct EncodeConfigData {
  uint32_t index;
  BufferInfo buffer_info;
  OhosBuffer buffer_data;
};

struct EncodeOutputBuffer {
  uint32_t index;
  OH_AVCodecBufferFlags flag;
  BufferInfo buffer_info;
  OhosBuffer buffer_data;
  bool contains_config_data = false;
  EncodeConfigData config_data;
};

struct EncodeConfigDataCache {
  uint8_t* config_data_addr = nullptr;
  uint32_t config_data_size;
  uint32_t config_info_size;
};

class CodecBridgeSignal {
 public:
  std::queue<EncodeOutputBuffer> out_buffer_queue_;
  std::atomic<bool> isOnError_ = false;
};

class CodecEncodeBridgeCallback
    : public std::enable_shared_from_this<CodecEncodeBridgeCallback> {
 public:
  CodecEncodeBridgeCallback();
  explicit CodecEncodeBridgeCallback(std::shared_ptr<CodecBridgeSignal> signal);
  virtual ~CodecEncodeBridgeCallback() = default;

  void OnError(int32_t error_code);
  void OnStreamChanged(OH_AVFormat* format);
  void OnNeedOutputData(uint32_t index,
                        BufferInfo info,
                        uint32_t flag,
                        OhosBuffer buffer);
  void OnNeedInputData(uint32_t index, OhosBuffer buffer);

  void ClearCache();
  scoped_refptr<base::SequencedTaskRunner> codec_callback_task_runner_;
  std::atomic<bool> is_running_ = false;
  base::RepeatingClosure on_buffers_available_cb_;

 private:
  void InitEncodeOuputBuffer(EncodeOutputBuffer& output_buffer);
  void ClearConfigCache();
  void ClearBufferInfoCache(BufferInfo& buffer_info);
  void ClearBufferDataCache(OhosBuffer& buffer_data);
  std::shared_ptr<CodecBridgeSignal> signal_;
  uint32_t config_index_;
  BufferInfo config_buffer_info_;
  OhosBuffer config_buffer_data_;
  EncodeConfigData config_data_;
};

class OHOSMediaCodecBridgeImpl : public OHOSMediaCodecBridge {
 public:
  static std::unique_ptr<OHOSMediaCodecBridge> CreateVideoEncoder(
      std::string& mime);

  OHOSMediaCodecBridgeImpl(const OHOSMediaCodecBridgeImpl&) = delete;
  OHOSMediaCodecBridgeImpl& operator=(const OHOSMediaCodecBridgeImpl&) = delete;

  ~OHOSMediaCodecBridgeImpl() override;

  CodecCodeAdapter CreateVideoCodecByMime(const std::string mime_type) override;
  CodecCodeAdapter CreateVideoCodecByName(const std::string name) override;
  CodecCodeAdapter Configure(
      OH_AVFormat& config_format,
      scoped_refptr<base::SequencedTaskRunner> codec_task_runner) override;
  CodecCodeAdapter Prepare() override;
  CodecCodeAdapter Start() override;
  CodecCodeAdapter Stop() override;
  CodecCodeAdapter Reset() override;
  CodecCodeAdapter Release() override;
  CodecCodeAdapter CreateInputSurface() override;
  CodecCodeAdapter DequeueOutputBuffer(uint32_t& index,
                                       BufferInfo& info,
                                       OH_AVCodecBufferFlags& flag,
                                       OhosBuffer& buffer) override;
  CodecCodeAdapter ReleaseOutputBuffer(uint32_t index, bool render) override;
  void ClearKeyFrameCache() override;
  CodecCodeAdapter RequestKeyFrameSoon() override;
  CodecCodeAdapter FillSurfaceBuffer(scoped_refptr<VideoFrame> frame,
                                     const int64_t timestamp_ms) override;

 private:
  explicit OHOSMediaCodecBridgeImpl(std::string& mime);
  void PopOutQueue();
  void UpdateStatusAndClearCache(bool is_running);
  void ClearConfigDataCache();
  OH_AVCodec* video_encoder_ = nullptr;
  std::shared_ptr<CodecBridgeSignal> signal_ = nullptr;
  std::shared_ptr<CodecEncodeBridgeCallback> cb_;
  scoped_refptr<base::SequencedTaskRunner> codec_task_runner_ = nullptr;
  std::atomic<bool> is_running_ = false;
  uint8_t* keyframe_addr_ = nullptr;
  EncodeConfigDataCache config_data_cache_{};
  OHNativeWindow* native_window_ = nullptr;
};

}  // namespace media

#endif  // MEDIA_BASE_OHOS_MEDIA_CODEC_BRIDGE_IMPL_H_
