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

#include "ohos_adapter/bridge/ark_imf_text_config_adapter_impl.h"

#include "ohos_adapter/bridge/ark_imf_cursor_info_adapter_impl.h"
#include "ohos_adapter/bridge/ark_imf_input_attribute_adapter_impl.h"
#include "ohos_adapter/bridge/ark_imf_selection_range_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkIMFTextConfigAdapterImpl::ArkIMFTextConfigAdapterImpl(std::shared_ptr<OHOS::NWeb::IMFTextConfigAdapter> ref)
    : real_(ref)
{}

ArkWebRefPtr<ArkIMFInputAttributeAdapter> ArkIMFTextConfigAdapterImpl::GetInputAttribute()
{
    std::shared_ptr<NWeb::IMFInputAttributeAdapter> attribute = real_->GetInputAttribute();
    if (CHECK_SHARED_PTR_IS_NULL(attribute)) {
        return nullptr;
    }
    return new ArkIMFInputAttributeAdapterImpl(attribute);
}

ArkWebRefPtr<ArkIMFCursorInfoAdapter> ArkIMFTextConfigAdapterImpl::GetCursorInfo()
{
    std::shared_ptr<NWeb::IMFCursorInfoAdapter> cursorInfo = real_->GetCursorInfo();
    if (CHECK_SHARED_PTR_IS_NULL(cursorInfo)) {
        return nullptr;
    }
    return new ArkIMFCursorInfoAdapterImpl(cursorInfo);
}

ArkWebRefPtr<ArkIMFSelectionRangeAdapter> ArkIMFTextConfigAdapterImpl::GetSelectionRange()
{
    std::shared_ptr<NWeb::IMFSelectionRangeAdapter> range = real_->GetSelectionRange();
    if (CHECK_SHARED_PTR_IS_NULL(range)) {
        return nullptr;
    }
    return new ArkIMFSelectionRangeAdapterImpl(range);
}

uint32_t ArkIMFTextConfigAdapterImpl::GetWindowId()
{
    return real_->GetWindowId();
}

double ArkIMFTextConfigAdapterImpl::GetPositionY()
{
    return real_->GetPositionY();
}

double ArkIMFTextConfigAdapterImpl::GetHeight()
{
    return real_->GetHeight();
}
} // namespace OHOS::ArkWeb
