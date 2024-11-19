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

#include "gpu/command_buffer/service/ohos/ohos_image_backing.h"

#include "base/logging.h"
#include "base/posix/eintr_wrapper.h"
#include "ui/gl/gl_utils.h"

namespace gpu {

OhosImageBacking::OhosImageBacking(
    const Mailbox& mailbox,
    viz::SharedImageFormat format,
    const gfx::Size& size,
    const gfx::ColorSpace& color_space,
    GrSurfaceOrigin surface_origin,
    SkAlphaType alpha_type,
    uint32_t usage,
    size_t estimated_size,
    bool is_thread_safe,
    base::ScopedFD initial_upload_fd)
    : ClearTrackingSharedImageBacking(mailbox,
                                      format,
                                      size,
                                      color_space,
                                      surface_origin,
                                      alpha_type,
                                      usage,
                                      estimated_size,
                                      is_thread_safe),
      write_sync_fd_(std::move(initial_upload_fd)) {}

OhosImageBacking::~OhosImageBacking() = default;

bool OhosImageBacking::BeginWrite(base::ScopedFD* fd_to_wait_on) {
  AutoLock auto_lock(this);

  if (is_writing_ || !active_readers_.empty() || is_overlay_accessing_) {
    LOG(ERROR) << "BeginWrite should only be called when there are no other "
                  "readers or writers";
    return false;
  }

  is_writing_ = true;
  (*fd_to_wait_on) =
      gl::MergeFDs(std::move(read_sync_fd_), std::move(write_sync_fd_));

  return true;
}

void OhosImageBacking::EndWrite(base::ScopedFD end_write_fd) {
  AutoLock auto_lock(this);

  if (!is_writing_) {
    LOG(ERROR) << "Attempt to end write to a SharedImageBacking without a "
                  "successful begin write";
    return;
  }

  is_writing_ = false;

  write_sync_fd_ = std::move(end_write_fd);
}

bool OhosImageBacking::BeginRead(const SharedImageRepresentation* reader,
                                    base::ScopedFD* fd_to_wait_on) {
  AutoLock auto_lock(this);

  if (is_writing_) {
    LOG(ERROR) << "BeginRead should only be called when there are no writers";
    return false;
  }

  if (active_readers_.contains(reader)) {
    LOG(ERROR) << "BeginRead was called twice on the same representation";
    return false;
  }

  active_readers_.insert(reader);
  if (write_sync_fd_.is_valid()) {
    (*fd_to_wait_on) = base::ScopedFD(HANDLE_EINTR(dup(write_sync_fd_.get())));
  } else {
    (*fd_to_wait_on) = base::ScopedFD{};
  }

  return true;
}

void OhosImageBacking::EndRead(const SharedImageRepresentation* reader,
                                  base::ScopedFD end_read_fd) {
  AutoLock auto_lock(this);

  if (!active_readers_.contains(reader)) {
    LOG(ERROR) << "Attempt to end read to a SharedImageBacking without a "
                  "successful begin read";
    return;
  }

  active_readers_.erase(reader);

  read_sync_fd_ =
      gl::MergeFDs(std::move(read_sync_fd_), std::move(end_read_fd));
}

base::ScopedFD OhosImageBacking::TakeReadFence() {
  AutoLock auto_lock(this);

  return std::move(read_sync_fd_);
}

}  // namespace gpu
