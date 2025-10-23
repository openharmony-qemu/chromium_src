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

namespace pdf {

#if BUILDFLAG(ARKWEB_PDF)
void PDFDocumentHelper::UpdateClientClippedSelectionBoundsForPDF(const gfx::Rect& clipped_selection_bounds) {
  if (!touch_selection_controller_client_manager_) {
    InitTouchSelectionClientManager();
  }

  if (!touch_selection_controller_client_manager_) {
    LOG(ERROR) << __func__ << ", PDF touch_selection_controller_client_manager_ is null.";
    return;
  }

  LOG(DEBUG) << "PDF clipped selection bounds: " << clipped_selection_bounds.ToString();
  gfx::Point bounds_origin = clipped_selection_bounds.origin();
  gfx::Size bounds_size = clipped_selection_bounds.size();
  gfx::PointF bounds_origin_f =
    ConvertToRoot(gfx::PointF(bounds_origin.x(), bounds_origin.y()));
  bounds_origin.set_x(bounds_origin_f.x());
  bounds_origin.set_y(bounds_origin_f.y());
  gfx::Rect converted_bounds(bounds_origin, bounds_size);
  touch_selection_controller_client_manager_->
      ConvertClientClippedSelectionBounds(converted_bounds);
  touch_selection_controller_client_manager_->
      UpdateClientClippedSelectionBounds(converted_bounds);
}

void PDFDocumentHelper::HideHandleAndQuickMenuForPDF(bool hide_handles) {
  if (!touch_selection_controller_client_manager_) {
    InitTouchSelectionClientManager();
  }

  if (!touch_selection_controller_client_manager_) {
    LOG(ERROR) << __func__ << ", PDF touch_selection_controller_client_manager_ is null.";
    return;
  }
  
  LOG(DEBUG) << "PDF hide handle and quick nenu: " << hide_handles;
  touch_selection_controller_client_manager_->
      HideHandleAndQuickMenuIfNecessary(hide_handles);
  if (!hide_handles) {
    UpdateQuickMenu();
  }
}

void PDFDocumentHelper::ResetResponsePendingInputEvent() {
  if (!touch_selection_controller_client_manager_) {
    InitTouchSelectionClientManager();
  }

  if (!touch_selection_controller_client_manager_) {
    LOG(ERROR) << __func__ << ", PDF touch_selection_controller_client_manager_ is null.";
    return;
  }
  
  touch_selection_controller_client_manager_->
      ResetResponsePendingInputEvent();
}

void PDFDocumentHelper::UpdateQuickMenu() {
  if (!touch_selection_controller_client_manager_) {
    InitTouchSelectionClientManager();
  }

  if (!touch_selection_controller_client_manager_) {
    LOG(ERROR) << __func__ << ", PDF touch_selection_controller_client_manager_ is null.";
    return;
  }

  touch_selection_controller_client_manager_->UpdateQuickMenu();
}
#endif  // BUILDFLAG(ARKWEB_PDF)

}  // namespace chrome_pdf
