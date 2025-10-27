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

#include "ohos_adapter/bridge/ark_clip_board_image_data_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkClipBoardImageDataAdapterImpl::ArkClipBoardImageDataAdapterImpl(
    std::shared_ptr<OHOS::NWeb::ClipBoardImageDataAdapter> ref)
    : real_(ref)
{}

int32_t ArkClipBoardImageDataAdapterImpl::GetColorType()
{
    return (int32_t)real_->GetColorType();
}

int32_t ArkClipBoardImageDataAdapterImpl::GetAlphaType()
{
    return (int32_t)real_->GetAlphaType();
}

uint32_t* ArkClipBoardImageDataAdapterImpl::GetData()
{
    return real_->GetData();
}

size_t ArkClipBoardImageDataAdapterImpl::GetDataSize()
{
    return real_->GetDataSize();
}

size_t ArkClipBoardImageDataAdapterImpl::GetRowBytes()
{
    return real_->GetRowBytes();
}

int32_t ArkClipBoardImageDataAdapterImpl::GetWidth()
{
    return real_->GetWidth();
}

int32_t ArkClipBoardImageDataAdapterImpl::GetHeight()
{
    return real_->GetHeight();
}

void ArkClipBoardImageDataAdapterImpl::SetColorType(int32_t color)
{
    real_->SetColorType((OHOS::NWeb::ClipBoardImageColorType)color);
}

void ArkClipBoardImageDataAdapterImpl::SetAlphaType(int32_t alpha)
{
    real_->SetAlphaType((OHOS::NWeb::ClipBoardImageAlphaType)alpha);
}

void ArkClipBoardImageDataAdapterImpl::SetData(uint32_t* data)
{
    real_->SetData(data);
}

void ArkClipBoardImageDataAdapterImpl::SetDataSize(size_t size)
{
    real_->SetDataSize(size);
}

void ArkClipBoardImageDataAdapterImpl::SetRowBytes(size_t rowBytes)
{
    real_->SetRowBytes(rowBytes);
}

void ArkClipBoardImageDataAdapterImpl::SetWidth(int32_t width)
{
    real_->SetWidth(width);
}

void ArkClipBoardImageDataAdapterImpl::SetHeight(int32_t height)
{
    real_->SetHeight(height);
}

} // namespace OHOS::ArkWeb
