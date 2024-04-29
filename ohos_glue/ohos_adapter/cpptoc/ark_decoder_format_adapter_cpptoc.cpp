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

#include "ohos_adapter/cpptoc/ark_decoder_format_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int32_t ARK_WEB_CALLBACK ark_decoder_format_adapter_get_width(
    struct _ark_decoder_format_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkDecoderFormatAdapterCppToC::Get(self)->GetWidth();
}

int32_t ARK_WEB_CALLBACK ark_decoder_format_adapter_get_height(
    struct _ark_decoder_format_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkDecoderFormatAdapterCppToC::Get(self)->GetHeight();
}

double ARK_WEB_CALLBACK ark_decoder_format_adapter_get_frame_rate(
    struct _ark_decoder_format_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkDecoderFormatAdapterCppToC::Get(self)->GetFrameRate();
}

void ARK_WEB_CALLBACK ark_decoder_format_adapter_set_width(
    struct _ark_decoder_format_adapter_t *self, int32_t width) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkDecoderFormatAdapterCppToC::Get(self)->SetWidth(width);
}

void ARK_WEB_CALLBACK ark_decoder_format_adapter_set_height(
    struct _ark_decoder_format_adapter_t *self, int32_t height) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkDecoderFormatAdapterCppToC::Get(self)->SetHeight(height);
}

void ARK_WEB_CALLBACK ark_decoder_format_adapter_set_frame_rate(
    struct _ark_decoder_format_adapter_t *self, double frameRate) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkDecoderFormatAdapterCppToC::Get(self)->SetFrameRate(frameRate);
}

} // namespace

ArkDecoderFormatAdapterCppToC::ArkDecoderFormatAdapterCppToC() {
  GetStruct()->get_width = ark_decoder_format_adapter_get_width;
  GetStruct()->get_height = ark_decoder_format_adapter_get_height;
  GetStruct()->get_frame_rate = ark_decoder_format_adapter_get_frame_rate;
  GetStruct()->set_width = ark_decoder_format_adapter_set_width;
  GetStruct()->set_height = ark_decoder_format_adapter_set_height;
  GetStruct()->set_frame_rate = ark_decoder_format_adapter_set_frame_rate;
}

ArkDecoderFormatAdapterCppToC::~ArkDecoderFormatAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkDecoderFormatAdapterCppToC, ArkDecoderFormatAdapter,
    ark_decoder_format_adapter_t>::kBridgeType = ARK_DECODER_FORMAT_ADAPTER;

} // namespace OHOS::ArkWeb
