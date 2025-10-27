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

#include "third_party/blink/renderer/core/html/html_object_element.h"
#include "third_party/blink/renderer/core/html/html_param_element.h"

namespace blink {

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void HTMLParamElement::AttributeChanged(const AttributeModificationParams& params) {
  HTMLElement::AttributeChanged(params);
  if (params.name != html_names::kNameAttr &&
      params.name != html_names::kValueAttr &&
      params.name != html_names::kIdAttr) {
    return;
  }
  if (params.old_value == params.new_value) {
    return;
  }
  auto* parent_object = DynamicTo<HTMLObjectElement>(parentElement());
  if (parent_object && parent_object->IsNativeType()) {
    Vector<ParamChangeInfo> param_changes;
    param_changes.push_back(ParamChangeInfo(ParamChangeInfo::Status::kUpdate,
                                            this->GetIdAttribute(),
                                            this->GetName(),
                                            this->Value()));
    parent_object->ProcessParamChanges(param_changes);
  }
}
#endif

}  // namespace blink