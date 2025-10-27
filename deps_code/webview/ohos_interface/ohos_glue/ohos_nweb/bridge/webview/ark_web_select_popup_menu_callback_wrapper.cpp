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

#include "ohos_nweb/bridge/ark_web_select_popup_menu_callback_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebSelectPopupMenuCallbackWrapper::ArkWebSelectPopupMenuCallbackWrapper(
    ArkWebRefPtr<ArkWebSelectPopupMenuCallback> ark_web_select_popup_menu_callback)
    : ark_web_select_popup_menu_callback_(ark_web_select_popup_menu_callback)
{}

void ArkWebSelectPopupMenuCallbackWrapper::Cancel()
{
    ark_web_select_popup_menu_callback_->Cancel();
}

void ArkWebSelectPopupMenuCallbackWrapper::Continue(const std::vector<int32_t>& indices)
{
    ArkWebInt32Vector stIndices = ArkWebBasicVectorClassToStruct<int32_t, ArkWebInt32Vector>(indices);

    ark_web_select_popup_menu_callback_->Continue(stIndices);

    ArkWebBasicVectorStructRelease<ArkWebInt32Vector>(stIndices);
}

} // namespace OHOS::ArkWeb
