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
