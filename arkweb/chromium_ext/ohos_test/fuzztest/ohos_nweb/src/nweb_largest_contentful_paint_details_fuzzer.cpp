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

#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include "nweb_largest_contentful_paint_details_impl.h"
#include "nweb_largest_contentful_paint_details.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if((data == nullptr) || (size < sizeof(int32_t))) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    int64_t navigationStartTime = fdp.ConsumeIntegral<int64_t>();
    int64_t largestImagePaintTime = fdp.ConsumeIntegral<int64_t>();
    int64_t largestTextPaintTime = fdp.ConsumeIntegral<int64_t>();
    int64_t largestImageLoadStartTime = fdp.ConsumeIntegral<int64_t>();
    int64_t largestImageLoadEndTime = fdp.ConsumeIntegral<int64_t>();
    double_t imageBPP = fdp.ConsumeFloatingPoint<double_t>();


    OHOS::NWeb::NWebLargestContentfulPaintDetailsImpl obj(
        navigationStartTime, largestImagePaintTime, largestTextPaintTime,
        largestImageLoadStartTime, largestImageLoadEndTime, imageBPP);
    obj.GetNavigationStartTime();
    obj.GetLargestImagePaintTime();
    obj.GetLargestTextPaintTime();
    obj.GetLargestImageLoadStartTime();
    obj.GetLargestImageLoadEndTime();
    obj.GetImageBPP();
    return 0;
}
 