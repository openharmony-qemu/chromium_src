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

#include "ohos_adapter/ctocpp/ark_media_codec_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_codec_callback_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_codec_config_para_adapter_cpptoc.h"
#include "ohos_adapter/ctocpp/ark_producer_surface_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecAdapterCToCpp::CreateVideoCodecByMime(
    const ArkWebString mimetype) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_video_codec_by_mime, 0);

  // Execute
  return _struct->create_video_codec_by_mime(_struct, mimetype);
}

ARK_WEB_NO_SANITIZE
int32_t
ArkMediaCodecAdapterCToCpp::CreateVideoCodecByName(const ArkWebString name) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_video_codec_by_name, 0);

  // Execute
  return _struct->create_video_codec_by_name(_struct, name);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecAdapterCToCpp::SetCodecCallback(
    const ArkWebRefPtr<ArkCodecCallbackAdapter> callback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_codec_callback, 0);

  // Execute
  return _struct->set_codec_callback(
      _struct, ArkCodecCallbackAdapterCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecAdapterCToCpp::Configure(
    const ArkWebRefPtr<ArkCodecConfigParaAdapter> config) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, configure, 0);

  // Execute
  return _struct->configure(_struct,
                            ArkCodecConfigParaAdapterCppToC::Invert(config));
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecAdapterCToCpp::Prepare() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, prepare, 0);

  // Execute
  return _struct->prepare(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecAdapterCToCpp::Start() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start, 0);

  // Execute
  return _struct->start(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecAdapterCToCpp::Stop() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, stop, 0);

  // Execute
  return _struct->stop(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecAdapterCToCpp::Reset() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, reset, 0);

  // Execute
  return _struct->reset(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecAdapterCToCpp::Release() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, release, 0);

  // Execute
  return _struct->release(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkProducerSurfaceAdapter>
ArkMediaCodecAdapterCToCpp::CreateInputSurface() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_input_surface, nullptr);

  // Execute
  ark_producer_surface_adapter_t *_retval =
      _struct->create_input_surface(_struct);

  // Return type: refptr_same
  return ArkProducerSurfaceAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecAdapterCToCpp::ReleaseOutputBuffer(uint32_t index,
                                                        bool isRender) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, release_output_buffer, 0);

  // Execute
  return _struct->release_output_buffer(_struct, index, isRender);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecAdapterCToCpp::RequestKeyFrameSoon() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, request_key_frame_soon, 0);

  // Execute
  return _struct->request_key_frame_soon(_struct);
}

ArkMediaCodecAdapterCToCpp::ArkMediaCodecAdapterCToCpp() {
}

ArkMediaCodecAdapterCToCpp::~ArkMediaCodecAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkMediaCodecAdapterCToCpp, ArkMediaCodecAdapter,
                           ark_media_codec_adapter_t>::kBridgeType =
        ARK_MEDIA_CODEC_ADAPTER;

} // namespace OHOS::ArkWeb
