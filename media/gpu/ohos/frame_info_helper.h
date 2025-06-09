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

#ifndef MEDIA_GPU_OHOS_FRAME_INFO_HELPER_H_
#define MEDIA_GPU_OHOS_FRAME_INFO_HELPER_H_

#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "media/gpu/media_gpu_export.h"
#include "media/gpu/ohos/shared_image_video_provider.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/gfx/geometry/rect.h"

namespace media {
class CodecOutputBufferRenderer;

class MEDIA_GPU_EXPORT FrameInfoHelper {
 public:
  struct FrameInfo {
    FrameInfo();
    ~FrameInfo();

    FrameInfo(FrameInfo&&);
    FrameInfo(const FrameInfo&);
    FrameInfo& operator=(const FrameInfo&);

    gfx::Size coded_size;
    gfx::Rect visible_rect;
    absl::optional<gpu::VulkanYCbCrInfo> ycbcr_info;
  };

  using FrameInfoReadyCB =
      base::OnceCallback<void(std::unique_ptr<CodecOutputBufferRenderer>,
                              FrameInfo)>;

  static std::unique_ptr<FrameInfoHelper> Create(
      scoped_refptr<base::SequencedTaskRunner> gpu_task_runner,
      SharedImageVideoProvider::GetStubCB get_stub_cb,
      scoped_refptr<gpu::RefCountedLock> drdc_lock);

  virtual ~FrameInfoHelper() = default;

  virtual void GetFrameInfo(
      std::unique_ptr<CodecOutputBufferRenderer> buffer_renderer,
      FrameInfoReadyCB callback) = 0;

 protected:
  FrameInfoHelper() = default;
};

}  // namespace media

#endif  // MEDIA_GPU_OHOS_FRAME_INFO_HELPER_H_
