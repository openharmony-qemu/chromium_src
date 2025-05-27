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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_CONTEXT_MENU_CONTROLLER_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_CONTEXT_MENU_CONTROLLER_EXT_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/public/common/context_menu_data/context_menu_data.h"
#include "third_party/blink/public/common/input/web_menu_source_type.h"
#include "third_party/blink/public/web/web_element.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/layout/geometry/physical_offset.h"
#include "third_party/blink/renderer/core/layout/hit_test_result.h"
#include "third_party/blink/renderer/core/page/context_menu_controller.h"

namespace blink {

class ContextMenuController;

class ContextMenuControllerExt : public ContextMenuController {
 public:
  explicit ContextMenuControllerExt(Page*);
  ContextMenuControllerExt(const ContextMenuControllerExt&) = delete;
  ContextMenuControllerExt operator=(const ContextMenuControllerExt&) = delete;
  ~ContextMenuControllerExt();
  ContextMenuControllerExt* AsContextMenuControllerExt() override {
    return this;
  }

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  GURL GetAbsoluteSrcUrl(const blink::WebElement& element);
  blink::WebElement GetImgChild(const blink::WebNode& node, gfx::Point point);
  GURL GetAbsoluteUrl(const blink::WebNode& node,
                      const std::u16string& url_fragment);
  GURL GetChildImageUrlFromElement(const blink::WebElement& element,
                                   gfx::Point point);
  void FindImgUrl(ContextMenuData& data,
                  HitTestResult& result,
                  const PhysicalOffset& point);
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_MENU)
  void GetImgUrl(HitTestResult& result,
                 ContextMenuData& data,
                 WebMenuSourceType source_type);
  void SetArkWebMenuData(ContextMenuData& data, HitTestResult& result);
  void IsAILink(ContextMenuData& data, HitTestResult& result);
#endif  // ARKWEB_MENU

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  bool ShouldShowFreeCopyMenu(const HitTestResult& result);
#endif

#if BUILDFLAG(IS_ARKWEB)
  void HandleArkWebContextMenu(ContextMenuData& data,
                               LocalFrame* frame,
                               HitTestResult& result,
                               LocalFrame* selected_frame,
                               WebMenuSourceType source_type);
#endif
};
}  // namespace blink
#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_CONTEXT_MENU_CONTROLLER_EXT_H_
