/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"
#include "nweb_display_listener.h"

#include "base/logging.h"
#include "ohos_adapter_helper.h"

namespace OHOS::NWeb {
DisplayScreenListener::DisplayScreenListener(
    std::shared_ptr<NWebDelegateInterface> nweb)
    : nweb_(std::weak_ptr<NWebDelegateInterface>(nweb)) {
  display_manager_adapter_ =
      OhosAdapterHelperExt::CreateDisplayMgrAdapter();
}

void DisplayScreenListener::OnCreate(DisplayId id) {}

void DisplayScreenListener::OnDestroy(DisplayId id) {}

void DisplayScreenListener::OnChange(DisplayId id) {
  if (display_manager_adapter_ == nullptr) {
    LOG(ERROR) << "display manager adapter is nullptr";
    return;
  }

  if (id == display_manager_adapter_->GetDefaultDisplayId()) {
    std::shared_ptr<DisplayAdapter> display =
        display_manager_adapter_->GetDefaultDisplay();
    auto nweb = nweb_.lock();
    if (nweb != nullptr && display != nullptr) {
      nweb->NotifyScreenInfoChanged(display->GetRotation(),
                                    display->GetDisplayOrientation());
    } else {
      LOG(ERROR) << "notify screen info change failed";
    }
  }
}

#if BUILDFLAG(ARKWEB_AI)
FoldStatusScreenListener::FoldStatusScreenListener(
    std::shared_ptr<NWebDelegateInterface> nweb)
    : nweb_(std::weak_ptr<NWebDelegateInterface>(nweb)) {
  foldstatus_manager_adapter_ =
      OhosAdapterHelperExt::CreateDisplayMgrAdapter();
}

void FoldStatusScreenListener::OnFoldStatusChanged(FoldStatus foldstatus) {
  if (foldstatus_manager_adapter_ == nullptr) {
    LOG(ERROR) << "foldstatus_manager_adapter_is nullptr";
    return;
  }
  auto nweb = nweb_.lock();
  if (nweb != nullptr) {
    nweb->OnFoldStatusChanged(foldstatus);
  } else {
    LOG(ERROR) << "notify OnFoldStatusChanged info change failed";
  }
}
#endif
}  // namespace OHOS::NWeb
