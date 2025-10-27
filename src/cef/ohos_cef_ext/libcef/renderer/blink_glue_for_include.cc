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

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
const int64_t kInvalidFrameId = -1;

void PenTouchInputFocus(blink::WebNode webNode) {
  blink::Node* node = webNode.Unwrap<blink::Node>();
  if (!node->IsElementNode()) {
    return;
  }
  blink::Element* element = blink::To<blink::Element>(node);
  if (!element) {
    return;
  }
  element->Focus();
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)