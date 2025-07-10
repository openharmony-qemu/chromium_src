/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "base/check.h"
#include "base/containers/contains.h"
#include "base/notreached.h"
#include "build/build_config.h"
#include "gpu/command_buffer/common/capabilities.h"
#include "ui/gfx/buffer_format_util.h"
#include "ui/gfx/geometry/size.h"
#include "arkweb/chromium_ext/gpu/command_buffer/common/gpu_memory_buffer_support_utils.h"
#include "gtest/gtest.h"
#include "ui/gfx/geometry/size.h"

namespace gpu {
namespace {

TEST(GpuMemoryBufferSupportUtilsTest, GetPlaneSize) {
    gfx::Size size(100, 100);

    EXPECT_EQ(GetPlaneSize(gfx::BufferPlane::DEFAULT, size), gfx::Size(100, 100));
    EXPECT_EQ(GetPlaneSize(gfx::BufferPlane::Y, size), gfx::Size(100, 100));
    EXPECT_EQ(GetPlaneSize(gfx::BufferPlane::A, size), gfx::Size(100, 100));
    EXPECT_EQ(GetPlaneSize(gfx::BufferPlane::U, size), gfx::Size(50, 50));
    EXPECT_EQ(GetPlaneSize(gfx::BufferPlane::V, size), gfx::Size(50, 50));
    EXPECT_EQ(GetPlaneSize(gfx::BufferPlane::UV, size), gfx::Size(50, 50));
}

TEST(GpuMemoryBufferSupportUtilsTest, GetPlaneBufferFormat) {
    EXPECT_EQ(GetPlaneBufferFormat(gfx::BufferPlane::Y, gfx::BufferFormat::YVU_420),
              gfx::BufferFormat::R_8);
    EXPECT_EQ(GetPlaneBufferFormat(gfx::BufferPlane::Y, gfx::BufferFormat::YUV_420_BIPLANAR),
              gfx::BufferFormat::R_8);
    EXPECT_EQ(GetPlaneBufferFormat(gfx::BufferPlane::Y, gfx::BufferFormat::YUVA_420_TRIPLANAR),
              gfx::BufferFormat::R_8);
    EXPECT_EQ(GetPlaneBufferFormat(gfx::BufferPlane::Y, gfx::BufferFormat::P010),
              gfx::BufferFormat::R_16);
    
    EXPECT_EQ(GetPlaneBufferFormat(gfx::BufferPlane::UV, gfx::BufferFormat::YUV_420_BIPLANAR),
              gfx::BufferFormat::RG_88);
    EXPECT_EQ(GetPlaneBufferFormat(gfx::BufferPlane::UV, gfx::BufferFormat::YUVA_420_TRIPLANAR),
              gfx::BufferFormat::RG_88);
    EXPECT_EQ(GetPlaneBufferFormat(gfx::BufferPlane::UV, gfx::BufferFormat::P010),
              gfx::BufferFormat::RG_1616);
    
    EXPECT_EQ(GetPlaneBufferFormat(gfx::BufferPlane::U, gfx::BufferFormat::YVU_420),
              gfx::BufferFormat::R_8);
    EXPECT_EQ(GetPlaneBufferFormat(gfx::BufferPlane::V, gfx::BufferFormat::YVU_420),
              gfx::BufferFormat::R_8);
    
    EXPECT_EQ(GetPlaneBufferFormat(gfx::BufferPlane::A, gfx::BufferFormat::YUVA_420_TRIPLANAR),
              gfx::BufferFormat::R_8);
    
    EXPECT_EQ(GetPlaneBufferFormat(gfx::BufferPlane::DEFAULT, gfx::BufferFormat::BGRA_8888),
              gfx::BufferFormat::RGBA_8888);
    
    EXPECT_DEATH(GetPlaneBufferFormat(gfx::BufferPlane::Y, gfx::BufferFormat::BGRA_8888), "");
    EXPECT_DEATH(GetPlaneBufferFormat(gfx::BufferPlane::UV, gfx::BufferFormat::BGRA_8888), "");
}

}
}