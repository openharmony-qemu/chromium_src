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

#include "arkweb/chromium_ext/components/viz/service/display_embedder/skia_output_surface_impl_on_gpu_utils.h"
#include "components/viz/service/display_embedder/skia_output_surface_impl_on_gpu.h"

namespace viz {

SkiaOutputSurfaceImplOnGpuUtils::SkiaOutputSurfaceImplOnGpuUtils(SkiaOutputSurfaceImplOnGpu* impl)
{
  this->skiaOutputSurfaceImplOnGpu = impl;
}

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
void SkiaOutputSurfaceImplOnGpuUtils::SendBlanklessSnapshotInfo(
    uint64_t blankless_key,
    int32_t lcp_time,
    int64_t pref_hash,
    const SkBitmap& bitmap,
    const std::vector<gfx::Rect>& quad_list) {
  return skiaOutputSurfaceImplOnGpu->dependency_->SendBlanklessSnapshotInfo(blankless_key,
                                                                            lcp_time,
                                                                            pref_hash,
                                                                            bitmap,
                                                                            quad_list);
}
#endif

} // namespace viz