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