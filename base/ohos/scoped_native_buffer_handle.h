// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

  // Consumes a handle and returns a file descriptor which can be used to
  // transmit the handle over IPC. A subsequent receiver may use
  // |DeserializeFromFileDescriptor()| to recover the buffer handle.
  //
  // NOTE: The returned file descriptor DOES NOT own a reference to the
  // underlying AHardwareBuffer. When using this for IPC, the caller is
  // responsible for retaining at least one reference to the buffer object to
  // keep it alive while the descriptor is in transit.
  base::ScopedFD SerializeAsFileDescriptor() const;

  // Consumes the supplied single-use file descriptor (which must have been
  // returned by a previous call to |SerializeAsFileDescriptor()|, perhaps in
  // a different process), and recovers an AHardwareBuffer object from it.
  //
  // This acquires a new reference to the AHardwareBuffer, with ownership passed
  // to the caller via the returned ScopedHardwareBufferHandle.
  [[nodiscard]] static ScopedNativeBufferHandle DeserializeFromFileDescriptor(
      base::ScopedFD fd);

 private:
  // Assumes ownership of an existing reference to |buffer|. This does NOT
  // acquire a new reference.
  explicit ScopedNativeBufferHandle(OHOSNativeBuffer buffer);

  OHOSNativeBuffer buffer_ = nullptr;
};

}  // namespace gpu

#endif  // _GPU_COMMAND_BUFFER_SERVICE_OHOS_SCOPED_HARDWARE_BUFFER_HANDLE_H_
