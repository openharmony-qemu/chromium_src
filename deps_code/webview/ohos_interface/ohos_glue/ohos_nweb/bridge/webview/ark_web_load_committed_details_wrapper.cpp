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

#include "ohos_nweb/bridge/ark_web_load_committed_details_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebLoadCommittedDetailsWrapper::ArkWebLoadCommittedDetailsWrapper(
    ArkWebRefPtr<ArkWebLoadCommittedDetails> ark_web_load_committed_details)
    : ark_web_load_committed_details_(ark_web_load_committed_details)
{}

bool ArkWebLoadCommittedDetailsWrapper::IsMainFrame()
{
    return ark_web_load_committed_details_->IsMainFrame();
}

bool ArkWebLoadCommittedDetailsWrapper::IsSameDocument()
{
    return ark_web_load_committed_details_->IsSameDocument();
}

bool ArkWebLoadCommittedDetailsWrapper::DidReplaceEntry()
{
    return ark_web_load_committed_details_->DidReplaceEntry();
}

ArkWebNavigationType ArkWebLoadCommittedDetailsWrapper::GetNavigationType()
{
    return static_cast<ArkWebNavigationType>(ark_web_load_committed_details_->GetNavigationType());
}

std::string ArkWebLoadCommittedDetailsWrapper::GetURL()
{
    ArkWebString stUrl = ark_web_load_committed_details_->GetURL();

    std::string objUrl = ArkWebStringStructToClass(stUrl);
    ArkWebStringStructRelease(stUrl);
    return objUrl;
}

} // namespace OHOS::ArkWeb
