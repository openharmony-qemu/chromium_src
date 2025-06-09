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
