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

#include "components/viz/common/frame_sinks/copy_output_result.h"
#include "components/viz/service/display_embedder/skia_output_surface_impl_on_gpu.h"
#include "arkweb/chromium_ext/components/viz/common/frame_sinks/arkweb_copy_output_result_utils.h"

namespace viz {

ArkwebCopyOutputResultUtils::ArkwebCopyOutputResultUtils(CopyOutputResult* copyOutputResult)
    : copyOutputResult_(copyOutputResult) {}

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
void ArkwebCopyOutputResultUtils::SetImplOnGpu(SkiaOutputSurfaceImplOnGpu* impl_on_gpu) {
  impl_on_gpu_ = impl_on_gpu;
}

SkiaOutputSurfaceImplOnGpu* ArkwebCopyOutputResultUtils::ImplOnGpu() const {
  return impl_on_gpu_;
}

void ArkwebCopyOutputResultUtils::SetBlanklessKey(uint64_t blankless_key) {
  blankless_key_ = blankless_key;
}

uint64_t ArkwebCopyOutputResultUtils::GetBlanklessKey() const {
  return blankless_key_;
}

void ArkwebCopyOutputResultUtils::SetLcpTime(int32_t lcp_time) {
  lcp_time_ = lcp_time;
}

int32_t ArkwebCopyOutputResultUtils::GetLcpTime() const {
  return lcp_time_;
}

void ArkwebCopyOutputResultUtils::SetPreferenceHash(int64_t pref_hash)
{
  pref_hash_ = pref_hash;
}

int64_t ArkwebCopyOutputResultUtils::GetPreferenceHash() const
{
  return pref_hash_;
}

void ArkwebCopyOutputResultUtils::SetQuadList(const std::vector<gfx::Rect>& quad_list) {
  quad_list_ = quad_list;
}

const std::vector<gfx::Rect>& ArkwebCopyOutputResultUtils::GetQuadList() const {
  return quad_list_;
}
#endif
}  // namespace viz