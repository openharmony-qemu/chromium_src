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

#include "ohos_adapter/ctocpp/ark_media_codec_decoder_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_decoder_callback_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_decoder_format_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecDecoderAdapterCToCpp::CreateVideoDecoderByMime(
    const ArkWebString &mimetype) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_decoder_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_video_decoder_by_mime, 0);

  // Execute
  return _struct->create_video_decoder_by_mime(_struct, &mimetype);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecDecoderAdapterCToCpp::CreateVideoDecoderByName(
    const ArkWebString &name) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_decoder_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_video_decoder_by_name, 0);

  // Execute
  return _struct->create_video_decoder_by_name(_struct, &name);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecDecoderAdapterCToCpp::ConfigureDecoder(
    const ArkWebRefPtr<ArkDecoderFormatAdapter> format) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_decoder_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, configure_decoder, 0);

  // Execute
  return _struct->configure_decoder(
      _struct, ArkDecoderFormatAdapterCppToC::Invert(format));
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecDecoderAdapterCToCpp::SetParameterDecoder(
    const ArkWebRefPtr<ArkDecoderFormatAdapter> format) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_decoder_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_parameter_decoder, 0);

  // Execute
  return _struct->set_parameter_decoder(
      _struct, ArkDecoderFormatAdapterCppToC::Invert(format));
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecDecoderAdapterCToCpp::SetOutputSurface(void *window) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_decoder_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_output_surface, 0);

  // Execute
  return _struct->set_output_surface(_struct, window);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecDecoderAdapterCToCpp::PrepareDecoder() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_decoder_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, prepare_decoder, 0);

  // Execute
  return _struct->prepare_decoder(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecDecoderAdapterCToCpp::StartDecoder() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_decoder_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start_decoder, 0);

  // Execute
  return _struct->start_decoder(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecDecoderAdapterCToCpp::StopDecoder() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_decoder_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, stop_decoder, 0);

  // Execute
  return _struct->stop_decoder(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecDecoderAdapterCToCpp::FlushDecoder() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_decoder_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, flush_decoder, 0);

  // Execute
  return _struct->flush_decoder(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecDecoderAdapterCToCpp::ResetDecoder() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_decoder_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, reset_decoder, 0);

  // Execute
  return _struct->reset_decoder(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecDecoderAdapterCToCpp::ReleaseDecoder() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_decoder_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, release_decoder, 0);

  // Execute
  return _struct->release_decoder(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecDecoderAdapterCToCpp::QueueInputBufferDec(
    uint32_t index, int64_t presentationTimeUs, int32_t size, int32_t offset,
    uint32_t flag) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_decoder_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, queue_input_buffer_dec, 0);

  // Execute
  return _struct->queue_input_buffer_dec(_struct, index, presentationTimeUs,
                                         size, offset, flag);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecDecoderAdapterCToCpp::GetOutputFormatDec(
    ArkWebRefPtr<ArkDecoderFormatAdapter> format) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_decoder_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_output_format_dec, 0);

  // Execute
  return _struct->get_output_format_dec(
      _struct, ArkDecoderFormatAdapterCppToC::Invert(format));
}

ARK_WEB_NO_SANITIZE
int32_t
ArkMediaCodecDecoderAdapterCToCpp::ReleaseOutputBufferDec(uint32_t index,
                                                          bool isRender) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_decoder_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, release_output_buffer_dec, 0);

  // Execute
  return _struct->release_output_buffer_dec(_struct, index, isRender);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMediaCodecDecoderAdapterCToCpp::SetCallbackDec(
    const ArkWebRefPtr<ArkDecoderCallbackAdapter> callback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_decoder_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_callback_dec, 0);

  // Execute
  return _struct->set_callback_dec(
      _struct, ArkDecoderCallbackAdapterCppToC::Invert(callback));
}

ArkMediaCodecDecoderAdapterCToCpp::ArkMediaCodecDecoderAdapterCToCpp() {
}

ArkMediaCodecDecoderAdapterCToCpp::~ArkMediaCodecDecoderAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkMediaCodecDecoderAdapterCToCpp, ArkMediaCodecDecoderAdapter,
    ark_media_codec_decoder_adapter_t>::kBridgeType =
    ARK_MEDIA_CODEC_DECODER_ADAPTER;

} // namespace OHOS::ArkWeb
