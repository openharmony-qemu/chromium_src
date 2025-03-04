// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_DRAG_OHOS_WHOLE_SCREEN_MOVE_LOOP_H
#define UI_OZONE_PLATFORM_OHOS_DRAG_OHOS_WHOLE_SCREEN_MOVE_LOOP_H

#include <memory>

#include "base/component_export.h"
#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "ui/base/cursor/platform_cursor.h"
#include "ui/events/platform/platform_event_dispatcher.h"
#include "ui/events/platform/scoped_event_dispatcher.h"
#include "ui/ozone/platform/ohos/drag/ohos_move_loop_delegate.h"

namespace ui {

class OhosWholeScreenMoveLoop : public ui::PlatformEventDispatcher {
 public:
  explicit OhosWholeScreenMoveLoop(OhosMoveLoopDelegate* delegate);
  OhosWholeScreenMoveLoop(const OhosWholeScreenMoveLoop&) = delete;
  OhosWholeScreenMoveLoop& operator=(const OhosWholeScreenMoveLoop&) = delete;
  ~OhosWholeScreenMoveLoop() override;

  // ui:::PlatformEventDispatcher:
  bool CanDispatchEvent(const ui::PlatformEvent& event) override;
  uint32_t DispatchEvent(const ui::PlatformEvent& event) override;

  bool RunMoveLoop(bool can_grab_pointer,
                   scoped_refptr<ui::PlatformCursor> old_cursor,
                   scoped_refptr<ui::PlatformCursor> new_cursor);
  void EndMoveLoop();

 private:
  raw_ptr<OhosMoveLoopDelegate> delegate_;
  bool in_move_loop_;
  scoped_refptr<ui::PlatformCursor> initial_cursor_;
  bool canceled_;
  base::OnceClosure quit_closure_;
  std::unique_ptr<ui::ScopedEventDispatcher> nested_dispatcher_;
  base::WeakPtrFactory<OhosWholeScreenMoveLoop> weak_factory_{this};
  bool grabbed_pointer_;
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_DRAG_OHOS_WHOLE_SCREEN_MOVE_LOOP_H
