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

#ifndef OHOS_ADAPTER_DRAG_ADAPTER_DRAG_DROP_OHOS_ADAPTER_H
#define OHOS_ADAPTER_DRAG_ADAPTER_DRAG_DROP_OHOS_ADAPTER_H

#include <cstdint>
#include <memory>
#include <string>

#include <multimedia/image_framework/image_mdk_common.h>
#include <multimedia/image_framework/image_pixel_map_mdk.h>

#include "ohos/adapter/export.h"
#include "third_party/aki/include/aki/value/array_buffer.h"

namespace ohos {
namespace adapter {

struct OhosBasicDragData {
  std::string text;
  std::string url;
  std::string urlTitle;
  std::string html;
  std::vector<uint8_t> bookmarkData;
  std::vector<uint8_t> webCustomData;
};

struct OhosStartDragParam {
  OhosBasicDragData basicData;
  std::string webImageFilePath;
  std::shared_ptr<char[]> pixelMapBuffer;
  int pixelMapWidth;
  int pixelMapHeight;
  int pixelMapTouchX;
  int pixelMapTouchY;
};

struct OhosDropData {
  OhosBasicDragData basicData;
  std::vector<std::string> filePaths;
};

struct OhosDragParamToJs {
  std::string text;
  std::string url;
  std::string urlTitle;
  std::string html;
  std::string webImageFilePath;
  aki::ArrayBuffer bookmarkBuffer;
  aki::ArrayBuffer webCustomBuffer;
  aki::ArrayBuffer pixelMapBuffer;
  int pixelMapWidth;
  int pixelMapHeight;
  int pixelMapTouchX;
  int pixelMapTouchY;
  std::string windowId;
};

class ADAPTER_EXPORT_API DragDropOhosAdapter {
 public:
  static DragDropOhosAdapter& GetInstance();
  DragDropOhosAdapter(const DragDropOhosAdapter&) = delete;
  DragDropOhosAdapter(DragDropOhosAdapter&&) = delete;
  DragDropOhosAdapter& operator=(const DragDropOhosAdapter&) = delete;
  virtual ~DragDropOhosAdapter() = default;

  void ExecuteDrag(std::shared_ptr<OhosStartDragParam> drag_param,
                   const std::string& window_id);

  std::string GetDraggedExtensionFileName();
  void SetDraggedExtensionFileName(const std::string& file_name);

 private:
  DragDropOhosAdapter();
  std::string dragged_extension_file_name_;
};

}  // namespace adapter
}  // namespace ohos

#endif  // OHOS_ADAPTER_DRAG_ADAPTER_DRAG_DROP_OHOS_ADAPTER_H
