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
#include "ohos/adapter/drag_drop/drag_drop_common.h"
#include "ohos/adapter/file_manager/file_manager_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"

using namespace ohos::adapter::xcomponent;

namespace ohos {
namespace adapter {

namespace {
constexpr int kImagePixelMap = 4;
}  // namespace

DragDropOhosAdapter::DragDropOhosAdapter() {}
DragDropOhosAdapter& DragDropOhosAdapter::GetInstance() {
  static DragDropOhosAdapter adapter;
  return adapter;
}

bool DragDropOhosAdapter::ExecuteDrag(
    std::shared_ptr<OhosStartDragParam> drag_param,
    const std::string& window_id) {
  auto func = ohos::adapter::GetJSFunction("DragDropAdapter.StartDrag");
  if (func == nullptr) {
    LOGE("[OhosDrag]ExecuteDrag fail, func is null");
    return false;
  }
  OhosDragParamToJs drag_param_to_js;
  ConvertDragParamForJs(drag_param, window_id, drag_param_to_js);

  auto promise = std::make_shared<std::promise<bool>>();
  auto future = promise->get_future();
  std::function<void(bool)> callback = [promise](bool result) -> void {
    promise->set_value(result);
  };
  func->Invoke<void>(drag_param_to_js, callback);
  auto status = future.wait_for(std::chrono::seconds(1));
  if (status == std::future_status::timeout) {
    LOGE("[OhosDrag]ExecuteDrag Wait timeout");
    return false;
  }
  bool result = future.get();
  return result;
}

void DragDropOhosAdapter::ConvertDragParamForJs(
    std::shared_ptr<OhosStartDragParam> drag_param,
    const std::string& window_id,
    OhosDragParamToJs& drag_param_to_js) {
  int pixelmap_width = drag_param->pixelmap_width;
  int pixelmap_height = drag_param->pixelmap_height;
  aki::ArrayBuffer pixel_buffer(
      (uint8_t*)drag_param->pixelmap_buffer.get(),
      pixelmap_width * pixelmap_height * kImagePixelMap);
  aki::ArrayBuffer bookmark_buffer(drag_param->basic_data.bookmark_data.data(),
                                   drag_param->basic_data.bookmark_data.size());
  aki::ArrayBuffer web_custom_buffer(
      drag_param->basic_data.web_custom_data.data(),
      drag_param->basic_data.web_custom_data.size());
  drag_param_to_js.text = drag_param->basic_data.text;
  drag_param_to_js.url = drag_param->basic_data.url;
  drag_param_to_js.url_title = drag_param->basic_data.url_title;
  drag_param_to_js.html = drag_param->basic_data.html;
  drag_param_to_js.web_image_file_path = drag_param->web_image_file_path;
  drag_param_to_js.bookmark_buffer = std::move(bookmark_buffer);
  drag_param_to_js.web_custom_buffer = std::move(web_custom_buffer);
  drag_param_to_js.pixelmap_buffer = std::move(pixel_buffer);
  drag_param_to_js.pixelmap_width = pixelmap_width;
  drag_param_to_js.pixelmap_height = pixelmap_height;
  drag_param_to_js.pixelmap_touch_x = drag_param->pixelmap_touch_x;
  drag_param_to_js.pixelmap_touch_y = drag_param->pixelmap_touch_y;
  drag_param_to_js.window_id = window_id;
}

void HandleDropData(const aki::Value drag_info_value,
                    const std::vector<std::string>& file_paths,
                    OhosDropData& drop_data) {
  drop_data.basic_data.text = drag_info_value["text"].As<std::string>();
  drop_data.basic_data.url = drag_info_value["url"].As<std::string>();
  drop_data.basic_data.url_title = drag_info_value["urlTitle"].As<std::string>();
  drop_data.basic_data.html = drag_info_value["html"].As<std::string>();
  drop_data.file_paths = file_paths;
  drop_data.basic_data.bookmark_data = std::vector<uint8_t>(
      drag_info_value["bookmarkBuffer"].As<aki::ArrayBuffer>().GetData(),
      drag_info_value["bookmarkBuffer"].As<aki::ArrayBuffer>().GetData() +
          drag_info_value["bookmarkBuffer"].As<aki::ArrayBuffer>().GetLength());
  drop_data.basic_data.web_custom_data = std::vector<uint8_t>(
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
  JSBIND_PROPERTY(url_title);
  JSBIND_PROPERTY(html);
  JSBIND_PROPERTY(web_image_file_path);
  JSBIND_PROPERTY(bookmark_buffer);
  JSBIND_PROPERTY(web_custom_buffer);
  JSBIND_PROPERTY(pixelmap_buffer);
  JSBIND_PROPERTY(pixelmap_width);
  JSBIND_PROPERTY(pixelmap_height);
  JSBIND_PROPERTY(pixelmap_touch_x);
  JSBIND_PROPERTY(pixelmap_touch_y);
  JSBIND_PROPERTY(window_id);
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
