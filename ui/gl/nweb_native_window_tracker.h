// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NWEB_NATIVE_WINDOW_TRACKER_H
#define NWEB_NATIVE_WINDOW_TRACKER_H

#define NWEB_EXPORT __attribute__((visibility("default")))

#include <mutex>
#include <unordered_map>

class NWEB_EXPORT NWebNativeWindowTracker {
 public:
  static NWebNativeWindowTracker& Instance();
  uint32_t AddNativeWindow(void* native_window);
  void* GetNativeWindow(uint32_t native_window_id);

 private:
  std::mutex native_window_map_lock_;
  std::unordered_map<uint32_t, void*> native_window_map_;
  uint32_t next_native_window_id_ = 0;
};

#endif  // NWEB_NATIVE_WINDOW_TRACKER_H
