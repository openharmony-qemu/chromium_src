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

#include "ohos_nweb/bridge/ark_web_web_storage_impl.h"

#include "ohos_nweb/bridge/ark_web_long_value_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_web_storage_origin_vector_value_callback_wrapper.h"
#include "ohos_nweb/cpptoc/ark_web_web_storage_origin_vector_cpptoc.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebWebStorageImpl::ArkWebWebStorageImpl(std::shared_ptr<OHOS::NWeb::NWebWebStorage> nweb_web_storage)
    : nweb_web_storage_(nweb_web_storage)
{}

ArkWebWebStorageOriginVector ArkWebWebStorageImpl::GetOrigins()
{
    return ArkWebWebStorageOriginVectorClassToStruct(nweb_web_storage_->GetOrigins());
}

void ArkWebWebStorageImpl::GetOrigins(ArkWebRefPtr<ArkWebWebStorageOriginVectorValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_web_storage_->GetOrigins(nullptr);
        return;
    }

    nweb_web_storage_->GetOrigins(std::make_shared<ArkWebWebStorageOriginVectorValueCallbackWrapper>(callback));
}

long ArkWebWebStorageImpl::GetOriginQuota(const ArkWebString& origin)
{
    return nweb_web_storage_->GetOriginQuota(ArkWebStringStructToClass(origin));
}

void ArkWebWebStorageImpl::GetOriginQuota(const ArkWebString& origin, ArkWebRefPtr<ArkWebLongValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_web_storage_->GetOriginQuota(ArkWebStringStructToClass(origin), nullptr);
        return;
    }

    nweb_web_storage_->GetOriginQuota(
        ArkWebStringStructToClass(origin), std::make_shared<ArkWebLongValueCallbackWrapper>(callback));
}

long ArkWebWebStorageImpl::GetOriginUsage(const ArkWebString& origin)
{
    return nweb_web_storage_->GetOriginUsage(ArkWebStringStructToClass(origin));
}

void ArkWebWebStorageImpl::GetOriginUsage(const ArkWebString& origin, ArkWebRefPtr<ArkWebLongValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_web_storage_->GetOriginUsage(ArkWebStringStructToClass(origin), nullptr);
        return;
    }

    nweb_web_storage_->GetOriginUsage(
        ArkWebStringStructToClass(origin), std::make_shared<ArkWebLongValueCallbackWrapper>(callback));
}

int ArkWebWebStorageImpl::DeleteOrigin(const ArkWebString& origin)
{
    return nweb_web_storage_->DeleteOrigin(ArkWebStringStructToClass(origin));
}

void ArkWebWebStorageImpl::DeleteAllData(bool incognito_mode)
{
    nweb_web_storage_->DeleteAllData(incognito_mode);
}

} // namespace OHOS::ArkWeb
