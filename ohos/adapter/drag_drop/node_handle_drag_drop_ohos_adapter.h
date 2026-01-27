/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
 
#ifndef OHOS_ADAPTER_DRAG_ADAPTER_NODE_HANDLE_DRAG_DROP_OHOS_ADAPTER_H
#define OHOS_ADAPTER_DRAG_ADAPTER_NODE_HANDLE_DRAG_DROP_OHOS_ADAPTER_H
 
#include <arkui/drag_and_drop.h>
#include <arkui/native_type.h>
#include <database/udmf/udmf.h>
 
#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
 
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/drag_drop/drag_drop_common.h"
#include "ohos/adapter/export.h"
 
class UdsPlainTextDeletor {
 public:
  void operator()(OH_UdsPlainText* ptr) const {
    if (ptr != nullptr) {
      OH_UdsPlainText_Destroy(ptr);
    }
  }
};
 
class UdsHyperlinkDeletor {
 public:
  void operator()(OH_UdsHyperlink* ptr) const {
    if (ptr != nullptr) {
      OH_UdsHyperlink_Destroy(ptr);
    }
  }
};
 
class UdsHtmlDeletor {
 public:
  void operator()(OH_UdsHtml* ptr) const {
    if (ptr != nullptr) {
      OH_UdsHtml_Destroy(ptr);
    }
  }
};
 
class UdsFileUriDeletor {
 public:
  void operator()(OH_UdsFileUri* ptr) const {
    if (ptr != nullptr) {
      OH_UdsFileUri_Destroy(ptr);
    }
  }
};
 
class UdmfDataDeletor {
 public:
  void operator()(OH_UdmfData* ptr) const {
    if (ptr != nullptr) {
      OH_UdmfData_Destroy(ptr);
    }
  }
};
 
class UdmfRecordDeletor {
 public:
  void operator()(OH_UdmfRecord* ptr) const {
    if (ptr != nullptr) {
      OH_UdmfRecord_Destroy(ptr);
    }
  }
};
 
namespace ohos {
namespace adapter {
 
using UdsPlainTextPtr = std::unique_ptr<OH_UdsPlainText, UdsPlainTextDeletor>;
using UdsHyperlinkPtr = std::unique_ptr<OH_UdsHyperlink, UdsHyperlinkDeletor>;
using UdsHtmlPtr = std::unique_ptr<OH_UdsHtml, UdsHtmlDeletor>;
using UdsFileUriPtr = std::unique_ptr<OH_UdsFileUri, UdsFileUriDeletor>;
using UdmfDataPtr = std::unique_ptr<OH_UdmfData, UdmfDataDeletor>;
using UdmfRecordPtr = std::unique_ptr<OH_UdmfRecord, UdmfRecordDeletor>;
 
class ADAPTER_EXPORT_API NodeHandleDragDropOhosAdapter {
 public:
  static NodeHandleDragDropOhosAdapter& GetInstance();
  NodeHandleDragDropOhosAdapter(const NodeHandleDragDropOhosAdapter&) = delete;
  NodeHandleDragDropOhosAdapter(NodeHandleDragDropOhosAdapter&&) = delete;
  NodeHandleDragDropOhosAdapter& operator=(
      const NodeHandleDragDropOhosAdapter&) = delete;
  virtual ~NodeHandleDragDropOhosAdapter();
 
  void OnDragEnterCB(const std::string xcomponent_id, ArkUI_NodeEvent* event);
  void OnDropCB(const std::string xcomponent_id, ArkUI_NodeEvent* event);
  void OnDragMoveCB(const std::string xcomponent_id, ArkUI_NodeEvent* event);
  void OnDragLeaveCB(const std::string xcomponent_id, ArkUI_NodeEvent* event);
  void OnDragEndCB(const std::string xcomponent_id);
 
  bool ExecuteDrag(std::shared_ptr<OhosStartDragParam> drag_param,
                   const std::string& window_id);
  void SetDraggingStarted(bool dragging_start) {
    is_dragging_started_.store(dragging_start, std::memory_order_release);
  }
  bool IsDraggingStarted() {
    return is_dragging_started_.load(std::memory_order_acquire);
  }
 
 private:
  NodeHandleDragDropOhosAdapter() = default;
  void HandlePlainTextRecord(std::shared_ptr<OhosStartDragParam> drag_param);
  void HandleUdsHyperlinkRecord(std::shared_ptr<OhosStartDragParam> drag_param);
  void HandleUdsHtmlRecord(std::shared_ptr<OhosStartDragParam> drag_param);
  void HandleWebImageRecord(std::shared_ptr<OhosStartDragParam> drag_param);
  void HandleWebCustomDataRecord(
      std::shared_ptr<OhosStartDragParam> drag_param);
  void HandleBookmarkDataRecord(std::shared_ptr<OhosStartDragParam> drag_param);
  void HandleRecordData(OH_UdmfRecord** records,
                        unsigned int count,
                        OhosDropData& drop_data);
  void ClearDragRecords();
  bool PreparePixelmapOptions(std::shared_ptr<OhosStartDragParam> drag_param);
  bool PrepareDragActionOptions(std::shared_ptr<OhosStartDragParam> drag_param);
  bool PrepareDragActionPixelmap(std::shared_ptr<OhosStartDragParam> drag_param,
                                 std::vector<OH_PixelmapNative*>& pixel_vector);
  bool PrepareDragData(std::shared_ptr<OhosStartDragParam> drag_param);
  void PreparePlainTextData(OH_UdmfRecord* umdf_record,
                            OhosDropData& drop_data);
  void PrepareHyperlinkData(OH_UdmfRecord* umdf_record,
                            OhosDropData& drop_data);
  void PrepareHtmlData(OH_UdmfRecord* umdf_record, OhosDropData& drop_data);
  void PrepareFileUriData(OH_UdmfRecord* umdf_record,
                          std::vector<std::string>& file_paths);
  void PrepareBookmarkData(OH_UdmfRecord* umdf_record, OhosDropData& drop_data);
  void PrepareWebCustomeData(OH_UdmfRecord* umdf_record,
                             OhosDropData& drop_data);
  void HandleDefaultValueForDragEnter(ArkUI_DragEvent* drag_event,
                                      OhosDropData& drop_data);
  void SetSuggestedDropOperation(ArkUI_NodeEvent* node_event,
                                 ArkUI_DragEvent** drag_event);
  void HandleDropDataForDrop(ArkUI_NodeEvent* node_event,
                             OhosDropData& drop_data,
                             const std::string xcomponent_id);
  OH_UdmfRecord* GetUdmfRecordForDragData();
  void ClearDragActionResource();
  bool HasWebImageRecord();
  std::string dragged_extension_file_name_;
  static std::string window_id_;
  std::vector<OH_UdmfRecord*> drag_records_;
  std::atomic<bool> is_dragging_started_ = false;
 
  ArkUI_DragAction* drag_action_ = nullptr;
  ArkUI_DragPreviewOption* preview_options_ = nullptr;
  OH_Pixelmap_InitializationOptions* pixelmap_initial_options_ = nullptr;
  OH_PixelmapNative* pixelmap_native_ = nullptr;
  OH_UdmfData* udmf_data_ = nullptr;
};
 
}  // namespace adapter
}  // namespace ohos
 
#endif  // OHOS_ADAPTER_DRAG_ADAPTER_NODE_HANDLE_DRAG_DROP_OHOS_ADAPTER_H
