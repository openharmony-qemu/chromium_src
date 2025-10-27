// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_SAFE_BROWSING_ERROR_PAGE_CONTROLLER_DELEGATE_IMPL_H_
#define CHROME_SAFE_BROWSING_ERROR_PAGE_CONTROLLER_DELEGATE_IMPL_H_
#pragma once

#include "components/security_interstitials/content/renderer/security_interstitial_page_controller.h"
#include "components/security_interstitials/core/controller_client.h"
#include "content/public/renderer/render_frame_observer.h"
#include "content/public/renderer/render_frame_observer_tracker.h"

namespace content {
class RenderFrame;
}  // namespace content

class ChromeSafeBrowsingErrorPageControllerDelegateImpl
    : public content::RenderFrameObserver,
      public content::RenderFrameObserverTracker<
          ChromeSafeBrowsingErrorPageControllerDelegateImpl> {
 public:
  explicit ChromeSafeBrowsingErrorPageControllerDelegateImpl(
      content::RenderFrame* render_frame);

  // Disallow copy and assign
  ChromeSafeBrowsingErrorPageControllerDelegateImpl(
      const ChromeSafeBrowsingErrorPageControllerDelegateImpl&) = delete;
  ChromeSafeBrowsingErrorPageControllerDelegateImpl& operator=(
      const ChromeSafeBrowsingErrorPageControllerDelegateImpl&) = delete;

  ~ChromeSafeBrowsingErrorPageControllerDelegateImpl() override;

  // Notifies us that a navigation error has occurred and will be committed
  void PrepareForErrorPage();

  // content::RenderFrameObserver:
  void OnDestruct() override;
  void DidCommitProvisionalLoad(ui::PageTransition transition) override;
  void DidFinishLoad() override;

 private:
  // Whether there is an error page pending to be committed.
  bool pending_error_ = false;

  // Whether the committed page is an error page.
  bool committed_error_ = false;
};

#endif  // CHROME_SAFE_BROWSING_ERROR_PAGE_CONTROLLER_DELEGATE_IMPL_H_
