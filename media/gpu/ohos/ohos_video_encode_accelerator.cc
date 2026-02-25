/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#include "ohos_video_encode_accelerator.h"

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
#include "third_party/libyuv/include/libyuv/convert_from.h"
#include "ui/gl/gl_bindings.h"

namespace media {

constexpr int kOhosEncodePollDelay = 10;
constexpr OH_AVPixelFormat kDefaultPixelformat = AV_PIXEL_FORMAT_YUVI420;

static inline const base::TimeDelta EncodePollDelay() {
  return base::Milliseconds(kOhosEncodePollDelay);
}

OHOSVideoEncodeAccelerator::OHOSVideoEncodeAccelerator()
    : error_occurred_(false) {
  LOG(INFO) << __func__ << " [VideoEncoder] use hardward Video Encoder.";
}

OHOSVideoEncodeAccelerator::~OHOSVideoEncodeAccelerator() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  LOG(INFO) << __func__ << " [VideoEncoder] discard hardward Video Encoder.";
}

VideoEncodeAccelerator::SupportedProfiles OHOSVideoEncodeAccelerator::GetSupportedProfiles() {
  SupportedProfiles profiles;
  std::vector<SupportedProfile> avc_supported_profiles =
      OhosMediaCodecUtil::GetSupportedAVCEncodeProfiles();
  profiles.insert(profiles.end(), avc_supported_profiles.begin(),
                  avc_supported_profiles.end());
  return profiles;
}

EncoderStatus OHOSVideoEncodeAccelerator::Initialize(
    const Config& config,
    Client* client,
    std::unique_ptr<MediaLog> media_log) {
  LOG(INFO) << __func__
            << " [VideoEncoder] config: " << config.AsHumanReadableString();
  DCHECK(!media_codec_);
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(client);
  log_ = std::move(media_log);
  client_ptr_factory_ = std::make_unique<base::WeakPtrFactory<Client>>(client);
  std::string mime_type;
  VideoCodec codec;
  uint32_t frame_input_count;
  if (config.output_profile == H264PROFILE_BASELINE ||
      config.output_profile == H264PROFILE_MAIN ||
      config.output_profile == H264PROFILE_HIGH) {
    codec = VideoCodec::kH264;
    mime_type = "video/avc";
    frame_input_count = 1;
  } else {
    return {EncoderStatus::Codes::kEncoderInitializationError};
  }

  frame_size_ = config.input_visible_size;
  media_codec_ = OHOSMediaCodecBridgeImpl::CreateVideoEncoder(mime_type);
  if (!media_codec_) {
    LOG(ERROR) << __func__ << " [VideoEncoder] fail to create encoder";
    return {EncoderStatus::Codes::kEncoderInitializationError};
  }

  OH_AVFormat* config_format = OH_AVFormat_Create();
  setAVFormatFromConfig(config_format, config);
  if (media_codec_->Configure(*config_format,
                              base::SequencedTaskRunner::GetCurrentDefault()) !=
      CodecCodeAdapter::ENCODER_OK) {
    LOG(ERROR) << __func__ << " [VideoEncoder] fail to Configure encoder";
    return {EncoderStatus::Codes::kEncoderInitializationError};
  }
  OH_AVFormat_Destroy(config_format);

  if (media_codec_->CreateInputSurface() != CodecCodeAdapter::ENCODER_OK) {
    LOG(ERROR) << __func__ << " [VideoEncoder] fail to create input surface";
    return {EncoderStatus::Codes::kEncoderInitializationError};
  }

  if (media_codec_->Prepare() != CodecCodeAdapter::ENCODER_OK) {
    LOG(ERROR) << __func__ << " [VideoEncoder] fail to Prepare encoder";
    return {EncoderStatus::Codes::kEncoderInitializationError};
  }

  if (media_codec_->Start() != CodecCodeAdapter::ENCODER_OK) {
    LOG(ERROR) << __func__ << " [VideoEncoder] fail to Start encoder";
    return {EncoderStatus::Codes::kEncoderInitializationError};
  }

  const std::size_t output_buffer_capacity = VideoFrame::AllocationSize(
      config.input_format, config.input_visible_size);
  base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE,
      base::BindOnce(&VideoEncodeAccelerator::Client::RequireBitstreamBuffers,
                     client_ptr_factory_->GetWeakPtr(), frame_input_count,
                     config.input_visible_size, output_buffer_capacity));
  return {EncoderStatus::Codes::kOk};
}

void OHOSVideoEncodeAccelerator::setAVFormatFromConfig(
    OH_AVFormat* config_format,
    const Config& config) {
  bool result = OH_AVFormat_SetIntValue(
      config_format, OH_MD_KEY_PROFILE,
      VideoCodecProfileToOHAVCProfile(config.output_profile));
  if (!result) {
    LOG(WARNING) << __func__
                 << " [VideoEncoder] fail to set OH_MD_KEY_PROFILE to "
                 << static_cast<int>(
                        VideoCodecProfileToOHAVCProfile(config.output_profile));
  }
  result = OH_AVFormat_SetIntValue(config_format, OH_MD_KEY_WIDTH,
                                   config.input_visible_size.width());
  if (!result) {
    LOG(WARNING) << __func__
                 << " [VideoEncoder] fail to set OH_MD_KEY_WIDTH to "
                 << config.input_visible_size.width();
  }
  result = OH_AVFormat_SetIntValue(config_format, OH_MD_KEY_HEIGHT,
                                   config.input_visible_size.height());
  if (!result) {
    LOG(WARNING) << __func__
                 << " [VideoEncoder] fail to set OH_MD_KEY_HEIGHT to "
                 << config.input_visible_size.height();
  }
  result = OH_AVFormat_SetLongValue(config_format, OH_MD_KEY_BITRATE,
                                    config.bitrate.target_bps());
  if (!result) {
    LOG(WARNING) << __func__
                 << " [VideoEncoder] fail to set OH_MD_KEY_BITRATE to "
                 << config.bitrate.target_bps();
  }
  result = OH_AVFormat_SetDoubleValue(config_format, OH_MD_KEY_FRAME_RATE,
                                      config.framerate);
  if (!result) {
    LOG(WARNING) << __func__
                 << " [VideoEncoder] fail to set OH_MD_KEY_FRAME_RATE to "
                 << config.framerate;
  }
  result = OH_AVFormat_SetIntValue(config_format, OH_MD_KEY_PIXEL_FORMAT,
                                   kDefaultPixelformat);
  if (!result) {
    LOG(WARNING) << __func__
                 << " [VideoEncoder] fail to set OH_MD_KEY_PIXEL_FORMAT to "
                 << kDefaultPixelformat;
  }
  result = OH_AVFormat_SetIntValue(config_format,
                                   OH_MD_KEY_VIDEO_ENCODE_BITRATE_MODE,
                                   BitrateModeToOHBitrateMode(config.bitrate));
  if (!result) {
    LOG(WARNING) << __func__
                 << " [VideoEncoder] fail to set "
                    "OH_MD_KEY_VIDEO_ENCODE_BITRATE_MODE to "
                 << static_cast<int>(
                        BitrateModeToOHBitrateMode(config.bitrate));
  }
}

OH_VideoEncodeBitrateMode OHOSVideoEncodeAccelerator::BitrateModeToOHBitrateMode(Bitrate bitrate) {
  switch (bitrate.mode()) {
    case Bitrate::Mode::kConstant:
      return OH_VideoEncodeBitrateMode::CBR;
    case Bitrate::Mode::kVariable:
      return OH_VideoEncodeBitrateMode::VBR;
    case Bitrate::Mode::kExternal:
    default:
      LOG(ERROR) << __func__ << " [VideoEncoder] unsupported bitrate mode: "
                 << static_cast<int>(bitrate.mode());
      return OH_VideoEncodeBitrateMode::CBR;
  }
}

OH_AVCProfile OHOSVideoEncodeAccelerator::VideoCodecProfileToOHAVCProfile(
    VideoCodecProfile profile) {
  switch (profile) {
    case H264PROFILE_BASELINE:
      return OH_AVCProfile::AVC_PROFILE_BASELINE;
    case H264PROFILE_MAIN:
      return OH_AVCProfile::AVC_PROFILE_MAIN;
    case H264PROFILE_HIGH:
      return OH_AVCProfile::AVC_PROFILE_HIGH;
    default:
      LOG(ERROR) << __func__ << " [VideoEncoder] unsupported AVC profile: "
                 << static_cast<int>(profile);
      return OH_AVCProfile::AVC_PROFILE_BASELINE;
  }
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
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  available_bitstream_buffers_.push_back(std::move(buffer));
  DoIOTask();
}

void OHOSVideoEncodeAccelerator::RequestEncodingParametersChange(
    const Bitrate& bitrate,
    uint32_t framerate,
    const std::optional<gfx::Size>& size) {
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
  if (error_occurred_) {
    LOG(WARNING) << __func__ << " [VideoEncoder] error_occurred.";
    return;
  }
  if (pending_frames_.empty()) {
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
      CodecCodeAdapter::ENCODER_OK) {
    return;
  }
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
  OH_AVCodecBufferFlags flag;
  OhosBuffer buffer;
  CodecCodeAdapter ret =
      media_codec_->DequeueOutputBuffer(index, info, flag, buffer);

  switch (ret) {
    case CodecCodeAdapter::ENCODER_RETRY:
      return;
    case CodecCodeAdapter::ENCODER_ERROR:
      NotifyErrorStatus({EncoderStatus::Codes::kEncoderFailedEncode,
                         "MediaCodec error in DequeueOutputBuffer"});
      // Unreachable because of previous statement, but included for clarity.
      return;
    case CodecCodeAdapter::ENCODER_OK:
      DCHECK_GE(index, 0);
      break;
    default:
      NOTREACHED();
      break;
  }
  base::TimeDelta frame_timestamp;
  base::TimeDelta presentaion_timestamp =
      base::Microseconds(info.presentation_time_us);
  const auto it = frame_timestamp_map_.find(presentaion_timestamp);
  if (it == frame_timestamp_map_.end()) {
    LOG(WARNING) << __func__
                 << " [VideoEncoder] DequeueOutput can not find timestamp "
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
  if (static_cast<std::size_t>(info.size) > bitstream_buffer.size()) {
    NotifyErrorStatus(
        {EncoderStatus::Codes::kEncoderFailedEncode,
         "Encoded buffer too large: " + base::NumberToString(info.size) + ">" +
             base::NumberToString(bitstream_buffer.size())});
    return;
  }

  memcpy(mapping.memory(), buffer.addr, buffer.buffer_size);
  bool key_frame =
      (flag == OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_SYNC_FRAME);
  if (key_frame) {
    media_codec_->ClearKeyFrameCache();
  }

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
  LOG(ERROR) << __func__ << " [VideoEncoder] Call NotifyErrorStatus(): code="
             << static_cast<int>(status.code())
             << ", message=" << status.message();
  if (!error_occurred_) {
    client_ptr_factory_->GetWeakPtr()->NotifyErrorStatus(status);
    error_occurred_ = true;
  }
}
}  // namespace media
