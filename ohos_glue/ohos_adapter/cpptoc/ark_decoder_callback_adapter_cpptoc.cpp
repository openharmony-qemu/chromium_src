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

#include "ohos_adapter/cpptoc/ark_decoder_callback_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_adapter/ctocpp/ark_buffer_info_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_ohos_buffer_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_decoder_callback_adapter_on_error(
    struct _ark_decoder_callback_adapter_t *self, int32_t errorType,
    int32_t errorCode) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkDecoderCallbackAdapterCppToC::Get(self)->OnError(errorType, errorCode);
}

void ARK_WEB_CALLBACK ark_decoder_callback_adapter_on_stream_changed(
    struct _ark_decoder_callback_adapter_t *self, int32_t width, int32_t height,
    double frameRate) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkDecoderCallbackAdapterCppToC::Get(self)->OnStreamChanged(width, height,
                                                              frameRate);
}

void ARK_WEB_CALLBACK ark_decoder_callback_adapter_on_need_input_data(
    struct _ark_decoder_callback_adapter_t *self, uint32_t index,
    ark_ohos_buffer_adapter_t *buffer) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkDecoderCallbackAdapterCppToC::Get(self)->OnNeedInputData(
      index, ArkOhosBufferAdapterCToCpp::Invert(buffer));
}

void ARK_WEB_CALLBACK ark_decoder_callback_adapter_on_need_output_data(
    struct _ark_decoder_callback_adapter_t *self, uint32_t index,
    ark_buffer_info_adapter_t *info, uint32_t flag) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkDecoderCallbackAdapterCppToC::Get(self)->OnNeedOutputData(
      index, ArkBufferInfoAdapterCToCpp::Invert(info), flag);
}

} // namespace

ArkDecoderCallbackAdapterCppToC::ArkDecoderCallbackAdapterCppToC() {
  GetStruct()->on_error = ark_decoder_callback_adapter_on_error;
  GetStruct()->on_stream_changed =
      ark_decoder_callback_adapter_on_stream_changed;
  GetStruct()->on_need_input_data =
      ark_decoder_callback_adapter_on_need_input_data;
  GetStruct()->on_need_output_data =
      ark_decoder_callback_adapter_on_need_output_data;
}

ArkDecoderCallbackAdapterCppToC::~ArkDecoderCallbackAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkDecoderCallbackAdapterCppToC, ArkDecoderCallbackAdapter,
    ark_decoder_callback_adapter_t>::kBridgeType = ARK_DECODER_CALLBACK_ADAPTER;

} // namespace OHOS::ArkWeb
