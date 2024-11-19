/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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


#include "gpu/command_buffer/service/ohos/scoped_native_buffer_fence_sync.h"

#include <utility>

namespace gpu {

ScopedNativeBufferFenceSync::ScopedNativeBufferFenceSync() = default;

ScopedNativeBufferFenceSync::ScopedNativeBufferFenceSync(
    ScopedNativeBufferHandle handle,
    base::ScopedFD fence_fd)
    : handle_(std::move(handle)),
      fence_fd_(std::move(fence_fd)) {}

ScopedNativeBufferFenceSync::~ScopedNativeBufferFenceSync() = default;

ScopedNativeBufferHandle ScopedNativeBufferFenceSync::TakeBuffer() {
  return std::move(handle_);
}

base::ScopedFD ScopedNativeBufferFenceSync::TakeFence() {
  return std::move(fence_fd_);
}

}  // namespace gpu
