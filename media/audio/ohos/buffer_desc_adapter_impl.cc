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

#include "buffer_desc_adapter_impl.h"

namespace OHOS::NWeb {

uint8_t* BufferDescAdapterImpl::GetBuffer() {
  return buffer_;
}

size_t BufferDescAdapterImpl::GetBufLength() {
  return buf_length_;
}

size_t BufferDescAdapterImpl::GetDataLength() {
  return data_length_;
}

void BufferDescAdapterImpl::SetBuffer(uint8_t* buffer) {
  buffer_ = buffer;
}

void BufferDescAdapterImpl::SetBufLength(size_t bufLength) {
  buf_length_ = bufLength;
}

void BufferDescAdapterImpl::SetDataLength(size_t dataLength) {
  data_length_ = dataLength;
}

}  // namespace OHOS::NWeb
