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

#ifndef GPU_COMMAND_BUFFER_SERVICE_OHOS_SHARED_IMAGE_BACKING_OHOS_H_
#define GPU_COMMAND_BUFFER_SERVICE_OHOS_SHARED_IMAGE_BACKING_OHOS_H_

#include "base/containers/flat_set.h"
#include "base/files/scoped_file.h"
#include "gpu/command_buffer/service/shared_image/shared_image_backing.h"

namespace gpu {

class SharedImageBackingOhos : public ClearTrackingSharedImageBacking {
 public:
  SharedImageBackingOhos(const Mailbox& mailbox,
                         viz::SharedImageFormat format,
                         const gfx::Size& size,
                         const gfx::ColorSpace& color_space,
                         GrSurfaceOrigin surface_origin,
                         SkAlphaType alpha_type,
                         uint32_t usage,
                         std::string debug_label,
                         size_t estimated_size,
                         bool is_thread_safe);

  ~SharedImageBackingOhos() override;
  SharedImageBackingOhos(const SharedImageBackingOhos&) = delete;
  SharedImageBackingOhos& operator=(const SharedImageBackingOhos&) = delete;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_OHOS_SHARED_IMAGE_BACKING_OHOS_H_
