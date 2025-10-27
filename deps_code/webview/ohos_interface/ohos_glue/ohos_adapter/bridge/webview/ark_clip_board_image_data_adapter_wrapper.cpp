/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_clip_board_image_data_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkClipBoardImageDataAdapterWrapper::ArkClipBoardImageDataAdapterWrapper(ArkWebRefPtr<ArkClipBoardImageDataAdapter> ref)
    : ctocpp_(ref)
{}

NWeb::ClipBoardImageColorType ArkClipBoardImageDataAdapterWrapper::GetColorType()
{
    return (NWeb::ClipBoardImageColorType)ctocpp_->GetColorType();
}

NWeb::ClipBoardImageAlphaType ArkClipBoardImageDataAdapterWrapper::GetAlphaType()
{
    return (NWeb::ClipBoardImageAlphaType)ctocpp_->GetAlphaType();
}

uint32_t* ArkClipBoardImageDataAdapterWrapper::GetData()
{
    return ctocpp_->GetData();
}

size_t ArkClipBoardImageDataAdapterWrapper::GetDataSize()
{
    return ctocpp_->GetDataSize();
}

size_t ArkClipBoardImageDataAdapterWrapper::GetRowBytes()
{
    return ctocpp_->GetRowBytes();
}

int32_t ArkClipBoardImageDataAdapterWrapper::GetWidth()
{
    return ctocpp_->GetWidth();
}

int32_t ArkClipBoardImageDataAdapterWrapper::GetHeight()
{
    return ctocpp_->GetHeight();
}

void ArkClipBoardImageDataAdapterWrapper::SetColorType(NWeb::ClipBoardImageColorType color)
{
    ctocpp_->SetColorType((int32_t)color);
}

void ArkClipBoardImageDataAdapterWrapper::SetAlphaType(NWeb::ClipBoardImageAlphaType alpha)
{
    ctocpp_->SetAlphaType((int32_t)alpha);
}

void ArkClipBoardImageDataAdapterWrapper::SetData(uint32_t* data)
{
    ctocpp_->SetData(data);
}

void ArkClipBoardImageDataAdapterWrapper::SetDataSize(size_t size)
{
    ctocpp_->SetDataSize(size);
}

void ArkClipBoardImageDataAdapterWrapper::SetRowBytes(size_t rowBytes)
{
    ctocpp_->SetRowBytes(rowBytes);
}

void ArkClipBoardImageDataAdapterWrapper::SetWidth(int32_t width)
{
    ctocpp_->SetWidth(width);
}

void ArkClipBoardImageDataAdapterWrapper::SetHeight(int32_t height)
{
    ctocpp_->SetHeight(height);
}

} // namespace OHOS::ArkWeb
