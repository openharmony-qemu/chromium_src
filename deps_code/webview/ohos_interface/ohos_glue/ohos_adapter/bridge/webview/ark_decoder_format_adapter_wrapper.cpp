/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_decoder_format_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkDecoderFormatAdapterWrapper::ArkDecoderFormatAdapterWrapper(ArkWebRefPtr<ArkDecoderFormatAdapter> ref) : ctocpp_(ref)
{}

int32_t ArkDecoderFormatAdapterWrapper::GetWidth()
{
    return ctocpp_->GetWidth();
}

int32_t ArkDecoderFormatAdapterWrapper::GetHeight()
{
    return ctocpp_->GetHeight();
}

double ArkDecoderFormatAdapterWrapper::GetFrameRate()
{
    return ctocpp_->GetFrameRate();
}

void ArkDecoderFormatAdapterWrapper::SetWidth(int32_t width)
{
    ctocpp_->SetWidth(width);
}

void ArkDecoderFormatAdapterWrapper::SetHeight(int32_t height)
{
    ctocpp_->SetHeight(height);
}

void ArkDecoderFormatAdapterWrapper::SetFrameRate(double frameRate)
{
    ctocpp_->SetFrameRate(frameRate);
}

} // namespace OHOS::ArkWeb
