// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/device/wake_lock/power_save_blocker/power_save_blocker.h"

#include <mutex>
#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "services/device/wake_lock/power_save_blocker/power_save_blocker.h"
#include "arkweb/chromium_ext/service/device/wake_lock/power_save_blocker/nweb_screen_lock_tracker.h"

namespace device {

class PowerSaveBlocker::Delegate {
 public:
  Delegate() = default;
  ~Delegate() = default;

  Delegate(const Delegate&) = delete;
  Delegate& operator=(const Delegate&) = delete;

  // 初始化方法
  void Init(mojom::WakeLockType type) {
    type_ = type;
  }

  void ApplyBlock(const int32_t& id);
  void RemoveBlock(const int32_t& id);

 private:
  mojom::WakeLockType type_ = mojom::WakeLockType::kPreventAppSuspension;
  std::map<int32_t, mojom::WakeLockType> lock_map_;
  std::mutex lock_map_mutex_;
};

void PowerSaveBlocker::Delegate::ApplyBlock(const int32_t& id) {
  std::lock_guard<std::mutex> lock(lock_map_mutex_);
  switch (type_) {
    case mojom::WakeLockType::kPreventAppSuspension:
      if (id != -1) {
        lock_map_.emplace(id, type_);
        NWebScreenLockTracker::Instance().Lock(id);
      }
      break;
    case mojom::WakeLockType::kPreventDisplaySleep:
    case mojom::WakeLockType::kPreventDisplaySleepAllowDimming:
      lock_map_.emplace(id, type_);
      NWebScreenLockTracker::Instance().Lock(id);
      break;
    default:
      LOG(INFO) << "Unhandled block type " << type_;
  }
}

void PowerSaveBlocker::Delegate::RemoveBlock(const int32_t& id) {
  std::lock_guard<std::mutex> lock(lock_map_mutex_);
  if (lock_map_.find(id) == lock_map_.end() || type_ != lock_map_[id]) {
    LOG(WARNING) << "The lock dose not exist, id: " << id
                 << ", type_: " << type_;
    return;
  }

  switch (type_) {
    case mojom::WakeLockType::kPreventAppSuspension:
      if (id != -1) {
        lock_map_.erase(id);
        NWebScreenLockTracker::Instance().UnLock(id);
      }
      break;
    case mojom::WakeLockType::kPreventDisplaySleep:
    case mojom::WakeLockType::kPreventDisplaySleepAllowDimming:
      lock_map_.erase(id);
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
    int32_t id)
    : delegate_(std::move(ui_task_runner)),
      id_(id) {
  // 使用SequenceBound AsyncCall方法初始化
  delegate_.AsyncCall(&Delegate::Init).WithArgs(type);
}

PowerSaveBlocker::~PowerSaveBlocker() {
  delegate_.AsyncCall(&Delegate::RemoveBlock).WithArgs(id_);
}

void PowerSaveBlocker::InitDisplaySleepBlocker(const int32_t id) {
  delegate_.AsyncCall(&Delegate::ApplyBlock).WithArgs(id);
}

}  // namespace device
