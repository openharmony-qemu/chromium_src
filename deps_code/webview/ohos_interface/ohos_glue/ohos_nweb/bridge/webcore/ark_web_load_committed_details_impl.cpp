/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohos_nweb/bridge/ark_web_load_committed_details_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebLoadCommittedDetailsImpl::ArkWebLoadCommittedDetailsImpl(
    std::shared_ptr<OHOS::NWeb::NWebLoadCommittedDetails> nweb_load_committed_details)
    : nweb_load_committed_details_(nweb_load_committed_details)
{}

bool ArkWebLoadCommittedDetailsImpl::IsMainFrame()
{
    return nweb_load_committed_details_->IsMainFrame();
}

bool ArkWebLoadCommittedDetailsImpl::IsSameDocument()
{
    return nweb_load_committed_details_->IsSameDocument();
}

bool ArkWebLoadCommittedDetailsImpl::DidReplaceEntry()
{
    return nweb_load_committed_details_->DidReplaceEntry();
}

int ArkWebLoadCommittedDetailsImpl::GetNavigationType()
{
    return static_cast<int>(nweb_load_committed_details_->GetNavigationType());
}

ArkWebString ArkWebLoadCommittedDetailsImpl::GetURL()
{
    return ArkWebStringClassToStruct(nweb_load_committed_details_->GetURL());
}

} // namespace OHOS::ArkWeb
