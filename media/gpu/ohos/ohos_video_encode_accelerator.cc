// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/gpu/ohos/ohos_video_encode_accelerator.h"

#include <memory>
#include <set>
#include <string>
#include <tuple>

#include "base/functional/bind.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/memory/shared_memory_mapping.h"
#include "base/memory/unsafe_shared_memory_region.h"
#include "base/metrics/histogram_macros.h"
#include "base/task/sequenced_task_runner.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder.h"
#include "gpu/ipc/service/gpu_channel.h"
#include "media/base/bitstream_buffer.h"
#include "media/base/limits.h"
#include "media/base/media_log.h"
#include "media/base/ohos/ohos_media_codec_util.h"
#include "media/video/picture.h"
#include "media_codec_adapter.h"
#include "third_party/bounds_checking_function/include/securec.h"
#include "third_party/libyuv/include/libyuv/convert_from.h"
#include "ui/gl/gl_bindings.h"

namespace media {

static inline const base::TimeDelta EncodePollDelay() {
  return base::Milliseconds(10);
}

OHOSVideoEncodeAccelerator::OHOSVideoEncodeAccelerator()
    : error_occurred_(false) {
  LOG(INFO) << "ohos video encode accelerator create";
}

OHOSVideoEncodeAccelerator::~OHOSVideoEncodeAccelerator() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  LOG(INFO) << "ohos video encode accelerator destroy";
}

VideoEncodeAccelerator::SupportedProfiles
OHOSVideoEncodeAccelerator::GetSupportedProfiles() {
  SupportedProfiles profiles;
  CapabilityData H264capabilityAdapter =
      OHOSMediaCodecUtil::GetCodecCapability("video/avc", true);
  VideoCodecProfile h264Profiles[] = {H264PROFILE_BASELINE, H264PROFILE_MAIN};
  for(const auto& codecProfile : h264Profiles) {
    SupportedProfile H264profile;
    H264profile.profile = codecProfile;
    H264profile.max_resolution.SetSize(H264capabilityAdapter.maxWidth,
                                     H264capabilityAdapter.maxHeight);
    H264profile.max_framerate_numerator = H264capabilityAdapter.maxframeRate;
    H264profile.rate_control_modes = media::VideoEncodeAccelerator::kConstantMode;
    LOG(INFO) << "OHOSVideoEncodeAccelerator::GetSupportedProfiles, maxWidth: " << H264capabilityAdapter.maxWidth
              << ", maxHeight: " << H264capabilityAdapter.maxHeight
              << ", maxframeRate" << H264capabilityAdapter.maxframeRate;
    profiles.push_back(H264profile);
  }
  return profiles;
}

bool OHOSVideoEncodeAccelerator::Initialize(
    const Config& config,
    Client* client,
    std::unique_ptr<MediaLog> media_log) {
  LOG(INFO) << __func__ << ", config:  " << config.AsHumanReadableString();
  DCHECK(!media_codec_);
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(client);
  log_ = std::move(media_log);
  client_ptr_factory_ = std::make_unique<base::WeakPtrFactory<Client>>(client);
  std::string mime_type;
  VideoCodec codec;
  uint32_t frame_input_count;
  if (config.output_profile == H264PROFILE_BASELINE ||
      config.output_profile == H264PROFILE_MAIN) {
    codec = VideoCodec::kH264;
    mime_type = "video/avc";
    frame_input_count = 1;
  } else {
    return false;
  }

  frame_size_ = config.input_visible_size;

  media_codec_ = OHOSMediaCodecBridgeImpl::CreateVideoEncoder(mime_type);
  if (!media_codec_) {
    LOG(ERROR) << "fail to create encoder";
    return false;
  }

  CodecConfigPara config_para;
  config_para.width = config.input_visible_size.width();
  config_para.height = config.input_visible_size.height();
  config_para.bitRate = config.bitrate.target_bps();
  config_para.frameRate = INITIAL_FRAMERATE;

  if (media_codec_->Configure(config_para,
                              base::SequencedTaskRunner::GetCurrentDefault()) !=
      CodecCodeAdapter::OK) {
    LOG(ERROR) << "fail to set encoder config";
    return false;
  }

  if (media_codec_->CreateInputSurface() != CodecCodeAdapter::OK) {
    LOG(ERROR) << "fail to create input surface";
    return false;
  }

  if (media_codec_->Prepare() != CodecCodeAdapter::OK) {
    LOG(ERROR) << "fail to prepare encoder";
    return false;
  }

  if (media_codec_->Start() != CodecCodeAdapter::OK) {
    LOG(ERROR) << "fail to prepare encoder";
    return false;
  }

  const size_t output_buffer_capacity = VideoFrame::AllocationSize(
      config.input_format, config.input_visible_size);
  base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE,
      base::BindOnce(&VideoEncodeAccelerator::Client::RequireBitstreamBuffers,
                     client_ptr_factory_->GetWeakPtr(), frame_input_count,
                     config.input_visible_size, output_buffer_capacity));
  return true;
}

void OHOSVideoEncodeAccelerator::MaybeStartIOTimer() {
  if (!io_timer_.IsRunning() &&
      (num_buffers_at_codec_ > 0 || !pending_frames_.empty())) {
    io_timer_.Start(FROM_HERE, EncodePollDelay(), this,
                    &OHOSVideoEncodeAccelerator::DoIOTask);
  }
}

void OHOSVideoEncodeAccelerator::MaybeStopIOTimer() {
  if (io_timer_.IsRunning() &&
      (num_buffers_at_codec_ == 0 && pending_frames_.empty())) {
    io_timer_.Stop();
  }
}

void OHOSVideoEncodeAccelerator::Encode(scoped_refptr<VideoFrame> frame,
                                        bool force_keyframe) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (frame->format() != PIXEL_FORMAT_I420) {
    NotifyErrorStatus(
        {EncoderStatus::Codes::kUnsupportedFrameFormat,
         "Unexpected format: " + VideoPixelFormatToString(frame->format())});
    return;
  }
  if (frame->visible_rect().size() != frame_size_) {
    NotifyErrorStatus({EncoderStatus::Codes::kInvalidInputFrame,
                       "Unexpected resolution: got " +
                           frame->visible_rect().size().ToString() +
                           ", expected " + frame_size_.ToString()});
    return;
  }

  pending_frames_.emplace(
      std::make_tuple(std::move(frame), force_keyframe, base::Time::Now()));
  DoIOTask();
}

void OHOSVideoEncodeAccelerator::UseOutputBitstreamBuffer(
    BitstreamBuffer buffer) {
  LOG(DEBUG) << __PRETTY_FUNCTION__ << ": bitstream_buffer_id=" << buffer.id();
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  available_bitstream_buffers_.push_back(std::move(buffer));
  DoIOTask();
}

void OHOSVideoEncodeAccelerator::RequestEncodingParametersChange(
    const Bitrate& bitrate,
    uint32_t framerate) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
}

void OHOSVideoEncodeAccelerator::Destroy() {
  LOG(INFO) << __PRETTY_FUNCTION__;
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  client_ptr_factory_.reset();
  if (media_codec_) {
    if (io_timer_.IsRunning()) {
      io_timer_.Stop();
    }
    media_codec_->Release();
  }
  delete this;
}

void OHOSVideoEncodeAccelerator::DoIOTask() {
  QueueInput();
  DequeueOutput();
  MaybeStartIOTimer();
  MaybeStopIOTimer();
}

void OHOSVideoEncodeAccelerator::QueueInput() {
  if (error_occurred_ || pending_frames_.empty()) {
    return;
  }

  const PendingFrames::value_type& input = pending_frames_.front();
  bool is_key_frame = std::get<1>(input);
  if (is_key_frame) {
    // Ideally MediaCodec would honor BUFFER_FLAG_SYNC_FRAME so we could
    // indicate this in the QueueInputBuffer() call below and guarantee _this_
    // frame be encoded as a key frame, but sadly that flag is ignored.
    // Instead, we request a key frame "soon".
    media_codec_->RequestKeyFrameSoon();
  }
  scoped_refptr<VideoFrame> frame = std::get<0>(input);
  presentation_timestamp_ += base::Microseconds(
      base::Time::kMicrosecondsPerSecond / INITIAL_FRAMERATE);
  DCHECK(frame_timestamp_map_.find(presentation_timestamp_) ==
         frame_timestamp_map_.end());
  frame_timestamp_map_[presentation_timestamp_] = frame->timestamp();

  if (media_codec_->FillSurfaceBuffer(
          std::move(frame), presentation_timestamp_.InMicroseconds()) !=
      CodecCodeAdapter::OK) {
    return;
  }
  LOG(DEBUG) << "QueueInput num_buffers_at_codec_ " << num_buffers_at_codec_;
  ++num_buffers_at_codec_;
  DCHECK(static_cast<int32_t>(frame_timestamp_map_.size()) ==
         num_buffers_at_codec_);
  pending_frames_.pop();
}

void OHOSVideoEncodeAccelerator::DequeueOutput() {
  if (error_occurred_ || available_bitstream_buffers_.empty() ||
      num_buffers_at_codec_ == 0) {
    return;
  }

  uint32_t index;
  BufferInfo info;
  BufferFlag flag;
  OhosBuffer buffer;
  CodecCodeAdapter ret =
      media_codec_->DequeueOutputBuffer(index, info, flag, buffer);

  switch (ret) {
    case CodecCodeAdapter::RETRY:
      return;

    case CodecCodeAdapter::ERROR:
      NotifyErrorStatus({EncoderStatus::Codes::kEncoderFailedEncode,
                         "MediaCodec error in DequeueOutputBuffer"});
      // Unreachable because of previous statement, but included for clarity.
      return;

    case CodecCodeAdapter::OK:
      DCHECK_GE(index, 0);
      break;

    default:
      NOTREACHED();
      break;
  }
  base::TimeDelta frame_timestamp;
  base::TimeDelta presentaion_timestamp =
      base::Microseconds(info.presentationTimeUs);
  const auto it = frame_timestamp_map_.find(presentaion_timestamp);
  if (it == frame_timestamp_map_.end()) {
    LOG(DEBUG) << "DequeueOutput can not find timestamp "
               << presentaion_timestamp.InMicroseconds();
    return;
  }
  DCHECK(it != frame_timestamp_map_.end());
  frame_timestamp = it->second;
  frame_timestamp_last_ = frame_timestamp;
  frame_timestamp_map_.erase(it);

  BitstreamBuffer bitstream_buffer =
      std::move(available_bitstream_buffers_.back());
  available_bitstream_buffers_.pop_back();

  base::UnsafeSharedMemoryRegion region = bitstream_buffer.TakeRegion();
  auto mapping =
      region.MapAt(bitstream_buffer.offset(), bitstream_buffer.size());
  if (!mapping.IsValid()) {
    NotifyErrorStatus(
        {EncoderStatus::Codes::kSystemAPICallError, "Failed to map SHM"});
    return;
  }
  if (info.size > bitstream_buffer.size()) {
    NotifyErrorStatus(
        {EncoderStatus::Codes::kEncoderFailedEncode,
         "Encoded buffer too large: " + base::NumberToString(info.size) + ">" +
             base::NumberToString(bitstream_buffer.size())});
    return;
  }

  if (memcpy_s(mapping.memory(), bitstream_buffer.size(), buffer.addr, buffer.bufferSize) != EOK) {
    NotifyErrorStatus(
        {EncoderStatus::Codes::kEncoderFailedEncode, "Failed to memcpy_s encoded data"});
  }
  bool key_frame = (flag == BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME);
  if (key_frame) {
    media_codec_->ClearKeyFrameCache();
  }
  LOG(DEBUG) << "DequeueOutput num_buffers_at_codec_ " << num_buffers_at_codec_
             << ", key_frame: " << key_frame;
  media_codec_->ReleaseOutputBuffer(index, false);
  --num_buffers_at_codec_;

  base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE,
      base::BindOnce(
          &VideoEncodeAccelerator::Client::BitstreamBufferReady,
          client_ptr_factory_->GetWeakPtr(), bitstream_buffer.id(),
          BitstreamBufferMetadata(info.size, key_frame, frame_timestamp)));
}

void OHOSVideoEncodeAccelerator::NotifyErrorStatus(EncoderStatus status) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  CHECK(!status.is_ok());
  CHECK(log_);
  MEDIA_LOG(ERROR, log_) << status.message();
  LOG(ERROR) << "Call NotifyErrorStatus(): code="
             << static_cast<int>(status.code())
             << ", message=" << status.message();
  if (!error_occurred_) {
    client_ptr_factory_->GetWeakPtr()->NotifyErrorStatus(status);
    error_occurred_ = true;
  }
}
}  // namespace media
