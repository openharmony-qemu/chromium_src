/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <fuzzer/FuzzedDataProvider.h>
#include <string>

#include "arkweb/chromium_ext/base/ohos/locale_utils.h"

namespace {

void FuzzComputeLanguageByRegion(FuzzedDataProvider& provider)
{
    std::string region = provider.ConsumeRandomLengthString(256);
    std::string result = base::ohos::ComputeLanguageByRegion(region);
}

void FuzzIsTraditionalLanguage()
{
    bool result = base::ohos::IsTraditionalLanguage();
}

void FuzzGetSystemLanguage()
{
    std::string result = base::ohos::GetSystemLanguage();
}

void FuzzWithFixedRegions()
{
    const std::string regions[] = {
        "HK", "MO", "GB", "US", "", "CN", "TW",
        "JP", "KR", "FR", "DE", "RU", "IN",
        "en-US", "zh-CN", "zh-HK", "zh-TW",
        "XX", "ZZ", "000", "!!!", "---",
    };
    for (const auto& region : regions) {
        base::ohos::ComputeLanguageByRegion(region);
    }
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    uint8_t action = provider.ConsumeIntegralInRange<uint8_t>(0, 3);
    switch (action) {
        case 0:
            FuzzComputeLanguageByRegion(provider);
            break;
        case 1:
            FuzzIsTraditionalLanguage();
            break;
        case 2:
            FuzzGetSystemLanguage();
            break;
        case 3:
            FuzzWithFixedRegions();
            break;
        default:
            break;
    }

    return 0;
}
