// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on gpu_mojo_media_client_android.cc originally written by
// Copyright 2021 The Chromium Authors. All rights reserved.
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
#if BUILDFLAG(IS_OHOS) && defined(OHOS_ENABLE_CDM)
#include "media/filters/ohos/ohos_audio_decoder.h"
#endif

#include "media/mojo/mojom/media_drm_storage.mojom.h"
#include "media/mojo/mojom/provision_fetcher.mojom.h"
#include "media/mojo/services/mojo_media_drm_storage.h"
#include "media/mojo/services/mojo_provision_fetcher.h"
#if BUILDFLAG(IS_OHOS) && defined(OHOS_ENABLE_CDM)
#include "media/base/ohos/ohos_cdm_factory.h"
#include "media/mojo/services/ohos_mojo_util.h"
#endif

#if BUILDFLAG(IS_OHOS) && defined(OHOS_ENABLE_CDM)
using media::ohos_mojo_util::CreateMediaDrmStorage;
using media::ohos_mojo_util::CreateProvisionFetcher;
#endif
namespace media {

std::unique_ptr<VideoDecoder> CreatePlatformVideoDecoder(
    VideoDecoderTraits& traits) {
  LOG(INFO) << "CreatePlatformVideoDecoder";
#if BUILDFLAG(ENABLE_HEIF_DECODER)
  scoped_refptr<gpu::RefCountedLock> ref_counted_lock;
  ref_counted_lock = base::MakeRefCounted<gpu::RefCountedLock>();

  std::unique_ptr<SharedImageVideoProvider> image_provider =
      std::make_unique<DirectSharedImageVideoProvider>(
          traits.gpu_task_runner, traits.get_command_buffer_stub_cb,
          ref_counted_lock);

  auto frame_info_helper = FrameInfoHelper::Create(
      traits.gpu_task_runner, traits.get_command_buffer_stub_cb,
      ref_counted_lock);

  return OhosVideoDecoder::Create(
      traits.gpu_preferences, traits.gpu_feature_info,
      traits.media_log->Clone(),
      CodecAllocator::GetInstance(traits.gpu_task_runner),
      std::make_unique<VideoFrameFactoryImpl>(
          traits.gpu_task_runner, traits.gpu_preferences,
          std::move(image_provider), std::move(frame_info_helper),
          ref_counted_lock),
      ref_counted_lock);
#else
  return nullptr;
#endif
}

absl::optional<SupportedVideoDecoderConfigs> GetPlatformSupportedVideoDecoderConfigs(
    gpu::GpuDriverBugWorkarounds gpu_workarounds,
    gpu::GpuPreferences gpu_preferences,
    const gpu::GPUInfo& gpu_info,
    base::OnceCallback<SupportedVideoDecoderConfigs()> get_vda_configs) {
  LOG(INFO) << "GetPlatformSupportedVideoDecoderConfigs";
#if BUILDFLAG(ENABLE_HEIF_DECODER)
  return OhosVideoDecoder::GetSupportedConfigs();
#else
  return std::vector<SupportedVideoDecoderConfig>();
#endif
}

// Not support platform audio decoder in ohos now.
std::unique_ptr<AudioDecoder> CreatePlatformAudioDecoder(
    scoped_refptr<base::SequencedTaskRunner> task_runner,
    std::unique_ptr<MediaLog> media_log) {
  return nullptr;
}

std::unique_ptr<AudioEncoder> CreatePlatformAudioEncoder(
    scoped_refptr<base::SequencedTaskRunner> task_runner) {
#if BUILDFLAG(IS_OHOS) && defined(OHOS_ENABLE_CDM)
  return std::make_unique<OHOSAudioDecoder>(std::move(task_runner));
#else
  return nullptr;
#endif
}

VideoDecoderType GetPlatformDecoderImplementationType(
    gpu::GpuDriverBugWorkarounds gpu_workarounds,
    gpu::GpuPreferences gpu_preferences,
    const gpu::GPUInfo& gpu_info) {
  return VideoDecoderType::kOHOS;
}

#if BUILDFLAG(IS_OHOS) && defined(OHOS_ENABLE_CDM)
std::unique_ptr<CdmFactory> CreatePlatformCdmFactory(
    mojom::FrameInterfaceFactory* frame_interfaces) {
  LOG(INFO) << "[DRM]" << __func__;
  return std::make_unique<OHOSCdmFactory>(
    base::BindRepeating(&CreateProvisionFetcher, frame_interfaces),
    base::BindRepeating(&CreateMediaDrmStorage, frame_interfaces));
}
#else
// There is no CdmFactory on ohos now.
class CdmFactory {};
std::unique_ptr<CdmFactory> CreatePlatformCdmFactory(
    mojom::FrameInterfaceFactory* frame_interfaces) {
  return nullptr;
}
#endif

}  // namespace media