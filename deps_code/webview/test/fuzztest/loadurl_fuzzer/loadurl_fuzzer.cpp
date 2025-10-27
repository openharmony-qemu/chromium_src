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

#include "loadurl_fuzzer.h"
#include <fuzzer/FuzzedDataProvider.h>

#include "nweb.h"
#include "nweb_create_window.h"

namespace OHOS {
constexpr uint8_t MAX_STRING_LENGTH = 64;
constexpr int MAX_VECTOR_SIZE = 10;
std::shared_ptr<OHOS::NWeb::NWeb> g_nweb = nullptr;
bool LoadUrlFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return true;
    }
    FuzzedDataProvider dataProvider(data, size);
    g_nweb = NWeb::GetNwebForTest();
    if (g_nweb == nullptr) {
        return true;
    }
    std::string url = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::map<std::string, std::string> additionalHttpHeaders;
    int map_size = dataProvider.ConsumeIntegralInRange<int>(1, MAX_VECTOR_SIZE);
    for (int i = 0; i < map_size; i++) {
        std::string key = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
        std::string value = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
        additionalHttpHeaders[key] = value;
    }
    g_nweb->Load(url, additionalHttpHeaders);
    return true;
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::LoadUrlFuzzTest(data, size);
    return 0;
}
