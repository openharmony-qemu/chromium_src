// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "nweb_screen_lock_tracker.h"
#include "base/logging.h"

NWebScreenLockTracker& NWebScreenLockTracker::Instance() {
  static NWebScreenLockTracker tracker;
  return tracker;
}

void NWebScreenLockTracker::AddScreenLock(int32_t windowId,
                                          int32_t nwebId,
                                          const SetKeepScreenOn& handle) {
  if (windowId < 0 || !handle) {
    LOG(ERROR) << "invalid windowId or handle";
    return;
  }
  std::lock_guard<std::mutex> lock(screen_lock_map_lock_);
  if (!screen_on_) {
    LOG(DEBUG) << "register screen on function";
    screen_on_ = std::move(handle);
  }
}

void NWebScreenLockTracker::RemoveScreenLock(int32_t windowId, int32_t nwebId) {
  LOG(DEBUG) << "no need to unregister";
}

void NWebScreenLockTracker::Lock() {
  std::lock_guard<std::mutex> lock(screen_lock_map_lock_);
  if (!screen_on_) {
    LOG(ERROR) << "set screen on function is nullptr";
    return;
  }
  count_++;
  if (count_ == 0) {
    LOG(ERROR) << "nweb screen lock wrong, count_ = " << count_;
    return;
  }
  if (is_screen_on_) {
    LOG(DEBUG) << "nweb screen lock ignore, count_ = " << count_;
    return;
  }
  is_screen_on_ = true;
  screen_on_(true);
}

void NWebScreenLockTracker::UnLock() {
  std::lock_guard<std::mutex> lock(screen_lock_map_lock_);
  if (!screen_on_) {
    LOG(ERROR) << "set screen on function is nullptr";
    return;
  }
  count_--;
  if (count_ < 0) {
    LOG(ERROR) << "nweb screen unlock wrong, count_ = " << count_;
    return;
  }
  if (!is_screen_on_ || count_ > 0) {
    LOG(DEBUG) << "nweb screen unlock ignore, count_ = " << count_;
    return;
  }
  is_screen_on_ = false;
  screen_on_(false);
}
