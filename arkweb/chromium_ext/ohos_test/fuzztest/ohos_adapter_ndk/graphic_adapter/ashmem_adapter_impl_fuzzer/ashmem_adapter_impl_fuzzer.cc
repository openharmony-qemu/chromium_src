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

#include "arkweb/ohos_adapter_ndk/interfaces/graphic_adapter.h"
#include "arkweb/ohos_adapter_ndk/utils/include/ashmem_adapter.h"

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;
namespace OHOS {

constexpr int MAX_SET_NUMBER = 10;

bool AshmemAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    char* name = strdup((dataProvider.ConsumeRandomLengthString(MAX_SET_NUMBER)).c_str());
    size_t nameSize = dataProvider.ConsumeIntegralInRange<size_t>(0, MAX_SET_NUMBER);
    int fd = AshmemAdapter::AshmemCreate(name, nameSize);
    ShmemAdapterClose(fd);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AshmemAdapterImplFuzzTest(data, size);
    return 0;
}