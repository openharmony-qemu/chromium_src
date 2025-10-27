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

#include "ohos_nweb/bridge/ark_web_print_attributes_adapter_wrapper.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

bool NWebPrintAttributesAdapterWrapper::GetBool(uint32_t attrId)
{
    if (ref_) {
        return ref_->GetBool(attrId);
    }
    return false;
}

uint32_t NWebPrintAttributesAdapterWrapper::GetUInt32(uint32_t attrId)
{
    if (ref_) {
        return ref_->GetUInt32(attrId);
    }
    return 0;
}

std::string NWebPrintAttributesAdapterWrapper::GetString(uint32_t attrId)
{
    if (ref_) {
        ArkWebString str = ref_->GetString(attrId);
        std::string ret = ArkWebStringStructToClass(str);
        ArkWebStringStructRelease(str);
        return ret;
    }
    return "";
}

std::vector<uint32_t> NWebPrintAttributesAdapterWrapper::GetUint32Vector(uint32_t attrId)
{
    if (ref_) {
        ArkWebUint32Vector data = ref_->GetUint32Vector(attrId);
        std::vector<uint32_t> ret = ArkWebBasicVectorStructToClass<uint32_t, ArkWebUint32Vector>(data);
        ArkWebBasicVectorStructRelease<ArkWebUint32Vector>(data);
        return ret;
    }
    return std::vector<uint32_t>();
}

} // namespace OHOS::ArkWeb