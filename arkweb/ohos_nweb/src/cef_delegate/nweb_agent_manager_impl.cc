/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ohos_nweb/src/cef_delegate/nweb_agent_manager_impl.h"

#include "ohos_nweb/src/cef_delegate/nweb_delegate.h"
#include "ohos_nweb/src/cef_delegate/nweb_preference_delegate.h"

namespace OHOS::NWeb {
NWebAgentManagerImpl::NWebAgentManagerImpl(
    base::WeakPtr<NWebDelegate> nweb_delegate)
    : nweb_delegate_(nweb_delegate) {}

void NWebAgentManagerImpl::SetAgentEnabled(bool enabled) {
    std::shared_ptr<NWebPreferenceDelegate> pref = 
        nweb_delegate_ ? nweb_delegate_->preference_delegate_ : nullptr;
    if (pref) {
        pref->PutArkwebAgentEnabled(enabled);
    }
}

bool NWebAgentManagerImpl::IsAgentEnabled() {
    std::shared_ptr<NWebPreferenceDelegate> pref = 
        nweb_delegate_ ? nweb_delegate_->preference_delegate_ : nullptr;
    return pref ? pref->GetArkwebAgentEnabled() : false;
}
}  // namespace OHOS::NWeb