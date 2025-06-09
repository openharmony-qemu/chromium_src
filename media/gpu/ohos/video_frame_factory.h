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

#ifndef MEDIA_GPU_OHOS_VIDEO_FRAME_FACTORY_H_
#define MEDIA_GPU_OHOS_VIDEO_FRAME_FACTORY_H_

#include <memory>

#include "base/memory/ref_counted.h"
#include "base/task/single_thread_task_runner.h"
#include "base/time/time.h"
#include "media/base/video_decoder.h"
#include "media/gpu/media_gpu_export.h"
#include "ui/gfx/geometry/size.h"

namespace media {

class CodecOutputBuffer;
class CodecSurfaceBundle;
class VideoFrame;

class MEDIA_GPU_EXPORT VideoFrameFactory {
 public:
  using InitCB = base::RepeatingCallback<void(
      scoped_refptr<gpu::NativeImageTextureOwner>)>;
  using OnceOutputCB = base::OnceCallback<void(scoped_refptr<VideoFrame>)>;

  VideoFrameFactory() = default;
  virtual ~VideoFrameFactory() = default;

  virtual void Initialize(InitCB init_cb) = 0;

  virtual void SetSurfaceBundle(
      scoped_refptr<CodecSurfaceBundle> surface_bundle) = 0;

  virtual void CreateVideoFrame(
      std::unique_ptr<CodecOutputBuffer> output_buffer,
      base::TimeDelta timestamp,
      gfx::Size natural_size,
      OnceOutputCB output_cb) = 0;

  virtual void RunAfterPendingVideoFrames(base::OnceClosure closure) = 0;
};

}  // namespace media

#endif  // MEDIA_GPU_OHOS_VIDEO_FRAME_FACTORY_H_
