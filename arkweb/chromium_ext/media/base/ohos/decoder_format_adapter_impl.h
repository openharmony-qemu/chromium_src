// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DECODER_FORMAT_ADAPTER_IMPL_H
#define DECODER_FORMAT_ADAPTER_IMPL_H

#include "media_codec_decoder_adapter.h"

namespace OHOS::NWeb {

class DecoderFormatAdapterImpl : public DecoderFormatAdapter {
 public:
  DecoderFormatAdapterImpl() = default;

  int32_t GetWidth() override;

  int32_t GetHeight() override;

  double GetFrameRate() override;

  void SetWidth(int32_t width) override;

  void SetHeight(int32_t height) override;

  void SetFrameRate(double frameRate) override;

 private:
  int32_t width_;
  int32_t height_;
  double frame_rate_;
};

}  // namespace OHOS::NWeb

#endif  // DECODER_FORMAT_ADAPTER_IMPL_H
