/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ohos/adapter/drag_drop/drag_drop_ohos_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/file_manager/file_manager_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"

using namespace ohos::adapter::xcomponent;

namespace ohos {
namespace adapter {

namespace {
const int IMAGE_PIXEL_MAP = 4;
}  // namespace

DragDropOhosAdapter::DragDropOhosAdapter() {}
DragDropOhosAdapter& DragDropOhosAdapter::GetInstance() {
  static DragDropOhosAdapter adapter;
  return adapter;
}

void DragDropOhosAdapter::ExecuteDrag(std::shared_ptr<OhosStartDragParam> drag_param,
                                      const std::string& window_id) {
  if (auto func = ohos::adapter::GetJSFunction("DragDropAdapter.StartDrag")) {
    int pixelMapWidth = drag_param->pixelMapWidth;
    int pixelMapHeight = drag_param->pixelMapHeight;
    aki::ArrayBuffer pixel_buffer(
        (uint8_t*)drag_param->pixelMapBuffer.get(),
        pixelMapWidth * pixelMapHeight *
            IMAGE_PIXEL_MAP);
    aki::ArrayBuffer bookmark_buffer(drag_param->basicData.bookmarkData.data(),
                                     drag_param->basicData.bookmarkData.size());
    aki::ArrayBuffer web_custom_buffer(drag_param->basicData.webCustomData.data(),
                                       drag_param->basicData.webCustomData.size());
    OhosDragParamToJs drag_param_to_js;
    drag_param_to_js.text = drag_param->basicData.text;
    drag_param_to_js.url = drag_param->basicData.url;
    drag_param_to_js.urlTitle = drag_param->basicData.urlTitle;
    drag_param_to_js.html = drag_param->basicData.html;
    drag_param_to_js.webImageFilePath = drag_param->webImageFilePath;
    drag_param_to_js.bookmarkBuffer = std::move(bookmark_buffer);
    drag_param_to_js.webCustomBuffer = std::move(web_custom_buffer);
    drag_param_to_js.pixelMapBuffer = std::move(pixel_buffer);
    drag_param_to_js.pixelMapWidth = pixelMapWidth;
    drag_param_to_js.pixelMapHeight = pixelMapHeight;
    drag_param_to_js.pixelMapTouchX = drag_param->pixelMapTouchX;
    drag_param_to_js.pixelMapTouchY = drag_param->pixelMapTouchY;
    drag_param_to_js.windowId = window_id;
    func->Invoke<void>(drag_param_to_js);
  }
}

void HandleDropData(const aki::Value drag_info_value,
                    const std::vector<std::string>& file_paths,
                    OhosDropData& drop_data) {
  drop_data.basicData.text = drag_info_value["text"].As<std::string>();
  drop_data.basicData.url = drag_info_value["url"].As<std::string>();
  drop_data.basicData.urlTitle = drag_info_value["urlTitle"].As<std::string>();
  drop_data.basicData.html = drag_info_value["html"].As<std::string>();
  drop_data.filePaths = file_paths;
  drop_data.basicData.bookmarkData = std::vector<uint8_t>(
      drag_info_value["bookmarkBuffer"].As<aki::ArrayBuffer>().GetData(),
      drag_info_value["bookmarkBuffer"].As<aki::ArrayBuffer>().GetData() +
          drag_info_value["bookmarkBuffer"].As<aki::ArrayBuffer>().GetLength());
  drop_data.basicData.webCustomData = std::vector<uint8_t>(
      drag_info_value["webCustomBuffer"].As<aki::ArrayBuffer>().GetData(),
      drag_info_value["webCustomBuffer"].As<aki::ArrayBuffer>().GetData() +
          drag_info_value["webCustomBuffer"].As<aki::ArrayBuffer>().GetLength());
}

void OnDragEnterCB(const std::string& xcomponent_id,
                   const aki::Value drag_info_value,
                   const std::vector<std::string>& file_paths) {
  auto render = xcomponent::XComponentManager::GetInstance()->GetXComponent(
      xcomponent_id);
  if (render == nullptr) {
    LOGE("[OhosDrag] [drag-enter-event]can not get render: %{public}s",
         xcomponent_id.c_str());
    return;
  }
  OhosDropData drop_data;
  HandleDropData(drag_info_value, file_paths, drop_data);
  render->OnDragEnterEvent(drop_data);
}

void OnDropCB(const std::string& xcomponent_id,
              const aki::Value drag_info_value,
              const std::vector<std::string>& file_uris) {
  auto render = xcomponent::XComponentManager::GetInstance()->GetXComponent(
      xcomponent_id);
  if (render == nullptr) {
    LOGE("[OhosDrag] [drag-drop-event]can not get render: %{public}s",
         xcomponent_id.c_str());
    return;
  }
  std::vector<std::string> file_paths;
  for (const auto& uri : file_uris) {
    std::string file_path;
    ohos::adapter::FileManagerAdapter::GetInstance().GetPathForUri(uri.c_str(),
                                                                   file_path);
    if (file_path.empty()) {
        LOGE("[OhosDrag] GetPathForUri failed , uri: %{public}s", uri.c_str());
        continue;
    }
    file_paths.push_back(std::move(file_path));
  }
  OhosDropData drop_data;
  HandleDropData(drag_info_value, file_paths, drop_data);
  render->OnDropEvent(drop_data);
}

void OnDragMoveCB(const std::string& xcomponent_id,
                  const float window_x,
                  const float window_y) {
  auto render = xcomponent::XComponentManager::GetInstance()->GetXComponent(
      xcomponent_id);
  if (render == nullptr) {
    LOGE("[OhosDrag] [drag-move-event]can not get render: %{public}s",
         xcomponent_id.c_str());
    return;
  }
  render->OnDragMoveEvent(window_x, window_y);
}

void OnDragLeaveCB(const std::string& xcomponent_id) {
  auto render = xcomponent::XComponentManager::GetInstance()->GetXComponent(
      xcomponent_id);
  if (render == nullptr) {
    LOGE("[OhosDrag] [drag-leave-event]can not get render: %{public}s",
         xcomponent_id.c_str());
    return;
  }
  render->OnDragLeaveEvent();
}

void OnDragEndCB(const std::string& xcomponent_id) {
  auto render = xcomponent::XComponentManager::GetInstance()->GetXComponent(
      xcomponent_id);
  if (render == nullptr) {
    LOGE("[OhosDrag] [drag-end-event]can not get render: %{public}s",
         xcomponent_id.c_str());
    return;
  }
  render->OnDragEndEvent();
}

std::string DragDropOhosAdapter::GetDraggedExtensionFileName() {
  return dragged_extension_file_name_;
}

void DragDropOhosAdapter::SetDraggedExtensionFileName(const std::string& file_name) {
  dragged_extension_file_name_ = file_name;
}

JSBIND_CLASS(OhosDragParamToJs) {
  JSBIND_CONSTRUCTOR<>();
  JSBIND_PROPERTY(text);
  JSBIND_PROPERTY(url);
  JSBIND_PROPERTY(urlTitle);
  JSBIND_PROPERTY(html);
  JSBIND_PROPERTY(webImageFilePath);
  JSBIND_PROPERTY(bookmarkBuffer);
  JSBIND_PROPERTY(webCustomBuffer);
  JSBIND_PROPERTY(pixelMapBuffer);
  JSBIND_PROPERTY(pixelMapWidth);
  JSBIND_PROPERTY(pixelMapHeight);
  JSBIND_PROPERTY(pixelMapTouchX);
  JSBIND_PROPERTY(pixelMapTouchY);
  JSBIND_PROPERTY(windowId);
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(OnDragEnterCB);
  JSBIND_FUNCTION(OnDropCB);
  JSBIND_FUNCTION(OnDragLeaveCB);
  JSBIND_FUNCTION(OnDragEndCB);
  JSBIND_FUNCTION(OnDragMoveCB);
}

}  // namespace adapter
}  // namespace ohos
