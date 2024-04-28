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

#include "ohos_adapter/ctocpp/ark_ohos_file_mapper_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int32_t ArkOhosFileMapperCToCpp::GetFd() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_file_mapper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_fd, 0);

  // Execute
  return _struct->get_fd(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkOhosFileMapperCToCpp::GetOffset() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_file_mapper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_offset, 0);

  // Execute
  return _struct->get_offset(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebString ArkOhosFileMapperCToCpp::GetFileName() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_file_mapper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_file_name,
                                   ark_web_string_default);

  // Execute
  return _struct->get_file_name(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkOhosFileMapperCToCpp::IsCompressed() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_file_mapper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_compressed, false);

  // Execute
  return _struct->is_compressed(_struct);
}

ARK_WEB_NO_SANITIZE
void *ArkOhosFileMapperCToCpp::GetDataPtr() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_file_mapper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_data_ptr, nullptr);

  // Execute
  return _struct->get_data_ptr(_struct);
}

ARK_WEB_NO_SANITIZE
size_t ArkOhosFileMapperCToCpp::GetDataLen() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_file_mapper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_data_len, 0);

  // Execute
  return _struct->get_data_len(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkOhosFileMapperCToCpp::UnzipData(uint8_t **dest, size_t &len) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_file_mapper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, unzip_data, false);

  // Execute
  return _struct->unzip_data(_struct, dest, &len);
}

ArkOhosFileMapperCToCpp::ArkOhosFileMapperCToCpp() {
}

ArkOhosFileMapperCToCpp::~ArkOhosFileMapperCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkOhosFileMapperCToCpp, ArkOhosFileMapper,
                           ark_ohos_file_mapper_t>::kBridgeType =
        ARK_OHOS_FILE_MAPPER;

} // namespace OHOS::ArkWeb
