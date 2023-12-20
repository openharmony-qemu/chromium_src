/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 */

#include "gpu/ipc/common/gpu_surface_id_tracker.h"

#include "base/check.h"
#include "build/build_config.h"

namespace gpu {

GpuSurfaceIdTracker::SurfaceRecord::SurfaceRecord(
    gfx::AcceleratedWidget widget,
    std::string& native_image_surface_id)
    : widget(widget), native_image_surface_id(native_image_surface_id) {}

GpuSurfaceIdTracker::SurfaceRecord::SurfaceRecord(SurfaceRecord&&) = default;

GpuSurfaceIdTracker::GpuSurfaceIdTracker()
    : next_surface_handle_(1) {}

GpuSurfaceIdTracker* GpuSurfaceIdTracker::GetInstance() {
  return base::Singleton<GpuSurfaceIdTracker>::get();
}

int GpuSurfaceIdTracker::AddSurfaceForNativeWidget(SurfaceRecord record) {
  base::AutoLock lock(surface_map_lock_);
  gpu::SurfaceHandle surface_handle = next_surface_handle_++;
  surface_map_.emplace(surface_handle, std::move(record));
  return surface_handle;
}

bool GpuSurfaceIdTracker::IsValidSurfaceHandle(
    gpu::SurfaceHandle surface_handle) const {
  base::AutoLock lock(surface_map_lock_);
  return surface_map_.find(surface_handle) != surface_map_.end();
}

void GpuSurfaceIdTracker::RemoveSurface(gpu::SurfaceHandle surface_handle) {
  base::AutoLock lock(surface_map_lock_);
  DCHECK(surface_map_.find(surface_handle) != surface_map_.end());
  surface_map_.erase(surface_handle);
}

std::string GpuSurfaceIdTracker::AcquireNativeImageSurfaceId(
    gpu::SurfaceHandle surface_handle) {
  base::AutoLock lock(surface_map_lock_);
  SurfaceMap::const_iterator it = surface_map_.find(surface_handle);
  if (it == surface_map_.end())
    return std::string();

  return it->second.native_image_surface_id;
}

std::size_t GpuSurfaceIdTracker::GetSurfaceCount() {
  base::AutoLock lock(surface_map_lock_);
  return surface_map_.size();
}

}  // namespace gpu
