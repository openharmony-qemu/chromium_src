// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/check.h"
#include "base/containers/contains.h"
#include "base/notreached.h"
#include "build/build_config.h"
#include "gpu/command_buffer/common/capabilities.h"
#include "ui/gfx/buffer_format_util.h"
#include "ui/gfx/geometry/size.h"
#include "arkweb/chromium_ext/gpu/command_buffer/common/gpu_memory_buffer_support_utils.h"

namespace gpu {
    
gfx::Size GetPlaneSize(gfx::BufferPlane plane, const gfx::Size& size) {
    switch (plane) {
        case gfx::BufferPlane::DEFAULT:
        case gfx::BufferPlane::Y:
        case gfx::BufferPlane::A:
            return size;
        case gfx::BufferPlane::U:
        case gfx::BufferPlane::V:
        case gfx::BufferPlane::UV:
            return gfx::ScaleToCeiledSize(size, 0.5);
    }
}

gfx::BufferFormat GetPlaneBufferFormat(gfx::BufferPlane plane,
                                       gfx::BufferFormat format) {
    switch (plane) {
        case gfx::BufferPlane::DEFAULT:
            return format;
        case gfx::BufferPlane::Y:
            if (format == gfx::BufferFormat::YVU_420 || format == gfx::BufferFormat::YUV_420_BIPLANAR ||
                format == gfx::BufferFormat::YUVA_420_TRIPLANAR) {
                return gfx::BufferFormat::R_8;
            }
            if (format == gfx::BufferFormat::P010) {
                return gfx::BufferFormat::R_16;
            }
            break;
        case gfx::BufferPlane::UV:
            if (format == gfx::BufferFormat::YUV_420_BIPLANAR || format == gfx::BufferFormat::YUVA_420_TRIPLANAR) {
                return gfx::BufferFormat::RG_88;
            }
            if (format == gfx::BufferFormat::P010) {
                return gfx::BufferFormat::RG_1616;
            }
            break;
        case gfx::BufferPlane::U:
            if (format == gfx::BufferFormat::YVU_420) {
                return gfx::BufferFormat::R_8;
            }
            break;
        case gfx::BufferPlane::V:
            if (format == gfx::BufferFormat::YVU_420) {
                return gfx::BufferFormat::R_8;
            }
            break;
        case gfx::BufferPlane::A:
            if (format == gfx::BufferFormat::YUVA_420_TRIPLANAR) {
                return gfx::BufferFormat::R_8;
            }
            break;
    }

  NOTREACHED();
}

}