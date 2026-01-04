/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
#include "arkweb/chromium_ext/content/public/browser/error_page_reload_reason.h"

void NavigationEntryImpl::SetReloadReason(ErrorPageReloadReason  reason) {
  current_reload_reason_ = reason;
  if (reason == ErrorPageReloadReason ::INVALID) {
    return;
  }
 
  if (reload_reason_list_.find(reason) == reload_reason_list_.end()) {
    reload_reason_list_.insert(reason);
  } else {
    LOG(INFO) << "Reload reason " << (int)reason << " has existed";
  }
}
#endif
