/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef GPU_IPC_COMMON_GPU_SURFACE_ID_LOOKUP_H_
#define GPU_IPC_COMMON_GPU_SURFACE_ID_LOOKUP_H_

#include <string>
#include <map>
#include "gpu/gpu_export.h"
#include "base/memory/singleton.h"
#include "base/synchronization/lock.h"
#include "gpu/ipc/common/surface_handle.h"
#include "third_party/abseil-cpp/absl/types/variant.h"
#include "ui/gfx/native_widget_types.h"

namespace gpu {

// This class provides an interface to look up window surface handles
// that cannot be sent through the IPC channel.
class GPU_EXPORT GpuSurfaceIdTracker {
 public:
  struct SurfaceRecord {
    SurfaceRecord(gfx::AcceleratedWidget widget,
                  std::string& native_image_surface_id);

    SurfaceRecord(SurfaceRecord&&);
    SurfaceRecord(const SurfaceRecord&) = delete;

    gfx::AcceleratedWidget widget;
    std::string native_image_surface_id;
  };

  GpuSurfaceIdTracker(const GpuSurfaceIdTracker&) = delete;
  GpuSurfaceIdTracker& operator=(const GpuSurfaceIdTracker&) = delete;

  GpuSurfaceIdTracker();
  virtual ~GpuSurfaceIdTracker() {}

  // Gets the global instance of the surface tracker.
  static GpuSurfaceIdTracker* Get() { return GetInstance(); }

  std::string AcquireNativeImageSurfaceId(
      gpu::SurfaceHandle surface_handle);

  // Adds a surface for a native widget. Returns the surface ID.
  int AddSurfaceForNativeWidget(SurfaceRecord record);

  // Return true if the surface handle is registered with the tracker.
  bool IsValidSurfaceHandle(gpu::SurfaceHandle surface_handle) const;

  // Removes a given existing surface.
  void RemoveSurface(gpu::SurfaceHandle surface_handle);

  // Returns the number of surfaces currently registered with the tracker.
  std::size_t GetSurfaceCount();

  // Gets the global instance of the surface tracker. Identical to Get(), but
  // named that way for the implementation of Singleton.
  static GpuSurfaceIdTracker* GetInstance();

 private:
  using SurfaceMap = std::map<gpu::SurfaceHandle, SurfaceRecord>;

  friend struct base::DefaultSingletonTraits<GpuSurfaceIdTracker>;

  mutable base::Lock surface_map_lock_;
  SurfaceMap surface_map_;
  int next_surface_handle_;
};

}  // namespace gpu

#endif  // GPU_IPC_COMMON_GPU_SURFACE_LOOKUP_H_
