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

#include "nweb_image_options_impl.h"

namespace OHOS::NWeb {

NWebImageOptionsImpl::NWebImageOptionsImpl(size_t width, size_t height,
                                           ImageColorType colorType,
                                           ImageAlphaType alphaType)
    : width_(width), height_(height), colorType_(colorType),
      alphaType_(alphaType) {
}

size_t NWebImageOptionsImpl::GetWidth() {
  return width_;
}

size_t NWebImageOptionsImpl::GetHeight() {
  return height_;
}

ImageColorType NWebImageOptionsImpl::GetColorType() {
  return colorType_;
}

ImageAlphaType NWebImageOptionsImpl::GetAlphaType() {
  return alphaType_;
}

} // namespace OHOS::NWeb
