// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CODEC_CONFIG_PARA_ADAPTER_IMPL_H
#define CODEC_CONFIG_PARA_ADAPTER_IMPL_H

#include "media_codec_adapter.h"

namespace OHOS::NWeb {

class CodecConfigParaAdapterImpl : public CodecConfigParaAdapter {
 public:
  CodecConfigParaAdapterImpl() = default;

  int32_t GetWidth() override;

  int32_t GetHeight() override;

  int64_t GetBitRate() override;

  double GetFrameRate() override;

  void SetWidth(int32_t width);

  void SetHeight(int32_t height);

  void SetBitRate(int64_t bitrate);

  void SetFrameRate(double frameRate);

 private:
  int32_t width_ = 0;
  int32_t height_ = 0;
  int64_t bit_rate_ = 0;
  double frame_rate_ = 0.0;
};

}  // namespace OHOS::NWeb

#endif  // CODEC_CONFIG_PARA_ADAPTER_IMPL_H
