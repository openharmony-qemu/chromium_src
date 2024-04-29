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

#include "ohos_adapter/cpptoc/ark_mmidevice_info_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int32_t ARK_WEB_CALLBACK
ark_mmidevice_info_adapter_get_id(struct _ark_mmidevice_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkMMIDeviceInfoAdapterCppToC::Get(self)->GetId();
}

int32_t ARK_WEB_CALLBACK ark_mmidevice_info_adapter_get_type(
    struct _ark_mmidevice_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkMMIDeviceInfoAdapterCppToC::Get(self)->GetType();
}

int32_t ARK_WEB_CALLBACK
ark_mmidevice_info_adapter_get_bus(struct _ark_mmidevice_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkMMIDeviceInfoAdapterCppToC::Get(self)->GetBus();
}

int32_t ARK_WEB_CALLBACK ark_mmidevice_info_adapter_get_version(
    struct _ark_mmidevice_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkMMIDeviceInfoAdapterCppToC::Get(self)->GetVersion();
}

int32_t ARK_WEB_CALLBACK ark_mmidevice_info_adapter_get_product(
    struct _ark_mmidevice_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkMMIDeviceInfoAdapterCppToC::Get(self)->GetProduct();
}

int32_t ARK_WEB_CALLBACK ark_mmidevice_info_adapter_get_vendor(
    struct _ark_mmidevice_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkMMIDeviceInfoAdapterCppToC::Get(self)->GetVendor();
}

ArkWebString ARK_WEB_CALLBACK ark_mmidevice_info_adapter_get_name(
    struct _ark_mmidevice_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkMMIDeviceInfoAdapterCppToC::Get(self)->GetName();
}

ArkWebString ARK_WEB_CALLBACK ark_mmidevice_info_adapter_get_phys(
    struct _ark_mmidevice_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkMMIDeviceInfoAdapterCppToC::Get(self)->GetPhys();
}

ArkWebString ARK_WEB_CALLBACK ark_mmidevice_info_adapter_get_uniq(
    struct _ark_mmidevice_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkMMIDeviceInfoAdapterCppToC::Get(self)->GetUniq();
}

void ARK_WEB_CALLBACK ark_mmidevice_info_adapter_set_id(
    struct _ark_mmidevice_info_adapter_t *self, int32_t id) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkMMIDeviceInfoAdapterCppToC::Get(self)->SetId(id);
}

void ARK_WEB_CALLBACK ark_mmidevice_info_adapter_set_type(
    struct _ark_mmidevice_info_adapter_t *self, int32_t type) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkMMIDeviceInfoAdapterCppToC::Get(self)->SetType(type);
}

void ARK_WEB_CALLBACK ark_mmidevice_info_adapter_set_bus(
    struct _ark_mmidevice_info_adapter_t *self, int32_t bus) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkMMIDeviceInfoAdapterCppToC::Get(self)->SetBus(bus);
}

void ARK_WEB_CALLBACK ark_mmidevice_info_adapter_set_version(
    struct _ark_mmidevice_info_adapter_t *self, int32_t version) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkMMIDeviceInfoAdapterCppToC::Get(self)->SetVersion(version);
}

void ARK_WEB_CALLBACK ark_mmidevice_info_adapter_set_product(
    struct _ark_mmidevice_info_adapter_t *self, int32_t product) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkMMIDeviceInfoAdapterCppToC::Get(self)->SetProduct(product);
}

void ARK_WEB_CALLBACK ark_mmidevice_info_adapter_set_vendor(
    struct _ark_mmidevice_info_adapter_t *self, int32_t vendor) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkMMIDeviceInfoAdapterCppToC::Get(self)->SetVendor(vendor);
}

void ARK_WEB_CALLBACK ark_mmidevice_info_adapter_set_name(
    struct _ark_mmidevice_info_adapter_t *self, ArkWebString name) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkMMIDeviceInfoAdapterCppToC::Get(self)->SetName(name);
}

void ARK_WEB_CALLBACK ark_mmidevice_info_adapter_set_phys(
    struct _ark_mmidevice_info_adapter_t *self, ArkWebString phys) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkMMIDeviceInfoAdapterCppToC::Get(self)->SetPhys(phys);
}

void ARK_WEB_CALLBACK ark_mmidevice_info_adapter_set_uniq(
    struct _ark_mmidevice_info_adapter_t *self, ArkWebString uniq) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkMMIDeviceInfoAdapterCppToC::Get(self)->SetUniq(uniq);
}

} // namespace

ArkMMIDeviceInfoAdapterCppToC::ArkMMIDeviceInfoAdapterCppToC() {
  GetStruct()->get_id = ark_mmidevice_info_adapter_get_id;
  GetStruct()->get_type = ark_mmidevice_info_adapter_get_type;
  GetStruct()->get_bus = ark_mmidevice_info_adapter_get_bus;
  GetStruct()->get_version = ark_mmidevice_info_adapter_get_version;
  GetStruct()->get_product = ark_mmidevice_info_adapter_get_product;
  GetStruct()->get_vendor = ark_mmidevice_info_adapter_get_vendor;
  GetStruct()->get_name = ark_mmidevice_info_adapter_get_name;
  GetStruct()->get_phys = ark_mmidevice_info_adapter_get_phys;
  GetStruct()->get_uniq = ark_mmidevice_info_adapter_get_uniq;
  GetStruct()->set_id = ark_mmidevice_info_adapter_set_id;
  GetStruct()->set_type = ark_mmidevice_info_adapter_set_type;
  GetStruct()->set_bus = ark_mmidevice_info_adapter_set_bus;
  GetStruct()->set_version = ark_mmidevice_info_adapter_set_version;
  GetStruct()->set_product = ark_mmidevice_info_adapter_set_product;
  GetStruct()->set_vendor = ark_mmidevice_info_adapter_set_vendor;
  GetStruct()->set_name = ark_mmidevice_info_adapter_set_name;
  GetStruct()->set_phys = ark_mmidevice_info_adapter_set_phys;
  GetStruct()->set_uniq = ark_mmidevice_info_adapter_set_uniq;
}

ArkMMIDeviceInfoAdapterCppToC::~ArkMMIDeviceInfoAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkMMIDeviceInfoAdapterCppToC, ArkMMIDeviceInfoAdapter,
    ark_mmidevice_info_adapter_t>::kBridgeType = ARK_MMIDEVICE_INFO_ADAPTER;

} // namespace OHOS::ArkWeb
