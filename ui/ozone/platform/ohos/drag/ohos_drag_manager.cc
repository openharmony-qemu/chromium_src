// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/drag/ohos_drag_manager.h"

#include "base/base_paths_ohos.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/run_loop.h"
#include "base/strings/utf_string_conversions.h"
#include "ohos/adapter/drag_drop/drag_drop_ohos_adapter.h"
#include "ohos/adapter/drag_drop/node_handle_drag_drop_ohos_adapter.h"
#include "ohos/adapter/node_handle/node_handle_impl.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkImage.h"
#include "ui/base/clipboard/clipboard_format_type.h"
#include "ui/base/cursor/cursor.h"
#include "ui/base/cursor/platform_cursor.h"
#include "ui/events/ozone/events_ozone.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/ozone/platform/ohos/common/ohos_util.h"
#include "ui/ozone/platform/ohos/host/ohos_toplevel_window.h"
#include "ui/platform_window/platform_window_delegate.h"
#include "ui/platform_window/wm/wm_drop_handler.h"
#include "ui/views/widget/desktop_aura/desktop_window_tree_host_platform.h"
#include "url/url_util.h"

namespace ui {

namespace {
using mojom::DragOperation;
const int kMaxUriDecodeLen = 2048;
const int kDefaultDragOperations = ui::DragDropTypes::DRAG_MOVE |
                                   ui::DragDropTypes::DRAG_COPY |
                                   ui::DragDropTypes::DRAG_LINK;
const int kDefaultWidth = 1;
const int kDefaultHeight = 1;
const std::string kBookmarkFormatString = "chromium/x-bookmark-entries";
const std::string kWebCustomFormatString = "chromium/x-web-custom-data";
const std::string kWebImageDragFolder = "dragdrop";
constexpr int kHalfDivisor = 2;
}

OhosDragManager::OhosDragManager(PlatformWindowDelegate* delegate,
                                 PlatformWindow* platform_window)
    : platform_window_delegate_(delegate),
      platform_window_(platform_window) {
  SetWmDragHandler(platform_window_, this);
  SetWmMoveLoopHandler(platform_window_, this);
  ohos_window_move_client_ =
      std::make_unique<OhosDesktopWindowMoveClient>(this);
}

OhosDragManager::~OhosDragManager() {
  CancelDrag();
}

bool OhosDragManager::RunMoveLoop(const gfx::Vector2d& drag_offset) {
  return ohos_window_move_client_->RunMoveLoop(!platform_window_->HasCapture(),
                                               drag_offset);
}

void OhosDragManager::EndMoveLoop() {
  ohos_window_move_client_->EndMoveLoop();
}

scoped_refptr<PlatformCursor> OhosDragManager::GetLastCursor() {
  return last_cursor_;
}

gfx::Size OhosDragManager::GetSize() {
  return platform_window_->GetBoundsInPixels().size();
}

void OhosDragManager::SetBoundsOnMove(const gfx::Rect& requested_bounds) {
  platform_window_->SetBoundsInPixels(requested_bounds);
}

void OhosDragManager::StartWindowMovingWithOffset(
    const gfx::PointF& offset_point) {
  OhosToplevelWindow* toplevel_window =
      static_cast<OhosToplevelWindow*>(platform_window_);
  if (toplevel_window == nullptr) {
    LOG(ERROR) << "[OhosDragTab]" << __FUNCTION__ << ", toplevel_window is null";
    return;
  }
  toplevel_window->StartWindowMovingWithOffset(offset_point.x(),
                                               offset_point.y());
}

display::Display OhosDragManager::GetCurrentDisplay() {
  display::Display current_display;
  if (platform_window_ == nullptr) {
    LOG(ERROR) << "[OhosDragTab]" << __FUNCTION__ << ", platform_window_ is null";
    return current_display;
  }
  OhosWindow* ohos_window = static_cast<OhosWindow*>(platform_window_);
  current_display = ohos_window->GetCurrentDisplay();
  return current_display;
}

bool OhosDragManager::StartDrag(
    const OSExchangeData& data,
    int operations,
    mojom::DragEventSource source,
    gfx::NativeCursor cursor,
    bool can_grab_pointer,
    base::OnceClosure drag_started_callback,
    WmDragHandler::DragFinishedCallback drag_finished_callback,
    WmDragHandler::LocationDelegate* location_delegate) {
  LOG(INFO) << "[OhosDrag]" << __FUNCTION__ << ",operations:" << operations
            << ",can_grab_pointer:" << can_grab_pointer;
  source_provider_ =
      static_cast<const OSExchangeDataProviderNonBacked*>(&data.provider());
  data_ = std::make_unique<ui::OSExchangeData>(source_provider_->Clone());
  drag_finished_callback_ = std::move(drag_finished_callback);
  drag_location_delegate_ = location_delegate;
  allowed_drag_operations_ = operations;
  notified_enter_ = false;
  is_drag_source_ = true;
  is_drag_end_ = false;

  drag_delegate_->SetDragSourceWidget();
  auto drag_param = std::make_shared<OhosStartDragParam>();
  PrepareDragParamForStartDrag(data, drag_param);

  bool result = false;
  if (ohos::adapter::nodeHandle::NodeHandleImpl::GetInstance()
          .IsSupportNodeHandle()) {
    result =
        ohos::adapter::NodeHandleDragDropOhosAdapter::GetInstance().ExecuteDrag(
            drag_param, platform_window_->GetWindowUniqueId());
  } else {
    result = ohos::adapter::DragDropOhosAdapter::GetInstance().ExecuteDrag(
        drag_param, platform_window_->GetWindowUniqueId());
  }
  if (!result) {
    LOG(ERROR) << "[OhosDrag]" << __FUNCTION__ << ", ExecuteDrag fail";
    return false;
  }

  base::RunLoop run_loop(base::RunLoop::Type::kNestableTasksAllowed);
  quit_closure_ = run_loop.QuitClosure();
  run_loop.Run();
  return false;
}

void OhosDragManager::CancelDrag() {
  if (!quit_closure_.is_null()) {
    std::move(quit_closure_).Run();
  }
}

void OhosDragManager::UpdateDragImage(const gfx::ImageSkia& image,
                                      const gfx::Vector2d& offset) {
  NOTIMPLEMENTED();
}

void OhosDragManager::UpdateDrag(const gfx::Point& window_point) {
  WmDropHandler* drop_handler = ui::GetWmDropHandler(*platform_window_);
  if (!drop_handler) {
    LOG(ERROR) << "[OhosDrag] execute drag update fail,no drop handler";
    return;
  }
  if (!source_provider_) {
    LOG(ERROR) << "[OhosDrag] execute drag update fail,no source_provider_";
    return;
  }
  if (!data_) {
    data_ = std::make_unique<ui::OSExchangeData>(source_provider_->Clone());
  }

  int suggested_operations = allowed_drag_operations_;
  if (data_->HasFile() && (suggested_operations
          & (DragDropTypes::DRAG_MOVE | DragDropTypes::DRAG_LINK))) {
    suggested_operations |= DragDropTypes::DRAG_COPY;
  }

  gfx::PointF local_point_in_dip(window_point);
  static_cast<views::DesktopWindowTreeHostPlatform*>(
      platform_window_delegate_.get())
      ->ConvertPixelsToDIP(&local_point_in_dip);
  last_point_ = local_point_in_dip;
  if (!notified_enter_) {
    auto data = std::make_unique<OSExchangeData>(source_provider_->Clone());
    drop_handler->OnDragEnter(local_point_in_dip, suggested_operations,
                              current_modifier_);
    drop_handler->OnDragDataAvailable(std::move(data));
    notified_enter_ = true;
  }
  drop_handler->OnDragMotion(local_point_in_dip, suggested_operations,
                             current_modifier_);
}


void OhosDragManager::DragEnter(const OhosDropData& drop_data,
                                gfx::PointF screen_point) {
  WmDropHandler* drop_handler = ui::GetWmDropHandler(*platform_window_);
  if (!drop_handler) {
    LOG(ERROR) << "[OhosDrag] execute drag enter fail,no drop handler";
    return;
  }
  std::unique_ptr<ui::OSExchangeDataProvider> provider =
      ui::OSExchangeDataProviderFactory::CreateProvider();
  data_ = std::make_unique<ui::OSExchangeData>(std::move(provider));

  allowed_drag_operations_ = kDefaultDragOperations;
  notified_enter_ = false;
  is_drag_end_ = false;
  HandleDropData(drop_data, data_->provider());
  source_provider_ =
      static_cast<const OSExchangeDataProviderNonBacked*>(&data_->provider());
}

void OhosDragManager::OnDrop(const OhosDropData& drop_data,
                             gfx::PointF screen_point) {
  WmDropHandler* drop_handler = ui::GetWmDropHandler(*platform_window_);
  if (!drop_handler) {
    LOG(ERROR) << "[OhosDrag] execute drop fail,no drop handler";
    return;
  }
  
  std::unique_ptr<ui::OSExchangeDataProvider> provider =
      ui::OSExchangeDataProviderFactory::CreateProvider();
  data_ = std::make_unique<ui::OSExchangeData>(std::move(provider));
  HandleDropData(drop_data, data_->provider());
  int suggested_operations = allowed_drag_operations_;
  drop_handler->OnDragDataAvailable(std::move(data_));
  drop_handler->OnDragMotion(last_point_, suggested_operations,
                             current_modifier_);

  drop_handler->OnDragDrop(current_modifier_);
  notified_enter_ = false;
  // end the drag event of the drag source window
  drag_delegate_->EndDrag();
}

void OhosDragManager::DragLeave() {
  WmDropHandler* drop_handler = ui::GetWmDropHandler(*platform_window_);
  if (!drop_handler) {
    LOG(ERROR) << "[OhosDrag] drag leave fail,no drop handler";
    return;
  }
  drop_handler->OnDragLeave();
  notified_enter_ = false;
  if (!is_drag_source_) {
    data_.reset();
  }
}

void OhosDragManager::DragEnd() {
  if (is_drag_end_) {
    return;
  }
  WmDropHandler* drop_handler = ui::GetWmDropHandler(*platform_window_);
  if (!drop_handler) {
    LOG(ERROR) << "[OhosDrag] execute drag end fail,no drop handler";
    return;
  }
  drop_handler->OnDragLeave();
  if (is_drag_source_) {
    drag_delegate_->ClearDragSourceWidget();
  }
  data_.reset();
  is_drag_source_ = false;
  is_drag_end_ = true;

  if (!quit_closure_.is_null()) {
    std::move(quit_closure_).Run();
  }
  if (!drag_finished_callback_.is_null()) {
    std::move(drag_finished_callback_)
        .Run(PreferredDragOperation(allowed_drag_operations_));
  }
}

std::vector<FileInfo> OhosDragManager::DecodeFileName(
    const std::vector<std::string>& file_paths) {
  std::vector<FileInfo> file_names;
  for (const std::string& file_path : file_paths) {
    url::RawCanonOutputW<kMaxUriDecodeLen> unescaped_path;
    url::DecodeURLEscapeSequences(file_path.c_str(),
                                  url::DecodeURLMode::kUTF8OrIsomorphic,
                                  &unescaped_path);
    std::string decode_file_path = base::UTF16ToUTF8(
        std::u16string_view(unescaped_path.data(), unescaped_path.length()));
    file_names.emplace_back(base::FilePath(FILE_PATH_LITERAL(decode_file_path)),
                            base::FilePath());
  }
  return file_names;
}

void OhosDragManager::HandleDropData(
    const OhosDropData& drop_data,
    OSExchangeDataProvider& provider) {
  if (!drop_data.basic_data.text.empty()) {
    provider.SetString(base::UTF8ToUTF16(drop_data.basic_data.text));
  }
  if (!drop_data.basic_data.url.empty()) {
    GURL url = GURL(drop_data.basic_data.url);
    provider.SetURL(url, base::UTF8ToUTF16(drop_data.basic_data.url_title));
  }
  if (!drop_data.basic_data.html.empty()) {
    GURL base_url;
    provider.SetHtml(base::UTF8ToUTF16(drop_data.basic_data.html), base_url);
  }
  if (!drop_data.file_paths.empty()) {
    std::vector<FileInfo> file_names = DecodeFileName(drop_data.file_paths);
    provider.SetFilenames(std::move(file_names));
    ohos::adapter::DragDropOhosAdapter::GetInstance()
        .SetDraggedExtensionFileName(file_names.front().path.value());
  }
  if (drop_data.basic_data.bookmark_data.size() > 0) {
    base::Pickle pickle = base::Pickle::WithData((
        base::span(reinterpret_cast<const uint8_t*>(drop_data.basic_data.bookmark_data.data()),
                   drop_data.basic_data.bookmark_data.size())));
    provider.SetPickledData(
        ui::ClipboardFormatType::GetType(kBookmarkFormatString),
        std::move(pickle));
  }
  if (drop_data.basic_data.web_custom_data.size() > 0) {
    base::Pickle pickle = base::Pickle::WithData((
        base::span(reinterpret_cast<const uint8_t*>(drop_data.basic_data.web_custom_data.data()),
                   drop_data.basic_data.web_custom_data.size())));
    provider.SetPickledData(
        ui::ClipboardFormatType::GetType(kWebCustomFormatString),
        std::move(pickle));
  }
}

void OhosDragManager::PrepareDragParamForStartDrag(
    const OSExchangeData& data,
    std::shared_ptr<OhosStartDragParam> drag_param) {
  HandleBasicDragData(data, drag_param);
  HandlePickleData(data, drag_param);
  HandleWebImageFilePath(data, drag_param);
  HandlePixelMapData(data, drag_param);
}

void OhosDragManager::HandleBasicDragData(
    const OSExchangeData& data,
    std::shared_ptr<OhosStartDragParam> drag_param) {
  if (std::optional<std::u16string> string = data.GetString();
      string.has_value() && !string->empty()) {
    drag_param->basic_data.text = base::UTF16ToUTF8(*string);
  }

  if (std::optional<ui::OSExchangeData::UrlInfo> url = data.GetURLAndTitle(
          ui::FilenameToURLPolicy::DO_NOT_CONVERT_FILENAMES);
      url.has_value() && url->url.is_valid()) {
    drag_param->basic_data.url = url->url.spec();
    drag_param->basic_data.url_title = base::UTF16ToUTF8(url->title);
  }

  if (std::optional<ui::OSExchangeData::HtmlInfo> html = data.GetHtml();
      html.has_value()) {
    drag_param->basic_data.html = base::UTF16ToUTF8(html->html);
  }
}

void OhosDragManager::HandleWebImageFilePath(
    const OSExchangeData& data,
    std::shared_ptr<OhosStartDragParam> drag_param) {
  if (std::optional<ui::OSExchangeData::FileContentsInfo> file_contents =
          data.GetFileContents();
      file_contents.has_value()) {
    base::FilePath folder_path;
    base::PathService::Get(base::DIR_TEMP, &folder_path);
    if (folder_path.empty()) {
      LOG(ERROR) << "[OhosDrag]" << __FUNCTION__
                 << ", folder_path get fail";
      return;
    }
    folder_path = folder_path.Append(kWebImageDragFolder);
    if (base::DirectoryExists(folder_path)) {
      if (!base::IsDirectoryEmpty(folder_path)) {
        base::DeletePathRecursively(folder_path);
      }
    }
    base::CreateDirectory(folder_path);
    if (!base::DirectoryExists(folder_path)) {
      LOG(ERROR) << "[OhosDrag]" << __FUNCTION__
                 << ", folder_path create fail";
      return;
    }

    base::FilePath file_path(folder_path);
    file_path = file_path.Append(file_contents->filename);
    if (base::WriteFile(file_path, file_contents->file_contents)) {
      drag_param->web_image_file_path = file_path.value().c_str();
    } else {
      LOG(ERROR) << "[OhosDrag]drag image file write fail";
    }
  }
}

void OhosDragManager::HandlePickleData(
    const OSExchangeData& data,
    std::shared_ptr<OhosStartDragParam> drag_param) {
  const ui::ClipboardFormatType& bookmark_type =
      ui::ClipboardFormatType::GetType(kBookmarkFormatString);
  if (std::optional<base::Pickle> bookmark_pickle =
          data.GetPickledData(bookmark_type);
      bookmark_pickle.has_value()) {
    const uint8_t* pickle_data = bookmark_pickle.value().data();
    size_t length = bookmark_pickle.value().size();
    drag_param->basic_data.bookmark_data =
        std::vector<uint8_t>(pickle_data, pickle_data + length);
  }

  const ui::ClipboardFormatType& web_custom_type =
      ui::ClipboardFormatType::GetType(kWebCustomFormatString);
  if (std::optional<base::Pickle> web_custom_pickle =
          data.GetPickledData(web_custom_type);
      web_custom_pickle.has_value()) {
    const uint8_t* pickle_data = web_custom_pickle.value().data();
    size_t length = web_custom_pickle.value().size();
    drag_param->basic_data.web_custom_data =
        std::vector<uint8_t>(pickle_data, pickle_data + length);
  }
}

void OhosDragManager::HandlePixelMapData(
    const OSExchangeData& data,
    std::shared_ptr<OhosStartDragParam> drag_param) {
  gfx::ImageSkia image = data.provider().GetDragImage();
  // Drag shadow |width|height| need actual number of pixels.
  // The width of ImageSkia is logical pixels,effected by |device_scale_factor|.
  // The width of SkBitmap is actual Pixel,not effected by
  // |device_scale_factor|.
  const SkBitmap* bitmap = image.bitmap();
  int width = bitmap->width();
  int height = bitmap->height();
  size_t row_bytes = static_cast<unsigned int>(width) * sizeof(uint32_t);
  size_t buffer_size = bitmap->computeByteSize();
  std::shared_ptr<char[]> buff = std::make_shared<char[]>(buffer_size);

  SkPixmap pixmap(SkImageInfo::MakeN32Premul(width, height), buff.get(),
                  row_bytes);
  image.bitmap()->peekPixels(&pixmap);
  if (!pixmap.readPixels(SkImageInfo::MakeN32Premul(width, height), buff.get(),
                         row_bytes, 0, 0)) {
    // Use the default width and height when the shadow data not exist.
    width = kDefaultWidth;
    height = kDefaultHeight;
  }
  drag_param->pixelmap_buffer = std::move(buff);
  drag_param->pixelmap_width = width;
  drag_param->pixelmap_height = height;

  gfx::Vector2d drag_image_offset = data.provider().GetDragImageOffset();
  int image_offset_x = drag_image_offset.x();
  int image_offset_y = drag_image_offset.y();
  // When the drag_image_offset data is abnormal, need to reposition the distance
  // based on the width and height of the drag image,
  // Move the mouse to the middle of the drag image.
  if (image_offset_x < 0 || image_offset_x > width) {
    image_offset_x = width / kHalfDivisor;
  }
  if (image_offset_y < 0 || image_offset_y > height) {
    image_offset_y = height / kHalfDivisor;
  }
  drag_param->pixelmap_touch_x = image_offset_x;
  drag_param->pixelmap_touch_y = image_offset_y;
}

}  // namespace ui
