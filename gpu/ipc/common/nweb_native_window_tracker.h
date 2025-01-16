// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NWEB_NATIVE_WINDOW_TRACKER_H
#define NWEB_NATIVE_WINDOW_TRACKER_H

#define NWEB_EXPORT __attribute__((visibility("default")))

#include <mutex>
#include <map>

#include "base/memory/singleton.h"
#include "base/synchronization/lock.h"
#include "ohos_adapter_helper.h"

class NWEB_EXPORT NWebNativeWindowTracker {
public:
  static NWebNativeWindowTracker* Get() { return GetInstance(); }
  static NWebNativeWindowTracker* Instance() { return GetInstance(); }

  NWebNativeWindowTracker(const NWebNativeWindowTracker&) = delete;
  NWebNativeWindowTracker& operator=(const NWebNativeWindowTracker&) = delete;

  int32_t AddNativeWindow(void* native_window);
  void* GetNativeWindow(int32_t native_window_id);
  void DestroyNativeWindow(int32_t native_window_id);
  static NWebNativeWindowTracker* GetInstance();
  std::shared_ptr<OHOS::NWeb::AafwkBrowserClientAdapter> g_browser_client_;
private:
  NWebNativeWindowTracker();
  ~NWebNativeWindowTracker();

  friend struct base::DefaultSingletonTraits<NWebNativeWindowTracker>;

  mutable base::Lock window_map_lock_;
  std::map<int32_t, void*> native_window_map_;
  int32_t next_native_window_id_;
};

#endif // NWEB_NATIVE_WINDOW_TRACKER_H
