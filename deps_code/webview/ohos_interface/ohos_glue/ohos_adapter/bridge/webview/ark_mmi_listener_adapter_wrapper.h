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

#ifndef ARK_MMI_LISTENER_ADAPTER_WRAPPER_H
#define ARK_MMI_LISTENER_ADAPTER_WRAPPER_H
#pragma once

#include "mmi_adapter.h"
#include "ohos_adapter/include/ark_mmi_adapter.h"

namespace OHOS::ArkWeb {

class ArkMMIListenerAdapterWrapper : public OHOS::NWeb::MMIListenerAdapter {
public:
    explicit ArkMMIListenerAdapterWrapper(ArkWebRefPtr<ArkMMIListenerAdapter>);

    void OnDeviceAdded(int32_t deviceId, const std::string& type) override;
    void OnDeviceRemoved(int32_t deviceId, const std::string& type) override;

private:
    ArkWebRefPtr<ArkMMIListenerAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_MMI_LISTENER_ADAPTER_WRAPPER_H
