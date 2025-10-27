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

#include "format_adapter_impl.h"

namespace OHOS::NWeb {

uint32_t FormatAdapterImpl::GetWidth()
{
    return width_;
}

uint32_t FormatAdapterImpl::GetHeight()
{
    return height_;
}

float FormatAdapterImpl::GetFrameRate()
{
    return frame_rate_;
}

VideoPixelFormatAdapter FormatAdapterImpl::GetPixelFormat()
{
    return pixel_format_;
}

void FormatAdapterImpl::SetWidth(uint32_t width)
{
    width_ = width;
}

void FormatAdapterImpl::SetHeight(uint32_t height)
{
    height_ = height;
}

void FormatAdapterImpl::SetFrameRate(float frameRate)
{
    frame_rate_ = frameRate;
}

void FormatAdapterImpl::SetPixelFormat(VideoPixelFormatAdapter format)
{
    pixel_format_ = format;
}

} // namespace OHOS::NWeb
