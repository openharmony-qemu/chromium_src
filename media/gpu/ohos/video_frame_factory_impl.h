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

#ifndef MEDIA_GPU_OHOS_VIDEO_FRAME_FACTORY_IMPL_H_
#define MEDIA_GPU_OHOS_VIDEO_FRAME_FACTORY_IMPL_H_

#include <memory>

#include "base/memory/weak_ptr.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
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

  bool video_frame_copy_required_ = true;

  std::unique_ptr<FrameInfoHelper> frame_info_helper_;

  SharedImageVideoProvider::ImageSpec image_spec_;

  SEQUENCE_CHECKER(sequence_checker_);

  base::WeakPtrFactory<VideoFrameFactoryImpl> weak_factory_{this};
};

}  // namespace media

#endif  // MEDIA_GPU_OHOS_VIDEO_FRAME_FACTORY_IMPL_H_
