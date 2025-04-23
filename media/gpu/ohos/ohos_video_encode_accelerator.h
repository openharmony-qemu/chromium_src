// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_GPU_OHOS_ANDROID_VIDEO_ENCODE_ACCELERATOR_H_
#define MEDIA_GPU_OHOS_ANDROID_VIDEO_ENCODE_ACCELERATOR_H_

#include <stddef.h>
#include <stdint.h>

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
#include "media_codec_adapter.h"

namespace media {
using namespace OHOS::NWeb;
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
  bool Initialize(const Config& config,
                  Client* client,
                  std::unique_ptr<MediaLog> media_log) override;
  void Encode(scoped_refptr<VideoFrame> frame, bool force_keyframe) override;
  void UseOutputBitstreamBuffer(BitstreamBuffer buffer) override;
  void RequestEncodingParametersChange(const Bitrate& bitrate,
                                       uint32_t framerate) override;
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

  SEQUENCE_CHECKER(sequence_checker_);

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
  std::unique_ptr<base::WeakPtrFactory<Client>> client_ptr_factory_;
};

}  // namespace media

#endif  // MEDIA_GPU_OHOS_ANDROID_VIDEO_ENCODE_ACCELERATOR_H_
