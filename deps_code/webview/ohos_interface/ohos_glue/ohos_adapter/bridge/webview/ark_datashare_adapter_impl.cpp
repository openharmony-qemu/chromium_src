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

#include "ohos_adapter/bridge/ark_datashare_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkDatashareAdapterImpl::ArkDatashareAdapterImpl(NWeb::DatashareAdapter& ref) : real_(ref) {}

int ArkDatashareAdapterImpl::OpenDataShareUriForRead(const ArkWebString& uriStr)
{
    std::string str = ArkWebStringStructToClass(uriStr);
    return real_.OpenDataShareUriForRead(str);
}

ArkWebString ArkDatashareAdapterImpl::GetFileDisplayName(const ArkWebString& uriStr)
{
    std::string str = ArkWebStringStructToClass(uriStr);
    std::string result = real_.GetFileDisplayName(str);
    return ArkWebStringClassToStruct(result);
}

ArkWebString ArkDatashareAdapterImpl::GetRealPath(const ArkWebString& uriStr)
{
    std::string str = ArkWebStringStructToClass(uriStr);
    std::string result = real_.GetRealPath(str);
    return ArkWebStringClassToStruct(result);
}

} // namespace OHOS::ArkWeb
