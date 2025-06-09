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

#include "ohos_whole_screen_move_loop.h"

#include <cstddef>
#include <memory>
#include <utility>

#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/run_loop.h"
#include "base/task/current_thread.h"
#include "base/task/single_thread_task_runner.h"
#include "ui/events/event.h"
#include "ui/events/event_utils.h"
#include "ui/events/platform/platform_event_source.h"

namespace ui {

OhosWholeScreenMoveLoop::OhosWholeScreenMoveLoop(OhosMoveLoopDelegate* delegate)
    : delegate_(delegate) {}

OhosWholeScreenMoveLoop::~OhosWholeScreenMoveLoop() {
  EndMoveLoop();
}

bool OhosWholeScreenMoveLoop::RunMoveLoop(
    [[maybe_unused]] bool can_grab_pointer,
    scoped_refptr<ui::PlatformCursor> old_cursor,
    scoped_refptr<ui::PlatformCursor> new_cursor) {
  LOG(INFO) << "[OhosTabDrag] " << __FUNCTION__
            << ", can_grab_pointer:" << can_grab_pointer;
  initial_cursor_ = old_cursor;
  std::unique_ptr<ui::ScopedEventDispatcher> old_dispatcher =
      std::move(nested_dispatcher_);
  nested_dispatcher_ =
      ui::PlatformEventSource::GetInstance()->OverrideDispatcher(this);
  base::WeakPtr<OhosWholeScreenMoveLoop> alive(weak_factory_.GetWeakPtr());

  in_move_loop_ = true;
  canceled_ = false;
  base::RunLoop run_loop(base::RunLoop::Type::kNestableTasksAllowed);
  quit_closure_ = run_loop.QuitClosure();
  run_loop.Run();
  if (!alive) {
    return false;
  }
  nested_dispatcher_ = std::move(old_dispatcher);
  return !canceled_;
}

bool OhosWholeScreenMoveLoop::CanDispatchEvent(const ui::PlatformEvent& event) {
  NOTIMPLEMENTED();
  return in_move_loop_;
}

uint32_t OhosWholeScreenMoveLoop::DispatchEvent(
    const ui::PlatformEvent& event) {
  if (!in_move_loop_ || event == nullptr) {
    return ui::POST_DISPATCH_PERFORM_DEFAULT;
  }
  switch (event->type()) {
    case ui::EventType::kMouseMoved:
    case ui::EventType::kMouseDragged: {
      delegate_->OnMouseMove();
      // Do not intercepted. The mouse event needs to be transparently
      // transmitted to the TabDragController.
      return ui::POST_DISPATCH_PERFORM_DEFAULT;
    }
    case ui::EventType::kTouchMoved: {
      delegate_->OnTouchMove();
      return ui::POST_DISPATCH_NONE;
    }
    case ui::EventType::kMouseReleased:
    case ui::EventType::kTouchReleased:
    case ui::EventType::kTouchCancelled: {
      delegate_->OnMoveEnd();
      if (!grabbed_pointer_) {
        return ui::POST_DISPATCH_PERFORM_DEFAULT;
      }
      return ui::POST_DISPATCH_NONE;
    }
    case ui::EventType::kKeyPressed:
      if (event->AsKeyEvent()->key_code() == ui::VKEY_ESCAPE) {
        canceled_ = true;
        EndMoveLoop();
        return ui::POST_DISPATCH_NONE;
      }
      break;
    default:
      break;
  }
  return ui::POST_DISPATCH_PERFORM_DEFAULT;
}

void OhosWholeScreenMoveLoop::EndMoveLoop() {
  base::WeakPtr<OhosWholeScreenMoveLoop> alive(weak_factory_.GetWeakPtr());
  if (!alive || !in_move_loop_) {
    return;
  }

  nested_dispatcher_.reset();
  delegate_->OnMoveLoopEnded();
  in_move_loop_ = false;
  if (quit_closure_) {
    std::move(quit_closure_).Run();
  }
}

}  // namespace ui
