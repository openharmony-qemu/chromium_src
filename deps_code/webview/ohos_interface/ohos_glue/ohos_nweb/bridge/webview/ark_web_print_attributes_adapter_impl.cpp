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

#include "ohos_nweb/bridge/ark_web_print_attributes_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

bool ArkWebPrintAttributesAdapterImpl::GetBool(uint32_t attrId)
{
    if (ref_) {
        return ref_->GetBool(attrId);
    }
    return false;
}

uint32_t ArkWebPrintAttributesAdapterImpl::GetUInt32(uint32_t attrId)
{
    if (ref_) {
        return ref_->GetUInt32(attrId);
    }
    return 0;
}

ArkWebString ArkWebPrintAttributesAdapterImpl::GetString(uint32_t attrId)
{
    if (ref_) {
        return ArkWebStringClassToStruct(ref_->GetString(attrId));
    }
    return ArkWebStringClassToStruct(std::string(""));
}

ArkWebUint32Vector ArkWebPrintAttributesAdapterImpl::GetUint32Vector(uint32_t attrId)
{
    if (ref_) {
        return ArkWebBasicVectorClassToStruct<uint32_t, ArkWebUint32Vector>(ref_->GetUint32Vector(attrId));
    }
    return ArkWebBasicVectorClassToStruct<uint32_t, ArkWebUint32Vector>(std::vector<uint32_t>());
}

} // namespace OHOS::ArkWeb