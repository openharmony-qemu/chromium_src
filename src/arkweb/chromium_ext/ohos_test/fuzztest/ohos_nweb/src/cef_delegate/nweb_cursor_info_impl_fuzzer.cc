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

#include "ohos_nweb/src/cef_delegate/nweb_cursor_info_impl.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"

using namespace OHOS::NWeb;

void NWebCursorInfoImplFuzzTest(FuzzedDataProvider* fdp) {
    int32_t x = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
    int32_t y = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
    float scale = fdp->ConsumeFloatingPoint<float>();
    int32_t width = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
    int32_t height = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
    uint8_t* buff = new uint8_t[100]{0};
    NWebCursorInfoImpl impl(x, y, scale, width, height, buff);

    impl.GetX();

    impl.GetY();

    impl.GetScale();

    impl.GetBuff();

    impl.GetWidth();

    impl.GetHeight();

    delete [] buff;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
   if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  NWebCursorInfoImplFuzzTest(&fdp);
  
  return 0;
}
