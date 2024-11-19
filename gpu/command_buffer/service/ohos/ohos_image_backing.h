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

#ifndef GPU_COMMAND_BUFFER_SERVICE_OHOS_OHOS_IMAGE_BACKING_H_
#define GPU_COMMAND_BUFFER_SERVICE_OHOS_OHOS_IMAGE_BACKING_H_

#include "base/containers/flat_set.h"
#include "base/files/scoped_file.h"
#include "gpu/command_buffer/service/shared_image/shared_image_backing.h"

namespace gpu {

class OhosImageBacking : public ClearTrackingSharedImageBacking {
 public:
  OhosImageBacking(const Mailbox& mailbox,
                         viz::SharedImageFormat format,
                         const gfx::Size& size,
                         const gfx::ColorSpace& color_space,
                         GrSurfaceOrigin surface_origin,
                         SkAlphaType alpha_type,
                         uint32_t usage,
                         size_t estimated_size,
                         bool is_thread_safe,
                         base::ScopedFD initial_upload_fd);

  ~OhosImageBacking() override;
  OhosImageBacking(const OhosImageBacking&) = delete;
  OhosImageBacking& operator=(const OhosImageBacking&) = delete;

  virtual bool BeginWrite(base::ScopedFD* fd_to_wait_on);
  virtual void EndWrite(base::ScopedFD end_write_fd);
  virtual bool BeginRead(const SharedImageRepresentation* reader,
                         base::ScopedFD* fd_to_wait_on);
  virtual void EndRead(const SharedImageRepresentation* reader,
                       base::ScopedFD end_read_fd);
  base::ScopedFD TakeReadFence();

 protected:
  // All reads and writes must wait for exiting writes to complete.
  base::ScopedFD write_sync_fd_ GUARDED_BY(lock_);
  bool is_writing_ GUARDED_BY(lock_) = false;

  // All writes must wait for existing reads to complete.
  base::ScopedFD read_sync_fd_ GUARDED_BY(lock_);
  base::flat_set<const SharedImageRepresentation*> active_readers_
      GUARDED_BY(lock_);

  bool is_overlay_accessing_ GUARDED_BY(lock_) = false;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_OHOS_OHOS_IMAGE_BACKING_H_
