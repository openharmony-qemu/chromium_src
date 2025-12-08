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

namespace {

#if BUILDFLAG(ARKWEB_PDF)
constexpr int kPageShadowTop = 3;
constexpr int kPageShadowBottom = 7;
constexpr int kPageShadowLeft = 5;
constexpr int kPageShadowRight = 5;

constexpr float kDisplayRatio = 96.0f;
constexpr float kPointRatio = 72.0f;
#endif  // BUILDFLAG(ARKWEB_PDF)

}  // namespace

#if BUILDFLAG(ARKWEB_PDF)
std::atomic<uint64_t> g_bookmark_id_{0};

void PDFiumEngine::UpdateSelectionBoundsAndPositions(gfx::Rect& left,
                                                     gfx::Rect& right,
                                                     gfx::Rect& clipped_selection_bounds,
                                                     const std::vector<PDFiumRange>& selections) {
  if (!selections.empty()) {
    int32_t rect_left = std::numeric_limits<int32_t>::max();
    int32_t rect_top = std::numeric_limits<int32_t>::max();
    int32_t rect_right = std::numeric_limits<int32_t>::min();
    int32_t rect_bottom = std::numeric_limits<int32_t>::min();
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

static std::string GenerateBookmarkId() {
  uint64_t id = g_bookmark_id_.fetch_add(1, std::memory_order_relaxed);
  if (id >= std::numeric_limits<uint64_t>::max() - 1) {
    LOG(ERROR) << "Bookmark ID overflow detected";
    return std::string("invalid_") + std::to_string(id);
  }
  return std::string("arkweb_pdf_") + std::to_string(id);
}

void PDFiumEngine::OnClickBookmark(const std::string& bookmark_id) {
  auto iter = bookmark_store_.find(bookmark_id);
  if (iter == bookmark_store_.end()) {
    LOG(ERROR) << "Click on bookmark " << bookmark_id << " but not found in bookmark store.";
    return;
  }
  FPDF_BOOKMARK bookmark = iter->second;
  if (!bookmark) {
    LOG(ERROR) << "Click on bookmark " << bookmark_id << " but it is nullptr.";
    return;
  }

  FPDF_DEST dest = FPDFBookmark_GetDest(doc(), bookmark);
  // Some bookmarks don't have a page to select.
  if (dest) {
    int page_index = FPDFDest_GetDestPageIndex(doc(), dest);
    if (PageIndexInBounds(page_index)) {
      std::optional<float> x;
      std::optional<float> y;
      std::optional<float> zoom;
      pages_[page_index]->GetPageDestinationTarget(dest, &x, &y, &zoom);
      // All things are here. But we need to refresh the frontend page status.
      // So we try to notify front-end to do navigation and scrolling.

      // Apply zoom and orientation.
      // like front-end does: this.viewport.convertPageToScreen(e.detail.page, e.detail);
      gfx::PointF point;
      if (x && y) {
        point = ConverPageToScreen(page_index, gfx::PointF(x.value_or(0.0), y.value_or(0.0)));
        // Apply shadow offset of left and top.
        point.Offset(kPageShadowLeft, kPageShadowTop);
      }
      float xf = point.x();
      float yf = point.y();
      client_->NavigateToDestination(page_index,
                                     &xf,
                                     &yf,
                                     base::OptionalToPtr(zoom));
    }
  } else {
    // Extract URI for bookmarks linking to an external page.
    FPDF_ACTION action = FPDFBookmark_GetAction(bookmark);
    if (action) {
      std::string uri = CallPDFiumStringBufferApi(
          base::BindRepeating(&FPDFAction_GetURIPath, doc(), action),
          /*check_expected_size=*/true);
      if (!uri.empty() && base::IsStringUTF8AllowingNoncharacters(uri)) {
        client_->NavigateTo(uri, WindowOpenDisposition::NEW_BACKGROUND_TAB);
      }
    }
  }
}

gfx::PointF PDFiumEngine::ConverPageToScreen(int page_index, gfx::PointF point) {
  if (static_cast<size_t>(page_index) > layout_.page_count()) {
    return gfx::PointF();
  }

  // Calculate the bounds for page minus the shadows.
  const gfx::Rect& page_rect = layout_.page_bounds_rect(page_index);
  int height = page_rect.height();
  int width = page_rect.width();
  // The point and pixel ratio as 96 DPI.
  const float points_to_pixels = kDisplayRatio / kPointRatio;
  float current_x = point.x() * points_to_pixels;
  float current_y = point.y() * points_to_pixels;

  // The PDF coordinate origin is bottom left. Reverse the Y-axis.
  current_y = height - current_y;

  float rotated_x = current_x;
  float rotated_y = current_y;

  switch (layout_.options().default_page_orientation()) {
    case PageOrientation::kClockwise90:
      rotated_x = width - current_y;
      rotated_y = current_x;
      break;
    case PageOrientation::kClockwise180:
      rotated_x = width - current_x;
      rotated_y = height - current_y;
      break;
    case PageOrientation::kClockwise270:
      rotated_x = current_y;
      rotated_y = height - current_x;
      break;
    default:
      break;
  }

  gfx::PointF transformed_point(rotated_x, rotated_y);
  return transformed_point;
}

void PDFiumEngine::CheckSelectionVisibility(gfx::Rect& top,
                                            gfx::Rect& bottom,
                                            gfx::Rect& clipped_selection_bounds) {
  // Check if selections are unvisible.
  if (clipped_selection_bounds.x() > plugin_size().width() ||
      clipped_selection_bounds.x() + clipped_selection_bounds.width() < 0 ||
      clipped_selection_bounds.y() > plugin_size().height() ||
      clipped_selection_bounds.y() + clipped_selection_bounds.height() < 0) {
    client_->SetIsSelectionVisible(false);
    return;
  }

  client_->SetIsSelectionVisible(true);
  if (top.x() < 0) {
    top.set_x(top.x() - plugin_size().width());
  }
  if (top.y() < 0) {
    top.set_y(top.y() - plugin_size().height());
  }
  if (bottom.x() + bottom.width() < 0) {
    bottom.set_x(bottom.x() - plugin_size().width());
  }
  if (bottom.y() < 0) {
    bottom.set_y(bottom.y() - plugin_size().height());
  }
}
#endif  // BUILDFLAG(ARKWEB_PDF)

}  // namespace chrome_pdf