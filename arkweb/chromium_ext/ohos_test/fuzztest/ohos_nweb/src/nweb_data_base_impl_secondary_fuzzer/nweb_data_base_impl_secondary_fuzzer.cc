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

#include "ohos_nweb/src/nweb_data_base_impl.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <iostream>
#include <string>

#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"

using namespace OHOS::NWeb;

void NWebSetPermissionByOriginFuzzTest(FuzzedDataProvider* fdp) {
    NWebDataBaseImpl impl;

    std::string origin = fdp->ConsumeRandomLengthString(256);
    int type = fdp->ConsumeIntegralInRange<int>(0, 64);
    bool result = fdp->ConsumeBool();
    bool incognito = false;
    impl.SetPermissionByOrigin(origin, type, result, incognito);
}

void NWebClearPermissionByOriginFuzzTest(FuzzedDataProvider* fdp) {
    NWebDataBaseImpl impl;

    std::string origin = fdp->ConsumeRandomLengthString(256);
    int type = fdp->ConsumeIntegralInRange<int>(0, 64);
    bool incognito = false;
    impl.ClearPermissionByOrigin(origin, type, incognito);
}

void NWebClearAllPermissionFuzzTest(FuzzedDataProvider* fdp) {
    NWebDataBaseImpl impl;

    int type = fdp->ConsumeIntegralInRange<int>(0, 64);
    bool incognito = false;
    impl.ClearAllPermission(type, incognito);
}

void NWebGetOriginsByPermissionFuzzTest(FuzzedDataProvider* fdp) {
    NWebDataBaseImpl impl;

    int type = fdp->ConsumeIntegralInRange<int>(0, 64);
    bool incognito = false;
    impl.GetOriginsByPermission(type, incognito);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
   if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  NWebSetPermissionByOriginFuzzTest(&fdp);
  NWebClearPermissionByOriginFuzzTest(&fdp);
  NWebClearAllPermissionFuzzTest(&fdp);
  NWebGetOriginsByPermissionFuzzTest(&fdp);
  return 0;
}