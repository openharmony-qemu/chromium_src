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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/page/drag_controller_ext.h"

#include <memory>

#include "third_party/blink/renderer/core/clipboard/data_transfer.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/dom/node.h"
#include "third_party/blink/renderer/core/dom/node_computed_style.h"
#include "third_party/blink/renderer/core/editing/frame_selection.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/visual_viewport.h"
#include "third_party/blink/renderer/core/layout/hit_test_result.h"
#include "third_party/blink/renderer/core/layout/layout_image.h"
#include "third_party/blink/renderer/core/layout/layout_object.h"
#include "third_party/blink/renderer/core/page/chrome_client.h"
#include "third_party/blink/renderer/core/page/drag_image.h"
#include "third_party/blink/renderer/core/page/drag_state.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "ui/display/screen_info.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace blink {
#if BUILDFLAG(ARKWEB_DRAG_DROP)
static const char* kDragLinkGrayStyle = "; color: gray;";
static const char* kDragLinkGrayStyleRemoveType1 = "color: gray;";
static const char* kDragLinkGrayStyleRemoveType2 = "color: gray";

extern bool CanDragImage(const Element& element);
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
DragControllerExt::DragControllerExt(Page* page)
    : DragController(page),
      is_dragging_(false),
      did_start_drag_image_effects_(false) {}

DragControllerExt::~DragControllerExt() {}
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
bool DragControllerExt::IsHyperLinkDragging() {
  if (!drag_state_) {
    LOG(DEBUG) << "DragDrop state null, drag nothing";
    return false;
  }

  if (drag_state_->drag_type_ != kDragSourceActionLink) {
    return false;
  }

  Node* node = drag_state_->drag_src_.Get();
  if (!node) {
    LOG(DEBUG) << "DragDrop node null, drag nothing";
    return false;
  }
  Element* element = static_cast<Element*>(node);
  if (!element) {
    LOG(WARNING) << "DragDrop element null, not a element node";
    return false;
  }

  auto document = node->ownerDocument();
  if (!document) {
    LOG(WARNING) << "DragDrop document null, not a hyper link dragging";
    return false;
  }
  LocalFrame* frame = document->GetFrame();
  if (!frame) {
    LOG(WARNING) << "DragDrop frame null, not a hyper link dragging";
    return false;
  }
  return drag_state_->drag_type_ == kDragSourceActionLink &&
         did_initiate_drag_ && frame->Selection().SelectionHasFocus();
}

bool DragControllerExt::DragLinkCheckSrcAndType() {
  if (!drag_state_) {
    LOG(DEBUG) << "DragDrop state null, drag nothing";
    return false;
  }
  if (drag_state_->drag_type_ != kDragSourceActionLink) {
    LOG(DEBUG) << "DragDrop type is not link, just pass";
    return false;
  }

  if (!did_initiate_drag_) {
    LOG(DEBUG) << "DragDrop drag not start";
    return false;
  }

  if (!drag_state_->drag_src_) {
    LOG(WARNING) << "DragDrop node src null, drag nothing";
    return false;
  }

  Node* node = drag_state_->drag_src_.Get();
  if (!node) {
    LOG(WARNING) << "DragDrop node src null, drag nothing";
    return false;
  }
  return true;
}

void DragControllerExt::UpdateLinkStyle(Node* node) {
  if (!node) {
    LOG(WARNING) << "DragDrop node null, do not update style";
    return;
  }

  NodeList* list = node->childNodes();
  if (!list) {
    LOG(WARNING) << "DragDrop has no child nodes, no need to update";
    return;
  }

  unsigned len = list->length();
  const WTF::String grayStyle(kDragLinkGrayStyle);
  for (unsigned i = 0; i < len; i++) {
    Node* tempNode = list->item(i);
    if (!tempNode) {
      continue;
    }

    auto* tempEle = DynamicTo<Element>(tempNode);
    if (!tempEle) {
      continue;
    }

    StringBuilder tmpStyle;
    tmpStyle.Append(
        String(AtomicString(tempEle->getAttribute(html_names::kStyleAttr))));
    tmpStyle.Append(grayStyle);
    tempEle->setAttribute(html_names::kStyleAttr, tmpStyle.ToAtomicString());
    UpdateLinkStyle(tempNode);
  }
}

NO_SANITIZE("cfi") void DragControllerExt::StartDragLinkEffects() {
  if (!DragLinkCheckSrcAndType()) {
    LOG(DEBUG) << "DragDrop check not pass, no need to change style";
    return;
  }

  Node* node = drag_state_->drag_src_.Get();
  if (!node) {
    LOG(WARNING) << "DragDrop node null, drag nothing";
    return;
  }

  auto* element = DynamicTo<Element>(node);
  if (element) {
    element->GetDocument().UpdateStyleAndLayoutTree();
  }

  StringBuilder tmpStyle;
  const WTF::String grayStyle(kDragLinkGrayStyle);
  tmpStyle.Append(
      String(AtomicString(element->getAttribute(html_names::kStyleAttr))));
  tmpStyle.Append(grayStyle);
  element->setAttribute(html_names::kStyleAttr, tmpStyle.ToAtomicString());
  UpdateLinkStyle(node);

  LayoutObject* layout_object = node->GetLayoutObject();
  if (layout_object) {
    layout_object->SetNeedsPaintPropertyUpdate();
  }
  InvalidateSelectionForDrag(node->ownerDocument());
}

void DragControllerExt::RestoreLinkStyle(Node* node) {
  if (!node) {
    LOG(DEBUG) << "DragDrop node null, do not update style";
    return;
  }

  NodeList* list = node->childNodes();
  if (list) {
    unsigned len = list->length();
    for (unsigned i = 0; i < len; i++) {
      Node* tempNode = list->item(i);
      if (tempNode) {
        auto* tempEle = DynamicTo<Element>(tempNode);
        if (tempEle) {
          FindAndRemoveGrayStyle(tempEle);
          RestoreLinkStyle(tempNode);
        }
      }
    }
  }
}

void DragControllerExt::FindAndRemoveGrayStyle(Element* tempEle) {
  const WTF::String grayStyle(kDragLinkGrayStyle);
  const WTF::String grayStyle1(kDragLinkGrayStyleRemoveType1);
  const WTF::String grayStyle2(kDragLinkGrayStyleRemoveType2);
  String styleAttr =
      String(AtomicString(tempEle->getAttribute(html_names::kStyleAttr)));
  size_t pos = styleAttr.Find(grayStyle);
  if (pos != WTF::kNotFound) {
    if (pos + grayStyle.length() == styleAttr.length()) {
      String oriStyle =
          styleAttr.replace((unsigned)pos, grayStyle.length(), "");
      tempEle->setAttribute(html_names::kStyleAttr, AtomicString(oriStyle));
    } else {
      String oriStyle =
          styleAttr.replace((unsigned)pos, grayStyle.length(), ";");
      tempEle->setAttribute(html_names::kStyleAttr, AtomicString(oriStyle));
    }
    return;
  }

  pos = styleAttr.Find(grayStyle1);
  if (pos != WTF::kNotFound) {
    String oriStyle = styleAttr.replace((unsigned)pos, grayStyle1.length(), "");
    tempEle->setAttribute(html_names::kStyleAttr, AtomicString(oriStyle));
    return;
  }

  pos = styleAttr.Find(grayStyle2);
  if (pos != WTF::kNotFound) {
    String oriStyle = styleAttr.replace((unsigned)pos, grayStyle2.length(), "");
    tempEle->setAttribute(html_names::kStyleAttr, AtomicString(oriStyle));
    return;
  }
}

NO_SANITIZE("cfi") void DragControllerExt::RestoreDragLinkEffects() {
  if (!DragLinkCheckSrcAndType()) {
    LOG(DEBUG) << "DragDrop check not pass, no need to change style";
    return;
  }

  Node* node = drag_state_->drag_src_.Get();
  if (!node) {
    LOG(DEBUG) << "DragDrop node null, drag nothing";
    return;
  }
  auto* element = DynamicTo<Element>(node);
  if (element) {
    element->GetDocument().UpdateStyleAndLayoutTree();
    element->GetDocument().UpdateHoverActiveState(false, false, nullptr);
  }

  FindAndRemoveGrayStyle(element);
  RestoreLinkStyle(node);

  LayoutObject* layout_object = node->GetLayoutObject();
  if (layout_object) {
    layout_object->SetNeedsPaintPropertyUpdate();
  }
  InvalidateSelectionForDrag(node->ownerDocument());
}

NO_SANITIZE("cfi") void DragControllerExt::StartDragTextEffects() {
  if (!drag_state_) {
    return;
  }

  if (!drag_state_->drag_src_) {
    return;
  }

  Node* node = drag_state_->drag_src_.Get();
  if (!node) {
    return;
  }

  // textEffect should not happend in ImageDrag
  Element* element = static_cast<Element*>(node);
  // Add the judgment of the dragging type for mixed dragging of image and text
  if (blink::CanDragImage(*element) &&
      drag_state_->drag_type_ == kDragSourceActionImage) {
    return;
  }

  InvalidateSelectionForDrag(node->ownerDocument());
}

NO_SANITIZE("cfi") void DragControllerExt::RestoreDragTextEffects() {
  if (!drag_state_) {
    return;
  }

  if (!drag_state_->drag_src_) {
    return;
  }

  Node* node = drag_state_->drag_src_.Get();
  if (!node) {
    return;
  }

  // ImageDrag should directly return
  Element* element = static_cast<Element*>(node);

  if (blink::CanDragImage(*element) &&
      drag_state_->drag_type_ == kDragSourceActionImage) {
    return;
  }

  InvalidateSelectionForDrag(node->ownerDocument());
}

bool DragControllerExt::IsInTextDraging() {
  if (!drag_state_) {
    return false;
  }
  return drag_state_->drag_type_ == kDragSourceActionSelection &&
         did_initiate_drag_;
}

bool DragControllerExt::IsInImageDraging() {
  if (!drag_state_) {
    return false;
  }
  return drag_state_->drag_type_ == kDragSourceActionImage &&
         did_initiate_drag_;
}

NO_SANITIZE("cfi") void DragControllerExt::StartDragImageEffects() {
  if (!drag_state_) {
    return;
  }

  if (!drag_state_->drag_src_) {
    return;
  }

  Node* node = drag_state_->drag_src_.Get();
  if (!node) {
    return;
  }

  // draggable or size should be checked before this call
  Element* element = static_cast<Element*>(node);
  if (!blink::CanDragImage(*element)) {
    return;
  }

  const ComputedStyle* style = node->GetComputedStyleForElementOrLayoutObject();
  // no effects if the image already have an opacity style
  if (!style || style->HasOpacity()) {
    return;
  }

  origin_style_.Clear();
  origin_style_.Append(
      String(AtomicString(element->getAttribute(html_names::kStyleAttr))));
  did_start_drag_image_effects_ = true;

  StringBuilder image_drag_style;
  image_drag_style.Append(origin_style_);
  image_drag_style.Append("; filter: opacity(0.4);");
  element->setAttribute(html_names::kStyleAttr,
                        image_drag_style.ToAtomicString());

  InvalidateSelectionForDrag(node->ownerDocument());
}

NO_SANITIZE("cfi") void DragControllerExt::RestoreDragImageEffects() {
  if (!drag_state_) {
    return;
  }

  if (!drag_state_->drag_src_) {
    return;
  }

  Node* node = drag_state_->drag_src_.Get();
  if (!node) {
    return;
  }

  Element* element = static_cast<Element*>(node);
  if (!blink::CanDragImage(*element)) {
    return;
  }

  const ComputedStyle* style = node->GetComputedStyleForElementOrLayoutObject();
  if (!style) {
    return;
  }
  if (did_start_drag_image_effects_) {
    element->setAttribute(html_names::kStyleAttr,
                          origin_style_.ToAtomicString());
  }
  did_start_drag_image_effects_ = false;
}

void DragControllerExt::InvalidateSelectionForDrag(Document* document) {
  if (!document) {
    return;
  }
  LocalFrame* frame = document->GetFrame();
  if (!frame) {
    return;
  }
  frame->Selection().AsFrameSelectionExt()->InvalidateSelectionForDrag();
}

gfx::RectF DragControllerExt::GetVisibleRectToUIInRootFrame(LocalFrame* frame) {
  if (!frame || !frame->View() || !frame->GetPage() || !page_) {
    return gfx::RectF();
  }

  gfx::Rect visibleRect =
      page_->GetChromeClient().AsChromeClientExt()->GetVisibleRectToWeb(frame);
  gfx::RectF visible_rect_in_root_frame(visibleRect);
  auto scroll_offset = frame->GetPage()->GetVisualViewport().GetScrollOffset();
  float page_scale_factor = frame->GetPage()->PageScaleFactor();

  visible_rect_in_root_frame.Scale(1.f / page_scale_factor);
  visible_rect_in_root_frame.Offset(scroll_offset);
  LOG(INFO) << "Dragdrop, visible_rect_in_root_frame : "
            << visible_rect_in_root_frame.ToString()
            << ", page_scale_factor : " << page_scale_factor
            << ", scroll_offset : " << scroll_offset.ToString();
  return visible_rect_in_root_frame;
}

#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
}  // namespace blink
