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

#ifndef ARK_WEB_ENGINE_PREFTCH_ARGS_WRAPPER_H_
#define ARK_WEB_ENGINE_PREFTCH_ARGS_WRAPPER_H_
#pragma once

#include "include/nweb.h"
#include "ohos_nweb/include/ark_web_engine_prefetch_args.h"

namespace OHOS::ArkWeb {

class ArkWebEnginePrefetchArgsWrapper : public OHOS::NWeb::NWebEnginePrefetchArgs {
public:
    ArkWebEnginePrefetchArgsWrapper(ArkWebRefPtr<ArkWebEnginePrefetchArgs> ark_web_engine_pre_args);
    ~ArkWebEnginePrefetchArgsWrapper() = default;

    std::string GetUrl() override;

    std::string GetMethod() override;

    std::string GetFormData() override;

private:
    ArkWebRefPtr<ArkWebEnginePrefetchArgs> ark_web_engine_pre_args_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_ENGINE_PREFTCH_ARGS_WRAPPER_H_
