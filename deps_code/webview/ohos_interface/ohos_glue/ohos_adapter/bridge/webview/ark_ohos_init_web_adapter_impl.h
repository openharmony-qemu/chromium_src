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

#ifndef ARK_OHOS_INIT_WEB_ADAPTER_IMPL_H
#define ARK_OHOS_INIT_WEB_ADAPTER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_ohos_init_web_adapter.h"
#include "ohos_init_web_adapter.h"

namespace OHOS::ArkWeb {

class ArkOhosInitWebAdapterImpl : public ArkOhosInitWebAdapter {
public:
    explicit ArkOhosInitWebAdapterImpl(std::shared_ptr<OHOS::NWeb::OhosInitWebAdapter>);

    void* GetRunWebInitedCallback() override;

    void SetRunWebInitedCallback(void* callback) override;

private:
    std::shared_ptr<OHOS::NWeb::OhosInitWebAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkOhosInitWebAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // OHOS_INIT_WEB_ADAPTER_H
