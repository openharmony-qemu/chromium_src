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

#include "native_buffer_config_impl.h"

namespace OHOS::NWeb {

NativeBufferConfigAdapterImpl::NativeBufferConfigAdapterImpl() {}

NativeBufferConfigAdapterImpl::~NativeBufferConfigAdapterImpl() {}

int NativeBufferConfigAdapterImpl::GetBufferWidth()
{
    return width_;
}

int NativeBufferConfigAdapterImpl::GetBufferHeight()
{
    return height_;
}

int NativeBufferConfigAdapterImpl::GetBufferFormat()
{
    return format_;
}

int NativeBufferConfigAdapterImpl::GetBufferUsage()
{
    return usage_;
}

int NativeBufferConfigAdapterImpl::GetBufferStride()
{
    return stride_;
}

void NativeBufferConfigAdapterImpl::SetBufferWidth(int width)
{
    width_ = width;
}

void NativeBufferConfigAdapterImpl::SetBufferHeight(int height)
{
    height_ = height;
}

void NativeBufferConfigAdapterImpl::SetBufferFormat(int format)
{
    format_ = format;
}

void NativeBufferConfigAdapterImpl::SetBufferUsage(int usage)
{
    usage_ = usage;
}

void NativeBufferConfigAdapterImpl::SetBufferStride(int stride)
{
    stride_ = stride;
}

}  // namespace OHOS::NWeb
