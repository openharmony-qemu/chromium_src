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

#ifndef GPU_COMMAND_BUFFER_SERVICE_OHOS_SCOPED_NATIVE_BUFFER_FENCE_SYNC_H
#define GPU_COMMAND_BUFFER_SERVICE_OHOS_SCOPED_NATIVE_BUFFER_FENCE_SYNC_H

#include "gpu/command_buffer/service/ohos/scoped_native_buffer_handle.h"
#include "base/base_export.h"
#include "base/files/scoped_file.h"

namespace gpu {

class BASE_EXPORT ScopedNativeBufferFenceSync {
 public:
  ScopedNativeBufferFenceSync();
  ScopedNativeBufferFenceSync(
      ScopedNativeBufferHandle handle,
      base::ScopedFD fence_fd);
  virtual ~ScopedNativeBufferFenceSync();

  OHOSNativeBuffer buffer() const { return handle_.get(); }
  ScopedNativeBufferHandle TakeBuffer();
  base::ScopedFD TakeFence();

  // Provides fence which is signaled when the reads for this buffer are done
  // and it can be reused. Must only be called once.
  virtual void SetReadFence(base::ScopedFD fence_fd) = 0;

 private:
  ScopedNativeBufferHandle handle_;
  base::ScopedFD fence_fd_;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_OHOS_SCOPED_NATIVE_BUFFER_FENCE_SYNC_H
