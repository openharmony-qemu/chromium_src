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

#include "ohos_adapter/ctocpp/ark_ohos_resource_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/ctocpp/ark_ohos_file_mapper_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
bool ArkOhosResourceAdapterCToCpp::GetRawFileData(const ArkWebString &rawFile,
                                                  size_t &len, uint8_t **dest,
                                                  bool isSys) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_resource_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_raw_file_data, false);

  // Execute
  return _struct->get_raw_file_data(_struct, &rawFile, &len, dest, isSys);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkOhosFileMapper>
ArkOhosResourceAdapterCToCpp::GetRawFileMapper(const ArkWebString &rawFile,
                                               bool isSys) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_resource_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_raw_file_mapper, nullptr);

  // Execute
  ark_ohos_file_mapper_t *_retval =
      _struct->get_raw_file_mapper(_struct, &rawFile, isSys);

  // Return type: refptr_same
  return ArkOhosFileMapperCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
bool ArkOhosResourceAdapterCToCpp::IsRawFileExist(const ArkWebString &rawFile,
                                                  bool isSys) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_resource_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_raw_file_exist, false);

  // Execute
  return _struct->is_raw_file_exist(_struct, &rawFile, isSys);
}

ARK_WEB_NO_SANITIZE
bool ArkOhosResourceAdapterCToCpp::GetRawFileLastModTime(
    const ArkWebString &rawFile, uint16_t &date, uint16_t &time, bool isSys) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_resource_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_raw_file_last_mod_time1, false);

  // Execute
  return _struct->get_raw_file_last_mod_time1(_struct, &rawFile, &date, &time,
                                              isSys);
}

ARK_WEB_NO_SANITIZE
bool ArkOhosResourceAdapterCToCpp::GetRawFileLastModTime(
    const ArkWebString &rawFile, time_t &time, bool isSys) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_resource_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_raw_file_last_mod_time2, false);

  // Execute
  return _struct->get_raw_file_last_mod_time2(_struct, &rawFile, &time, isSys);
}

ArkOhosResourceAdapterCToCpp::ArkOhosResourceAdapterCToCpp() {
}

ArkOhosResourceAdapterCToCpp::~ArkOhosResourceAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkOhosResourceAdapterCToCpp, ArkOhosResourceAdapter,
                           ark_ohos_resource_adapter_t>::kBridgeType =
        ARK_OHOS_RESOURCE_ADAPTER;

} // namespace OHOS::ArkWeb
