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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/page/context_menu_controller_ext.h"

#include "arkweb/build/features/features.h"

#if BUILDFLAG(IS_ARKWEB)
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#include "third_party/blink/public/common/context_menu_data/context_menu_data.h"
#include "third_party/blink/public/common/input/web_menu_source_type.h"
#include "third_party/blink/public/mojom/context_menu/context_menu.mojom-blink.h"
#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_element_collection.h"
#include "third_party/blink/renderer/core/css/css_image_value.h"
#include "third_party/blink/renderer/core/css/css_uri_value.h"
#include "third_party/blink/renderer/core/dom/node_computed_style.h"
#include "third_party/blink/renderer/core/editing/visible_position.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/frame/visual_viewport.h"
#include "third_party/blink/renderer/core/html/canvas/html_canvas_element.h"
#include "third_party/blink/renderer/core/html/html_embed_element.h"
#include "third_party/blink/renderer/core/html/html_image_element.h"
#include "third_party/blink/renderer/core/html/html_object_element.h"
#include "third_party/blink/renderer/core/html/media/html_audio_element.h"
#include "third_party/blink/renderer/core/html/media/html_media_element.h"
#include "third_party/blink/renderer/core/html/media/html_video_element.h"
#include "third_party/blink/renderer/core/html/parser/html_parser_idioms.h"
#include "third_party/blink/renderer/core/layout/layout_box.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "ui/gfx/geometry/rect_conversions.h"
#endif

namespace blink {
#if BUILDFLAG(ARKWEB_MENU)
namespace {
constexpr char kAILinkAttr[] = "ohos-arkweb-data-detectors-type";
}
#endif  // BUILDFLAG(ARKWEB_MENU)

ContextMenuControllerExt::ContextMenuControllerExt(Page* page)
    : ContextMenuController(page) {}

ContextMenuControllerExt::~ContextMenuControllerExt() = default;

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
bool ContextMenuControllerExt::ShouldShowFreeCopyMenu(
    const HitTestResult& result) {
  if (!result.InnerNode() || !result.InnerNode()->GetLayoutObject()) {
    LOG(INFO) << "InnerNode or GetLayoutObject is null";
    return false;
  }
  if (!result.InnerNode()->GetLayoutObject()->IsSelectable()) {
    LOG(INFO) << "LayoutObject is not selectable";
    return false;
  }
  if (result.IsLiveLink()) {
    const PositionInFlatTreeWithAffinity pos =
        CreateVisiblePosition(FromPositionInDOMTree<EditingInFlatTreeStrategy>(
                                  result.GetPosition()))
            .ToPositionWithAffinity();
    if (pos.IsNull() ||
        !pos.AnchorNode()->IsDescendantOf(result.URLElement())) {
      LOG(INFO) << "Position is null or not a descendant of URL element";
      return false;
    }
  }
  return true;
}
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
GURL ContextMenuControllerExt::GetChildImageUrlFromElement(
    const blink::WebElement& element,
    gfx::Point point) {
  if (element.IsNull()) {
    return GURL();
  }
  const blink::WebElement child_img = GetImgChild(element, point);
  if (child_img.IsNull()) {
    return GURL();
  }
  return GetAbsoluteSrcUrl(child_img);
}

GURL ContextMenuControllerExt::GetAbsoluteSrcUrl(
    const blink::WebElement& element) {
  if (element.IsNull()) {
    return GURL();
  }
  return GetAbsoluteUrl(element, element.GetAttribute("src").Utf16());
}

blink::WebElement ContextMenuControllerExt::GetImgChild(
    const blink::WebNode& node,
    gfx::Point point) {
  blink::WebElementCollection collection = node.GetElementsByHTMLTagName("img");
  if (collection.IsNull()) {
    return blink::WebElement();
  }
  blink::WebElement cur_img = collection.FirstItem();
  if (cur_img.IsNull()) {
    return blink::WebElement();
  }
  gfx::Rect location_rect = cur_img.BoundsInWidget();
  if (location_rect.Contains(point)) {
    return cur_img;
  }

  // Find the rest of the img
  for (unsigned i = 0; i < collection.length() - 1; i++) {
    cur_img = collection.NextItem();
    if (!cur_img.IsNull()) {
      location_rect = cur_img.BoundsInWidget();
      if (location_rect.Contains(point)) {
        return cur_img;
      }
    }
  }
  return blink::WebElement();
}

GURL ContextMenuControllerExt::GetAbsoluteUrl(
    const blink::WebNode& node,
    const std::u16string& url_fragment) {
  if (node.IsNull() || url_fragment.empty()) {
    return GURL();
  }

  if (node.GetDocument().IsNull()) {
    return GURL();
  }

  KURL img_url =
      node.GetDocument().CompleteURL(blink::WebString::FromUTF16(url_fragment));
  return GURL(img_url);
}

void ContextMenuControllerExt::FindImgUrl(ContextMenuData& data,
                                          HitTestResult& result,
                                          const PhysicalOffset& point) {
  bool is_browser = base::CommandLine::ForCurrentProcess()->HasSwitch(::switches::kEnableNwebEx);
  if (data.src_url.is_empty() && result.URLElement() && !is_browser) {
    // try to find image url
    data.src_url = GetChildImageUrlFromElement(
        blink::WebElement(result.URLElement()),
        gfx::Point(point.ToLayoutPoint().X().ToInt(),
                   point.ToLayoutPoint().Y().ToInt()));
    if (!data.src_url.is_empty()) {
      data.has_image_contents = true;
    }
  }
}
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_MENU)
bool IsHitTestStopNode(const Node& node) {
  return IsA<HTMLVideoElement>(node) || IsA<HTMLAudioElement>(node) ||
         IsA<HTMLCanvasElement>(node) || IsA<HTMLObjectElement>(node) ||
         IsA<HTMLEmbedElement>(node);
}

void ContextMenuControllerExt::GetImgUrl(HitTestResult& result,
                                         ContextMenuData& data,
                                         WebMenuSourceType source_type) {
  bool is_browser = base::CommandLine::ForCurrentProcess()->HasSwitch(
      ::switches::kEnableNwebEx);
  String background_url;
  //  if InnerNode is not the image , find image from fallback.
  Node* inner_node = result.InnerNode();
  if (inner_node && inner_node->IsContainerNode() &&
      result.AbsoluteImageURL().IsEmpty() && !is_browser) {
    for (const auto& hit_test_result_node : result.ListBasedTestResult()) {
      Node* node = hit_test_result_node.Get();
      if (!node) {
        continue;
      }
      if (IsHitTestStopNode(*node)) {
        break;
      }
      // the node is <IMG> , try to get image url.
      if (IsA<HTMLImageElement>(node)) {
        HTMLImageElement* img_element = To<HTMLImageElement>(node);
        AtomicString img_src = img_element->getAttribute(html_names::kSrcAttr);
        if (!img_src.empty()) {
          data.src_url = GURL(inner_node->GetDocument().CompleteURL(
                blink::StripLeadingAndTrailingHTMLSpaces(img_src)));
            data.media_type = mojom::ContextMenuDataMediaType::kImage;
            data.media_flags |= ContextMenuData::kMediaCanPrint;
            data.has_image_contents = true;
            break;
          }
        }

      // try to get background image url.
      const ComputedStyle* style =
          node->GetComputedStyleForElementOrLayoutObject();
      if (!style || !style->HasBackgroundImage()) {
        continue;
      }

      StyleImage* style_image = style->BackgroundLayers().GetImage();
      if (!style_image) {
        continue;
      }

      CSSValue* value = style_image->ComputedCSSValue(
          *style, false, CSSValuePhase::kResolvedValue);
      if (value && value->IsURIValue()) {
        background_url =
            DynamicTo<cssvalue::CSSURIValue>(*value)->ValueForSerialization();
        break;
      } else if (value && value->IsImageValue() &&
                 source_type == kMenuSourceLongPress) {
        background_url = DynamicTo<CSSImageValue>(*value)->RelativeUrl();
        break;
      }
    }
  }
}

void ContextMenuControllerExt::SetArkWebMenuData(ContextMenuData& data, HitTestResult& result) {
  data.src_url = GURL(result.AbsoluteImageURL());
  data.media_type = mojom::blink::ContextMenuDataMediaType::kImage;
  data.media_flags |= ContextMenuData::kMediaCanPrint;
  data.has_image_contents = result.GetImage() && !result.GetImage()->IsNull();
}


void ContextMenuControllerExt::IsAILink(ContextMenuData& data, HitTestResult& result) {
  data.is_ai_link = false;
  if (!data.link_url.is_empty()) {
    Element* link_element = result.InnerElement();
    if (link_element) {
      auto attributes_collection = link_element->AttributesWithoutUpdate();
      for (const Attribute& attr : attributes_collection) {
        if (attr.LocalName().GetString() == kAILinkAttr) {
          data.is_ai_link = true;
        }
      }
    }
  }
}

void ContextMenuControllerExt::SetImageRectFromPotentialImageNode(
    ContextMenuData& data,
    const blink::Node* potential_image_node) {
  if (potential_image_node && data.has_image_contents) {
    LayoutBox* layout_box = potential_image_node->GetLayoutBox();
    if (layout_box) {
      data.image_rect =
          gfx::ToEnclosingRect(layout_box->AbsoluteContentQuad().BoundingBox());
      LOG(DEBUG) << "potential_image_node image_rect: "
                 << data.image_rect.ToString();
    }
  }
}
#endif  // ARKWEB_MENU

void ContextMenuControllerExt::HandleArkWebContextMenu(
    ContextMenuData& data,
    LocalFrame* frame,
    HitTestResult& result,
    LocalFrame* selected_frame,
    WebMenuSourceType source_type) {
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  bool contextmenu_customization_enabled =
      page_->GetSettings().IsContextMenuCustomizationEnabled();
  if (contextmenu_customization_enabled &&
      source_type == kMenuSourceShowFreeCopyMenu &&
      data.selected_text.empty() && !selected_frame->SelectedText().empty()) {
    data.selected_text = selected_frame->SelectedText().Utf8();
  }

  data.is_selectable = false;
  if (contextmenu_customization_enabled) {
    data.is_selectable = ShouldShowFreeCopyMenu(result);
  }
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  if (data.media_type == mojom::blink::ContextMenuDataMediaType::kImage &&
      frame && frame->GetPage() && frame->View()) {
    const gfx::Rect& image_rect = result.imp_utils_->GetReplacedContentRect();
    gfx::Size image_size_in_pixels = gfx::ScaleToFlooredSize(
        image_rect.size(), frame->GetPage()->GetVisualViewport().Scale());
    gfx::Point adjusted_image_location =
        frame->View()->FrameToViewport(image_rect.origin());
    data.image_rect = gfx::Rect(adjusted_image_location, image_size_in_pixels);
  }  else if (!data.link_url.is_empty() &&
      frame && frame->GetPage() && frame->View()) {
    const gfx::Rect& image_rect = result.imp_utils_->GetLinkElementRect();
    gfx::Size image_size_in_pixels = gfx::ScaleToFlooredSize(
        image_rect.size(), frame->GetPage()->GetVisualViewport().Scale());
    gfx::Point adjusted_image_location =
        frame->View()->FrameToViewport(image_rect.origin());
    data.image_rect = gfx::Rect(adjusted_image_location, image_size_in_pixels);
  }
#endif
}
}  // namespace blink
