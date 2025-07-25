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

 #include <fuzzer/FuzzedDataProvider.h>

 #include <iostream>
 #include <map>
 #include <memory>
 #include <string>
 #include <unordered_map>
 
 #include "gmock/gmock.h"
 #include "gtest/gtest.h"
 #include "ohos_nweb/include/nweb.h"
 #include "ohos_nweb/src/nweb_imf_cursor_info_adapter_impl.h"
 #include "ohos_nweb/src/nweb_impl.h"
 
 using namespace OHOS::NWeb;
 
 namespace OHOS::NWeb {
 void NWebImfCursorInfoAdapterFuzzTest(const uint8_t* data, size_t size) {
   if ((data == nullptr) || (size < sizeof(int32_t))) {
     return false;
   }
   FuzzedDataProvider fdp(data, size);
   double left = fdp.ConsumeFloatingPoint<double>();
   double top = fdp.ConsumeFloatingPoint<double>();
   double width = fdp.ConsumeFloatingPoint<double>();
   double height = fdp.ConsumeFloatingPoint<double>();
   NWebIMFCursorInfoAdapterImpl instance;
   instance.SetLeft(left);
   instance.SetTop(top);
   instance.SetWidth(width);
   instance.SetHeight(height);
   instance.GetLeft();
   instance.GetTop();
   instance.GetWidth();
   instance.GetHeight();
 }
 }  // namespace OHOS::NWeb
 
 extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
   NWebImfCursorInfoAdapterFuzzTest(data, size);
   return 0;
 }
 