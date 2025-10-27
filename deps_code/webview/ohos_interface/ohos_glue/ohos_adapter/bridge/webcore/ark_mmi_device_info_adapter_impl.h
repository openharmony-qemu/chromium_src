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

#ifndef ARK_MMI_DEVICE_INFO_ADAPTER_IMPL_H
#define ARK_MMI_DEVICE_INFO_ADAPTER_IMPL_H
#pragma once

#include "mmi_adapter.h"
#include "ohos_adapter/include/ark_mmi_adapter.h"

namespace OHOS::ArkWeb {

class ArkMMIDeviceInfoAdapterImpl : public ArkMMIDeviceInfoAdapter {
public:
    explicit ArkMMIDeviceInfoAdapterImpl(std::shared_ptr<OHOS::NWeb::MMIDeviceInfoAdapter>);

    int32_t GetId() override;

    int32_t GetType() override;

    int32_t GetBus() override;

    int32_t GetVersion() override;

    int32_t GetProduct() override;

    int32_t GetVendor() override;

    ArkWebString GetName() override;

    ArkWebString GetPhys() override;

    ArkWebString GetUniq() override;

    void SetId(int32_t id) override;

    void SetType(int32_t type) override;

    void SetBus(int32_t bus) override;

    void SetVersion(int32_t version) override;

    void SetProduct(int32_t product) override;

    void SetVendor(int32_t vendor) override;

    void SetName(ArkWebString name) override;

    void SetPhys(ArkWebString phys) override;

    void SetUniq(ArkWebString uniq) override;

private:
    std::shared_ptr<OHOS::NWeb::MMIDeviceInfoAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkMMIDeviceInfoAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_MMI_DEVICE_INFO_ADAPTER_IMPL_H
