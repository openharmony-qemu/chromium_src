// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
