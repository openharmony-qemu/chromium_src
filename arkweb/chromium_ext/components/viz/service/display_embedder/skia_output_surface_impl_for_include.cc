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

#ifndef COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_SURFACE_IMPL_H_
#error "must be in include form COMPONENTS_VIZ_SERVICE_DISPLAY_EMBEDDER_SKIA_OUTPUT_SURFACE_IMPL_H_"
#endif

#include "components/viz/service/display_embedder/skia_output_surface_impl.h"

namespace viz {

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void SkiaOutputSurfaceImpl::SetBypassVsyncCondition(int32_t condition) {
  auto task = base::BindOnce(&SkiaOutputSurfaceImplOnGpu::SetBypassVsyncCondition,
                             base::Unretained(impl_on_gpu_.get()), condition);
  EnqueueGpuTask(std::move(task), {}, /*make_current=*/true,
                 /*need_framebuffer=*/!dependency_->IsOffscreen());
}
#endif

} // namespace viz