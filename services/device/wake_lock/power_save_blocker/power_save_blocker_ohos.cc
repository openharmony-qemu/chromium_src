// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "ohos/adapter/runninglock/runninglock_adapter.h"
#include "services/device/wake_lock/power_save_blocker/power_save_blocker.h"

namespace device {

class PowerSaveBlocker::Delegate
    : public base::RefCountedThreadSafe<PowerSaveBlocker::Delegate> {
 public:
  Delegate(mojom::WakeLockType type) : type_(type) {}

  Delegate(const Delegate&) = delete;
  Delegate& operator=(const Delegate&) = delete;

  void ApplyBlock(const int32_t& id);
  void RemoveBlock(const int32_t& id);

 private:
  mojom::WakeLockType type_;
  friend class base::RefCountedThreadSafe<Delegate>;
  ~Delegate() {}
};

void PowerSaveBlocker::Delegate::ApplyBlock(const int32_t& id) {
  if (type_ != mojom::WakeLockType::kPreventDisplaySleep) {
    return;
  }

  switch (type_) {
    case mojom::WakeLockType::kPreventAppSuspension:
      if (id != -1) {
        ohos::adapter::runninglock::RunningLockAdapter::GetInstance().Start();
      }
      break;
    case mojom::WakeLockType::kPreventDisplaySleep:
    case mojom::WakeLockType::kPreventDisplaySleepAllowDimming:
      ohos::adapter::runninglock::RunningLockAdapter::GetInstance().Start();
      break;
    default:
      LOG(INFO) << "Unhandled block type " << type_;
  }
}

void PowerSaveBlocker::Delegate::RemoveBlock(const int32_t& id) {
  if (type_ != mojom::WakeLockType::kPreventDisplaySleep) {
    return;
  }

  switch (type_) {
    case mojom::WakeLockType::kPreventAppSuspension:
      if (id != -1) {
        ohos::adapter::runninglock::RunningLockAdapter::GetInstance().Stop();
      }
      break;
    case mojom::WakeLockType::kPreventDisplaySleep:
    case mojom::WakeLockType::kPreventDisplaySleepAllowDimming:
      ohos::adapter::runninglock::RunningLockAdapter::GetInstance().Stop();
      break;
    default:
      LOG(INFO) << "Unhandled block type " << type_;
  }
}

PowerSaveBlocker::PowerSaveBlocker(
    mojom::WakeLockType type,
    mojom::WakeLockReason reason,
    const std::string& description,
    scoped_refptr<base::SequencedTaskRunner> ui_task_runner,
    scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner,
    int32_t id)
    : delegate_(new Delegate(type)),
      ui_task_runner_(ui_task_runner),
      blocking_task_runner_(blocking_task_runner),
      id_(id) {
  if (delegate_.get()) {
    delegate_->ApplyBlock(id);
  }
}

PowerSaveBlocker::~PowerSaveBlocker() {
  if (delegate_.get()) {
    delegate_->RemoveBlock(id_);
  }
}

}  // namespace device
