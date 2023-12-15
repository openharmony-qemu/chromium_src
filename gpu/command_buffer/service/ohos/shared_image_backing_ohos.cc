// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/ohos/shared_image_backing_ohos.h"

#include "base/logging.h"
#include "base/posix/eintr_wrapper.h"
#include "ui/gl/gl_utils.h"

namespace gpu {

SharedImageBackingOhos::SharedImageBackingOhos(
    const Mailbox& mailbox,
    viz::SharedImageFormat format,
    const gfx::Size& size,
    const gfx::ColorSpace& color_space,
    GrSurfaceOrigin surface_origin,
    SkAlphaType alpha_type,
    uint32_t usage,
    size_t estimated_size,
    bool is_thread_safe)
    : ClearTrackingSharedImageBacking(mailbox,
                                      format,
                                      size,
                                      color_space,
                                      surface_origin,
                                      alpha_type,
                                      usage,
                                      estimated_size,
                                      is_thread_safe) {}

SharedImageBackingOhos::~SharedImageBackingOhos() = default;

}  // namespace gpu
