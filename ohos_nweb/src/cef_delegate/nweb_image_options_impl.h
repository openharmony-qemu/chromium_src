/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef NWEB_IMAGE_OPTIONS_IMPL_H
#define NWEB_IMAGE_OPTIONS_IMPL_H

#include "nweb_handler.h"

namespace OHOS::NWeb {

class NWebImageOptionsImpl : public NWebImageOptions {
public:
  NWebImageOptionsImpl(size_t width, size_t height, ImageColorType colorType,
                       ImageAlphaType alphaType);
  ~NWebImageOptionsImpl() = default;

  size_t GetWidth() override;

  size_t GetHeight() override;

  ImageColorType GetColorType() override;

  ImageAlphaType GetAlphaType() override;

private:
  size_t width_ = 0;
  size_t height_ = 0;
  ImageColorType colorType_ = ImageColorType::COLOR_TYPE_UNKNOWN;
  ImageAlphaType alphaType_ = ImageAlphaType::ALPHA_TYPE_UNKNOWN;
};

} // namespace OHOS::NWeb

#endif // NWEB_IMAGE_OPTIONS_IMPL_H
