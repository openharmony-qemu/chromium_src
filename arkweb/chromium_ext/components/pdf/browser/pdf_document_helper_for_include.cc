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
void PDFDocumentHelper::ConvertAndUpdateSelectionBounds(
    const gfx::Rect& clipped_selection_bounds) {
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

  // Convert selection bounds. The converted_bounds value will change.
  touch_selection_controller_client_manager_->
      ConvertClientClippedSelectionBounds(converted_bounds);
  // Update the final converted_bounds.
  touch_selection_controller_client_manager_->
      UpdateClientClippedSelectionBounds(converted_bounds);
}

void PDFDocumentHelper::HideHandleAndQuickMenu(bool hide) {
  if (!touch_selection_controller_client_manager_) {
    InitTouchSelectionClientManager();
  }

  if (!touch_selection_controller_client_manager_) {
    LOG(ERROR) << __func__ << ", PDF touch_selection_controller_client_manager_ is null.";
    return;
  }
  
  LOG(DEBUG) << "PDF hide handle and quick menu: " << hide;
  touch_selection_controller_client_manager_->
      HideHandleAndQuickMenuIfNecessary(hide);
  if (!hide) {
    touch_selection_controller_client_manager_->SetQuickMenuRequested(true);
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

int32_t PDFDocumentHelper::SafeScale(int32_t value, float scale_factor) {
  double result = static_cast<double>(value) * static_cast<double>(scale_factor);
  // Check if it exceeds the range of int32_t.
  if (result > std::numeric_limits<int32_t>::max()) {
    return std::numeric_limits<int32_t>::max();
  }
  if (result < std::numeric_limits<int32_t>::min()) {
    return std::numeric_limits<int32_t>::min();
  }
  return static_cast<int32_t>(std::round(result));
}

void PDFDocumentHelper::SetIsPdfDocument(bool is_pdf_document) {
  if (!touch_selection_controller_client_manager_) {
    InitTouchSelectionClientManager();
  }

  if (!touch_selection_controller_client_manager_) {
    LOG(ERROR) << __func__ << ", PDF touch_selection_controller_client_manager_ is null.";
    return;
  }

  LOG(INFO) << "Set is PDF document: " << is_pdf_document;
  touch_selection_controller_client_manager_->SetIsPdfDocument(is_pdf_document);
}

void PDFDocumentHelper::ClearTextSelection() {
  if (!remote_pdf_client_) {
    LOG(ERROR) << __func__ << ", PDF remote_pdf_client_ is null.";
    return;
  }
  remote_pdf_client_->ClearTextSelection();
}

void PDFDocumentHelper::UpdateScaleFactor() {
  auto main_frame = render_frame_host().GetOutermostMainFrameOrEmbedder();
  if (!main_frame) {
    LOG(ERROR) << __func__ << ", PDF main_frame is null.";
    return;
  }
  auto host_impl = content::RenderFrameHostImpl::From(main_frame);
  if (!host_impl) {
    LOG(ERROR) << __func__ << ", PDF host_impl is null.";
    return;
  }
  float page_scale_factor = host_impl->GetPageScaleFactor();
  if (page_scale_factor <= 0) {
    LOG(ERROR) << __func__ << ", PDF invalid scale factor.";
    return;
  }
  page_scale_factor_ = page_scale_factor;
}

void PDFDocumentHelper::OnScaleChanged(float new_page_scale_factor) {
  page_scale_factor_ = new_page_scale_factor;
  if (!remote_pdf_client_) {
    LOG(ERROR) << __func__ << ", PDF remote_pdf_client_ is null.";
    return;
  }
  remote_pdf_client_->OnScaleChanged();
}
#endif  // BUILDFLAG(ARKWEB_PDF)

}  // namespace chrome_pdf
