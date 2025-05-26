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

#include "arkweb/build/features/features.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/renderer/core/css/style_sheet_contents.h"

namespace blink {

#if BUILDFLAG(ARKWEB_ADBLOCK)
void StyleSheetContents::SetStyleSheetType(WebDocument::StyleSheetType type) {
  switch (type) {
    case WebDocument::StyleSheetType::kAdBlock:
      is_for_adblock_ = true;
      break;
    case WebDocument::StyleSheetType::kUserAdBlock:
      is_for_user_adblock_ = true;
      break;
    default:
      break;
  }
}
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

}  // namespace blink
