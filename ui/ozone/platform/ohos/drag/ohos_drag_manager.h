// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_DRAG_OHOS_DRAG_MANAGER_H_
#define UI_OZONE_PLATFORM_OHOS_DRAG_OHOS_DRAG_MANAGER_H_

#include "base/component_export.h"
#include "base/functional/callback.h"
#include "ohos/adapter/drag_drop/drag_drop_ohos_adapter.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/base/dragdrop/drag_drop_types.h"
#include "ui/base/dragdrop/mojom/drag_drop_types.mojom-forward.h"
#include "ui/base/dragdrop/os_exchange_data.h"
#include "ui/base/dragdrop/os_exchange_data_provider.h"
#include "ui/base/dragdrop/os_exchange_data_provider_factory.h"
#include "ui/base/dragdrop/os_exchange_data_provider_non_backed.h"
#include "ui/events/event_target.h"
#include "ui/events/platform/platform_event_dispatcher.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/geometry/vector2d.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/ozone/platform/ohos/drag/ohos_desktop_window_move_client.h"
#include "ui/ozone/platform/ohos/drag/ohos_move_loop_delegate.h"
#include "ui/platform_window/platform_window.h"
#include "ui/platform_window/platform_window_init_properties.h"
#include "ui/platform_window/wm/wm_drag_handler.h"
#include "ui/platform_window/wm/wm_move_loop_handler.h"
#include "ui/platform_window/wm/wm_move_resize_handler.h"

namespace ui {

class OhosDragManager : public WmDragHandler,
                        public WmMoveLoopHandler,
                        public OhosDesktopWindowMoveClient::Delegate {
 public:
  class Delegate {
   public:
    virtual void SetDragSourceWidget() = 0;
    virtual void ClearDragSourceWidget() = 0;
    virtual void EndDrag() = 0;
  };

  OhosDragManager(const OhosDragManager&) = delete;
  OhosDragManager& operator=(const OhosDragManager&) = delete;

  OhosDragManager(PlatformWindowDelegate* delegate,
                  PlatformWindow* platform_window);

  // WmMoveLoopHandler:
  bool RunMoveLoop(const gfx::Vector2d& drag_offset) override;
  void EndMoveLoop() override;

  // WmDragHandler:
  bool StartDrag(const OSExchangeData& data,
                 int operations,
                 mojom::DragEventSource source,
                 gfx::NativeCursor cursor,
                 bool can_grab_pointer,
                 base::OnceClosure drag_started_callback,
                 WmDragHandler::DragFinishedCallback drag_finished_callback,
                 WmDragHandler::LocationDelegate* delegate) override;

  void CancelDrag() override;
  void UpdateDragImage(const gfx::ImageSkia& image,
                       const gfx::Vector2d& offset) override;

  // OhosDesktopWindowMoveClient::Delegate
  scoped_refptr<PlatformCursor> GetLastCursor() override;
  gfx::Size GetSize() override;
  void SetBoundsOnMove(const gfx::Rect& requested_bounds) override;

  bool notified_enter_ = false;
  raw_ptr<const OSExchangeDataProviderNonBacked> source_provider_ = nullptr;

  void DragEnter(const ohos::adapter::OhosDragInfo& drag_info,
                 gfx::PointF point);
  void UpdateDrag(const gfx::Point& window_point);
  void DragLeave();
  void OnDrop(const ohos::adapter::OhosDragInfo& drag_info, gfx::PointF point);
  void DragEnd();

  void SetDelegate(OhosDragManager::Delegate* drag_delegate) {
    drag_delegate_ = drag_delegate;
  }

 private:
  WmDragHandler::DragFinishedCallback drag_finished_callback_;
  raw_ptr<WmDragHandler::LocationDelegate> drag_location_delegate_ = nullptr;
  int allowed_drag_operations_ = 0;
  std::unique_ptr<OhosDesktopWindowMoveClient> ohos_window_move_client_;
  scoped_refptr<ui::PlatformCursor> last_cursor_;
  const raw_ptr<PlatformWindowDelegate> platform_window_delegate_;
  const raw_ptr<PlatformWindow> platform_window_;
  raw_ptr<OhosDragManager::Delegate> drag_delegate_ = nullptr;
  std::unique_ptr<ui::OSExchangeData> data_;
  base::WeakPtrFactory<OhosDragManager> weak_factory_{this};
  bool is_drag_source_ = false;
  base::OnceClosure quit_closure_;
  gfx::PointF last_point_;
  int current_modifier_;
  bool is_drag_end_ = true;

  std::vector<FileInfo> DecodeFileName(
      const std::vector<std::string>& file_paths);
  void HandleDragData(const ohos::adapter::OhosDragInfo& drag_info,
                      OSExchangeDataProvider& provider);
  void PrepareDragInfo(const OSExchangeData& data);
  void HandleImageFileUri(const base::FilePath& file_name,
                          const std::string& file_contents,
                          std::string& image_uri);
  void HandlePickleData(const OSExchangeData& data,
                        ohos::adapter::OhosDragInfo& drag_info);
};
}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_DRAG_OHOS_DRAG_MANAGER_H_
