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
// The minimum scale level allowed.
constexpr double kMinScale = 0.001f;

constexpr base::TimeDelta kScrollDelay = base::Milliseconds(100);
constexpr base::TimeDelta kSelectionDelay = base::Milliseconds(100);
constexpr base::TimeDelta kPaintDelay = base::Milliseconds(100);
constexpr base::TimeDelta kMenuDelay = base::Milliseconds(100);
#endif  // BUILDFLAG(ARKWEB_PDF)

}  // namespace

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

void PdfViewWebPlugin::ConvertAndUpdateSelectionBounds(gfx::Rect& clipped_selection_bounds) {
  if (std::abs(device_scale_) <= kMinScale) {
    LOG(ERROR) << "PDF device scale is almost equal to 0.";
    return;
  }

  const float inverse_scale = 1.0 / device_scale_;
  gfx::Point converted_origin(clipped_selection_bounds.x() + available_area_.x(),
                              clipped_selection_bounds.y());
  converted_origin.set_x(converted_origin.x() * inverse_scale);
  converted_origin.set_y(converted_origin.y() * inverse_scale);
  gfx::Size converted_size(clipped_selection_bounds.width() * inverse_scale,
                           clipped_selection_bounds.height() * inverse_scale);
  clipped_selection_bounds.set_origin(converted_origin);
  clipped_selection_bounds.set_size(converted_size);
  pdf_host_->ConvertAndUpdateSelectionBounds(clipped_selection_bounds);
}

void PdfViewWebPlugin::SelectionChangedAtScrollStopped() {
  if (!engine_) {
    LOG(ERROR) << __func__ << ", PDF engine_ is null.";
  }
  engine_->SelectionChangedAtScrollStopped();
}

void PdfViewWebPlugin::SetIsTouching(bool is_touching) {
  bool expected_is_touching = is_touching_.load();
  if (expected_is_touching == is_touching) {
    return;
  }
  is_touching_.store(is_touching);
  HideOrShowMenuAfterDelay();
}

void PdfViewWebPlugin::SetIsScrolling(bool is_scrolling) {
  bool expected_is_scrolling = is_scrolling_.load();
  if (expected_is_scrolling == is_scrolling) {
    return;
  }
  is_scrolling_.store(is_scrolling);
  HideOrShowMenuAfterDelay();
}

void PdfViewWebPlugin::SetIsPinching(bool is_pinching) {
  bool expected_is_pinching = is_pinching_.load();
  if (expected_is_pinching == is_pinching) {
    return;
  }
  is_pinching_.store(is_pinching);
  HideOrShowMenuAfterDelay();
}

void PdfViewWebPlugin::SetIsSelectionVisible(bool visible) {
  bool expected_is_selection_visible = is_selection_visible_.load();
  if (expected_is_selection_visible == visible) {
    return;
  }
  is_selection_visible_.store(visible);
  HideOrShowMenuAfterDelay();
}

bool PdfViewWebPlugin::ShouldHideMenu() {
  bool is_selection_visible = is_selection_visible_.load();
  bool is_touching = is_touching_.load();
  bool is_scrolling = is_scrolling_.load();
  bool is_pinching = is_pinching_.load();
  return !is_selection_visible || is_touching || is_scrolling || is_pinching;
}

void PdfViewWebPlugin::HideOrShowMenuAfterDelay() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  bool hide_menu = ShouldHideMenu();
  if (cancelable_menu_delayed_task_.IsValid()) {
    cancelable_menu_delayed_task_.CancelTask();
  } else {
    // If no task can be canceled, run it immediately.
    HideHandleAndQuickMenu(hide_menu);
  }
  auto task_runner = GetTaskRunner();
  if (!task_runner) {
    LOG(ERROR) << "PDF task runner is null.";
    return;
  }
  cancelable_menu_delayed_task_ = task_runner->PostCancelableDelayedTask(
      base::subtle::PostDelayedTaskPassKey(),
      FROM_HERE,
      base::BindOnce(&PdfViewWebPlugin::HideHandleAndQuickMenu,
                     weak_factory_.GetWeakPtr(),
                     hide_menu),
      kMenuDelay);
}

void PdfViewWebPlugin::HideHandleAndQuickMenu(bool hide) {
  bool expected_is_menu_hidden = is_menu_hidden_.load();
  if (expected_is_menu_hidden == hide) {
    return;
  }
  is_menu_hidden_.store(hide);
  pdf_host_->HideHandleAndQuickMenu(hide);
}

scoped_refptr<base::SequencedTaskRunner> PdfViewWebPlugin::GetTaskRunner() {
  return task_runner_ ? task_runner_
                      : base::SequencedTaskRunner::GetCurrentDefault();
}

void PdfViewWebPlugin::SetScrollStoppedAfterDelay() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (cancelable_scroll_delayed_task_.IsValid()) {
    cancelable_scroll_delayed_task_.CancelTask();
  }
  auto task_runner = GetTaskRunner();
  if (!task_runner) {
    LOG(ERROR) << "PDF task runner is null.";
    return;
  }
  cancelable_scroll_delayed_task_ = task_runner->PostCancelableDelayedTask(
      base::subtle::PostDelayedTaskPassKey(),
      FROM_HERE,
      base::BindOnce(&PdfViewWebPlugin::SetIsScrolling, weak_factory_.GetWeakPtr(), false),
      kScrollDelay);
}

void PdfViewWebPlugin::ResetResponsePendingInputEvent() {
  pdf_host_->ResetResponsePendingInputEvent();
}

void PdfViewWebPlugin::HandleClickBookmarkMessage(const base::Value::Dict& message) {
  const std::string* nullableId = message.FindString("id");
  if (!nullableId || nullableId->empty() || nullableId->length() > 255) {
    LOG(ERROR) << "Invalid bookmark ID received";
    return;
  }
  if (!engine_) {
    LOG(ERROR) << __func__ << ", PDF engine_ is null.";
  }
  engine_->OnClickBookmark(*nullableId);
}

void PdfViewWebPlugin::SelectionChangedAfterDelay() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (cancelable_selection_delayed_task_.IsValid()) {
    cancelable_selection_delayed_task_.CancelTask();
  }
  auto task_runner = GetTaskRunner();
  if (!task_runner) {
    LOG(ERROR) << "PDF task runner is null.";
    return;
  }
  cancelable_selection_delayed_task_ = task_runner->PostCancelableDelayedTask(
      base::subtle::PostDelayedTaskPassKey(),
      FROM_HERE,
      base::BindOnce(&PdfViewWebPlugin::SelectionChangedAtScrollStopped, weak_factory_.GetWeakPtr()),
      kSelectionDelay);
}

void PdfViewWebPlugin::ClearTextSelection() {
  if (!engine_) {
    LOG(ERROR) << __func__ << ", PDF engine_ is null.";
  }
  engine_->ClearTextSelection();
}

void PdfViewWebPlugin::DoPaintAtScrollStopped() {
  paint_manager_.DoPaintAtScrollStopped();
}

void PdfViewWebPlugin::DoPaintAfterDelay() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (cancelable_paint_delayed_task_.IsValid()) {
    cancelable_paint_delayed_task_.CancelTask();
  }
  auto task_runner = GetTaskRunner();
  if (!task_runner) {
    LOG(ERROR) << "PDF task runner is null.";
    return;
  }
  cancelable_paint_delayed_task_ = task_runner->PostCancelableDelayedTask(
      base::subtle::PostDelayedTaskPassKey(),
      FROM_HERE,
      base::BindOnce(&PdfViewWebPlugin::DoPaintAtScrollStopped, weak_factory_.GetWeakPtr()),
      kPaintDelay);
}

void PdfViewWebPlugin::OnScaleChanged() {
  if (!engine_) {
    LOG(ERROR) << __func__ << ", PDF engine_ is null.";
  }
  engine_->ClearTextSelection();
}

void PdfViewWebPlugin::SetIsLeftHandleVisible(bool visible) {
  bool expected_is_left_visible = is_left_visible_.load();
  if (expected_is_left_visible == visible) {
    return;
  }
  is_left_visible_.store(visible);
  pdf_host_->SetIsLeftHandleVisible(is_left_visible_.load());
}

void PdfViewWebPlugin::SetIsRightHandleVisible(bool visible) {
  bool expected_is_right_visible = is_right_visible_.load();
  if (expected_is_right_visible == visible) {
    return;
  }
  is_right_visible_.store(visible);
  pdf_host_->SetIsRightHandleVisible(is_right_visible_.load());
}
#endif  // BUILDFLAG(ARKWEB_PDF)

}  // namespace chrome_pdf
