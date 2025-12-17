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
#include "arkweb/ohos_adapter_ndk/color_picker_adapter/color_picker_adapter_impl.h"

#include <fuzzer/FuzzedDataProvider.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "arkweb/ohos_nweb/src/nweb_hilog.h"

using namespace OHOS::NWeb;

void ColorPickerAdapterImplFuzzTest(FuzzedDataProvider* fdp) {
  double x = fdp->ConsumeFloatingPoint<double>();
  double y = fdp->ConsumeFloatingPoint<double>();
  ColorPickerCallback color_picker_callback = [](bool success, uint32_t color) {
    WVLOG_I("OnEyeDropperResult, success == %{public}d, color == %{public}d",
            success, color);
  };

  ColorPickerAdapterImpl impl;

  impl.StartColorPicker(x, y, color_picker_callback);

  impl.StartColorPickerWithColorValue(x, y, color_picker_callback);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  ColorPickerAdapterImplFuzzTest(&fdp);

  return 0;
}
