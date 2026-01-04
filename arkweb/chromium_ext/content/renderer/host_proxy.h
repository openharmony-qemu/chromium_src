// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_HOST_PROXY_H
#define CONTENT_RENDERER_HOST_PROXY_H

#include <condition_variable>

#include "third_party/ohos_ndk/includes/ohos_adapter/aafwk_browser_host_adapter.h"

namespace content {
class HostProxy : public OHOS::NWeb::AafwkBrowserHostAdapter {
 public:
  HostProxy() = default;
  ~HostProxy() = default;

  virtual void* GetSurfaceFromKernel(int32_t surface_id) override;
  virtual void DestroySurfaceFromKernel(int32_t surface_id) override;
  void* GetSurfaceFromKernelWithRef(int32_t surface_id) override;
};
}  // namespace content

#endif  // CONTENT_RENDERER_HOST_PROXY_H
