// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
      auto* mouse_event = event->AsMouseEvent();
      delegate_->OnMouseMovement(mouse_event->root_location(),
                                 mouse_event->flags(),
                                 mouse_event->time_stamp());
      return ui::POST_DISPATCH_NONE;
    }
    case ui::EventType::kMouseReleased: {
      if (event->AsMouseEvent()->IsLeftMouseButton()) {
        delegate_->OnMouseReleased();
        if (!grabbed_pointer_) {
          return ui::POST_DISPATCH_PERFORM_DEFAULT;
        }
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
