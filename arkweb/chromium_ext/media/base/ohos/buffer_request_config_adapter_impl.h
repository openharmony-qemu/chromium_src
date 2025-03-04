// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

 private:
  int32_t width_;
  int32_t height_;
  int32_t
      stride_alignment_;  // output parameter, system components can ignore it
  int32_t format_;        // GraphicPixelFormat
  uint64_t usage_;
  int32_t timeout_;
  ColorGamutAdapter color_gamut_ = ColorGamutAdapter::SRGB;
  TransformTypeAdapter transform_type_ = TransformTypeAdapter::ROTATE_NONE;
};

}  // namespace OHOS::NWeb

#endif  // BUFFER_REQUEST_CONFIG_ADAPTER_IMPL_H
