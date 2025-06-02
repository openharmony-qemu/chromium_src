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

#include "arkweb/chromium_ext/third_party/blink/public/common/context_menu_data/context_menu_params_builder_utils.h"

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif
#include "third_party/blink/public/common/context_menu_data/context_menu_data.h"
#include "third_party/blink/public/common/context_menu_data/untrustworthy_context_menu_params.h"
#include "third_party/blink/public/mojom/context_menu/context_menu.mojom.h"
#include "ui/base/mojom/menu_source_type.mojom-forward.h"
#include "ui/base/ui_base_types.h"

namespace blink {
void ContextMenuParamsBuilderUtils::SetOHOSContextMenuParam(
    blink::UntrustworthyContextMenuParams& params,
    const blink::ContextMenuData& data) {
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  params.source_type = static_cast<ui::mojom::MenuSourceType>(data.source_type);

  LOG(INFO)
      << "ContextMenuParamsBuilderUtils::Build [params] "
      << "is_editable = " << params.is_editable << ", edit_flags = "
      << params.edit_flags
      << ", source_type = " << params.source_type
      << ", media_type = " << params.media_type;
#endif  // #if BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  params.is_selectable = data.is_selectable;
#endif
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  params.image_rect = data.image_rect;
  params.is_ai_link = data.is_ai_link;
#endif
}
}  // namespace blink
