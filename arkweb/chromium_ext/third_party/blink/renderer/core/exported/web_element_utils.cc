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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/exported/web_element_utils.h"
#include "third_party/blink/renderer/core/html/html_element.h"

namespace blink {

WebElementUtils::WebElementUtils() {}

bool WebElementUtils::IsContentEditable() {
  const auto* html_element =
    blink::DynamicTo<HTMLElement>(ConstUnwrap<Element>());
  if (!html_element) {
    return false;
  }
  ContentEditableType normalized_value =
      html_element->contentEditableNormalized();
  return normalized_value == ContentEditableType::kContentEditable ||
         normalized_value == ContentEditableType::kPlaintextOnly;
}

}  // namespace blink