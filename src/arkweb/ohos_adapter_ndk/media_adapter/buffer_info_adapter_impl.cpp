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

#include "buffer_info_adapter_impl.h"

namespace OHOS::NWeb {

int64_t BufferInfoAdapterImpl::GetPresentationTimeUs()
{
    return presentationTimeUs_;
}

int32_t BufferInfoAdapterImpl::GetSize()
{
    return size_;
}

int32_t BufferInfoAdapterImpl::GetOffset()
{
    return offset_;
}

void BufferInfoAdapterImpl::SetPresentationTimeUs(int64_t time)
{
    presentationTimeUs_ = time;
}

void BufferInfoAdapterImpl::SetSize(int32_t size)
{
    size_ = size;
}

void BufferInfoAdapterImpl::SetOffset(int32_t offset)
{
    offset_ = offset;
}

} // namespace OHOS::NWeb
