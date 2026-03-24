// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_DRAG_ADAPTER_DRAG_DROP_OHOS_ADAPTER_H
#define OHOS_ADAPTER_DRAG_ADAPTER_DRAG_DROP_OHOS_ADAPTER_H

#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "ohos/adapter/drag_drop/drag_drop_common.h"
#include "ohos/adapter/export.h"
#include "third_party/aki/include/aki/value/array_buffer.h"

namespace ohos {
namespace adapter {
struct OhosDragParamToJs {
  std::string text;
  std::string url;
  std::string url_title;
  std::string html;
  std::string web_image_file_path;
  aki::ArrayBuffer bookmark_buffer;
  aki::ArrayBuffer web_custom_buffer;
  aki::ArrayBuffer pixelmap_buffer;
  int pixelmap_width;
  int pixelmap_height;
  int pixelmap_touch_x;
  int pixelmap_touch_y;
  std::string window_id;
};

class ADAPTER_EXPORT_API DragDropOhosAdapter {
 public:
  static DragDropOhosAdapter& GetInstance();
  DragDropOhosAdapter(const DragDropOhosAdapter&) = delete;
  DragDropOhosAdapter(DragDropOhosAdapter&&) = delete;
  DragDropOhosAdapter& operator=(const DragDropOhosAdapter&) = delete;
  virtual ~DragDropOhosAdapter() = default;

  bool ExecuteDrag(std::shared_ptr<OhosStartDragParam> drag_param,
                   const std::string& window_id);

  std::string GetDraggedExtensionFileName();
  void SetDraggedExtensionFileName(const std::string& file_name);
  void SetDraggingStarted(bool dragging_start) {
    is_dragging_started_.store(dragging_start, std::memory_order_release);
  }
  bool IsDraggingStarted() {
    return is_dragging_started_.load(std::memory_order_acquire);
  }

 private:
  DragDropOhosAdapter();
  std::string dragged_extension_file_name_;
  std::atomic<bool> is_dragging_started_ = false;
};

}  // namespace adapter
}  // namespace ohos

#endif  // OHOS_ADAPTER_DRAG_ADAPTER_DRAG_DROP_OHOS_ADAPTER_H
