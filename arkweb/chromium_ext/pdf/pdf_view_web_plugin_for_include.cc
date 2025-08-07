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

#if BUILDFLAG(ARKWEB_PDF)
void PdfViewWebPlugin::NotifyPdfScrollAtBottom(float scroll_position_y, float max_y) {
  if (document_size_.height() == 0) {
    return;
  }
  const int EPS_PDF_SCROLL_POSITION_Y = 2;
  const bool last_scroll_at_bottom_status = scroll_at_bottom_status_;
  if (scroll_position_y + EPS_PDF_SCROLL_POSITION_Y > max_y) {
    scroll_at_bottom_status_ = true;
  } else if (scroll_position_y + plugin_dip_size_.height() < max_y) {
    scroll_at_bottom_status_ = false;
  }
  if (!last_scroll_at_bottom_status && scroll_at_bottom_status_) {
    LOG(INFO) << "PdfViewWebPlugin::NotifyPdfScrollAtBottom, scroll_position_y: " << scroll_position_y
              << ", plugin_dip_size_.height(): " << plugin_dip_size_.height()
              << ", max_y: " << max_y;
    client_->OnPdfScrollAtBottom(url_);
  }
}

// static
int32_t PdfViewWebPlugin::CastFpdfErrorToPdfLoadEvent(int pdf_error) {
  enum class PdfLoadEvent : int32_t {
    LOAD_SUCCESS = 0,
    PARSE_ERROR_FILE = 1,
    PARSE_ERROR_FORMAT = 2,
    PARSE_ERROR_PASSWORD = 3,
    PARSE_ERROR_HANDLER = 4
  };
  PdfLoadEvent load_event = PdfLoadEvent::LOAD_SUCCESS;
  switch (pdf_error) {
    case FPDF_ERR_SUCCESS:
      load_event = PdfLoadEvent::LOAD_SUCCESS;
      break;
    case FPDF_ERR_FILE:
      load_event = PdfLoadEvent::PARSE_ERROR_FILE;
      break;
    case FPDF_ERR_FORMAT:
      load_event = PdfLoadEvent::PARSE_ERROR_FORMAT;
      break;
    case FPDF_ERR_PASSWORD:
      load_event = PdfLoadEvent::PARSE_ERROR_PASSWORD;
      break;
    case FPDF_ERR_UNKNOWN:
    case FPDF_ERR_SECURITY:
    case FPDF_ERR_PAGE:
    default:
      load_event = PdfLoadEvent::PARSE_ERROR_HANDLER;
      break;
  }
  return static_cast<int32_t>(load_event);
}

void PdfViewWebPlugin::UpdateClientClippedSelectionBoundsForPDF(const gfx::Rect& clipped_selection_bounds) {
  pdf_host_->UpdateClientClippedSelectionBoundsForPDF(clipped_selection_bounds);
}

void PdfViewWebPlugin::HideHandleAndQuickMenuForPDF(bool hide_handles) {
  pdf_host_->HideHandleAndQuickMenuForPDF(hide_handles);
}
#endif  // BUILDFLAG(ARKWEB_PDF)

}  // namespace chrome_pdf
