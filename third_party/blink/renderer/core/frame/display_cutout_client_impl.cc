// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/core/frame/display_cutout_client_impl.h"

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/display_cutout_client_imp_utils.h"
#include "third_party/blink/renderer/core/css/document_style_environment_variables.h"
#include "third_party/blink/renderer/core/css/style_engine.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/platform/runtime_enabled_features.h"

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
#include "third_party/blink/renderer/core/page/page.h"
#include "ui/gfx/geometry/insets.h"
#endif

namespace blink {

DisplayCutoutClientImpl::DisplayCutoutClientImpl(
    LocalFrame* frame,
    mojo::PendingAssociatedReceiver<mojom::blink::DisplayCutoutClient> receiver)
    : frame_(frame),
      receiver_(this, frame->DomWindow()->GetExecutionContext()) {
  receiver_.Bind(std::move(receiver), frame->GetFrameScheduler()->GetTaskRunner(
                                          TaskType::kInternalDefault));
  utils_ = MakeGarbageCollected<DisplayCutoutClientImplUtils>(this);
}

void DisplayCutoutClientImpl::BindMojoReceiver(
    LocalFrame* frame,
    mojo::PendingAssociatedReceiver<mojom::blink::DisplayCutoutClient>
        receiver) {
  if (!frame) {
    return;
  }
  MakeGarbageCollected<DisplayCutoutClientImpl>(frame, std::move(receiver));
}

void DisplayCutoutClientImpl::SetSafeArea(const gfx::Insets& safe_area) {
  utils_->SetSafeArea(safe_area);
}

void DisplayCutoutClientImpl::Trace(Visitor* visitor) const {
  visitor->Trace(frame_);
  visitor->Trace(receiver_);
  visitor->Trace(utils_);
}

}  // namespace blink
