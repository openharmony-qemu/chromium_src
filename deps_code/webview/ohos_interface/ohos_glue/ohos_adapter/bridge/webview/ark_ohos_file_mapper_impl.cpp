/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_ohos_file_mapper_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkOhosFileMapperImpl::ArkOhosFileMapperImpl(std::shared_ptr<OHOS::NWeb::OhosFileMapper> ref) : real_(ref) {}

int32_t ArkOhosFileMapperImpl::GetFd()
{
    return real_->GetFd();
}

int32_t ArkOhosFileMapperImpl::GetOffset()
{
    return real_->GetOffset();
}

ArkWebString ArkOhosFileMapperImpl::GetFileName()
{
    return ArkWebStringClassToStruct(real_->GetFileName());
}

bool ArkOhosFileMapperImpl::IsCompressed()
{
    return real_->IsCompressed();
}

void* ArkOhosFileMapperImpl::GetDataPtr()
{
    return real_->GetDataPtr();
}

size_t ArkOhosFileMapperImpl::GetDataLen()
{
    return real_->GetDataLen();
}

bool ArkOhosFileMapperImpl::UnzipData(uint8_t** dest, size_t& len)
{
    return real_->UnzipData(dest, len);
}

} // namespace OHOS::ArkWeb
