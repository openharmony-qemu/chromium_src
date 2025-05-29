// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_COMMON_GPU_MEMORY_BUFFER_SUPPORT_UTILS_H_
#define GPU_COMMAND_BUFFER_COMMON_GPU_MEMORY_BUFFER_SUPPORT_UTILS_H_

#include "base/containers/enum_set.h"
#include "build/build_config.h"
#include "gpu/gpu_export.h"
#include "ui/gfx/buffer_types.h"
#include "ui/gfx/geometry/size.h"

namespace gpu {

// Return the size for |plane| with image |size|. E.g, for the Y plane of
// YUV_420_BIPLANAR, return size subsampled by a factor of 2. Assumes
// IsPlaneValidForGpuMemoryBufferFormat returns true for the provided arguments.
GPU_EXPORT gfx::Size GetPlaneSize(gfx::BufferPlane plane,
                                  const gfx::Size& size);

// Return the buffer format for |plane| of |format|. E.g, for the Y plane of
// YUV_420_BIPLANAR, return R_8. Assumes IsPlaneValidForGpuMemoryBufferFormat
// returns true for the provided arguments.
GPU_EXPORT gfx::BufferFormat GetPlaneBufferFormat(gfx::BufferPlane plane,
                                                  gfx::BufferFormat format);

}

#endif // GPU_COMMAND_BUFFER_COMMON_GPU_MEMORY_BUFFER_SUPPORT_UTILS_H_