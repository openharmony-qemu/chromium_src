// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on display_cutout_host_impl.h originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#ifndef CONTENT_BROWSER_DISPLAY_CUTOUT_DISPLAY_CUTOUT_HOST_OHOS_H_
#define CONTENT_BROWSER_DISPLAY_CUTOUT_DISPLAY_CUTOUT_HOST_OHOS_H_

#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "content/public/browser/render_frame_host_receiver_set.h"
#include "content/public/browser/web_contents_observer.h"
#include "third_party/blink/public/mojom/page/display_cutout.mojom.h"
#include "ui/gfx/geometry/insets.h"

namespace content {

class RenderFrameHostImpl;
class WebContentsImpl;

class DisplayCutoutHostOhos : public blink::mojom::DisplayCutoutHost {
 public:
  explicit DisplayCutoutHostOhos(WebContentsImpl*);

  DisplayCutoutHostOhos(const DisplayCutoutHostOhos&) = delete;
  DisplayCutoutHostOhos& operator=(const DisplayCutoutHostOhos&) = delete;

  ~DisplayCutoutHostOhos() override;

  // Binds a new receiver for the specified frame.
  void BindReceiver(
      mojo::PendingAssociatedReceiver<blink::mojom::DisplayCutoutHost> receiver,
      RenderFrameHost* rfh);

  // blink::mojom::DisplayCutoutHost
  void NotifyViewportFitChanged(blink::mojom::ViewportFit value) override;

  // Stores the updated viewport fit value for a |frame| and notifies observers
  // if it has changed.
  void ViewportFitChangedForFrame(RenderFrameHost* rfh,
                                  blink::mojom::ViewportFit value);

  // Called by WebContents when various events occur.
  void DidAcquireFullscreen(RenderFrameHost* rfh);
  void DidExitFullscreen();
  void DidFinishNavigation(NavigationHandle* navigation_handle);
  void RenderFrameDeleted(RenderFrameHost* rfh);
  void RenderFrameCreated(RenderFrameHost* rfh);

  // Updates the safe area insets on the current frame.
  void SetDisplayCutoutSafeArea(gfx::Insets insets);

#if BUILDFLAG(ARKWEB_TEST)
#define private public
#endif
 private:
  // Set the current |RenderFrameHost| that should have control over the
  // viewport fit value and we should set safe area insets on.
  void SetCurrentRenderFrameHost(RenderFrameHost* rfh,
                                 blink::mojom::ViewportFit value);

  // Send the safe area insets to a |RenderFrameHost|.
  void SendSafeAreaToFrame(RenderFrameHost* rfh, gfx::Insets insets);

  // Stores the current safe area insets.
  gfx::Insets insets_;

  // Stores the current |RenderFrameHost| that has the applied safe area insets
  // and is controlling the viewport fit value. This value is different than
  // `WebContentsImpl::current_fullscreen_frame_` because it also considers
  // browser side driven fullscreen mode, not just renderer side requested
  // frames.
  base::WeakPtr<RenderFrameHostImpl> current_rfh_;

  // Holds WebContents associated mojo receivers.
  RenderFrameHostReceiverSet<blink::mojom::DisplayCutoutHost> receivers_;

  // Weak pointer to the owning |WebContentsImpl| instance.
  raw_ptr<WebContentsImpl> web_contents_impl_;
  blink::mojom::ViewportFit mainFrameViewportFit_ = blink::mojom::ViewportFit::kAuto;
#if BUILDFLAG(ARKWEB_TEST)
#undef private
#endif
};

}  // namespace content

#endif  // CONTENT_BROWSER_DISPLAY_CUTOUT_DISPLAY_CUTOUT_HOST_OHOS_H_
