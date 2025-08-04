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

#include "arkweb/chromium_ext/third_party/blink/common/event/input_event_ohos.h"

#include <fuzzer/FuzzedDataProvider.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>

using namespace blink;

class MockWebInputEvent : public WebInputEvent {
 public:
  MockWebInputEvent(Type type, int modifiers, base::TimeTicks time_stamp)
      : WebInputEvent(type, modifiers, time_stamp) {}
  std::unique_ptr<WebInputEvent> Clone() const override { return nullptr; }
  bool CanCoalesce(const blink::WebInputEvent& event) const override {
    return false;
  }
  void Coalesce(const WebInputEvent& event) override {}
};

void InputEventOhosFuzzTest(FuzzedDataProvider* fdp) {
  WebInputEvent::Type type = fdp->ConsumeEnum<WebInputEvent::Type>();
  int modifiers = fdp->ConsumeIntegralInRange<int>(0, 256);
  MockWebInputEvent event(type, modifiers, base::TimeTicks());
  InputEventOhos impl;

  InputEventOhos::GetWebEventName(event);

  InputEventOhos::FilterLogEvent(event);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  InputEventOhosFuzzTest(&fdp);

  return 0;
}
