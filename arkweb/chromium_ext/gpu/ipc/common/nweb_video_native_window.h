/*
 * Copyright (c) 2022-2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NWEB_VIDEO_NATIVE_WINDOW_H
#define NWEB_VIDEO_NATIVE_WINDOW_H

#define NWEB_EXPORT __attribute__((visibility("default")))

#include <map>
#include <mutex>

#include "base/memory/singleton.h"
#include "base/synchronization/lock.h"

class NWEB_EXPORT NWebVideoNativeWindow {
 public:
  static NWebVideoNativeWindow* Get() { return GetInstance(); }
  static NWebVideoNativeWindow* Instance() { return GetInstance(); }

  NWebVideoNativeWindow(const NWebVideoNativeWindow&) = delete;
  NWebVideoNativeWindow& operator=(const NWebVideoNativeWindow&) = delete;

  int32_t AddNativeWindow(void* native_window);
  void* GetNativeWindow(int32_t native_window_id, bool is_ref = false);
  void DestroyNativeWindow(int32_t native_window_id);
  static NWebVideoNativeWindow* GetInstance();

 private:
  NWebVideoNativeWindow();
  ~NWebVideoNativeWindow();

  friend struct base::DefaultSingletonTraits<NWebVideoNativeWindow>;

  mutable base::Lock window_map_lock_;
  std::map<int32_t, void*> native_window_map_;
  int32_t next_native_window_id_;
};

#endif  // NWEB_NATIVE_WINDOW_TRACKER_H
