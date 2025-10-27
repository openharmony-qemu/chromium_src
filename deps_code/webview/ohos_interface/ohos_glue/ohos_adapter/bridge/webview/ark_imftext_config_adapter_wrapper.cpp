/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_imftext_config_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_imfcursor_info_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_imfinput_attribute_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_imfselection_range_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkIMFTextConfigAdapterWrapper::ArkIMFTextConfigAdapterWrapper(ArkWebRefPtr<ArkIMFTextConfigAdapter> ref) : ctocpp_(ref)
{}

std::shared_ptr<NWeb::IMFInputAttributeAdapter> ArkIMFTextConfigAdapterWrapper::GetInputAttribute()
{
    ArkWebRefPtr<ArkIMFInputAttributeAdapter> adapter = ctocpp_->GetInputAttribute();
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }
    return std::make_shared<ArkIMFInputAttributeAdapterWrapper>(adapter);
}

std::shared_ptr<NWeb::IMFCursorInfoAdapter> ArkIMFTextConfigAdapterWrapper::GetCursorInfo()
{
    ArkWebRefPtr<ArkIMFCursorInfoAdapter> adapter = ctocpp_->GetCursorInfo();
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }
    return std::make_shared<ArkIMFCursorInfoAdapterWrapper>(adapter);
}

std::shared_ptr<NWeb::IMFSelectionRangeAdapter> ArkIMFTextConfigAdapterWrapper::GetSelectionRange()
{
    ArkWebRefPtr<ArkIMFSelectionRangeAdapter> adapter = ctocpp_->GetSelectionRange();
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }
    return std::make_shared<ArkIMFSelectionRangeAdapterWrapper>(adapter);
}

uint32_t ArkIMFTextConfigAdapterWrapper::GetWindowId()
{
    return ctocpp_->GetWindowId();
}

double ArkIMFTextConfigAdapterWrapper::GetPositionY()
{
    return ctocpp_->GetPositionY();
}

double ArkIMFTextConfigAdapterWrapper::GetHeight()
{
    return ctocpp_->GetHeight();
}
} // namespace OHOS::ArkWeb
