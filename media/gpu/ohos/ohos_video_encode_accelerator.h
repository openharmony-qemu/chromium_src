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

#ifndef MEDIA_GPU_OHOS_ANDROID_VIDEO_ENCODE_ACCELERATOR_H_
#define MEDIA_GPU_OHOS_ANDROID_VIDEO_ENCODE_ACCELERATOR_H_

#include <cstddef>
#include <cstdint>

#include <map>
#include <memory>
#include <tuple>
#include <vector>

#include "base/containers/queue.h"
#include "base/memory/weak_ptr.h"
#include "base/sequence_checker.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "media/base/bitrate.h"
#include "media/base/ohos/ohos_media_codec_bridge.h"
#include "media/base/ohos/ohos_media_codec_bridge_impl.h"
#include "media/base/ohos/ohos_media_codec_util.h"
#include "media/gpu/media_gpu_export.h"
#include "media/video/video_encode_accelerator.h"

namespace media {
class BitstreamBuffer;

class MEDIA_GPU_EXPORT OHOSVideoEncodeAccelerator
    : public VideoEncodeAccelerator {
 public:
  OHOSVideoEncodeAccelerator();

  OHOSVideoEncodeAccelerator(const OHOSVideoEncodeAccelerator&) = delete;
  OHOSVideoEncodeAccelerator& operator=(const OHOSVideoEncodeAccelerator&) =
      delete;

  ~OHOSVideoEncodeAccelerator() override;

  // VideoEncodeAccelerator implementation.
  VideoEncodeAccelerator::SupportedProfiles GetSupportedProfiles() override;
  EncoderStatus Initialize(const Config& config,
                  Client* client,
                  std::unique_ptr<MediaLog> media_log) override;
  OH_VideoEncodeBitrateMode BitrateModeToOHBitrateMode(Bitrate bitrate);
  OH_AVCProfile VideoCodecProfileToOHAVCProfile(VideoCodecProfile profile);

  void Encode(scoped_refptr<VideoFrame> frame, bool force_keyframe) override;
  void UseOutputBitstreamBuffer(BitstreamBuffer buffer) override;
  void RequestEncodingParametersChange(
      const Bitrate& bitrate,
      uint32_t framerate,
      const std::optional<gfx::Size>& size) override;
  void Destroy() override;

 private:
  enum {
    // Arbitrary choice.
    INITIAL_FRAMERATE = 30,
    // Default I-Frame interval in seconds.
    IFRAME_INTERVAL_H264 = 20,
    IFRAME_INTERVAL_VPX = 100,
    IFRAME_INTERVAL = INT32_MAX,
  };
  void DoIOTask();
  void QueueInput();
  void DequeueOutput();
  void MaybeStartIOTimer();
  void MaybeStopIOTimer();
  void NotifyErrorStatus(EncoderStatus status);
  void setAVFormatFromConfig(OH_AVFormat* config_format, const Config& config);

  SEQUENCE_CHECKER(sequence_checker_);

  std::unique_ptr<base::WeakPtrFactory<Client>> client_ptr_factory_;
  std::unique_ptr<OHOSMediaCodecBridge> media_codec_;
  using PendingFrames =
      base::queue<std::tuple<scoped_refptr<VideoFrame>, bool, base::Time>>;
  PendingFrames pending_frames_;
  gfx::Size frame_size_;
  std::vector<BitstreamBuffer> available_bitstream_buffers_;
  std::unique_ptr<MediaLog> log_;
  // The difference between number of buffers queued & dequeued at the codec.
  int32_t num_buffers_at_codec_ = 0;

  // A monotonically-growing value.
  base::TimeDelta presentation_timestamp_;

  std::map<base::TimeDelta /* presentation_timestamp */,
           base::TimeDelta /* frame_timestamp */>
      frame_timestamp_map_;

  // Repeating timer responsible for draining pending IO to the codec.
  base::RepeatingTimer io_timer_;

  // True if there is encoder error.
  bool error_occurred_ = false;
  base::TimeDelta frame_timestamp_last_;
};

}  // namespace media

#endif  // MEDIA_GPU_OHOS_ANDROID_VIDEO_ENCODE_ACCELERATOR_H_
