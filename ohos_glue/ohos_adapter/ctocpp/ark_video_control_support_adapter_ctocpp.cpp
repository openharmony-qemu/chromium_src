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

#include "ohos_adapter/ctocpp/ark_video_control_support_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
bool ArkVideoControlSupportAdapterCToCpp::GetPan() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_video_control_support_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_pan, false);

  // Execute
  return _struct->get_pan(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkVideoControlSupportAdapterCToCpp::GetTilt() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_video_control_support_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_tilt, false);

  // Execute
  return _struct->get_tilt(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkVideoControlSupportAdapterCToCpp::GetZoom() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_video_control_support_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_zoom, false);

  // Execute
  return _struct->get_zoom(_struct);
}

ArkVideoControlSupportAdapterCToCpp::ArkVideoControlSupportAdapterCToCpp() {
}

ArkVideoControlSupportAdapterCToCpp::~ArkVideoControlSupportAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkVideoControlSupportAdapterCToCpp, ArkVideoControlSupportAdapter,
    ark_video_control_support_adapter_t>::kBridgeType =
    ARK_VIDEO_CONTROL_SUPPORT_ADAPTER;

} // namespace OHOS::ArkWeb
