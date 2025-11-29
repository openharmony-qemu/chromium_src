// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
