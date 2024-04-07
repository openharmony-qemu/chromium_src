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
