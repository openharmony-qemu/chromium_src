// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "host_proxy.h"

#include "base/logging.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"

namespace content {
// LCOV_EXCL_START
void* HostProxy::GetSurfaceFromKernel(int32_t surface_id) {
  return NWebNativeWindowTracker::GetInstance()->GetNativeWindow(surface_id);
}

void HostProxy::DestroySurfaceFromKernel(int32_t surface_id) {
  NWebNativeWindowTracker::GetInstance()->DestroyNativeWindow(surface_id);
}

void* HostProxy::GetSurfaceFromKernelWithRef(int32_t surface_id) {
  LOG(DEBUG) << "GetSurfaceFromKernelWithRef surface_id " << surface_id;
  return NWebNativeWindowTracker::GetInstance()->GetNativeWindow(surface_id, true);
}
// LCOV_EXCL_STOP

}  // namespace content
                      