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

#include "buffer_flush_config_adapter_impl.h"

namespace OHOS::NWeb {

int32_t BufferFlushConfigAdapterImpl::GetX() {
  return x_;
}

int32_t BufferFlushConfigAdapterImpl::GetY() {
  return y_;
}

int32_t BufferFlushConfigAdapterImpl::GetW() {
  return w_;
}

int32_t BufferFlushConfigAdapterImpl::GetH() {
  return h_;
}

int64_t BufferFlushConfigAdapterImpl::GetTimestamp() {
  return timestamp_;
}

void BufferFlushConfigAdapterImpl::SetX(int32_t x) {
  x_ = x;
}

void BufferFlushConfigAdapterImpl::SetY(int32_t y) {
  y_ = y;
}

void BufferFlushConfigAdapterImpl::SetW(int32_t w) {
  w_ = w;
}

void BufferFlushConfigAdapterImpl::SetH(int32_t h) {
  h_ = h;
}

void BufferFlushConfigAdapterImpl::SetTimestamp(int64_t timestamp) {
  timestamp_ = timestamp;
}
}  // namespace OHOS::NWeb
