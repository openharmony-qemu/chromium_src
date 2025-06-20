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

#ifndef BUFFER_REQUEST_CONFIG_ADAPTER_IMPL_H
#define BUFFER_REQUEST_CONFIG_ADAPTER_IMPL_H

#include "graphic_adapter.h"

namespace OHOS::NWeb {

class BufferRequestConfigAdapterImpl : public BufferRequestConfigAdapter {
 public:
  BufferRequestConfigAdapterImpl() = default;

  int32_t GetWidth() override;

  int32_t GetHeight() override;

  int32_t GetStrideAlignment() override;

  int32_t GetFormat() override;

  uint64_t GetUsage() override;

  int32_t GetTimeout() override;

  int64_t GetTimestamp() override;
  
  ColorGamutAdapter GetColorGamut() override;

  TransformTypeAdapter GetTransformType() override;

  void SetWidth(int32_t);

  void SetHeight(int32_t);

  void SetStrideAlignment(int32_t);

  void SetFormat(int32_t);

  void SetUsage(uint64_t);

  void SetTimeout(int32_t);

  void SetColorGamut(ColorGamutAdapter);

  void SetTransformType(TransformTypeAdapter);
  
  void SetTimestamp(int64_t);

 private:
  int32_t width_;
  int32_t height_;
  int32_t
      stride_alignment_;  // output parameter, system components can ignore it
  int32_t format_;        // GraphicPixelFormat
  uint64_t usage_;
  int32_t timeout_;
  int64_t timestamp_;
  ColorGamutAdapter color_gamut_ = ColorGamutAdapter::SRGB;
  TransformTypeAdapter transform_type_ = TransformTypeAdapter::ROTATE_NONE;
};

}  // namespace OHOS::NWeb

#endif  // BUFFER_REQUEST_CONFIG_ADAPTER_IMPL_H
