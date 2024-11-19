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

#ifndef _GPU_COMMAND_BUFFER_SERVICE_OHOS_SCOPED_HARDWARE_BUFFER_HANDLE_H_
#define _GPU_COMMAND_BUFFER_SERVICE_OHOS_SCOPED_HARDWARE_BUFFER_HANDLE_H_

#include "base/base_export.h"
#include "base/files/scoped_file.h"
#include "base/memory/raw_ptr.h"
#include "ohos_adapter_helper.h"

typedef void* OHOSNativeBuffer;

namespace gpu {

typedef enum OH_NativeBuffer_Usage {
    NATIVEBUFFER_USAGE_CPU_READ = (1ULL << 0),        /// < CPU read buffer */
    NATIVEBUFFER_USAGE_CPU_WRITE = (1ULL << 1),       /// < CPU write memory */
    NATIVEBUFFER_USAGE_MEM_DMA = (1ULL << 3),         /// < Direct memory access (DMA) buffer */
    NATIVEBUFFER_USAGE_HW_RENDER = (1ULL << 8),       /// < For GPU write case */
    NATIVEBUFFER_USAGE_HW_TEXTURE = (1ULL << 9),      /// < For GPU read case */
    NATIVEBUFFER_USAGE_CPU_READ_OFTEN = (1ULL << 16), /// < Often be mapped for direct CPU reads */
    NATIVEBUFFER_USAGE_ALIGNMENT_512 = (1ULL << 18),  /// < 512 bytes alignment */
} OH_NativeBuffer_Usage;

// Owns a single reference to an AHardwareBuffer object.
class BASE_EXPORT ScopedNativeBufferHandle {
 public:
  ScopedNativeBufferHandle();

  // Takes ownership of |other|'s buffer reference. Does NOT acquire a new one.
  ScopedNativeBufferHandle(ScopedNativeBufferHandle&& other);

  ScopedNativeBufferHandle(const ScopedNativeBufferHandle&) = delete;
  ScopedNativeBufferHandle& operator=(const ScopedNativeBufferHandle&) =
      delete;

  // Releases this handle's reference to the underlying buffer object if still
  // valid.
  ~ScopedNativeBufferHandle();

  // Assumes ownership of an existing reference to |buffer|. This does NOT
  // acquire a new reference.
  static ScopedNativeBufferHandle Adopt(OHOSNativeBuffer buffer);

  // Adds a reference to |buffer| managed by this handle.
  static ScopedNativeBufferHandle Create(OHOSNativeBuffer buffer);

  // Takes ownership of |other|'s buffer reference. Does NOT acquire a new one.
  ScopedNativeBufferHandle& operator=(ScopedNativeBufferHandle&& other);

  bool is_valid() const;

  OHOSNativeBuffer get() const;

  // Releases this handle's reference to the underlying buffer object if still
  // valid. Invalidates this handle.
  void reset();

  // Passes implicit ownership of this handle's reference over to the caller,
  // invalidating |this|. Returns the raw buffer handle.
  //
  // The caller is responsible for eventually releasing this reference to the
  // buffer object.
  [[nodiscard]] OHOSNativeBuffer Take();

  // Creates a new handle with its own newly acquired reference to the
  // underlying buffer object. |this| must be a valid handle.
  ScopedNativeBufferHandle Clone() const;

 private:
  // Assumes ownership of an existing reference to |buffer|. This does NOT
  // acquire a new reference.
  explicit ScopedNativeBufferHandle(OHOSNativeBuffer buffer);

  OHOSNativeBuffer buffer_ = nullptr;
};

}  // namespace gpu

#endif  // _GPU_COMMAND_BUFFER_SERVICE_OHOS_SCOPED_HARDWARE_BUFFER_HANDLE_H_
