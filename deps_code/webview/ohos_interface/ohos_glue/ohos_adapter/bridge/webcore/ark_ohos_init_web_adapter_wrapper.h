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

#ifndef ARK_OHOS_INIT_WEB_ADAPTER_WRAPPER_H
#define ARK_OHOS_INIT_WEB_ADAPTER_WRAPPER_H
#pragma once

#include "ohos_adapter/include/ark_ohos_init_web_adapter.h"
#include "ohos_init_web_adapter.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

class ArkOhosInitWebAdapterWrapper : public OHOS::NWeb::OhosInitWebAdapter {
public:
    explicit ArkOhosInitWebAdapterWrapper(ArkWebRefPtr<ArkOhosInitWebAdapter>);

    NWeb::WebRunInitedCallback* GetRunWebInitedCallback() override;

    void SetRunWebInitedCallback(NWeb::WebRunInitedCallback* callback) override;

private:
    ArkWebRefPtr<ArkOhosInitWebAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_OHOS_INIT_WEB_ADAPTER_WRAPPER_H
