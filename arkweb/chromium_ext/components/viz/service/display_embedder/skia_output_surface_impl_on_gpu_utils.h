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

#ifndef COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_SURFACE_IMPL_ON_GPU_UTILS_H_
#define COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_SURFACE_IMPL_ON_GPU_UTILS_H_

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include <vector>
#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/gfx/geometry/rect.h"
#endif

namespace viz {
class SkiaOutputSurfaceImplOnGpu;

class SkiaOutputSurfaceImplOnGpuUtils {
 public:
  SkiaOutputSurfaceImplOnGpu* skiaOutputSurfaceImplOnGpu;
  SkiaOutputSurfaceImplOnGpuUtils(SkiaOutputSurfaceImplOnGpu* impl);

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void SendBlanklessSnapshotInfo(uint64_t blankless_key, int32_t lcp_time, int64_t pref_hash, const SkBitmap& bitmap,
                                 const std::vector<gfx::Rect>& quad_list);
#endif
};

} // namespace viz

#endif  // COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_SURFACE_IMPL_ON_GPU_UTILS_H_