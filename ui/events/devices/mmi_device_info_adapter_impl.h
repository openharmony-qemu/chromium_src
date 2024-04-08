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

#ifndef MMI_DEVICE_INFO_ADAPTER_IMPL_H
#define MMI_DEVICE_INFO_ADAPTER_IMPL_H

#include "mmi_adapter.h"

namespace OHOS::NWeb {

struct MMIDeviceInfo {
 public:
  int32_t id = -1;
  int32_t type = 0;
  int32_t bus = 0;
  int32_t version = 0;
  int32_t product = 0;
  int32_t vendor = 0;
  std::string name;
  std::string phys;
  std::string uniq;
};

class MMIDeviceInfoAdapterImpl : public MMIDeviceInfoAdapter {
 public:
  MMIDeviceInfoAdapterImpl() = default;

  int32_t GetId() override;

  int32_t GetType() override;

  int32_t GetBus() override;

  int32_t GetVersion() override;

  int32_t GetProduct() override;

  int32_t GetVendor() override;

  std::string GetName() override;

  std::string GetPhys() override;

  std::string GetUniq() override;

  void SetId(int32_t id) override;

  void SetType(int32_t type) override;

  void SetBus(int32_t bus) override;

  void SetVersion(int32_t version) override;

  void SetProduct(int32_t product) override;

  void SetVendor(int32_t vendor) override;

  void SetName(std::string name) override;

  void SetPhys(std::string phys) override;

  void SetUniq(std::string uniq) override;

 private:
  int32_t id_ = -1;
  int32_t type_ = 0;
  int32_t bus_ = 0;
  int32_t version_ = 0;
  int32_t product_ = 0;
  int32_t vendor_ = 0;
  std::string name_ = "";
  std::string phys_ = "";
  std::string uniq_ = "";
};

}  // namespace OHOS::NWeb

struct OHOS::NWeb::MMIDeviceInfo transformToMMIDeviceInfo(
    std::shared_ptr<OHOS::NWeb::MMIDeviceInfoAdapter> adapter);

#endif  // MMI_DEVICE_INFO_ADAPTER_IMPL_H
