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

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include <mutex>
#include "arkweb/chromium_ext/base/ohos/blankless/blankless_controller.h"
#include "arkweb/chromium_ext/components/viz/host/blankless_data_controller.h"
#include "base/task/thread_pool.h"
#include "base/time/time.h"
#endif

namespace viz {

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
void GpuHostImpl::StartMonitor() {
  gpu_service_remote_->StartMonitor();
}

void GpuHostImpl::StopMonitor() {
  gpu_service_remote_->StopMonitor();
}
#endif

#if BUILDFLAG(IS_ARKWEB)
void GpuHostImpl::SetVisible(int32_t nweb_id, bool visible) {
  gpu_service_remote_->SetVisible(nweb_id, visible);
}
#endif

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
void GpuHostImpl::SetHasTouchPoint(bool has_touch_point) {
  gpu_service_remote_->SetHasTouchPoint(has_touch_point);
}

void GpuHostImpl::ReportSlidingFrameRate(int32_t frame_rate) {
  gpu_service_remote_->ReportSlidingFrameRate(frame_rate);
}

void GpuHostImpl::SetLTPOStrategy(int32_t strategy) {
  gpu_service_remote_->SetLTPOStrategy(strategy);
}
#endif

#if BUILDFLAG(ARKWEB_D_VSYNC)
void GpuHostImpl::SetIsFling(bool is_fling_enabled) {
  gpu_service_remote_->SetIsFling(is_fling_enabled);
}
#endif

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
std::string GpuHostImpl::GetSurfaceId(int32_t native_embed_id) {
  LOG(DEBUG) << "get surface id = " << native_embed_id;
  mojo::SyncCallRestrictions::ScopedAllowSyncCall allow_sync;
  std::string surface_id = "";
  gpu_service_remote_->GetSurfaceId(native_embed_id, &surface_id);
  return surface_id;
}

void GpuHostImpl::SetTransformHint(uint32_t rotation, uint32_t window_id) {
  LOG(DEBUG) << "SetTransformHint angle: " << rotation;
  gpu_service_remote_->SetTransformHint(rotation, window_id);
}

void GpuHostImpl::DestroyNativeWindow(uint32_t native_window_id) {
  LOG(DEBUG) << "destroy native window id = " << native_window_id;
  gpu_service_remote_->DestroyNativeWindow(native_window_id);
}
#endif

void GpuHostImpl::Discard(uint32_t native_window_id)
{
  if (!gpu_service_remote_) {
    return;
  }
  LOG(DEBUG) << "discard native window id = " << native_window_id;
  gpu_service_remote_->Discard(native_window_id);
}

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
void GpuHostImpl::SendBlanklessSnapshotInfo(uint64_t blankless_key,
                                            int32_t lcp_time,
                                            int64_t pref_hash,
                                            const SkBitmap& bitmap,
                                            const std::vector<gfx::Rect>& quad_list) {
  TRACE_EVENT1("io", "blankless GpuHostImpl::SendBlanklessSnapshotInfo", "blankless_key", blankless_key);
  base::ThreadPool::PostTask(
      FROM_HERE,
      {base::MayBlock(), base::TaskShutdownBehavior::SKIP_ON_SHUTDOWN,
       base::TaskPriority::USER_BLOCKING},
      base::BindOnce(&GpuHostImpl::DumpBlanklessSnapshot, blankless_key, lcp_time, pref_hash, bitmap, quad_list));
}

void GpuHostImpl::DumpBlanklessSnapshot(uint64_t blankless_key,
                                        int32_t lcp_time,
                                        int64_t pref_hash,
                                        const SkBitmap& bitmap,
                                        const std::vector<gfx::Rect>& quad_list) {
  TRACE_EVENT1("io", "blankless GpuHostImpl::DumpBlanklessSnapshot", "blankless_key", blankless_key);
  LOG(DEBUG) << "GpuHostImpl::DumpBlanklessSnapshot url begin : key " << blankless_key
    << ", lcp_time " << lcp_time << ", pref_hash " << pref_hash;
  auto& databaseAdapter = base::ohos::BlanklessDataController::GetInstance();
  std::vector<base::ohos::BlanklessDataController::SnapShotRect> rect_list;
  if (quad_list.size() > 0) {
    rect_list.reserve(quad_list.size());
  }
  for (const auto& quad: quad_list) {
    rect_list.push_back({
      quad.x(),
      quad.y(),
      quad.width(),
      quad.height(),
    });
  }
  databaseAdapter.DumpBlanklessSnapshot(blankless_key, lcp_time, pref_hash, bitmap, rect_list);
}

void GpuHostImpl::ClearBlanklessSnapshotInfo(uint64_t blankless_key) {
  TRACE_EVENT1("io", "blankless GpuHostImpl::ClearBlanklessSnapshotInfo", "blankless_key", blankless_key);
  LOG(DEBUG) << "blankless clear last snapshot info, blankless_key " << blankless_key;
  auto& databaseAdapter = base::ohos::BlanklessDataController::GetInstance();
  databaseAdapter.ClearSnapshot(blankless_key);
}
#endif
} // namespace viz