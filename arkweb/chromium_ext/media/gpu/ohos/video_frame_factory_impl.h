// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on video_frame_factory_impl.h originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#ifndef MEDIA_GPU_OHOS_VIDEO_FRAME_FACTORY_IMPL_H_
#define MEDIA_GPU_OHOS_VIDEO_FRAME_FACTORY_IMPL_H_

#include <memory>

#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/memory/weak_ptr.h"
#include "base/system/sys_info.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "gpu/config/gpu_finch_features.h"
#include "gpu/config/gpu_preferences.h"
#include "media/base/video_frame.h"
#include "media/gpu/media_gpu_export.h"
#include "media/gpu/ohos/codec_buffer_wait_coordinator.h"
#include "media/gpu/ohos/codec_image.h"
#include "media/gpu/ohos/codec_wrapper.h"
#include "media/gpu/ohos/frame_info_helper.h"
#include "media/gpu/ohos/shared_image_video_provider.h"
#include "media/gpu/ohos/video_frame_factory.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/gl/gl_bindings.h"

namespace media {
class CodecImageGroup;
class MaybeRenderEarlyManager;

class MEDIA_GPU_EXPORT VideoFrameFactoryImpl
    : public VideoFrameFactory,
      public gpu::RefCountedLockHelperDrDc {
 public:
  using ImageReadyCB =
      base::OnceCallback<void(gpu::Mailbox mailbox,
                              VideoFrame::ReleaseMailboxCB release_cb)>;

  using ImageWithInfoReadyCB =
      base::OnceCallback<void(std::unique_ptr<CodecOutputBufferRenderer>,
                              FrameInfoHelper::FrameInfo,
                              SharedImageVideoProvider::ImageRecord)>;

  VideoFrameFactoryImpl(
      scoped_refptr<base::SingleThreadTaskRunner> gpu_task_runner,
      const gpu::GpuPreferences& gpu_preferences,
      std::unique_ptr<SharedImageVideoProvider> image_provider,
      std::unique_ptr<FrameInfoHelper> frame_info_helper,
      scoped_refptr<gpu::RefCountedLock> drdc_lock);

  VideoFrameFactoryImpl(const VideoFrameFactoryImpl&) = delete;
  VideoFrameFactoryImpl& operator=(const VideoFrameFactoryImpl&) = delete;

  ~VideoFrameFactoryImpl() override;

  void Initialize(InitCB init_cb) override;
  void SetSurfaceBundle(
      scoped_refptr<CodecSurfaceBundle> surface_bundle) override;
  void CreateVideoFrame(std::unique_ptr<CodecOutputBuffer> output_buffer,
                        base::TimeDelta timestamp,
                        gfx::Size natural_size,
                        OnceOutputCB output_cb) override;
  void RunAfterPendingVideoFrames(base::OnceClosure closure) override;
#if BUILDFLAG(ARKWEB_PIP)
  void PipEnable(bool enable) override;
  bool IsPipEnable();
#endif
 private:
  void RequestImage(std::unique_ptr<CodecOutputBufferRenderer> buffer_renderer,
                    ImageWithInfoReadyCB image_ready_cb);

  static void CreateVideoFrame_OnImageReady(
      base::WeakPtr<VideoFrameFactoryImpl> thiz,
      OnceOutputCB output_cb,
      base::TimeDelta timestamp,
      gfx::Size natural_size,
      bool is_texture_owner_backed,
      VideoPixelFormat pixel_format,
      bool video_frame_copy_required,
      scoped_refptr<base::SequencedTaskRunner> gpu_task_runner,
      std::unique_ptr<CodecOutputBufferRenderer> output_buffer_renderer,
      FrameInfoHelper::FrameInfo frame_info,
      SharedImageVideoProvider::ImageRecord record);

  void CreateVideoFrame_OnFrameInfoReady(
      ImageWithInfoReadyCB image_ready_cb,
      std::unique_ptr<CodecOutputBufferRenderer> output_buffer_renderer,
      FrameInfoHelper::FrameInfo frame_info);

  std::unique_ptr<SharedImageVideoProvider> image_provider_;
  scoped_refptr<base::SingleThreadTaskRunner> gpu_task_runner_;

  scoped_refptr<CodecBufferWaitCoordinator> codec_buffer_wait_coordinator_;

  bool video_frame_copy_required_ = features::IsUsingVulkan() ||
                                    base::ohos::IsEmulator() ||
#if BUILDFLAG(ARKWEB_PIP)
                                    IsPipEnable() ||
#endif
                                    base::SysInfo::IsLowEndDevice();

  std::unique_ptr<FrameInfoHelper> frame_info_helper_;

  SharedImageVideoProvider::ImageSpec image_spec_;

  SEQUENCE_CHECKER(sequence_checker_);

  base::WeakPtrFactory<VideoFrameFactoryImpl> weak_factory_{this};
#if BUILDFLAG(ARKWEB_PIP)
  bool pip_enable_ = false;
#endif
};

}  // namespace media

#endif  // MEDIA_GPU_OHOS_VIDEO_FRAME_FACTORY_IMPL_H_
