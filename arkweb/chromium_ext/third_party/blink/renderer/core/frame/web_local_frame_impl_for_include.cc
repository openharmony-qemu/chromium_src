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
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
#include "third_party/blink/renderer/core/editing/selection_controller.h"
#endif
#if BUILDFLAG(ARKWEB_AI)
#include "third_party/blink/renderer/core/editing/selection_controller.h"
#endif

namespace blink {

#if BUILDFLAG(ARKWEB_MENU)
void WebLocalFrameImpl::SelectRangeV2(const gfx::Point& position,
                                      bool is_base) {
  TRACE_EVENT0("blink", "WebLocalFrameImpl::SelectRangeV2");

  // TODO(editing-dev): The use of UpdateStyleAndLayout
  // needs to be audited.  See http://crbug.com/590369 for more details.
  GetFrame()->GetDocument()->UpdateStyleAndLayout(
      DocumentUpdateReason::kSelection);

  GetFrame()->Selection().AsFrameSelectionExt()->MoveRangeSelection(
      GetFrame()->View()->ViewportToFrame(position), is_base,
      blink::TextGranularity::kCharacter);
}
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
void WebLocalFrameImpl::DidSubresourceFiltered() {
  if (!Client()) {
    return;
  }

  Client()->AsWebLocalFrameClientExt()->DidSubresourceFiltered();
}

bool WebLocalFrameImpl::GetGlobalAdblockEnabled() {
  if (!Client()) {
    return false;
  }

  return Client()->AsWebLocalFrameClientExt()->GetGlobalAdblockEnabled();
}

void WebLocalFrameImpl::SetHasElemHideTypeOption(
    bool has_elemhide_type_option) {
  frame_->SetHasElemHideTypeOption(has_elemhide_type_option);
}

void WebLocalFrameImpl::SetHasDocumentTypeOption(
    bool has_document_type_option) {
  frame_->SetHasDocumentTypeOption(has_document_type_option);
}

void WebLocalFrameImpl::SetHasGenericHideTypeOption(
    bool has_generichide_type_option) {
  frame_->SetHasGenericHideTypeOption(has_generichide_type_option);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
void WebLocalFrameImpl::SelectClosetWordAndShowSelectionMenu() {
  if (!ViewImpl() || !ViewImpl()->GetPage()) {
    return;
  }

  SelectionController& selection_controller =
      GetFrame()->GetEventHandler().GetSelectionController();
  if (selection_controller.ShowSelectionByLastLongPressHitTestResult()) {
    selection_controller.FocusDocumentView();
  }
}
#endif

#if BUILDFLAG(ARKWEB_AI)
void WebLocalFrameImpl::OnDataDetectorSelectText() {
  LOG(DEBUG) << "WebLocalFrameImpl::OnDataDetectorSelectText";
  if (!ViewImpl() || !ViewImpl()->GetPage()) {
    return;
  }
  SelectionController& selection_controller =
      GetFrame()->GetEventHandler().GetSelectionController();
  if (selection_controller.ShowSelectionByLastLinkHitTestResult()) {
    selection_controller.FocusDocumentView();
  }
}
#endif
}  // namespace blink
