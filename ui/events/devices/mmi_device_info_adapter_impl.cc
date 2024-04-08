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

#include "mmi_device_info_adapter_impl.h"

struct OHOS::NWeb::MMIDeviceInfo transformToMMIDeviceInfo(
    std::shared_ptr<OHOS::NWeb::MMIDeviceInfoAdapter> adapter) {
  struct OHOS::NWeb::MMIDeviceInfo info;
  if (!adapter) {
    return info;
  }

  info.id = adapter->GetId();
  info.type = adapter->GetType();
  info.bus = adapter->GetBus();
  info.version = adapter->GetVersion();
  info.product = adapter->GetProduct();
  info.vendor = adapter->GetVendor();
  info.name = adapter->GetName();
  info.phys = adapter->GetPhys();
  info.uniq = adapter->GetUniq();
  return info;
}

namespace OHOS::NWeb {

int32_t MMIDeviceInfoAdapterImpl::GetId() {
  return id_;
}

int32_t MMIDeviceInfoAdapterImpl::GetType() {
  return type_;
}

int32_t MMIDeviceInfoAdapterImpl::GetBus() {
  return bus_;
}

int32_t MMIDeviceInfoAdapterImpl::GetVersion() {
  return version_;
}

int32_t MMIDeviceInfoAdapterImpl::GetProduct() {
  return product_;
}

int32_t MMIDeviceInfoAdapterImpl::GetVendor() {
  return vendor_;
}

std::string MMIDeviceInfoAdapterImpl::GetName() {
  return name_;
}

std::string MMIDeviceInfoAdapterImpl::GetPhys() {
  return phys_;
}

std::string MMIDeviceInfoAdapterImpl::GetUniq() {
  return uniq_;
}

void MMIDeviceInfoAdapterImpl::SetId(int32_t id) {
  id_ = id;
}

void MMIDeviceInfoAdapterImpl::SetType(int32_t type) {
  type_ = type;
}

void MMIDeviceInfoAdapterImpl::SetBus(int32_t bus) {
  bus_ = bus;
}

void MMIDeviceInfoAdapterImpl::SetVersion(int32_t version) {
  version_ = version;
}

void MMIDeviceInfoAdapterImpl::SetProduct(int32_t product) {
  product_ = product;
}

void MMIDeviceInfoAdapterImpl::SetVendor(int32_t vendor) {
  vendor_ = vendor;
}

void MMIDeviceInfoAdapterImpl::SetName(std::string name) {
  name_ = name;
}

void MMIDeviceInfoAdapterImpl::SetPhys(std::string phys) {
  phys_ = phys;
}

void MMIDeviceInfoAdapterImpl::SetUniq(std::string uniq) {
  uniq_ = uniq;
}

}  // namespace OHOS::NWeb
