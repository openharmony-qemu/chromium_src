// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "nweb_screen_lock_tracker.h"
#include "base/logging.h"

#include <memory>

NWebScreenLockTracker& NWebScreenLockTracker::Instance() {
  static NWebScreenLockTracker tracker;
  return tracker;
}

SetKeepScreenLockHandle::SetKeepScreenLockHandle(int32_t id, const SetKeepScreenOn& handle)
    : screen_lock_count_(0), screen_lock_invalid_id_count_(0), is_screen_on_(false) {
  AddScreenLockHandle(id, std::move(handle));
}

void SetKeepScreenLockHandle::AddScreenLockHandle(int32_t id, const SetKeepScreenOn& handle) {
  auto iter = handle_map_.find(id);
  if (iter != handle_map_.end()) {
    return;
  }
  std::unique_ptr<SetKeepScreenLock> screen_lock(new SetKeepScreenLock(std::move(handle)));
  handle_map_.emplace(id, std::move(screen_lock));
}

void SetKeepScreenLockHandle::RemoveScreenLockHandle(int32_t id) {
  auto iter = handle_map_.find(id);
  if (iter == handle_map_.end()) {
    LOG(DEBUG) << "nweb screen remove handle id not fount";
    return;
  }
  LOG(DEBUG) << "nweb screen remove handle iter.count:" << iter->second->count_ << ", count:" << screen_lock_count_;
  screen_lock_count_ = screen_lock_count_ - iter->second->count_;
  if (is_screen_on_ && (screen_lock_count_ <= 0 || handle_map_.size() == 1)) {
    is_screen_on_ = false;
    screen_lock_count_ = 0;
    iter->second->handle_(false);
  }
  handle_map_.erase(id);
}

bool SetKeepScreenLockHandle::IsEmpty() {
  return handle_map_.empty();
}

bool SetKeepScreenLockHandle::Lock(int32_t id) {
  if (id == -1) {
    if (!handle_map_.empty()) {
      screen_lock_count_++;
      screen_lock_invalid_id_count_++;
      LOG(DEBUG) << "nweb screen lock invalid_id_count:" << screen_lock_invalid_id_count_
                 << ", count:" << screen_lock_count_;
      if (!is_screen_on_ && screen_lock_count_ > 0) {
        is_screen_on_ = true;
        handle_map_.begin()->second->handle_(true);
      }
    }
    return false;
  }
  auto iter = handle_map_.find(id);
  if (iter == handle_map_.end()) {
    return false;
  }
  screen_lock_count_++;
  iter->second->count_++;
  LOG(DEBUG) << "nweb screen lock iter.count:" << iter->second->count_ << ", count:" << screen_lock_count_;
  if (screen_lock_count_ <= 0) {
    LOG(ERROR) << "nweb screen lock error, count:" << screen_lock_count_;
    return false;
  }
  if (is_screen_on_) {
    LOG(DEBUG) << "nweb screen lock ignore";
    return true;
  }
  is_screen_on_ = true;
  iter->second->handle_(true);
  return true;
}

bool SetKeepScreenLockHandle::UnLock(int32_t id) {
  if (id == -1) {
    if (!handle_map_.empty() && screen_lock_count_ > 0 && screen_lock_invalid_id_count_ > 0) {
      screen_lock_count_--;
      screen_lock_invalid_id_count_--;
      LOG(DEBUG) << "nweb screen unlock invalid_id_count:" << screen_lock_invalid_id_count_
                 << ", count:" << screen_lock_count_;
      if (screen_lock_count_ == 0 && is_screen_on_) {
        is_screen_on_ = false;
        handle_map_.begin()->second->handle_(false);
      }
    }
    return false;
  }
  auto iter = handle_map_.find(id);
  if (iter == handle_map_.end()) {
    return false;
  }
  screen_lock_count_--;
  iter->second->count_--;
  LOG(DEBUG) << "nweb screen unlock iter.count:" << iter->second->count_ << ", count:" << screen_lock_count_;
  if (screen_lock_count_ < 0) {
    screen_lock_count_ = 0;
    iter->second->count_ = 0;
    LOG(ERROR) << "nweb screen unlock error";
    return false;
  }
  if (!is_screen_on_ || screen_lock_count_ > 0) {
    LOG(DEBUG) << "nweb screen unlock ignore";
    return true;
  }
  is_screen_on_ = false;
  iter->second->handle_(false);
  return true;
}

void NWebScreenLockTracker::AddScreenLock(int32_t windowId,
                                          int32_t id,
                                          const SetKeepScreenOn& handle) {
  LOG(DEBUG) << "nweb screen add screen windowId:" << windowId << ", id:" << id;
  std::lock_guard<std::mutex> lock(screen_lock_map_lock_);
  auto iter = screen_lock_map_.find(windowId);
  if (iter != screen_lock_map_.end()) {
    iter->second->AddScreenLockHandle(id, std::move(handle));
    return;
  }

  std::unique_ptr<SetKeepScreenLockHandle> screen_lock_handle(new SetKeepScreenLockHandle(id, std::move(handle)));
  screen_lock_map_.emplace(windowId, std::move(screen_lock_handle));
}

void NWebScreenLockTracker::RemoveScreenLock(int32_t windowId, int32_t id) {
  LOG(DEBUG) << "nweb screen remove screen windowId:" << windowId << ", id:" << id;
  std::lock_guard<std::mutex> lock(screen_lock_map_lock_);
  auto iter = screen_lock_map_.find(windowId);
  if (iter == screen_lock_map_.end()) {
    return;
  }

  iter->second->RemoveScreenLockHandle(id);
  if (iter->second->IsEmpty()) {
    screen_lock_map_.erase(windowId);
  }
}

void NWebScreenLockTracker::Lock(int32_t id) {
  std::lock_guard<std::mutex> lock(screen_lock_map_lock_);
  for (auto window_iter = screen_lock_map_.begin(); window_iter != screen_lock_map_.end(); ++window_iter) {
    if (window_iter->second->Lock(id)) {
      return;
    }
  }
}

void NWebScreenLockTracker::UnLock(int32_t id) {
  std::lock_guard<std::mutex> lock(screen_lock_map_lock_);
  for (auto window_iter = screen_lock_map_.begin(); window_iter != screen_lock_map_.end(); ++window_iter) {
    if (window_iter->second->UnLock(id)) {
      return;
    }
  }
}
