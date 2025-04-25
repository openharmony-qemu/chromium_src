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

#ifndef NWEB_CURSOR_INFO_IMPL_H
#define NWEB_CURSOR_INFO_IMPL_H

#include "base/memory/raw_ptr.h"
#include "nweb_handler.h"

namespace OHOS::NWeb {

class NWebCursorInfoImpl : public NWebCursorInfo {
public:
  NWebCursorInfoImpl() = default;
  NWebCursorInfoImpl(int32_t x, int32_t y, float scale, int32_t width,
                     int32_t height, uint8_t *buff);
  int32_t GetX() override;
  int32_t GetY() override;
  float GetScale() override;
  uint8_t *GetBuff() override;
  int32_t GetWidth() override;
  int32_t GetHeight() override;

private:
  int32_t x_ = 0;
  int32_t y_ = 0;
  float scale_ = 0.0;
  int32_t width_ = 0;
  int32_t height_ = 0;

  raw_ptr<uint8_t> buff_ = nullptr;
};

} // namespace OHOS::NWeb

#endif
