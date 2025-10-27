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

#include "ohos_nweb/bridge/ark_web_web_storage_wrapper.h"

#include "ohos_nweb/bridge/ark_web_long_value_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_web_storage_origin_vector_value_callback_impl.h"
#include "ohos_nweb/ctocpp/ark_web_web_storage_origin_vector_ctocpp.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebWebStorageWrapper::ArkWebWebStorageWrapper(ArkWebRefPtr<ArkWebWebStorage> ark_web_web_storage)
    : ark_web_web_storage_(ark_web_web_storage)
{}

std::vector<std::shared_ptr<OHOS::NWeb::NWebWebStorageOrigin>> ArkWebWebStorageWrapper::GetOrigins()
{
    return ArkWebWebStorageOriginVectorStructToClass(ark_web_web_storage_->GetOrigins());
}

void ArkWebWebStorageWrapper::GetOrigins(std::shared_ptr<OHOS::NWeb::NWebWebStorageOriginVectorValueCallback> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_web_storage_->GetOrigins(nullptr);
        return;
    }

    ark_web_web_storage_->GetOrigins(new ArkWebWebStorageOriginVectorValueCallbackImpl(callback));
}

long ArkWebWebStorageWrapper::GetOriginQuota(const std::string& origin)
{
    ArkWebString stOrigin = ArkWebStringClassToStruct(origin);

    long code = ark_web_web_storage_->GetOriginQuota(stOrigin);

    ArkWebStringStructRelease(stOrigin);
    return code;
}

void ArkWebWebStorageWrapper::GetOriginQuota(
    const std::string& origin, std::shared_ptr<OHOS::NWeb::NWebLongValueCallback> callback)
{
    ArkWebString stOrigin = ArkWebStringClassToStruct(origin);

    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_web_storage_->GetOriginQuota(stOrigin, nullptr);
        return;
    } else {
        ark_web_web_storage_->GetOriginQuota(stOrigin, new ArkWebLongValueCallbackImpl(callback));
    }

    ArkWebStringStructRelease(stOrigin);
}

long ArkWebWebStorageWrapper::GetOriginUsage(const std::string& origin)
{
    ArkWebString stOrigin = ArkWebStringClassToStruct(origin);

    long code = ark_web_web_storage_->GetOriginUsage(stOrigin);

    ArkWebStringStructRelease(stOrigin);
    return code;
}

void ArkWebWebStorageWrapper::GetOriginUsage(
    const std::string& origin, std::shared_ptr<OHOS::NWeb::NWebLongValueCallback> callback)
{
    ArkWebString stOrigin = ArkWebStringClassToStruct(origin);

    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_web_storage_->GetOriginUsage(stOrigin, nullptr);
    } else {
        ark_web_web_storage_->GetOriginUsage(stOrigin, new ArkWebLongValueCallbackImpl(callback));
    }

    ArkWebStringStructRelease(stOrigin);
}

int ArkWebWebStorageWrapper::DeleteOrigin(const std::string& origin)
{
    ArkWebString stOrigin = ArkWebStringClassToStruct(origin);

    int code = ark_web_web_storage_->DeleteOrigin(stOrigin);

    ArkWebStringStructRelease(stOrigin);
    return code;
}

void ArkWebWebStorageWrapper::DeleteAllData(bool incognito_mode)
{
    ark_web_web_storage_->DeleteAllData(incognito_mode);
}

} // namespace OHOS::ArkWeb
