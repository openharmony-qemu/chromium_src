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

#include "ohos_nweb/bridge/ark_web_engine_prefetch_args_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebEnginePrefetchArgsWrapper::ArkWebEnginePrefetchArgsWrapper(
    ArkWebRefPtr<ArkWebEnginePrefetchArgs> ark_web_engine_pre_args)
    : ark_web_engine_pre_args_(ark_web_engine_pre_args)
{}

std::string ArkWebEnginePrefetchArgsWrapper::GetUrl()
{
    ArkWebString stUrl = ark_web_engine_pre_args_->GetUrl();

    std::string objUrl = ArkWebStringStructToClass(stUrl);
    ArkWebStringStructRelease(stUrl);
    return objUrl;
}

std::string ArkWebEnginePrefetchArgsWrapper::GetMethod()
{
    ArkWebString stMethod = ark_web_engine_pre_args_->GetMethod();

    std::string objMethod = ArkWebStringStructToClass(stMethod);
    ArkWebStringStructRelease(stMethod);
    return objMethod;
}

std::string ArkWebEnginePrefetchArgsWrapper::GetFormData()
{
    ArkWebString stFormData = ark_web_engine_pre_args_->GetFormData();

    std::string objFormData = ArkWebStringStructToClass(stFormData);
    ArkWebStringStructRelease(stFormData);
    return objFormData;
}

} // namespace OHOS::ArkWeb
