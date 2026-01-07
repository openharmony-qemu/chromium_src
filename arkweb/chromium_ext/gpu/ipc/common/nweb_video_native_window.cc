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

#include "nweb_video_native_window.h"

#include "base/check.h"
#include "base/logging.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"

//LCOV_EXCL_START
NWebVideoNativeWindow::NWebVideoNativeWindow()
    : next_native_window_id_(1) {}

NWebVideoNativeWindow::~NWebVideoNativeWindow() {}

NWebVideoNativeWindow* NWebVideoNativeWindow::GetInstance() {
  return base::Singleton<NWebVideoNativeWindow>::get();
}
//LCOV_EXCL_STOP

int32_t NWebVideoNativeWindow::AddNativeWindow(void* native_window) {
  base::AutoLock lock(window_map_lock_);
  int32_t native_window_id = next_native_window_id_++;
  native_window_map_.emplace(native_window_id, std::move((void*)native_window));
  LOG(DEBUG) << "Add native window id = " << native_window_id;
  return native_window_id;
}

void* NWebVideoNativeWindow::GetNativeWindow(int32_t native_window_id, bool is_ref) {
  base::AutoLock lock(window_map_lock_);
  auto it = native_window_map_.find(native_window_id);
  if (it == native_window_map_.end()) {
    return nullptr;
  }

  LOG(DEBUG) << __FUNCTION__
             << "Get for native_window id = " << native_window_id;
  if (is_ref) {
    OHOS::NWeb::OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().AddNativeWindowRef(it->second);
  }
  return it->second;
}

void NWebVideoNativeWindow::DestroyNativeWindow(int32_t native_window_id) {
  base::AutoLock lock(window_map_lock_);
  auto it = native_window_map_.find(native_window_id);
  if (it != native_window_map_.end()) {
    native_window_map_.erase(native_window_id);
    LOG(DEBUG) << __FUNCTION__
               << "Destroy native_window id = " << native_window_id;
  }
}
