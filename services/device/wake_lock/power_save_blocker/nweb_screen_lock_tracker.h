// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NWEB_SCREEN_LOCK_TRACKER_H
#define NWEB_SCREEN_LOCK_TRACKER_H

#define NWEB_EXPORT __attribute__((visibility("default")))

#include <mutex>
#include <unordered_map>

using SetKeepScreenOn = std::function<void(bool)>;

class SetKeepScreenLock {
public:
  SetKeepScreenLock(const SetKeepScreenOn& handle): handle_(std::move(handle)) {}

  int32_t count_ = 0;

  SetKeepScreenOn handle_;
};

class SetKeepScreenLockHandle {
public:
  SetKeepScreenLockHandle(int32_t id, const SetKeepScreenOn& handle);

  void AddScreenLockHandle(int32_t id, const SetKeepScreenOn& handle);

  void RemoveScreenLockHandle(int32_t id);

  bool IsEmpty();

  bool Lock(int32_t id);

  bool UnLock(int32_t id);

private:
  int32_t screen_lock_count_ = 0;

  int32_t screen_lock_invalid_id_count_ = 0;

  bool is_screen_on_ = false;

  std::unordered_map<int32_t, std::unique_ptr<SetKeepScreenLock>> handle_map_;
};

class NWEB_EXPORT NWebScreenLockTracker {
 public:
  static NWebScreenLockTracker& Instance();
  void AddScreenLock(int32_t windowId,
                     int32_t id,
                     const SetKeepScreenOn& handle);
  void RemoveScreenLock(int32_t windowId, int32_t id);
  void Lock(int32_t id);
  void UnLock(int32_t id);

 private:
  std::mutex screen_lock_map_lock_;
  std::unordered_map<int32_t, std::unique_ptr<SetKeepScreenLockHandle>> screen_lock_map_;
};

#endif  // NWEB_SCREEN_LOCK_TRACKER_H
