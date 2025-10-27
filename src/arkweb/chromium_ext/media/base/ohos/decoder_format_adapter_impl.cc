// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "decoder_format_adapter_impl.h"

namespace OHOS::NWeb {

int32_t DecoderFormatAdapterImpl::GetWidth() {
  return width_;
}

int32_t DecoderFormatAdapterImpl::GetHeight() {
  return height_;
}

double DecoderFormatAdapterImpl::GetFrameRate() {
  return frame_rate_;
}

void DecoderFormatAdapterImpl::SetWidth(int32_t width) {
  width_ = width;
}

void DecoderFormatAdapterImpl::SetHeight(int32_t height) {
  height_ = height;
}

void DecoderFormatAdapterImpl::SetFrameRate(double frameRate) {
  frame_rate_ = frameRate;
}

}  // namespace OHOS::NWeb
