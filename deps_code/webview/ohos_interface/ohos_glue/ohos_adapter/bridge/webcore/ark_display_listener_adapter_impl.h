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

#ifndef ARK_DISPLAY_LISTENER_ADAPTER_IMPL_H
#define ARK_DISPLAY_LISTENER_ADAPTER_IMPL_H
#pragma once

#include "display_manager_adapter.h"
#include "ohos_adapter/include/ark_display_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkDisplayListenerAdapterImpl : public ArkDisplayListenerAdapter {
public:
    explicit ArkDisplayListenerAdapterImpl(std::shared_ptr<OHOS::NWeb::DisplayListenerAdapter>);

    void OnCreate(uint64_t displayId) override;

    void OnDestroy(uint64_t displayId) override;

    void OnChange(uint64_t displayId) override;

private:
    std::shared_ptr<OHOS::NWeb::DisplayListenerAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkDisplayListenerAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_DISPLAY_LISTENER_ADAPTER_IMPL_H
