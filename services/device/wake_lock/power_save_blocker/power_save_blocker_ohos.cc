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

#include "services/device/wake_lock/power_save_blocker/power_save_blocker.h"

#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "ohos/adapter/runninglock/runninglock_adapter.h"

namespace device {

class PowerSaveBlocker::Delegate
    : public base::RefCountedThreadSafe<PowerSaveBlocker::Delegate> {
 public:
  Delegate(mojom::WakeLockType type) : type_(type) {}

  Delegate(const Delegate&) = delete;
  Delegate& operator=(const Delegate&) = delete;

  void ApplyBlock();
  void RemoveBlock();

 private:
  mojom::WakeLockType type_;
  friend class base::RefCountedThreadSafe<Delegate>;
  ~Delegate() {}
};

void PowerSaveBlocker::Delegate::ApplyBlock() {
  if (type_ != mojom::WakeLockType::kPreventDisplaySleep) {
    return;
  }
  ohos::adapter::runninglock::RunningLockAdapter::GetInstance().Start();
}

void PowerSaveBlocker::Delegate::RemoveBlock() {
  if (type_ != mojom::WakeLockType::kPreventDisplaySleep) {
    return;
  }
  ohos::adapter::runninglock::RunningLockAdapter::GetInstance().Stop();
}

PowerSaveBlocker::PowerSaveBlocker(
    mojom::WakeLockType type,
    mojom::WakeLockReason reason,
    const std::string& description,
    scoped_refptr<base::SequencedTaskRunner> ui_task_runner,
    scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner)
    : delegate_(new Delegate(type)),
      ui_task_runner_(ui_task_runner),
      blocking_task_runner_(blocking_task_runner) {
  if (delegate_.get()) {
    delegate_->ApplyBlock();
  }
}

PowerSaveBlocker::~PowerSaveBlocker() {
  if (delegate_.get()) {
    delegate_->RemoveBlock();
  }
}

}  // namespace device
