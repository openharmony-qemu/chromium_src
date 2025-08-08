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

#ifndef COMPONENTS_VIZ_COMMON_FRAME_SINKS_ARKWEB_COPY_OUTPUT_REQUEST_UTILS_H_
#define COMPONENTS_VIZ_COMMON_FRAME_SINKS_ARKWEB_COPY_OUTPUT_REQUEST_UTILS_H_

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include <string>
#include "ui/gfx/geometry/rect.h"
#include "arkweb/chromium_ext/base/ohos/blankless/blankless_controller.h"
#endif
namespace viz {
class CopyOutputRequest;

class ArkwebCopyOutputRequestUtils {
 public:
  ArkwebCopyOutputRequestUtils(CopyOutputRequest* CopyOutputRequest);

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void SetBlanklessInfo(const base::ohos::BlanklessInfo& info);

  const base::ohos::BlanklessInfo& GetBlanklessInfo() const;

  bool IsBlanklessInfoValid() const;

  void SetQuadList(const std::vector<gfx::Rect>& quad_list);

  const std::vector<gfx::Rect>& GetQuadList() const;
#endif

 private:
  raw_ptr<CopyOutputRequest> copyOutputRequest_;

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  base::ohos::BlanklessInfo info_;
  std::vector<gfx::Rect> quad_list_;
#endif
};
}  // namespace viz
#endif  // COMPONENTS_VIZ_COMMON_FRAME_SINKS_ARKWEB_COPY_OUTPUT_REQUEST_UTILS_H_