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

#include "ohos_adapter/bridge/ark_decoder_format_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkDecoderFormatAdapterImpl::ArkDecoderFormatAdapterImpl(std::shared_ptr<OHOS::NWeb::DecoderFormatAdapter> ref)
    : real_(ref)
{}

int32_t ArkDecoderFormatAdapterImpl::GetWidth()
{
    return real_->GetWidth();
}

int32_t ArkDecoderFormatAdapterImpl::GetHeight()
{
    return real_->GetHeight();
}

double ArkDecoderFormatAdapterImpl::GetFrameRate()
{
    return real_->GetFrameRate();
}

void ArkDecoderFormatAdapterImpl::SetWidth(int32_t width)
{
    return real_->SetWidth(width);
}

void ArkDecoderFormatAdapterImpl::SetHeight(int32_t height)
{
    return real_->SetHeight(height);
}

void ArkDecoderFormatAdapterImpl::SetFrameRate(double frameRate)
{
    return real_->SetFrameRate(frameRate);
}

} // namespace OHOS::ArkWeb
