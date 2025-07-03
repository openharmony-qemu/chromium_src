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

#ifndef COMPONENTS_VIZ_COMMON_FRAME_SINKS_ARKWEB_COPY_OUTPUT_RESULT_UTILS_H_
#define COMPONENTS_VIZ_COMMON_FRAME_SINKS_ARKWEB_COPY_OUTPUT_RESULT_UTILS_H_

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include <string>
#include "arkweb/chromium_ext/base/ohos/blankless/blankless_controller.h"
#include "ui/gfx/geometry/rect.h"
#endif
namespace viz {
class CopyOutputResult;
class SkiaOutputSurfaceImplOnGpu;

class ArkwebCopyOutputResultUtils {
 public:
  ArkwebCopyOutputResultUtils(CopyOutputResult* copyOutputResult);

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void SetImplOnGpu(SkiaOutputSurfaceImplOnGpu* impl_on_gpu);

  SkiaOutputSurfaceImplOnGpu* ImplOnGpu() const;

  void SetBlanklessKey(uint64_t blankless_key);

  uint64_t GetBlanklessKey() const;

  void SetLcpTime(int32_t lcp_time);

  int32_t GetLcpTime() const;

  void SetPreferenceHash(int64_t pref_hash);

  int64_t GetPreferenceHash() const;

  void SetQuadList(const std::vector<gfx::Rect>& quad_list);

  const std::vector<gfx::Rect>& GetQuadList() const;
#endif

 private:
  CopyOutputResult* copyOutputResult_;

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  SkiaOutputSurfaceImplOnGpu* impl_on_gpu_ = nullptr;
  uint64_t blankless_key_ = base::ohos::BlanklessController::INVALID_BLANKLESS_KEY;
  int32_t lcp_time_ = 0;
  int64_t pref_hash_ = 0;
  std::vector<gfx::Rect> quad_list_;
#endif
};

}  // namespace viz
#endif  // COMPONENTS_VIZ_COMMON_FRAME_SINKS_ARKWEB_COPY_OUTPUT_RESULT_UTILS_H_