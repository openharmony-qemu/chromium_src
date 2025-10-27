// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "codec_config_para_adapter_impl.h"

namespace OHOS::NWeb {

int32_t CodecConfigParaAdapterImpl::GetWidth() {
  return width_;
}

int32_t CodecConfigParaAdapterImpl::GetHeight() {
  return height_;
}

int64_t CodecConfigParaAdapterImpl::GetBitRate() {
  return bit_rate_;
}

double CodecConfigParaAdapterImpl::GetFrameRate() {
  return frame_rate_;
}

void CodecConfigParaAdapterImpl::SetWidth(int32_t width) {
  width_ = width;
}

void CodecConfigParaAdapterImpl::SetHeight(int32_t height) {
  height_ = height;
}

void CodecConfigParaAdapterImpl::SetBitRate(int64_t bitrate) {
  bit_rate_ = bitrate;
}

void CodecConfigParaAdapterImpl::SetFrameRate(double frameRate) {
  frame_rate_ = frameRate;
}

}  // namespace OHOS::NWeb
