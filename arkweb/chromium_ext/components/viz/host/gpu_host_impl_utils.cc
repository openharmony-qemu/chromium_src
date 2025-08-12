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
#include "arkweb/chromium_ext/components/viz/host/blankless_data_controller.h"
#include "base/functional/bind.h"
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

void GpuHostImpl::SetIsScroll(bool is_scroll_enabled) {
  gpu_service_remote_->SetIsScroll(is_scroll_enabled);
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
void GpuHostImpl::SendBlanklessSnapshotInfo(mojom::BlanklessSendInfoPtr infoPtr,
                                            const std::vector<gfx::Rect>& quad_list,
                                            mojo::ScopedSharedBufferHandle buffer,
                                            mojom::BlanklessBitmapMetadataPtr metadata) {
  if (!infoPtr) {
    LOG(WARNING) << "blankless SendBlanklessSnapshotInfo invalid snapshot infoPtr.";
    return;
  }
  TRACE_EVENT1("io", "blankless GpuHostImpl::SendBlanklessSnapshotInfo", "blankless_key", infoPtr->blankless_key);
  uint64_t key = infoPtr->blankless_key;
  auto task = base::BindOnce(&GpuHostImpl::DumpBlanklessSnapshot,
                             std::move(infoPtr), quad_list,
                             std::move(buffer), std::move(metadata));
  auto& databaseAdapter = base::ohos::BlanklessDataController::GetInstance();
  databaseAdapter.PostDumpTaskWithDelay(key, std::move(task));
}

void GpuHostImpl::DumpBlanklessSnapshot(mojom::BlanklessSendInfoPtr infoPtr,
                                        const std::vector<gfx::Rect>& quad_list,
                                        mojo::ScopedSharedBufferHandle buffer,
                                        mojom::BlanklessBitmapMetadataPtr metadata) {
  if (!infoPtr) {
    LOG(WARNING) << "blankless DumpBlanklessSnapshot invalid snapshot infoPtr.";
    return;
  }
  TRACE_EVENT1("io", "blankless GpuHostImpl::DumpBlanklessSnapshot", "blankless_key", infoPtr->blankless_key);
  LOG(DEBUG) << "GpuHostImpl::DumpBlanklessSnapshot url begin : key " << infoPtr->blankless_key
    << ", lcp_time " << infoPtr->lcp_time << ", pref_hash " << infoPtr->pref_hash;
  
  // the next all the process is sync, here we restore the skbitmap from mojo
  if (!buffer || !buffer.is_valid() ||
    !metadata || metadata->width <= 0 || metadata->height <= 0 || metadata->size == 0 ||
    metadata->color_type < static_cast<int32_t>(SkColorType::kUnknown_SkColorType) ||
    metadata->color_type > static_cast<int32_t>(SkColorType::kLastEnum_SkColorType) ||
    metadata->alpha_type < static_cast<int32_t>(SkAlphaType::kUnknown_SkAlphaType) ||
    metadata->alpha_type > static_cast<int32_t>(SkAlphaType::kLastEnum_SkAlphaType)) {
    LOG(WARNING) << "blankless GpuHostImpl::DumpBlanklessSnapshot invalid snapshot info";
    return;
  }
  mojo::ScopedSharedBufferMapping mapping = buffer->Map(metadata->size);
  if (!mapping) {
    LOG(WARNING) << "blankless DumpBlanklessSnapshot Failed to map shared buffer.";
    return;
  }
  SkImageInfo info = SkImageInfo::Make(metadata->width, metadata->height,
                                        static_cast<SkColorType>(metadata->color_type),
                                        static_cast<SkAlphaType>(metadata->alpha_type));

  // bind the shared mem, no mem copy, the buffer and metadata will be destroyed when this function finished.
  SkBitmap bitmap;
  if (!bitmap.installPixels(info, mapping.get(), info.minRowBytes())) {
    LOG(WARNING) << "blankless DumpBlanklessSnapshot Failed to install pixels into bitmap.";
    return;
  }

  base::ohos::BlanklessInfo blankless_info = {
    .blankless_key = infoPtr->blankless_key,
    .nweb_id = infoPtr->nweb_id,
    .lcp_time = infoPtr->lcp_time,
    .system_time = infoPtr->system_time,
    .pref_hash = infoPtr->pref_hash
  };

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
  databaseAdapter.DumpBlanklessSnapshot(blankless_info, bitmap, rect_list);
}

void GpuHostImpl::ClearBlanklessSnapshotInfo(uint64_t blankless_key) {
  TRACE_EVENT1("io", "blankless GpuHostImpl::ClearBlanklessSnapshotInfo", "blankless_key", blankless_key);
  LOG(DEBUG) << "blankless clear last snapshot info, blankless_key " << blankless_key;
  auto& databaseAdapter = base::ohos::BlanklessDataController::GetInstance();
  databaseAdapter.ClearSnapshot(blankless_key);
}
#endif
} // namespace viz