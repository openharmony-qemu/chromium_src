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

#include "ohos_nweb/bridge/ark_web_engine_prefetch_args_impl.h"

namespace OHOS::ArkWeb {

ArkWebEnginePrefetchArgsImpl::ArkWebEnginePrefetchArgsImpl(
    std::shared_ptr<OHOS::NWeb::NWebEnginePrefetchArgs> nweb_engine_pre_args)
    : nweb_engine_pre_args_(nweb_engine_pre_args)
{}

ArkWebString ArkWebEnginePrefetchArgsImpl::GetUrl()
{
    return ArkWebStringClassToStruct(nweb_engine_pre_args_->GetUrl());
}

ArkWebString ArkWebEnginePrefetchArgsImpl::GetMethod()
{
    return ArkWebStringClassToStruct(nweb_engine_pre_args_->GetMethod());
}

ArkWebString ArkWebEnginePrefetchArgsImpl::GetFormData()
{
    return ArkWebStringClassToStruct(nweb_engine_pre_args_->GetFormData());
}
} // namespace OHOS::ArkWeb
