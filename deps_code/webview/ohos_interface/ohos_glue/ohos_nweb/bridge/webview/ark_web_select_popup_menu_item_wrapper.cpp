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

#include "ohos_nweb/bridge/ark_web_select_popup_menu_item_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebSelectPopupMenuItemWrapper::ArkWebSelectPopupMenuItemWrapper(
    ArkWebRefPtr<ArkWebSelectPopupMenuItem> ark_web_select_popup_menu_item)
    : ark_web_select_popup_menu_item_(ark_web_select_popup_menu_item)
{}

NWebSelectPopupMenuItemType ArkWebSelectPopupMenuItemWrapper::GetType()
{
    return static_cast<NWebSelectPopupMenuItemType>(ark_web_select_popup_menu_item_->GetType());
}

std::string ArkWebSelectPopupMenuItemWrapper::GetLabel()
{
    ArkWebString stLabel = ark_web_select_popup_menu_item_->GetLabel();

    std::string objLabel = ArkWebStringStructToClass(stLabel);
    ArkWebStringStructRelease(stLabel);
    return objLabel;
}

uint32_t ArkWebSelectPopupMenuItemWrapper::GetAction()
{
    return ark_web_select_popup_menu_item_->GetAction();
}

std::string ArkWebSelectPopupMenuItemWrapper::GetToolTip()
{
    ArkWebString stToolTip = ark_web_select_popup_menu_item_->GetToolTip();

    std::string objToolTip = ArkWebStringStructToClass(stToolTip);
    ArkWebStringStructRelease(stToolTip);
    return objToolTip;
}

bool ArkWebSelectPopupMenuItemWrapper::GetIsChecked()
{
    return ark_web_select_popup_menu_item_->GetIsChecked();
}

bool ArkWebSelectPopupMenuItemWrapper::GetIsEnabled()
{
    return ark_web_select_popup_menu_item_->GetIsEnabled();
}

NWebTextDirection ArkWebSelectPopupMenuItemWrapper::GetTextDirection()
{
    return static_cast<NWebTextDirection>(ark_web_select_popup_menu_item_->GetTextDirection());
}

bool ArkWebSelectPopupMenuItemWrapper::GetHasTextDirectionOverride()
{
    return ark_web_select_popup_menu_item_->GetHasTextDirectionOverride();
}

} // namespace OHOS::ArkWeb
