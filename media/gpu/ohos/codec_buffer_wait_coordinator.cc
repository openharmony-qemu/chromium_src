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

#include "media/gpu/ohos/codec_buffer_wait_coordinator.h"
#include "base/metrics/histogram_macros.h"
#include "base/task/single_thread_task_runner.h"

namespace media {

struct FrameAvailableEvent
    : public base::RefCountedThreadSafe<FrameAvailableEvent> {
  FrameAvailableEvent()
      : event(base::WaitableEvent::ResetPolicy::AUTOMATIC,
              base::WaitableEvent::InitialState::NOT_SIGNALED) {}
  void Signal() { event.Signal(); }
  base::WaitableEvent event;

 private:
  friend class RefCountedThreadSafe<FrameAvailableEvent>;
  ~FrameAvailableEvent() = default;
};

CodecBufferWaitCoordinator::CodecBufferWaitCoordinator(
    scoped_refptr<gpu::NativeImageTextureOwner> texture_owner,
    scoped_refptr<gpu::RefCountedLock> drdc_lock)
    : RefCountedLockHelperDrDc(std::move(drdc_lock)),
      texture_owner_(std::move(texture_owner)),
      frame_available_event_(new FrameAvailableEvent()),
      task_runner_(base::SingleThreadTaskRunner::GetCurrentDefault()) {
  DCHECK(texture_owner_);
  texture_owner_->SetFrameAvailableCallback(base::BindRepeating(
      &FrameAvailableEvent::Signal, frame_available_event_));
}

CodecBufferWaitCoordinator::~CodecBufferWaitCoordinator() {
  DCHECK(texture_owner_);
}

void CodecBufferWaitCoordinator::SetReleaseTimeToNow() {
  AssertAcquiredDrDcLock();
  release_time_ = base::TimeTicks::Now();
}

bool CodecBufferWaitCoordinator::IsExpectingFrameAvailable() {
  AssertAcquiredDrDcLock();
  return !release_time_.is_null();
}

void CodecBufferWaitCoordinator::WaitForFrameAvailable() {
  AssertAcquiredDrDcLock();
  DCHECK(!release_time_.is_null());

  const base::TimeTicks call_time = base::TimeTicks::Now();
  const base::TimeDelta elapsed = call_time - release_time_;
  const base::TimeDelta remaining = max_wait_.value() - elapsed;
  release_time_ = base::TimeTicks();
  bool timed_out = false;

  if (remaining <= base::TimeDelta()) {
    if (!frame_available_event_->event.IsSignaled()) {
      LOG(ERROR) << "Deferred WaitForFrameAvailable() timed out, elapsed: "
               << elapsed.InMillisecondsF() << "ms";
      timed_out = true;
    }
  } else {
    DCHECK_LE(remaining, max_wait_.value());
    if (!frame_available_event_->event.TimedWait(remaining)) {
      LOG(ERROR) << "WaitForFrameAvailable() timed out, elapsed: "
               << elapsed.InMillisecondsF()
               << "ms, additionally waited: " << remaining.InMillisecondsF()
               << "ms, total: " << (elapsed + remaining).InMillisecondsF()
               << "ms";
      timed_out = true;
    }
  }
}

}  // namespace media
