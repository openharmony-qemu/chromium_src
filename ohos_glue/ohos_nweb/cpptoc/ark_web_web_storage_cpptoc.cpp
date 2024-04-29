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

#include "ohos_nweb/cpptoc/ark_web_web_storage_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_nweb/ctocpp/ark_web_long_value_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_web_storage_origin_vector_value_callback_ctocpp.h"

namespace OHOS::ArkWeb {

namespace {

ArkWebWebStorageOriginVector ARK_WEB_CALLBACK
ark_web_web_storage_get_origins1(struct _ark_web_web_storage_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_web_storage_origin_vector_default);

  // Execute
  return ArkWebWebStorageCppToC::Get(self)->GetOrigins();
}

void ARK_WEB_CALLBACK ark_web_web_storage_get_origins2(
    struct _ark_web_web_storage_t *self,
    ark_web_web_storage_origin_vector_value_callback_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebWebStorageCppToC::Get(self)->GetOrigins(
      ArkWebWebStorageOriginVectorValueCallbackCToCpp::Invert(callback));
}

long ARK_WEB_CALLBACK ark_web_web_storage_get_origin_quota1(
    struct _ark_web_web_storage_t *self, const ArkWebString *origin) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(origin, 0);

  // Execute
  return ArkWebWebStorageCppToC::Get(self)->GetOriginQuota(*origin);
}

void ARK_WEB_CALLBACK ark_web_web_storage_get_origin_quota2(
    struct _ark_web_web_storage_t *self, const ArkWebString *origin,
    ark_web_long_value_callback_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(origin, );

  // Execute
  ArkWebWebStorageCppToC::Get(self)->GetOriginQuota(
      *origin, ArkWebLongValueCallbackCToCpp::Invert(callback));
}

long ARK_WEB_CALLBACK ark_web_web_storage_get_origin_usage1(
    struct _ark_web_web_storage_t *self, const ArkWebString *origin) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(origin, 0);

  // Execute
  return ArkWebWebStorageCppToC::Get(self)->GetOriginUsage(*origin);
}

void ARK_WEB_CALLBACK ark_web_web_storage_get_origin_usage2(
    struct _ark_web_web_storage_t *self, const ArkWebString *origin,
    ark_web_long_value_callback_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(origin, );

  // Execute
  ArkWebWebStorageCppToC::Get(self)->GetOriginUsage(
      *origin, ArkWebLongValueCallbackCToCpp::Invert(callback));
}

int ARK_WEB_CALLBACK ark_web_web_storage_delete_origin(
    struct _ark_web_web_storage_t *self, const ArkWebString *origin) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(origin, 0);

  // Execute
  return ArkWebWebStorageCppToC::Get(self)->DeleteOrigin(*origin);
}

void ARK_WEB_CALLBACK ark_web_web_storage_delete_all_data(
    struct _ark_web_web_storage_t *self, bool incognito_mode) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebWebStorageCppToC::Get(self)->DeleteAllData(incognito_mode);
}

} // namespace

ArkWebWebStorageCppToC::ArkWebWebStorageCppToC() {
  GetStruct()->get_origins1 = ark_web_web_storage_get_origins1;
  GetStruct()->get_origins2 = ark_web_web_storage_get_origins2;
  GetStruct()->get_origin_quota1 = ark_web_web_storage_get_origin_quota1;
  GetStruct()->get_origin_quota2 = ark_web_web_storage_get_origin_quota2;
  GetStruct()->get_origin_usage1 = ark_web_web_storage_get_origin_usage1;
  GetStruct()->get_origin_usage2 = ark_web_web_storage_get_origin_usage2;
  GetStruct()->delete_origin = ark_web_web_storage_delete_origin;
  GetStruct()->delete_all_data = ark_web_web_storage_delete_all_data;
}

ArkWebWebStorageCppToC::~ArkWebWebStorageCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkWebWebStorageCppToC, ArkWebWebStorage,
                           ark_web_web_storage_t>::kBridgeType =
        ARK_WEB_WEB_STORAGE;

} // namespace OHOS::ArkWeb
