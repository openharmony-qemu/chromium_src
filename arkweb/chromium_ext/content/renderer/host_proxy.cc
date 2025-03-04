// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "host_proxy.h"

#include "gpu/ipc/common/nweb_native_window_tracker.h"

namespace content {
void* HostProxy::GetSurfaceFromKernel(int32_t surface_id) {
  return NWebNativeWindowTracker::GetInstance()->GetNativeWindow(surface_id);
}

void HostProxy::DestroySurfaceFromKernel(int32_t surface_id) {
  NWebNativeWindowTracker::GetInstance()->DestroyNativeWindow(surface_id);
}

}  // namespace content
                      