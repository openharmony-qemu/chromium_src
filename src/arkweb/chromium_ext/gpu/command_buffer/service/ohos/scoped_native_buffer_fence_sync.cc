// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/ohos/scoped_native_buffer_fence_sync.h"

#include <utility>

namespace gpu {

ScopedNativeBufferFenceSync::ScopedNativeBufferFenceSync() = default;

ScopedNativeBufferFenceSync::ScopedNativeBufferFenceSync(
    ScopedNativeBufferHandle handle,
    base::ScopedFD fence_fd)
    : handle_(std::move(handle)), fence_fd_(std::move(fence_fd)) {}

ScopedNativeBufferFenceSync::~ScopedNativeBufferFenceSync() = default;

ScopedNativeBufferHandle ScopedNativeBufferFenceSync::TakeBuffer() {
  return std::move(handle_);
}

base::ScopedFD ScopedNativeBufferFenceSync::TakeFence() {
  return std::move(fence_fd_);
}

}  // namespace gpu
