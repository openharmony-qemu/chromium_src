/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MEDIA_GPU_OHOS_CODEC_BUFFER_WAIT_COORDINATOR_H_
#define MEDIA_GPU_OHOS_CODEC_BUFFER_WAIT_COORDINATOR_H_

#include "base/memory/ref_counted.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/thread_checker.h"
#include "base/time/time.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/ohos/native_image_texture_owner.h"
#include "media/base/tuneable.h"
#include "media/gpu/media_gpu_export.h"

namespace media {

struct FrameAvailableEvent;

class MEDIA_GPU_EXPORT CodecBufferWaitCoordinator
    : public base::RefCountedThreadSafe<CodecBufferWaitCoordinator>,
      public gpu::RefCountedLockHelperDrDc {
 public:
  explicit CodecBufferWaitCoordinator(
      scoped_refptr<gpu::NativeImageTextureOwner> texture_owner,
      scoped_refptr<gpu::RefCountedLock> drdc_lock);

  CodecBufferWaitCoordinator(const CodecBufferWaitCoordinator&) = delete;
  CodecBufferWaitCoordinator& operator=(const CodecBufferWaitCoordinator&) =
      delete;

  scoped_refptr<gpu::NativeImageTextureOwner> texture_owner() const {
    DCHECK(texture_owner_);
    return texture_owner_;
  }

  virtual void SetReleaseTimeToNow();

  virtual bool IsExpectingFrameAvailable();

  virtual void WaitForFrameAvailable();

  scoped_refptr<base::SingleThreadTaskRunner> task_runner() {
    return task_runner_;
  }

 protected:
  virtual ~CodecBufferWaitCoordinator();

 private:
  friend class base::RefCountedThreadSafe<CodecBufferWaitCoordinator>;

  scoped_refptr<gpu::NativeImageTextureOwner> texture_owner_;

  base::TimeTicks release_time_;
  scoped_refptr<FrameAvailableEvent> frame_available_event_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

  Tuneable<base::TimeDelta> max_wait_ = {
      "MediaCodecOutputBufferMaxWaitTime", base::Milliseconds(0),
      base::Milliseconds(5), base::Milliseconds(20)};
};

}  // namespace media

#endif  // MEDIA_GPU_OHOS_CODEC_BUFFER_WAIT_COORDINATOR_H_
