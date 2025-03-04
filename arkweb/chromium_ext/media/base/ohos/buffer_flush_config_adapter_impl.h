// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BUFFER_FLUSH_CONFIG_ADAPTER_IMPL_H
#define BUFFER_FLUSH_CONFIG_ADAPTER_IMPL_H

#include "graphic_adapter.h"

namespace OHOS::NWeb {

class BufferFlushConfigAdapterImpl : public BufferFlushConfigAdapter {
 public:
  BufferFlushConfigAdapterImpl() = default;

  int32_t GetX() override;

  int32_t GetY() override;

  int32_t GetW() override;

  int32_t GetH() override;

  int64_t GetTimestamp() override;

  void SetX(int32_t);

  void SetY(int32_t);

  void SetW(int32_t);

  void SetH(int32_t);

  void SetTimestamp(int64_t);

 private:
  int32_t x_ = 0;
  int32_t y_ = 0;
  int32_t w_ = 0;
  int32_t h_ = 0;
  int64_t timestamp_ = 0;
};

}  // namespace OHOS::NWeb

#endif  // BUFFER_FLUSH_CONFIG_ADAPTER_IMPL_H
