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

#include "ohos_nweb/bridge/ark_web_select_popup_menu_item_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebSelectPopupMenuItemImpl::ArkWebSelectPopupMenuItemImpl(
    std::shared_ptr<OHOS::NWeb::NWebSelectPopupMenuItem> nweb_select_popup_menu_item)
    : nweb_select_popup_menu_item_(nweb_select_popup_menu_item)
{}

int ArkWebSelectPopupMenuItemImpl::GetType()
{
    return static_cast<int>(nweb_select_popup_menu_item_->GetType());
}

ArkWebString ArkWebSelectPopupMenuItemImpl::GetLabel()
{
    return ArkWebStringClassToStruct(nweb_select_popup_menu_item_->GetLabel());
}

uint32_t ArkWebSelectPopupMenuItemImpl::GetAction()
{
    return nweb_select_popup_menu_item_->GetAction();
}

ArkWebString ArkWebSelectPopupMenuItemImpl::GetToolTip()
{
    return ArkWebStringClassToStruct(nweb_select_popup_menu_item_->GetToolTip());
}

bool ArkWebSelectPopupMenuItemImpl::GetIsChecked()
{
    return nweb_select_popup_menu_item_->GetIsChecked();
}

bool ArkWebSelectPopupMenuItemImpl::GetIsEnabled()
{
    return nweb_select_popup_menu_item_->GetIsEnabled();
}

int ArkWebSelectPopupMenuItemImpl::GetTextDirection()
{
    return static_cast<int>(nweb_select_popup_menu_item_->GetTextDirection());
}

bool ArkWebSelectPopupMenuItemImpl::GetHasTextDirectionOverride()
{
    return nweb_select_popup_menu_item_->GetHasTextDirectionOverride();
}

} // namespace OHOS::ArkWeb
