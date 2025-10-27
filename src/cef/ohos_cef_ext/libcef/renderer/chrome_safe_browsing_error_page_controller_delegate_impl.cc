// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/renderer/chrome_safe_browsing_error_page_controller_delegate_impl.h"

#include "content/public/renderer/render_frame.h"

ChromeSafeBrowsingErrorPageControllerDelegateImpl::
    ChromeSafeBrowsingErrorPageControllerDelegateImpl(
        content::RenderFrame* render_frame)
    : content::RenderFrameObserver(render_frame),
      content::RenderFrameObserverTracker<
          ChromeSafeBrowsingErrorPageControllerDelegateImpl>(render_frame) {}

ChromeSafeBrowsingErrorPageControllerDelegateImpl::
    ~ChromeSafeBrowsingErrorPageControllerDelegateImpl() = default;

void ChromeSafeBrowsingErrorPageControllerDelegateImpl::PrepareForErrorPage() {
  pending_error_ = true;
}

void ChromeSafeBrowsingErrorPageControllerDelegateImpl::OnDestruct() {
  delete this;
}

void ChromeSafeBrowsingErrorPageControllerDelegateImpl::
    DidCommitProvisionalLoad(ui::PageTransition transition) {
  committed_error_ = pending_error_;
  pending_error_ = false;
}

void ChromeSafeBrowsingErrorPageControllerDelegateImpl::DidFinishLoad() {
  if (committed_error_) {
    security_interstitials::SecurityInterstitialPageController::Install(
        render_frame());
  }
}
