// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/display_cutout/display_cutout_host_ohos.h"

#include "content/browser/display_cutout/display_cutout_constants.h"
#include "content/browser/renderer_host/render_frame_host_impl.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/navigation_handle.h"
#include "mojo/public/cpp/bindings/associated_remote.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_provider.h"

namespace content {

DisplayCutoutHostOhos::DisplayCutoutHostOhos(WebContentsImpl* web_contents)
    : receivers_(web_contents, this), web_contents_impl_(web_contents) {}

DisplayCutoutHostOhos::~DisplayCutoutHostOhos() = default;

void DisplayCutoutHostOhos::BindReceiver(
    mojo::PendingAssociatedReceiver<blink::mojom::DisplayCutoutHost> receiver,
    RenderFrameHost* rfh) {
  receivers_.Bind(rfh, std::move(receiver));
}

void DisplayCutoutHostOhos::NotifyViewportFitChanged(
    blink::mojom::ViewportFit value) {
  ViewportFitChangedForFrame(receivers_.GetCurrentTargetFrame(), value);
  if (current_rfh_) {
    SendSafeAreaToFrame(current_rfh_.get(), insets_);
  }
}

void DisplayCutoutHostOhos::ViewportFitChangedForFrame(
    RenderFrameHost* rfh,
    blink::mojom::ViewportFit value) {
  current_viewport_fit_ = value;
  web_contents_impl_->NotifyViewportFitChanged(value);
}

void DisplayCutoutHostOhos::DidAcquireFullscreen(RenderFrameHost* rfh) {}

void DisplayCutoutHostOhos::DidExitFullscreen() {}

void DisplayCutoutHostOhos::DidFinishNavigation(
    NavigationHandle* navigation_handle) {
  if (!navigation_handle->IsInPrimaryMainFrame() ||
      navigation_handle->IsSameDocument()) {
    return;
  }
  SetCurrentRenderFrameHost(web_contents_impl_->GetPrimaryMainFrame());
}

void DisplayCutoutHostOhos::RenderFrameDeleted(RenderFrameHost* rfh) {}

void DisplayCutoutHostOhos::RenderFrameCreated(RenderFrameHost* rfh) {
  ViewportFitChangedForFrame(rfh, blink::mojom::ViewportFit::kAuto);
}

void DisplayCutoutHostOhos::SetDisplayCutoutSafeArea(gfx::Insets insets) {
  LOG(INFO) << __func__ << " " << insets_.ToString() << "->"
            << insets.ToString();
  insets_ = insets;

  if (current_rfh_) {
    SendSafeAreaToFrame(current_rfh_.get(), insets);
  }
}

void DisplayCutoutHostOhos::SetCurrentRenderFrameHost(RenderFrameHost* rfh) {
  if (!rfh) {
    return;
  }

  // Update the |current_rfh_| with the new frame.
  current_rfh_ = static_cast<RenderFrameHostImpl*>(rfh)->GetWeakPtr();

  if (current_viewport_fit_ == blink::mojom::ViewportFit::kCover) {
    // Send the current safe area to the new frame.
    SendSafeAreaToFrame(rfh, insets_);
  } else {
    SendSafeAreaToFrame(rfh, gfx::Insets());
  }
}

void DisplayCutoutHostOhos::SendSafeAreaToFrame(RenderFrameHost* rfh,
                                                gfx::Insets insets) {
  blink::AssociatedInterfaceProvider* provider =
      rfh->GetRemoteAssociatedInterfaces();
  if (!provider) {
    return;
  }

  mojo::AssociatedRemote<blink::mojom::DisplayCutoutClient> client;
  provider->GetInterface(client.BindNewEndpointAndPassReceiver());
  client->SetSafeArea(insets);
}

}  // namespace content
