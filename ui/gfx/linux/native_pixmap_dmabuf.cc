// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/linux/native_pixmap_dmabuf.h"

#include <utility>
#include "base/logging.h"
#include "base/posix/eintr_wrapper.h"
#if BUILDFLAG(ARKWEB_HEIF_SUPPORT)
#include <native_window/external_window.h>
#endif // ARKWEB_HEIF_SUPPORT

namespace gfx {

NativePixmapDmaBuf::NativePixmapDmaBuf(const gfx::Size& size,
                                       gfx::BufferFormat format,
#if BUILDFLAG(ARKWEB_HEIF_SUPPORT)
                                       gfx::NativePixmapHandle handle,
                                       void* window_buffer)
    : size_(size), format_(format), handle_(std::move(handle)), native_window_buffer_(window_buffer) {}
#else
                                       gfx::NativePixmapHandle handle)
    : size_(size), format_(format), handle_(std::move(handle)) {}
#endif

NativePixmapDmaBuf::~NativePixmapDmaBuf() {
#if BUILDFLAG(ARKWEB_HEIF_SUPPORT)
  if (native_window_buffer_) {
    OH_NativeWindow_DestroyNativeWindowBuffer(
        static_cast<OHNativeWindowBuffer*>(native_window_buffer_));
    native_window_buffer_ = nullptr;
  }
#endif // ARKWEB_HEIF_SUPPORT
}

bool NativePixmapDmaBuf::AreDmaBufFdsValid() const {
  if (handle_.planes.empty())
    return false;

  for (const auto& plane : handle_.planes) {
    if (!plane.fd.is_valid())
      return false;
  }
  return true;
}

int NativePixmapDmaBuf::GetDmaBufFd(size_t plane) const {
  DCHECK_LT(plane, handle_.planes.size());
  return handle_.planes[plane].fd.get();
}

uint32_t NativePixmapDmaBuf::GetDmaBufPitch(size_t plane) const {
  DCHECK_LT(plane, handle_.planes.size());
  return base::checked_cast<uint32_t>(handle_.planes[plane].stride);
}

size_t NativePixmapDmaBuf::GetDmaBufOffset(size_t plane) const {
  DCHECK_LT(plane, handle_.planes.size());
  return base::checked_cast<size_t>(handle_.planes[plane].offset);
}

size_t NativePixmapDmaBuf::GetDmaBufPlaneSize(size_t plane) const {
  DCHECK_LT(plane, handle_.planes.size());
  return base::checked_cast<size_t>(handle_.planes[plane].size);
}

uint64_t NativePixmapDmaBuf::GetBufferFormatModifier() const {
  return handle_.modifier;
}

gfx::BufferFormat NativePixmapDmaBuf::GetBufferFormat() const {
  return format_;
}

size_t NativePixmapDmaBuf::GetNumberOfPlanes() const {
  return handle_.planes.size();
}

bool NativePixmapDmaBuf::SupportsZeroCopyWebGPUImport() const {
  // TODO(crbug.com/40201271): Figure out how to import multi-planar pixmap into
  // WebGPU without copy.
  return false;
}

gfx::Size NativePixmapDmaBuf::GetBufferSize() const {
  return size_;
}

uint32_t NativePixmapDmaBuf::GetUniqueId() const {
  return 0;
}

bool NativePixmapDmaBuf::ScheduleOverlayPlane(
    gfx::AcceleratedWidget widget,
    const gfx::OverlayPlaneData& overlay_plane_data,
    std::vector<gfx::GpuFence> acquire_fences,
    std::vector<gfx::GpuFence> release_fences) {
  return false;
}

gfx::NativePixmapHandle NativePixmapDmaBuf::ExportHandle() const {
  return gfx::CloneHandleForIPC(handle_);
}

}  // namespace gfx
