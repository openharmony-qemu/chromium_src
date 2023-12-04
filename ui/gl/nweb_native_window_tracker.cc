// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "nweb_native_window_tracker.h"

NWebNativeWindowTracker& NWebNativeWindowTracker::Instance() {
  static NWebNativeWindowTracker tracker;
  return tracker;
}

uint32_t NWebNativeWindowTracker::AddNativeWindow(void* native_window) {
  std::lock_guard<std::mutex> lock(native_window_map_lock_);
  uint32_t native_window_id = ++next_native_window_id_;
  native_window_map_.emplace(native_window_id, (void*)native_window);
  return native_window_id;
}

void* NWebNativeWindowTracker::GetNativeWindow(uint32_t native_window_id) {
  std::lock_guard<std::mutex> lock(native_window_map_lock_);
  auto it = native_window_map_.find(native_window_id);
  if (it == native_window_map_.end()) {
    return nullptr;
  }
  return it->second;
}
