/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_IMAGE_ENCODER_ADAPTER_
#define OHOS_IMAGE_ENCODER_ADAPTER_

#include <memory>
#include <string>
#include <multimedia/image_framework/image/image_packer_native.h>
#include <multimedia/image_framework/image/pixelmap_native.h>
#include "third_party/skia/include/core/SkBitmap.h"

namespace OHOS {
namespace NWeb {

class OhosImageEncoderAdapter {
public:
    static OhosImageEncoderAdapter& GetInstance();
    // encode hardware, check if path is valid before calling Encode
    bool Encode(const SkBitmap& bitmap, const std::string& path);

private:
    OhosImageEncoderAdapter() = default;
    ~OhosImageEncoderAdapter() = default;
    std::shared_ptr<OH_PackingOptions> CreatePackingOptions();
    std::shared_ptr<OH_PixelmapNative> CreatePixelmap(const SkBitmap& bitmap);
};

}  // namespace NWeb
}  // namespace OHOS
#endif  // OHOS_IMAGE_ENCODER_ADAPTER_