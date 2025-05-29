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
 } // namespace viz