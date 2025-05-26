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

#ifndef ARKWEB_INPUT_METHOD_CONTROLLER_UTILS_H_
#define ARKWEB_INPUT_METHOD_CONTROLLER_UTILS_H_

#include "arkweb/build/features/features.h"
#include "base/gtest_prod_util.h"
#include "base/memory/raw_ptr.h"
#include "third_party/blink/public/platform/web_text_input_info.h"
#include "third_party/blink/public/platform/web_text_input_type.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/editing/commands/typing_command.h"
#include "third_party/blink/renderer/core/editing/forward.h"
#include "third_party/blink/renderer/core/editing/ime/cached_text_input_info.h"
#include "third_party/blink/renderer/core/editing/ime/ime_text_span.h"
#include "third_party/blink/renderer/core/editing/plain_text_range.h"
#include "third_party/blink/renderer/core/events/input_event.h"
#include "third_party/blink/renderer/core/execution_context/execution_context_lifecycle_observer.h"
#include "third_party/blink/renderer/platform/graphics/dom_node_id.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class InputMethodController;

class ArkwebInputMethodControllerUtils {
 public:
  friend class InputMethodController;
  explicit ArkwebInputMethodControllerUtils(
      InputMethodController* input_method_controller);
  ~ArkwebInputMethodControllerUtils();

  ArkwebInputMethodControllerUtils(const ArkwebInputMethodControllerUtils&) =
      delete;
  ArkwebInputMethodControllerUtils& operator=(
      const ArkwebInputMethodControllerUtils&) = delete;

  EphemeralRange CompositionEphemeralRange() const;

  bool HasComposition() const;

  void Clear();

  void GetInputElementAttributes(HashMap<String, String>& attributes) const;

 private:
  const raw_ptr<InputMethodController> input_method_controller_;
  Document& GetDocument() const;

  LocalFrame& GetFrame() const;

  bool IsAvailable() const;

  void InsertTextDuringCompositionWithEvents(
      LocalFrame& frame,
      const String& text,
      TypingCommand::Options options,
      TypingCommand::TextCompositionType composition_type);
  void DispatchCompositionEndEvent(LocalFrame& frame, const String& text);

  // is_max_length_overflow: Flags that handle situations where the maximum
  // length of a text input field is exceeded
  void SelectCompositionEx(bool is_max_length_overflow = false) const;

  // is_max_length_overflow: Flags that handle situations where the maximum
  // length of a text input field is exceeded
  [[nodiscard]] bool ReplaceCompositionEx(const String& text,
                                          bool is_max_length_overflow = false);
};
}  // namespace blink

#endif  // ARKWEB_INPUT_METHOD_CONTROLLER_UTILS_H_
