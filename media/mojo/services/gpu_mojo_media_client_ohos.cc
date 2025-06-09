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

#include "media/mojo/services/gpu_mojo_media_client.h"

#include "base/memory/ptr_util.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/config/gpu_finch_features.h"
#include "media/base/media_log.h"
#include "media/base/media_switches.h"
#include "media/gpu/ohos/codec_allocator.h"
#include "media/gpu/ohos/direct_shared_image_video_provider.h"
#include "media/gpu/ohos/ohos_video_decoder.h"
#include "media/gpu/ohos/video_frame_factory_impl.h"
#include "media/mojo/mojom/provision_fetcher.mojom.h"
#include "media/mojo/services/mojo_media_drm_storage.h"
#include "media/mojo/services/mojo_provision_fetcher.h"
namespace media {

class GpuMojoMediaClientOHOS final : public GpuMojoMediaClient {
 public:
  GpuMojoMediaClientOHOS(GpuMojoMediaClientTraits& traits)
      : GpuMojoMediaClient(traits) {}
  ~GpuMojoMediaClientOHOS() final = default;

 protected:
  std::unique_ptr<VideoDecoder> CreatePlatformVideoDecoder(
    VideoDecoderTraits& traits) {
  LOG(INFO) << "CreatePlatformVideoDecoder";
  scoped_refptr<gpu::RefCountedLock> ref_counted_lock;
  ref_counted_lock = base::MakeRefCounted<gpu::RefCountedLock>();

  std::unique_ptr<SharedImageVideoProvider> image_provider =
      std::make_unique<DirectSharedImageVideoProvider>(
          gpu_task_runner_, traits.get_command_buffer_stub_cb,
          ref_counted_lock);

  auto frame_info_helper = FrameInfoHelper::Create(
      gpu_task_runner_, traits.get_command_buffer_stub_cb,
      ref_counted_lock);

  return OhosVideoDecoder::Create(
      gpu_preferences_,  gpu_feature_info_,
      traits.media_log->Clone(),
      CodecAllocator::GetInstance(gpu_task_runner_),
      std::make_unique<VideoFrameFactoryImpl>(
          gpu_task_runner_, gpu_preferences_,
          std::move(image_provider), std::move(frame_info_helper),
          ref_counted_lock),
      ref_counted_lock);
}

  std::optional<SupportedVideoDecoderConfigs>
  GetPlatformSupportedVideoDecoderConfigs() final {
    LOG(INFO) << "GetPlatformSupportedVideoDecoderConfigs";
    return OhosVideoDecoder::GetSupportedConfigs();
  }

  VideoDecoderType GetPlatformDecoderImplementationType() final {
    return VideoDecoderType::kOHOS;
  }
};

std::unique_ptr<GpuMojoMediaClient> CreateGpuMediaService(
    GpuMojoMediaClientTraits& traits) {
  return std::make_unique<GpuMojoMediaClientOHOS>(traits);
}

}  // namespace media
