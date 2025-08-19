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

#include "arkweb/chromium_ext/media/gpu/ohos/ohos_heif_image_decoder.h"
#include "arkweb/chromium_ext/media/gpu/ohos/ohos_image_decoder.h"

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>


namespace media {
using namespace OHOS::NWeb;

bool OHOSHeifImageDecodeFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OhosHeifImageDecoder hiefDecoder;
    heifDecoder.GetType();
    heifDecoder.GetYUVColorSpace();
    OhosImageDecoder imageDecoder;
    imageDecoder->Initialize();
    auto rawValue = fdp.ConsumeIntegralInRange<int32_t>(0, 4);
    OhosImageDecodeStatus status = static_cast<OhosImageDecodeStatus>(rawValue);
    imageDecoder->ExportAsNativePixmapDmaBuf(status)
    imageDecoder->GetSupportedProfile();
    imageDecoder->GetOhosImageDecoderAdapter();
    imageDecoder->ReleaseDecodedPixelMap();

    return true;
}

} // namespace media

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr || size == 0) {
        return 0;
    }
    media::OHOSHeifImageDecodeFuzzTest(data, size);
    return 0;
}