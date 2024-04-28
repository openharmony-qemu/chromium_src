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

#include "ohos_nweb/ctocpp/ark_web_create_native_media_player_callback_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_nweb/cpptoc/ark_web_media_info_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_native_media_player_handler_cpptoc.h"
#include "ohos_nweb/ctocpp/ark_web_native_media_player_bridge_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkWebNativeMediaPlayerBridge>
ArkWebCreateNativeMediaPlayerCallbackCToCpp::OnCreate(
    ArkWebRefPtr<ArkWebNativeMediaPlayerHandler> handler,
    ArkWebRefPtr<ArkWebMediaInfo> mediaInfo) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_create_native_media_player_callback_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_create, nullptr);

  // Execute
  ark_web_native_media_player_bridge_t *_retval = _struct->on_create(
      _struct, ArkWebNativeMediaPlayerHandlerCppToC::Invert(handler),
      ArkWebMediaInfoCppToC::Invert(mediaInfo));

  // Return type: refptr_same
  return ArkWebNativeMediaPlayerBridgeCToCpp::Invert(_retval);
}

ArkWebCreateNativeMediaPlayerCallbackCToCpp::
    ArkWebCreateNativeMediaPlayerCallbackCToCpp() {
}

ArkWebCreateNativeMediaPlayerCallbackCToCpp::
    ~ArkWebCreateNativeMediaPlayerCallbackCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkWebCreateNativeMediaPlayerCallbackCToCpp,
    ArkWebCreateNativeMediaPlayerCallback,
    ark_web_create_native_media_player_callback_t>::kBridgeType =
    ARK_WEB_CREATE_NATIVE_MEDIA_PLAYER_CALLBACK;

} // namespace OHOS::ArkWeb
