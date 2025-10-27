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

#ifndef ARK_MMI_ADAPTER_WRAPPER_H
#define ARK_MMI_ADAPTER_WRAPPER_H
#pragma once

#include "mmi_adapter.h"
#include "ohos_adapter/include/ark_mmi_adapter.h"

namespace OHOS::ArkWeb {

class ArkMMIAdapterWrapper : public OHOS::NWeb::MMIAdapter {
public:
    explicit ArkMMIAdapterWrapper(ArkWebRefPtr<ArkMMIAdapter>);

    char* KeyCodeToString(int32_t keyCode) override;

    int32_t RegisterMMIInputListener(std::shared_ptr<NWeb::MMIInputListenerAdapter> eventCallback) override;

    void UnregisterMMIInputListener(int32_t monitorId) override;

    int32_t RegisterDevListener(std::string type, std::shared_ptr<NWeb::MMIListenerAdapter> listener) override;

    int32_t UnregisterDevListener(std::string type) override;

    int32_t GetKeyboardType(int32_t deviceId, int32_t& type) override;

    int32_t GetDeviceIds(std::vector<int32_t>& ids) override;

    int32_t GetDeviceInfo(int32_t deviceId, std::shared_ptr<NWeb::MMIDeviceInfoAdapter> info) override;

    int32_t GetMaxTouchPoints() override;

private:
    ArkWebRefPtr<ArkMMIAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_MMI_ADAPTER_WRAPPER_H
