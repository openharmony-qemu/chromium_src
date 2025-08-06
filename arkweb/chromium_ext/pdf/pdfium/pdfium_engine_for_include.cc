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

void PDFiumEngine::OnSelectionPositionChangedForPDF(gfx::Rect& left,
                                                    gfx::Rect& right,
                                                    gfx::Rect& clipped_selection_bounds,
                                                    const std::vector<PDFiumRange>& selections) {
  if (!selections.empty()) {
    int rect_left = std::numeric_limits<int32_t>::max();
    int rect_top = std::numeric_limits<int32_t>::max();
    int rect_right = 0;
    int rect_bottom = 0;
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
      const std::vector<gfx::Rect>& screen_rects =
        sel.GetScreenRects(GetVisibleRect().origin(), current_zoom_,
                           layout_.options().default_page_orientation());
      for (const auto& rect : screen_rects) {
        rect_left = std::min(rect_left, rect.x());
        rect_top = std::min(rect_top, rect.y());
        rect_right = std::max(rect_right, rect.x() + rect.width());
        rect_bottom = std::max(rect_bottom, rect.y() + rect.height());
      }
    }

    clipped_selection_bounds = gfx::Rect(rect_left, rect_top,
                                         rect_right - rect_left, rect_bottom - rect_top);
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