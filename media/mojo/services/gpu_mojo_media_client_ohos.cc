// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

#if BUILDFLAG(ENABLE_WISEPLAY)
#include "media/filters/ohos/ohos_audio_decoder.h"
#include "media/base/ohos/ohos_cdm_factory.h"
#include "media/mojo/services/ohos_mojo_util.h"
#endif

#include "media/mojo/mojom/media_drm_storage.mojom.h"
#include "media/mojo/mojom/provision_fetcher.mojom.h"
#include "media/mojo/services/mojo_media_drm_storage.h"
#include "media/mojo/services/mojo_provision_fetcher.h"

#if BUILDFLAG(ENABLE_WISEPLAY)
using media::ohos_mojo_util::CreateMediaDrmStorage;
using media::ohos_mojo_util::CreateProvisionFetcher;
#endif

namespace media {

class GpuMojoMediaClientOHOS final : public GpuMojoMediaClient {
 public:
  GpuMojoMediaClientOHOS(GpuMojoMediaClientTraits& traits)
      : GpuMojoMediaClient(traits) {}
  ~GpuMojoMediaClientOHOS() final = default;

 protected:
  std::unique_ptr<VideoDecoder> CreatePlatformVideoDecoder(
    VideoDecoderTraits& traits) final {
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

  // Not support platform audio decoder in ohos now.
  std::unique_ptr<AudioDecoder> CreatePlatformAudioDecoder(
      scoped_refptr<base::SequencedTaskRunner> task_runner,
      std::unique_ptr<MediaLog> media_log) final {
#if BUILDFLAG(ENABLE_WISEPLAY)
    return std::make_unique<OhosAudioDecoder>(std::move(task_runner),
                                              std::move(media_log));
#else
    return nullptr;
#endif
  }

  std::unique_ptr<AudioEncoder> CreatePlatformAudioEncoder(
    scoped_refptr<base::SequencedTaskRunner> task_runner) final {
  return nullptr;
}

std::unique_ptr<CdmFactory> CreatePlatformCdmFactory(
    mojom::FrameInterfaceFactory* frame_interfaces) final {
#if BUILDFLAG(ENABLE_WISEPLAY)
  return std::make_unique<OhosCdmFactory>(
      base::BindRepeating(&CreateProvisionFetcher, frame_interfaces),
      base::BindRepeating(&CreateMediaDrmStorage, frame_interfaces));
#else
  return nullptr;
#endif  // BUILDFLAG(ENABLE_WISEPLAY)
}
};

std::unique_ptr<GpuMojoMediaClient> CreateGpuMediaService(
    GpuMojoMediaClientTraits& traits) {
  return std::make_unique<GpuMojoMediaClientOHOS>(traits);
}

}  // namespace media
