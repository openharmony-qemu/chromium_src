// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/display_cutout/display_cutout_host_ohos.h"

#include "base/ohos/sys_info_utils.h"
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
  content::RenderFrameHost* rfh = receivers_.GetCurrentTargetFrame();
  if (!rfh->IsInPrimaryMainFrame()) {
    return;
  }
  mainFrameViewportFit_ = value;
  SetCurrentRenderFrameHost(rfh, value);
}

void DisplayCutoutHostOhos::DidAcquireFullscreen(RenderFrameHost* rfh) {}

void DisplayCutoutHostOhos::DidExitFullscreen() {}

void DisplayCutoutHostOhos::DidFinishNavigation(NavigationHandle* navigation_handle) {}

void DisplayCutoutHostOhos::RenderFrameDeleted(RenderFrameHost* rfh) {}

void DisplayCutoutHostOhos::RenderFrameCreated(RenderFrameHost* rfh) {}

void DisplayCutoutHostOhos::SetDisplayCutoutSafeArea(gfx::Insets insets) {
  if (insets == insets_) {
    return;
  }

  LOG(INFO) << __func__ << " " << insets_.ToString() << "->"
            << insets.ToString() << " rfh:" << current_rfh_.get();

  insets_ = insets;

  if (current_rfh_) {
    if (mainFrameViewportFit_ == blink::mojom::ViewportFit::kCover) {
      SendSafeAreaToFrame(current_rfh_.get(), insets);
    }
  }
}

void DisplayCutoutHostOhos::SetCurrentRenderFrameHost(
    RenderFrameHost* rfh,
    blink::mojom::ViewportFit value) {
  if (!rfh) {
    return;
  }

  web_contents_impl_->NotifyViewportFitChanged(value);
  if (value == blink::mojom::ViewportFit::kCover) {
    // Update the |current_rfh_| with the new frame.
    current_rfh_ = static_cast<RenderFrameHostImpl*>(rfh)->GetWeakPtr();
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
