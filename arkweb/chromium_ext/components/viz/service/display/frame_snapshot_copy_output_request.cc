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

#include "components/viz/service/display/frame_snapshot_copy_output_request.h"

#include "arkweb/chromium_ext/components/viz/common/frame_sinks/arkweb_copy_output_result_utils.h"
#include "base/trace_event/trace_event.h"
#include "components/viz/common/frame_sinks/copy_output_request.h"
#include "components/viz/common/frame_sinks/copy_output_result.h"
#include "components/viz/service/display_embedder/skia_output_surface_impl_on_gpu.h"
#include "third_party/skia/include/core/SkBitmap.h"

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "arkweb/chromium_ext/base/ohos/blankless/blankless_controller.h"
#endif

namespace viz {
FrameSnapshotCopyOutputRequest::FrameSnapshotCopyOutputRequest() : CopyOutputRequest(
  ResultFormat::RGBA,
  ResultDestination::kSystemMemory,
  base::BindOnce([](std::unique_ptr<CopyOutputResult> result) {
    if (!result) {
      return;
    }
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
    TRACE_EVENT0("viz", "blankless FrameSnapshotCopyOutputRequest callback");
    ArkwebCopyOutputResultUtils* utils = result->copy_output_result_utils();
    if (utils && utils->GetBlanklessKey() != base::ohos::BlanklessController::INVALID_BLANKLESS_KEY &&
        utils->ImplOnGpu() && utils->ImplOnGpu()->impl_utils()) {
      SkBitmap bitmap = result->ScopedAccessSkBitmap().bitmap();
      LOG(DEBUG) << "blankless Send SnapShot Info";
      utils->ImplOnGpu()->impl_utils()->SendBlanklessSnapshotInfo(
          utils->GetBlanklessKey(), utils->GetLcpTime(), utils->GetPreferenceHash(), bitmap, utils->GetQuadList());
    }
#endif
  })) {}
}  // namespace viz