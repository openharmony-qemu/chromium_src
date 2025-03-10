// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/device/wake_lock/power_save_blocker/power_save_blocker.h"

#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "nweb_screen_lock_tracker.h"

namespace device {

class PowerSaveBlocker::Delegate
    : public base::RefCountedThreadSafe<PowerSaveBlocker::Delegate> {
 public:
  Delegate(mojom::WakeLockType type);

  Delegate(const Delegate&) = delete;
  Delegate& operator=(const Delegate&) = delete;

  void ApplyBlock(const int32_t& id);
  void RemoveBlock(const int32_t& id);

 private:
  friend class base::RefCountedThreadSafe<Delegate>;
  virtual ~Delegate() {}

  mojom::WakeLockType type_;
};

PowerSaveBlocker::Delegate::Delegate(mojom::WakeLockType type): type_(type) {}

void PowerSaveBlocker::Delegate::ApplyBlock(const int32_t& id) {
  switch (type_) {
    case mojom::WakeLockType::kPreventAppSuspension:
      if (id != -1) {
        NWebScreenLockTracker::Instance().Lock(id);
      }
      break;
    case mojom::WakeLockType::kPreventDisplaySleep:
    case mojom::WakeLockType::kPreventDisplaySleepAllowDimming:
      NWebScreenLockTracker::Instance().Lock(id);
      break;
    default:
      LOG(INFO) << "Unhandled block type " << type_;
  }
}

void PowerSaveBlocker::Delegate::RemoveBlock(const int32_t& id) {
  switch (type_) {
    case mojom::WakeLockType::kPreventAppSuspension:
      if (id != -1) {
        NWebScreenLockTracker::Instance().UnLock(id);
      }
      break;
    case mojom::WakeLockType::kPreventDisplaySleep:
    case mojom::WakeLockType::kPreventDisplaySleepAllowDimming:
      NWebScreenLockTracker::Instance().UnLock(id);
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
}

PowerSaveBlocker::~PowerSaveBlocker() {
  if (delegate_.get()) {
    delegate_->RemoveBlock(id_);
  }
}

void PowerSaveBlocker::InitDisplaySleepBlocker(const int32_t id) {
  if (delegate_.get()) {
    delegate_->ApplyBlock(id);
  }
}

}  // namespace device
