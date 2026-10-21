// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mojo/public/cpp/bindings/associated_group_controller.h"

#include "base/debug/leak_annotations.h"
#include "base/logging.h"
#include "mojo/public/cpp/bindings/associated_group.h"

namespace mojo {

AssociatedGroupController::AssociatedGroupController(
    scoped_refptr<base::SequencedTaskRunner> task_runner)
    : bound_task_runner_(std::move(task_runner)) {}

AssociatedGroupController::~AssociatedGroupController() = default;

void AssociatedGroupController::Release() const {
  // Hides the base class's Release() to ensure ref-count decrements always
  // happen on the bound sequence. If we're off-sequence, the Release is posted
  // rather than run immediately, so that destruction and sequence-affine
  // teardown are properly serialized.
  // If |bound_task_runner_| is null (e.g. legacy construction path), fall back
  // to the base class's Release() directly — no sequence-affinity constraint
  // can be enforced, log error instead of CHECK crash.
  if (!bound_task_runner_) {
    LOG(ERROR) << "AssociatedGroupController::Release() called with null "
               << "bound_task_runner_; falling back to base Release().";
    if (base::subtle::RefCountedThreadSafeBase::Release()) {
      delete this;
    }
    return;
  }
  if (!bound_task_runner_->RunsTasksInCurrentSequence()) {
    // Destroying off-sequence which would violate sequence-affinity,
    // so we mark the object as leaking in case the PostTask fails,
    // i.e. at browser shutdown.
    bound_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(
            [](const AssociatedGroupController* self) { self->Release(); },
            base::Unretained(this)));
    ANNOTATE_LEAKING_OBJECT_PTR(this);
    return;
  }
  if (base::subtle::RefCountedThreadSafeBase::Release()) {
    delete this;
  }
}

ScopedInterfaceEndpointHandle
AssociatedGroupController::CreateScopedInterfaceEndpointHandle(InterfaceId id) {
  return ScopedInterfaceEndpointHandle(id, this);
}

bool AssociatedGroupController::NotifyAssociation(
    ScopedInterfaceEndpointHandle* handle_to_send,
    InterfaceId id) {
  return handle_to_send->NotifyAssociation(id, this);
}

}  // namespace mojo
