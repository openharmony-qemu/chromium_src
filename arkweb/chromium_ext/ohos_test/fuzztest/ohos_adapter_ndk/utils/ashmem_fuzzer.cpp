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

#include "arkweb/ohos_adapter_ndk/utils/include/ashmem_adapter.h"
#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS::NWeb {
namespace {
    constexpr uint8_t MAX_STRING_LENGTH = 255;
    constexpr int32_t MAX_INT_SIZE = 255;
    constexpr int32_t MIN_INT_SIZE = -255;
}

void AshmemFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);

    int cLength = dataProvider.ConsumeIntegralInRange<int>(0, MAX_INT_SIZE);
    char res[cLength + 1];
    for(int i = 0; i < cLength; i++) {
        char testName = dataProvider.ConsumeIntegralInRange<char>(0x0020, 0x007E);
        res[i] = testName;
    }
    res[cLength] = '\0';
    char *name = res;
    int testSize = dataProvider.ConsumeIntegralInRange<int>(MIN_INT_SIZE, MAX_INT_SIZE);
    int fd = ShmemAdapterCreate(name, testSize);
    int prot = dataProvider.ConsumeIntegralInRange<int>(MIN_INT_SIZE, MAX_INT_SIZE);
    ShmemAdapterSetProt(fd, prot);
    ShmemAdapterGetSize(fd);
    ShmemAdapterGetProt(fd);
    void *startAddr = ShmemAdapterMap(fd, prot);
    ShmemAdapterUnmap(startAddr, ShmemAdapterGetSize(fd));
    ShmemAdapterClose(fd);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    AshmemFuzzTest(data, size);
    return 0;
}
} // namespace OHOS