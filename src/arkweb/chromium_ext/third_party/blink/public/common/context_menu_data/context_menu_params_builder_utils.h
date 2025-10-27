/* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_CONTEXT_MENU_DATA_CONTEXT_MENU_PARAMS_BUILDER_UTILS_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_CONTEXT_MENU_DATA_CONTEXT_MENU_PARAMS_BUILDER_UTILS_H_

#include "third_party/blink/public/common/context_menu_data/context_menu_params_builder.h"

namespace blink {

struct UntrustworthyContextMenuParams;
struct ContextMenuData;

class BLINK_COMMON_EXPORT ContextMenuParamsBuilderUtils {
 public:
  static void SetOHOSContextMenuParam(blink::UntrustworthyContextMenuParams& params,
                               const blink::ContextMenuData& data);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_CONTEXT_MENU_DATA_CONTEXT_MENU_PARAMS_BUILDER_UTILS_H_
