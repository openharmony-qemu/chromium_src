/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nweb_render_handler.h"

#include <sys/time.h>
#include <cerrno>
#include <cstring>

#include "base/logging.h"
#include "base/command_line.h"
#include "nweb_delegate_interface.h"
#include "nweb_touch_handle_state_impl.h"

#include "ohos_adapter_helper.h"
#include "content/public/common/content_switches.h"
#ifdef OHOS_DRAG_DROP
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "cef/libcef/common/drag_data_impl.h"
#include "content/public/common/drop_data.h"
#include "nweb_drag_data.h"
#include "nweb_drag_data_impl.h"
#endif  // #ifdef OHOS_DRAG_DROP

namespace {
cef_screen_orientation_type_t ConvertOrientationType(
    OHOS::NWeb::OrientationType type,
    bool default_portrait) {
  switch (type) {
    case OHOS::NWeb::OrientationType::UNSPECIFIED:
      return default_portrait
                 ? cef_screen_orientation_type_t::PORTRAIT_PRIMARY
                 : cef_screen_orientation_type_t::LANDSCAPE_PRIMARY;
    case OHOS::NWeb::OrientationType::VERTICAL:
      return cef_screen_orientation_type_t::PORTRAIT_PRIMARY;
    case OHOS::NWeb::OrientationType::HORIZONTAL:
      return default_portrait
                 ? cef_screen_orientation_type_t::LANDSCAPE_SECONDARY
                 : cef_screen_orientation_type_t::LANDSCAPE_PRIMARY;
    case OHOS::NWeb::OrientationType::REVERSE_VERTICAL:
      return cef_screen_orientation_type_t::PORTRAIT_SECONDARY;
    case OHOS::NWeb::OrientationType::REVERSE_HORIZONTAL:
      return default_portrait
                 ? cef_screen_orientation_type_t::LANDSCAPE_PRIMARY
                 : cef_screen_orientation_type_t::LANDSCAPE_SECONDARY;
    // Now ohos platform don't hava sensor orientation.
    // Will be support later.
    case OHOS::NWeb::OrientationType::SENSOR:
    case OHOS::NWeb::OrientationType::SENSOR_VERTICAL:
    case OHOS::NWeb::OrientationType::SENSOR_HORIZONTAL:
      return cef_screen_orientation_type_t::UNDEFINED;
    default:
      return cef_screen_orientation_type_t::UNDEFINED;
  }
}

uint16_t ConvertRotationAngel(OHOS::NWeb::RotationType type,
                              bool default_portrait) {
  // Notice: 90 and 270 is reverse.

  switch (type) {
    case OHOS::NWeb::RotationType::ROTATION_0:
      return default_portrait ? 0 : 90;
    case OHOS::NWeb::RotationType::ROTATION_90:
      return default_portrait ? 270 : 0;
    case OHOS::NWeb::RotationType::ROTATION_180:
      return default_portrait ? 180 : 270;
    case OHOS::NWeb::RotationType::ROTATION_270:
      return default_portrait ? 90 : 180;
    default:
      return 0;
  }
}
}  // namespace

namespace OHOS::NWeb {
// static
CefRefPtr<NWebRenderHandler> NWebRenderHandler::Create() {
  CefRefPtr<NWebRenderHandler> renderHandler(new NWebRenderHandler());
  if (renderHandler == nullptr) {
    LOG(ERROR) << "fail to create NWebRenderHandler instance";
    return nullptr;
  }
  return renderHandler;
}

void NWebRenderHandler::RegisterRenderCb(
    std::function<void(const char*)> render_update_cb) {
  render_update_cb_ = render_update_cb;
}

void NWebRenderHandler::RegisterNWebHandler(
    std::shared_ptr<NWebHandler> handler) {
  handler_ = std::weak_ptr<NWebHandler>(handler);
}

void NWebRenderHandler::SetInputMethodClient(
    CefRefPtr<NWebInputMethodClient> client) {
  inputmethod_client_ = client;
}

#if defined(OHOS_INPUT_EVENTS)
void NWebRenderHandler::SetNWebDelegateInterface(
    std::shared_ptr<NWebDelegateInterface> client) {
  delegate_interface_ = std::weak_ptr<NWebDelegateInterface>(client);
}

void NWebRenderHandler::OnOverscroll(CefRefPtr<CefBrowser> browser,
                                     const float x,
                                     const float y) {
  if (auto handler = handler_.lock()) {
    handler->OnOverScroll(x, y);
  }
}

void NWebRenderHandler::OnSelectionChanged(CefRefPtr<CefBrowser> browser,
                                           const CefString& text,
                                           const CefRange& selected_range) {
  if (inputmethod_client_) {
    inputmethod_client_->OnSelectionChanged(browser, text, selected_range);
  }
}

void NWebRenderHandler::OnCursorUpdate(CefRefPtr<CefBrowser> browser,
                                       const CefRect& rect) {
  if (!inputmethod_client_) {
    LOG(ERROR)
        << "NWebRenderHandler::OnCursorUpdate inputmethod_client_ is nullptr";
    return;
  }
  inputmethod_client_->OnCursorUpdate(rect);
}

void NWebRenderHandler::SetFocusStatus(bool focus_status) {
  if (inputmethod_client_) {
    inputmethod_client_->SetFocusStatus(focus_status);
  }
}
#endif  // defined(OHOS_INPUT_EVENTS)

#if BUILDFLAG(IS_OHOS)
void NWebRenderHandler::OnEditableChanged(CefRefPtr<CefBrowser> browser,
                                          bool is_editable_node) {
  if (!inputmethod_client_) {
    LOG(ERROR) << "inputmethod_client_ is nullptr.";
    return;
  }
  inputmethod_client_->OnEditableChanged(browser, is_editable_node);
}
#endif // #if BUILDFLAG(IS_OHOS)

void NWebRenderHandler::Resize(uint32_t width, uint32_t height) {
  width_ = width;
  height_ = height;
}

void NWebRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser,
                                    CefRect& rect) {
  rect.x = 0;
  rect.y = 0;
  if (screen_info_.display_ratio <= 0) {
    rect.width = width_;
    rect.height = height_;
  } else {
    // Surface greater than Web compoment in case show black line.
    if ((*base::CommandLine::ForCurrentProcess()).HasSwitch(switches::kForBrowser)) {
      rect.width = std::ceil(width_ / screen_info_.display_ratio) + 1;
    } else {
      rect.width = std::ceil(width_ / screen_info_.display_ratio);
    }
    rect.height = std::ceil(height_ / screen_info_.display_ratio);
  }

  if (rect.width <= 0) {
    rect.width = 1;
  }
  if (rect.height <= 0) {
    rect.height = 1;
  }
}

// #ifdef OHOS_SCREEN_ROTATION
void NWebRenderHandler::SetScreenInfo(const NWebScreenInfo& screen_info) {
  screen_info_ = screen_info;
  auto delegete = delegate_interface_.lock();
  if (delegete) {
    delegete->SetVirtualPixelRatio(screen_info_.display_ratio);
  }
}
// #endif  // #ifdef OHOS_SCREEN_ROTATION

bool NWebRenderHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser,
                                      CefScreenInfo& screen_info) {
  screen_info.orientation = ConvertOrientationType(
      screen_info_.orientation, screen_info_.default_portrait);
  screen_info.angle = ConvertRotationAngel(screen_info_.rotation,
                                           screen_info_.default_portrait);
  screen_info.rect.width = screen_info_.width;
  screen_info.rect.height = screen_info_.height;
  screen_info.device_scale_factor = screen_info_.display_ratio;

  // TODO: currently display dont have interface to get. We use fix value
  // instead.
  screen_info.depth = 24;
  screen_info.depth_per_component = 8;

  cef_device_ratio_ = screen_info.device_scale_factor;
  return true;
}

void NWebRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser,
                                PaintElementType type,
                                const RectList& dirty_rects,
                                const void* buffer,
                                int width,
                                int height) {
  // run render callback to take buffer.
  // note: buffer must be consumed asap with no lock protected
  if (render_update_cb_ == nullptr) {
    return;
  }
  if ((uint32_t)width != width_ || (uint32_t)height != height_) {
    LOG(INFO) << "frame size(" << width << "*" << height
              << ") is not identical to request size (" << width_ << "*"
              << height_ << "), drop this frame";
    constexpr uint8_t kBitsPerPixel = 4;
    uint32_t white_frame_size = width_ * height_ * kBitsPerPixel;
    char* white_frame = new char[white_frame_size];
    const char pixel_in_white = 0xFF;
    (void)memset(white_frame, pixel_in_white, white_frame_size);
    render_update_cb_(white_frame);
    delete[] white_frame;
  } else {
    render_update_cb_(reinterpret_cast<const char*>(buffer));
  }
}

void NWebRenderHandler::OnRootLayerChanged(CefRefPtr<CefBrowser> browser,
                                           int height,
                                           int width) {
  content_height_ = height;
  content_width_ = width;
  if (auto handler = handler_.lock()) {
    handler->OnRootLayerChanged(width * screen_info_.display_ratio,
                                height * screen_info_.display_ratio);
  }
}

void NWebRenderHandler::OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser,
                                              double x,
                                              double y) {
  if (auto handler = handler_.lock()) {
    handler->OnScroll(x, y);
  }
}

int NWebRenderHandler::ContentHeight() {
  return content_height_;
}

void NWebRenderHandler::OnImeCompositionRangeChanged(
    CefRefPtr<CefBrowser> browser,
    const CefRange& selected_range,
    const RectList& character_bounds) {
  LOG(INFO) << "NWebRenderHandler::OnImeCompositionRangeChanged";
}

void NWebRenderHandler::OnTextSelectionChanged(CefRefPtr<CefBrowser> browser,
                                               const CefString& selected_text,
                                               const CefRange& selected_range) {
  if (inputmethod_client_) {
    inputmethod_client_->OnTextSelectionChanged(browser, selected_text,
                                                selected_range);
  }
}

void NWebRenderHandler::OnVirtualKeyboardRequested(
    CefRefPtr<CefBrowser> browser,
    TextInputMode input_mode,
    TextInputType input_type,
    bool show_keyboard) {
  LOG(INFO) << "NWebRenderHandler::OnVirtualKeyboardRequested input_mode = "
            << input_mode << ", input_type = " << input_type
            << ", show_keyboard = " << show_keyboard;
#if defined(OHOS_INPUT_EVENTS)
  if (!inputmethod_client_) {
    LOG(ERROR) << "inputmethod_client_ is nullptr.";
    return;
  }

  if (input_mode != CEF_TEXT_INPUT_MODE_NONE) {
    auto delegate = delegate_interface_.lock();
    if (delegate && delegate->OnFocus()) {
      inputmethod_client_->Attach(browser, show_keyboard, input_type);
    }
  } else {
    inputmethod_client_->HideTextInput();
  }
#endif  // defined(OHOS_INPUT_EVENTS)
}

void NWebRenderHandler::GetTouchHandleSize(
    CefRefPtr<CefBrowser> browser,
    cef_horizontal_alignment_t orientation,
    CefSize& size) {
  // TODO: need to refactor in 3.2.8.1 use arkui refactor.
  size.width = 10;
  size.height = 10;
  if (screen_info_.display_ratio <= 0.0) {
    LOG(INFO) << "invalid display_ratio_, display_ratio_ = "
              << screen_info_.display_ratio;
    return;
  }
  if (auto handler = handler_.lock()) {
    TouchHandleHotZone hot_zone;
    handler->OnGetTouchHandleHotZone(hot_zone);
    if (hot_zone.width > 0 && hot_zone.height > 0) {
      size.width = static_cast<int>(hot_zone.width) + 1;
      size.height = static_cast<int>(hot_zone.height) + 1;
    }
  }
  LOG(INFO) << "GetTouchHandleSize " << size.width << " " << size.height;
}

std::shared_ptr<NWebTouchHandleState> NWebRenderHandler::GetTouchHandleState(
    NWebTouchHandleState::TouchHandleType type) {
  switch (type) {
    case NWebTouchHandleState::TouchHandleType::INSERT_HANDLE:
      return insert_handle_.enabled
                 ? std::make_shared<NWebTouchHandleStateImpl>(insert_handle_)
                 : nullptr;
    case NWebTouchHandleState::TouchHandleType::SELECTION_BEGIN_HANDLE:
      return start_selection_handle_.enabled
                 ? std::make_shared<NWebTouchHandleStateImpl>(
                       start_selection_handle_)
                 : nullptr;
    case NWebTouchHandleState::TouchHandleType::SELECTION_END_HANDLE:
      return end_selection_handle_.enabled
                 ? std::make_shared<NWebTouchHandleStateImpl>(
                       end_selection_handle_)
                 : nullptr;
    default:
      return nullptr;
  }
}

CefTouchHandleState NWebRenderHandler::ConvertTouchHandleDisplayRatio(
    const CefTouchHandleState& touch_handle) {
  CefTouchHandleState result_touch_handle = touch_handle;
  if (screen_info_.display_ratio <= 0) {
    LOG(WARNING) << "virtual display ratio is invalid";
    return result_touch_handle;
  }
  result_touch_handle.edge_height *= screen_info_.display_ratio;
  result_touch_handle.origin.x *= screen_info_.display_ratio;
  result_touch_handle.origin.y *= screen_info_.display_ratio;
  return result_touch_handle;
}

void NWebRenderHandler::OnTouchSelectionChanged(
    const CefTouchHandleState& insert_handle,
    const CefTouchHandleState& start_selection_handle,
    const CefTouchHandleState& end_selection_handle,
    bool need_report) {
  insert_handle_ = ConvertTouchHandleDisplayRatio(insert_handle);
  start_selection_handle_ =
      ConvertTouchHandleDisplayRatio(start_selection_handle);
  end_selection_handle_ = ConvertTouchHandleDisplayRatio(end_selection_handle);
  if (!need_report) {
    return;
  }
  if (auto handler = handler_.lock()) {
    handler->OnTouchSelectionChanged(
        std::make_shared<NWebTouchHandleStateImpl>(insert_handle_),
        std::make_shared<NWebTouchHandleStateImpl>(start_selection_handle_),
        std::make_shared<NWebTouchHandleStateImpl>(end_selection_handle_));
  }
}

#ifdef OHOS_DRAG_DROP
void NWebRenderHandler::ImageDragForFileUri(CefRefPtr<CefDragData> drag_data) {
  // default temp dir in sandbox
  CefString tempPath("/data/storage/el2/base/haps/entry/temp/dragdrop/");
  auto delegete = delegate_interface_.lock();
  if (delegete && !delegete->GetAppTempDir().empty()) {
    tempPath = delegete->GetAppTempDir() + "/dragdrop/";
  }

  if (base::DirectoryExists(base::FilePath(tempPath))) {
    if (!base::IsDirectoryEmpty(base::FilePath(tempPath))) {
      base::DeletePathRecursively(base::FilePath(tempPath));
    }
  } else {
    LOG(INFO) << "DragDrop temp dir not exist, create it";
  }
  base::CreateDirectory(base::FilePath(tempPath));

  CefString fileName = drag_data->GetFileName();
  if (!fileName.ToString().empty()) {
    CefString fullName(tempPath.ToString() + fileName.ToString());
    if (base::PathExists(base::FilePath(fullName))) {
      LOG(INFO) << "DragDrop image file already exist, delete it first";
      base::DeleteFile(base::FilePath(fullName));
    }

    const int image_file_size_max = 10 * 1024 * 1024; // 10M
    if (drag_data->GetImageFileSize() > image_file_size_max) {
      LOG(WARNING) << "DragDrop The image size exceeds 10MB";
      return;
    }

    CefRefPtr<CefStreamWriter> stream(CefStreamWriter::CreateForFile(fullName));
    size_t size = drag_data->GetFileContents(stream);
    if (size == drag_data->GetImageFileSize()) {
      LOG(INFO) << "DragDrop image file write success, size:" << size;
    } else {
      LOG(ERROR) << "DragDrop image file write failed";
    }
  } else {
    LOG(INFO) << "DragDrop this is not a image drag, pass";
  }
}

// chromium内核上报的拖拽数据
bool NWebRenderHandler::StartDragging(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefDragData> drag_data,
                                      DragOperationsMask allowed_ops,
                                      int x,
                                      int y) {
  LOG(INFO) << "DragDrop StartDragging received dragData from chromium start "
               "dragging callback, operation = "
            << allowed_ops << ", x = " << x << ", y = " << y;
  if (!drag_data && !drag_data->HasImage()) {
    LOG(ERROR) << "drag data invalid";
    return false;
  }

  auto fragment = drag_data->GetFragmentText();
  LOG(INFO) << "DragDrop drag data GetFragmentText:" << fragment.ToString();
  auto link_url = drag_data->GetLinkURL();
  LOG(INFO) << "DragDrop drag data GetLinkURL:" << link_url.ToString();
  auto link_html = drag_data->GetFragmentHtml();
  LOG(INFO) << "DragDrop drag data GetFragmentHtml:" << link_html.ToString();

  ImageDragForFileUri(drag_data);
  CefPoint drag_touch_point(x, y);

  std::vector<CefPoint> start_edge{
      CefPoint(start_selection_handle_.origin.x,
               start_selection_handle_.origin.y -
                   start_selection_handle_.edge_height),
      CefPoint(start_selection_handle_.origin.x,
               start_selection_handle_.origin.y)};
  std::vector<CefPoint> end_edge{
      CefPoint(
          end_selection_handle_.origin.x,
          end_selection_handle_.origin.y - end_selection_handle_.edge_height),
      CefPoint(end_selection_handle_.origin.x, end_selection_handle_.origin.y)};

  nweb_drag_data_ = std::make_shared<NWebDragDataImpl>(
      drag_data, drag_touch_point, start_edge, end_edge,
      screen_info_.display_ratio, is_irregular_drag_background_);

  auto handler = handler_.lock();
  if (handler == nullptr) {
    LOG(ERROR) << "can't get strong ptr with handler";
    return false;
  }
  return handler->OnDragAndDropDataUdmf(nweb_drag_data_);
}

void NWebRenderHandler::UpdateDragCursor(CefRefPtr<CefBrowser> browser,
                                         DragOperation operation) {
  auto handler = handler_.lock();
  if (handler == nullptr) {
    LOG(ERROR) << "DragDrop can't get strong ptr with handler";
    return;
  }
  handler->UpdateDragCursor(
      static_cast<NWebDragData::DragOperation>(operation));
}

#if defined(OHOS_COMPOSITE_RENDER)
void NWebRenderHandler::OnCompleteSwapWithNewSize() {
  if (auto handler = handler_.lock()) {
    handler->OnCompleteSwapWithNewSize();
  }
}

void NWebRenderHandler::OnResizeNotWork() {
  if (auto handler = handler_.lock()) {
    handler->OnResizeNotWork();
  }
}
#endif  // defined(OHOS_COMPOSITE_RENDER)

CefRefPtr<CefDragData> NWebRenderHandler::GetDragData() {
  if (!nweb_drag_data_) {
    content::DropData drop_data;
    CefRefPtr<CefDragDataImpl> drag_data(new CefDragDataImpl(drop_data));
    nweb_drag_data_ = std::make_shared<NWebDragDataImpl>(drag_data.get());
  }
  return std::static_pointer_cast<NWebDragDataImpl>(nweb_drag_data_)
      ->GetDragData();
}

void NWebRenderHandler::FreePixlMapData() {
  if (nweb_drag_data_) {
    std::static_pointer_cast<NWebDragDataImpl>(nweb_drag_data_)
        ->FreePixlMapData();
  }
}

void NWebRenderHandler::SetIrregularDragBackground(
    bool is_irregular_background) {
  is_irregular_drag_background_ = is_irregular_background;
}

#endif  // #ifdef OHOS_DRAG_DROP

#if BUILDFLAG(IS_OHOS)
void NWebRenderHandler::OnOverScrollFlingVelocity(CefRefPtr<CefBrowser> browser,
                                                  const float x,
                                                  const float y,
                                                  bool is_fling) {
  double display_ratio = 1.0;
  if (screen_info_.display_ratio > 0) {
    display_ratio = screen_info_.display_ratio;
  }
  if (auto handler = handler_.lock()) {
    // Value multiplied by virtual pixel ratio.
    handler->OnOverScrollFlingVelocity(x * screen_info_.display_ratio,
                                       y * screen_info_.display_ratio,
                                       is_fling);
  }
}

void NWebRenderHandler::OnOverScrollFlingEnd(CefRefPtr<CefBrowser> browser) {
  if (auto handler = handler_.lock()) {
    handler->OnOverScrollFlingEnd();
  }
}

void NWebRenderHandler::OnScrollState(CefRefPtr<CefBrowser> browser,
                                      bool scroll_state) {
  if (auto handler = handler_.lock()) {
    handler->OnScrollState(scroll_state);
  }
}

bool NWebRenderHandler::FilterScrollEvent(CefRefPtr<CefBrowser> browser,
                                          const float x,
                                          const float y,
                                          const float fling_x,
                                          const float fling_y) {
  double display_ratio = 1.0;
  if (screen_info_.display_ratio > 0) {
    display_ratio = screen_info_.display_ratio;
  }
  if (auto handler = handler_.lock()) {
    // Value multiplied by virtual pixel ratio.
    return handler->FilterScrollEvent(x * screen_info_.display_ratio,
                                      y * screen_info_.display_ratio,
                                      fling_x * screen_info_.display_ratio,
                                      fling_y * screen_info_.display_ratio);
  }
  return false;
}
#endif
}  // namespace OHOS::NWeb
