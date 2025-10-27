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
    return 0;
}

uint32_t FormatAdapterImpl::GetHeight()
{
    return 0;
}

float FormatAdapterImpl::GetFrameRate()
{
    return 0.0;
}

VideoPixelFormatAdapter FormatAdapterImpl::GetPixelFormat()
{
    return VideoPixelFormatAdapter::FORMAT_UNKNOWN;
}

void FormatAdapterImpl::SetWidth(uint32_t width)
{
}

void FormatAdapterImpl::SetHeight(uint32_t height)
{
}

void FormatAdapterImpl::SetFrameRate(float frameRate)
{
}

void FormatAdapterImpl::SetPixelFormat(VideoPixelFormatAdapter format)
{
}

} // namespace OHOS::NWeb
