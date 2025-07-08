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

namespace chrome_pdf {

void PDFiumEngine::OnSelectionPositionChangedForPDF(gfx::Rect& left, gfx::Rect& right, const std::vector<PDFiumRange>& selections) {
    if (!selections.empty()) {
    PDFiumRange fitst_selection = selections[0];
    PDFiumRange last_selection = selections.back();
    for (const auto& sel : selections) {
      if (fitst_selection.page_index() > sel.page_index() ||
          (fitst_selection.page_index() == sel.page_index() &&
          fitst_selection.char_index() > sel.char_index())) {
        fitst_selection = sel;
      }
      if (last_selection.page_index() < sel.page_index() ||
          (last_selection.page_index() == sel.page_index() &&
          last_selection.char_index() < sel.char_index())) {
        last_selection = sel;
      }
    }
    const std::vector<gfx::Rect>& left_screen_rects =
        fitst_selection.GetScreenRects(GetVisibleRect().origin(), current_zoom_,
                           layout_.options().default_page_orientation());
    const std::vector<gfx::Rect>& right_screen_rects =
        last_selection.GetScreenRects(GetVisibleRect().origin(), current_zoom_,
                           layout_.options().default_page_orientation());
    if (!left_screen_rects.empty()) {
      left = left_screen_rects[0];
    }
    if (!right_screen_rects.empty()) {
      right = right_screen_rects.back();
    }
  }
}

}  // namespace chrome_pdf