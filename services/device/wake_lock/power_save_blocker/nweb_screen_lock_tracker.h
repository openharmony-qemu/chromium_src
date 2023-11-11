// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NWEB_SCREEN_LOCK_TRACKER_H
#define NWEB_SCREEN_LOCK_TRACKER_H

#define NWEB_EXPORT __attribute__((visibility("default")))

#include <mutex>
#include <unordered_map>

using SetKeepScreenOn = std::function<void(bool)>;

class NWEB_EXPORT NWebScreenLockTracker {
 public:
  static NWebScreenLockTracker& Instance();
  void AddScreenLock(int32_t windowId,
                     int32_t nwebId,
                     const SetKeepScreenOn& handle);
  void RemoveScreenLock(int32_t windowId, int32_t nwebId);
  void Lock();
  void UnLock();

 private:
  std::mutex screen_lock_map_lock_;
  SetKeepScreenOn screen_on_ = nullptr;
  int32_t count_ = 0;
  bool is_screen_on_ = false;
};

#endif  // NWEB_SCREEN_LOCK_TRACKER_H