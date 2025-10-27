/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARK_ONCE_CALLBACK_ADAPTER_IMPL_H
#define ARK_ONCE_CALLBACK_ADAPTER_IMPL_H
#pragma once

#include "event_handler_adapter.h"
#include "ohos_adapter/include/ark_event_handler_adapter.h"

namespace OHOS::ArkWeb {

class ArkOnceCallbackAdapterImpl : public ArkOnceCallbackAdapter {
public:
    explicit ArkOnceCallbackAdapterImpl(std::shared_ptr<OHOS::NWeb::OnceCallbackAdapter>);

    void OnRunnable() override;

private:
    std::shared_ptr<OHOS::NWeb::OnceCallbackAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkOnceCallbackAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_ONCE_CALLBACK_ADAPTER_IMPL_H
