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

#include "third_party/blink/renderer/core/testing/sim/sim_test.h"

#include "third_party/blink/renderer/core/input/event_handler.h"
#include "third_party/blink/renderer/core/input/mouse_event_manager.h"
#include "third_party/blink/renderer/core/input/gesture_manager.h"

namespace blink {
EventHandler& SimTestExt::GetEventHandler() {
  return GetDocument().GetFrame()->GetEventHandler();
}

MouseEventManager& SimTestExt::GetMouseEventManager() {
  return *GetEventHandler().mouse_event_manager_;
}

GestureManager& SimTestExt::GetGestureManager() {
  return *GetEventHandler().gesture_manager_;
}

LocalFrame& SimTestExt::GetLocalFrame() {
  return *GetEventHandler().frame_;
}

void SimTestExt::LogCatch() {
  testing::internal::CaptureStderr();
}

bool SimTestExt::LogCheck(const std::string& expect_log) {
  std::string log_output = testing::internal::GetCapturedStderr();
  std::cout << "---\n" << log_output << "---\n";
  return log_output.find(expect_log) != std::string::npos;
}
}  // namespace blink