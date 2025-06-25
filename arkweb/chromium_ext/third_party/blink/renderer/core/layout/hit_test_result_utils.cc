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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/layout/hit_test_result_utils.h"

#include "third_party/blink/renderer/core/layout/layout_block.h"
#include "third_party/blink/renderer/core/layout/layout_embedded_content.h"
#include "third_party/blink/renderer/core/layout/layout_image.h"
#include "third_party/blink/renderer/core/layout/layout_view.h"
#include "third_party/blink/renderer/core/layout/physical_box_fragment.h"
#include "third_party/blink/renderer/core/layout/svg/layout_svg_image.h"
namespace blink {
HitTestResultUtils::HitTestResultUtils(HitTestResult* impl) {
    this->hit_test_result_ = impl;
}

#if BUILDFLAG(ARKWEB_DRAG_DROP)
gfx::Rect HitTestResultUtils::GetReplacedContentRect() const {
  gfx::Rect image_rect = ImageRect();
  Node* inner_node = hit_test_result_->InnerNode();
  if (!inner_node) {
    return image_rect;
  }
  if (LayoutBox* layout_box = inner_node->GetLayoutBox()) {
    if (auto* replaced = DynamicTo<LayoutReplaced>(layout_box)) {
      gfx::Rect absolute_image_rect =
          ToPixelSnappedRect(replaced->ReplacedContentRect());
      LOG(INFO) << "GetReplacedContentRect image_rect:" << image_rect.ToString()
                << ", absolute_image_rect:" << absolute_image_rect.ToString();
      if (absolute_image_rect.width() > 0 && absolute_image_rect.height() > 0) {
        absolute_image_rect.Offset(image_rect.OffsetFromOrigin());
        return absolute_image_rect;
      }
    }
  }
  return image_rect;
}

gfx::Rect HitTestResultUtils::GetLinkElementRect() const {
  gfx::Rect content_rect;
  auto* element = hit_test_result_->InnerElement();
  if (element) {
    gfx::Rect element_rect = element->VisibleBoundsInLocalRoot();
    LOG(INFO) << "HitTestResultUtils::GetLinkElementRect: " << element_rect.ToString();
    return element_rect;
  }
  return content_rect;
}

gfx::Rect HitTestResultUtils::ImageRect() const {
  auto imgNode = hit_test_result_->InnerNodeOrImageMapImage();
  if (imgNode && imgNode->isConnected() && imgNode->GetLayoutBox()) {
    return hit_test_result_->ImageRect();
  } else {
    LOG(INFO) << "Get ImageRect filed";
    return gfx::Rect();
  }
}
#endif
}
