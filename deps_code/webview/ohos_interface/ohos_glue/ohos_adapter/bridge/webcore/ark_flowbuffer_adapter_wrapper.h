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

#ifndef ARK_FLOWBUFFER_ADAPTER_WRAPPER_H
#define ARK_FLOWBUFFER_ADAPTER_WRAPPER_H
#pragma once

#include "flowbuffer_adapter.h"
#include "ohos_adapter/include/ark_flowbuffer_adapter.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

class ArkFlowbufferAdapterWrapper : public OHOS::NWeb::FlowbufferAdapter {
public:
    explicit ArkFlowbufferAdapterWrapper(ArkWebRefPtr<ArkFlowbufferAdapter>);

    void StartPerformanceBoost() override;

    void* CreateAshmem(size_t size, int mapType, int& fd) override;

    void* CreateAshmemWithFd(const int fd, size_t size, int mapType) override;

private:
    ArkWebRefPtr<ArkFlowbufferAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_FLOWBUFFER_ADAPTER_WRAPPER_H
