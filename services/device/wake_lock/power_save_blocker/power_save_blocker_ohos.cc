// Copyright (c) 2021 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/device/wake_lock/power_save_blocker/power_save_blocker.h"

#include "base/memory/ref_counted.h"
#include "base/logging.h"
#include "nweb_screen_lock_tracker.h"

namespace device {

class PowerSaveBlocker::Delegate
    : public base::RefCountedThreadSafe<PowerSaveBlocker::Delegate> {
 public:
  Delegate();

  Delegate(const Delegate&) = delete;
  Delegate& operator=(const Delegate&) = delete;

  void ApplyBlock();
  void RemoveBlock();

 private:
  friend class base::RefCountedThreadSafe<Delegate>;
  virtual ~Delegate() {}
};

PowerSaveBlocker::Delegate::Delegate() {}

void PowerSaveBlocker::Delegate::ApplyBlock() {
  NWebScreenLockTracker::Instance().Lock();
}

void PowerSaveBlocker::Delegate::RemoveBlock() {
  NWebScreenLockTracker::Instance().UnLock();
}

PowerSaveBlocker::PowerSaveBlocker(
    mojom::WakeLockType type,
    mojom::WakeLockReason reason,
    const std::string& description,
    scoped_refptr<base::SequencedTaskRunner> ui_task_runner,
    scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner)
    : delegate_(new Delegate()),
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
