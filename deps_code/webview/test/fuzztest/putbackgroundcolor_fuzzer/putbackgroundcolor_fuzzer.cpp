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

#include "putbackgroundcolor_fuzzer.h"

#include <securec.h>

#include "nweb.h"
#include "nweb_create_window.h"

namespace OHOS {
    std::shared_ptr<OHOS::NWeb::NWeb> g_nweb = nullptr;
    bool PutBackgroundColorFuzzTest(const uint8_t* data, size_t size)
    {
        if ((data == nullptr) || (size < sizeof(int))) {
            return true;
        }
        g_nweb = NWeb::GetNwebForTest();
        if (g_nweb == nullptr) {
            return true;
        }
        int color;
        if (memcpy_s(&color, sizeof(int), data, sizeof(int)) != 0) {
            return true;
        }
        g_nweb->PutBackgroundColor(color);
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::PutBackgroundColorFuzzTest(data, size);
    return 0;
}
