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

#include "setcapturecallback_fuzzer.h"

#include <securec.h>
#include <sys/mman.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "ohos_adapter_helper.h"
#include "screen_capture_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {

bool ApplySetCaptureCallbackFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return true;
    }
    ScreenCaptureAdapterImpl impl;
    std::shared_ptr<ScreenCaptureCallbackAdapter> cb;
    impl.SetCaptureCallback(cb);
    impl.SetCaptureCallback(nullptr);
    return true;
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::ApplySetCaptureCallbackFuzzTest(data, size);
    return 0;
}