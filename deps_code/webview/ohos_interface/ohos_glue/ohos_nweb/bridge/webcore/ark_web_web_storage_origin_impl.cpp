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

#include "ohos_nweb/bridge/ark_web_web_storage_origin_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebWebStorageOriginImpl::ArkWebWebStorageOriginImpl(
    std::shared_ptr<OHOS::NWeb::NWebWebStorageOrigin> nweb_web_storage_origin)
    : nweb_web_storage_origin_(nweb_web_storage_origin)
{}

long ArkWebWebStorageOriginImpl::GetQuota()
{
    return nweb_web_storage_origin_->GetQuota();
}

void ArkWebWebStorageOriginImpl::SetQuota(long quota)
{
    nweb_web_storage_origin_->SetQuota(quota);
}

long ArkWebWebStorageOriginImpl::GetUsage()
{
    return nweb_web_storage_origin_->GetUsage();
}

void ArkWebWebStorageOriginImpl::SetUsage(long usage)
{
    nweb_web_storage_origin_->SetUsage(usage);
}

ArkWebString ArkWebWebStorageOriginImpl::GetOrigin()
{
    return ArkWebStringClassToStruct(nweb_web_storage_origin_->GetOrigin());
}

void ArkWebWebStorageOriginImpl::SetOrigin(const ArkWebString& origin)
{
    nweb_web_storage_origin_->SetOrigin(ArkWebStringStructToClass(origin));
}

} // namespace OHOS::ArkWeb
