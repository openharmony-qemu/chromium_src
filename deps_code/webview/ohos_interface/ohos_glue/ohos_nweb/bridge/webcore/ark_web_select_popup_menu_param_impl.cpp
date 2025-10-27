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

#include "ohos_nweb/bridge/ark_web_select_popup_menu_param_impl.h"

#include "ohos_nweb/bridge/ark_web_select_menu_bound_impl.h"
#include "ohos_nweb/cpptoc/ark_web_select_popup_menu_item_vector_cpptoc.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebSelectPopupMenuParamImpl::ArkWebSelectPopupMenuParamImpl(
    std::shared_ptr<OHOS::NWeb::NWebSelectPopupMenuParam> nweb_select_popup_menu_param)
    : nweb_select_popup_menu_param_(nweb_select_popup_menu_param)
{}

ArkWebSelectPopupMenuItemVector ArkWebSelectPopupMenuParamImpl::GetMenuItems()
{
    return ArkWebSelectPopupMenuItemVectorClassToStruct(nweb_select_popup_menu_param_->GetMenuItems());
}

int ArkWebSelectPopupMenuParamImpl::GetItemHeight()
{
    return nweb_select_popup_menu_param_->GetItemHeight();
}

int ArkWebSelectPopupMenuParamImpl::GetSelectedItem()
{
    return nweb_select_popup_menu_param_->GetSelectedItem();
}

double ArkWebSelectPopupMenuParamImpl::GetItemFontSize()
{
    return nweb_select_popup_menu_param_->GetItemFontSize();
}

bool ArkWebSelectPopupMenuParamImpl::GetIsRightAligned()
{
    return nweb_select_popup_menu_param_->GetIsRightAligned();
}

ArkWebRefPtr<ArkWebSelectMenuBound> ArkWebSelectPopupMenuParamImpl::GetSelectMenuBound()
{
    std::shared_ptr<OHOS::NWeb::NWebSelectMenuBound> nweb_select_menu_bound =
        nweb_select_popup_menu_param_->GetSelectMenuBound();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_select_menu_bound)) {
        return nullptr;
    }

    return new ArkWebSelectMenuBoundImpl(nweb_select_menu_bound);
}

bool ArkWebSelectPopupMenuParamImpl::GetIsAllowMultipleSelection()
{
    return nweb_select_popup_menu_param_->GetIsAllowMultipleSelection();
}

} // namespace OHOS::ArkWeb
