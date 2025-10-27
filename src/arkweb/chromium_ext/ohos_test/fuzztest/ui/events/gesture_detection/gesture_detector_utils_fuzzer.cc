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
#include "arkweb/chromium_ext/ui/events/gesture_detection/gesture_detector_ext.h"

#include <fuzzer/FuzzedDataProvider.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>

using namespace ui;

void GestureDetectorUtilsFuzzTest(FuzzedDataProvider* fdp) {
  GestureDetector::Config config;
  bool is_lost_focus = fdp->ConsumeBool();
  GestureDetectorExt impl(config, nullptr, nullptr);

  impl.CancelTaps(is_lost_focus);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  GestureDetectorUtilsFuzzTest(&fdp);

  return 0;
}
