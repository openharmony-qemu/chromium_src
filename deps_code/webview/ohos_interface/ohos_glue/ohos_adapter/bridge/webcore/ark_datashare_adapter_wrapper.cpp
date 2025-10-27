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

#include "ohos_adapter/bridge/ark_datashare_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkDatashareAdapterWrapper::ArkDatashareAdapterWrapper(ArkWebRefPtr<ArkDatashareAdapter> ref) : ctocpp_(ref) {}

int ArkDatashareAdapterWrapper::OpenDataShareUriForRead(const std::string& uriStr)
{
    if (!ctocpp_) {
        return -1;
    }

    ArkWebString str = ArkWebStringClassToStruct(uriStr);
    int result = ctocpp_->OpenDataShareUriForRead(str);

    ArkWebStringStructRelease(str);
    return result;
}

std::string ArkDatashareAdapterWrapper::GetFileDisplayName(const std::string& uriStr)
{
    if (!ctocpp_) {
        return "";
    }

    ArkWebString ark_str = ArkWebStringClassToStruct(uriStr);
    ArkWebString ark_result = ctocpp_->GetFileDisplayName(ark_str);

    std::string result = ArkWebStringStructToClass(ark_result);

    ArkWebStringStructRelease(ark_str);
    ArkWebStringStructRelease(ark_result);
    return result;
}

std::string ArkDatashareAdapterWrapper::GetRealPath(const std::string& uriStr)
{
    if (!ctocpp_) {
        return "";
    }

    ArkWebString ark_str = ArkWebStringClassToStruct(uriStr);
    ArkWebString ark_result = ctocpp_->GetRealPath(ark_str);

    std::string result = ArkWebStringStructToClass(ark_result);

    ArkWebStringStructRelease(ark_str);
    ArkWebStringStructRelease(ark_result);
    return result;
}

} // namespace OHOS::ArkWeb
