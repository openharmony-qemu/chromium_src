/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef NATIVE_ABILITY_RUNTIME_ADAPTER_IMPL_H
#define NATIVE_ABILITY_RUNTIME_ADAPTER_IMPL_H

#include <string>
#include <AbilityKit/ability_runtime/application_context.h>

namespace OHOS::NWeb {

class NativeAbilityRuntimeAdapterImpl {
public:
    NativeAbilityRuntimeAdapterImpl() = default;

    ~NativeAbilityRuntimeAdapterImpl() = default;

    bool NotifyPageChanged(const char* targetPageName, int32_t targetPageNameLength, int32_t windowId);
};
} // namespace
#endif // NATIVE_ABILITY_RUNTIME_ADAPTER_IMPL_H
