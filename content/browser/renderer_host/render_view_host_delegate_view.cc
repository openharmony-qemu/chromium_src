// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/renderer_host/render_view_host_delegate_view.h"

#include "build/build_config.h"

namespace content {

#if BUILDFLAG(IS_ANDROID)
ui::OverscrollRefreshHandler*
content::RenderViewHostDelegateView::GetOverscrollRefreshHandler() const {
  return nullptr;
}
#endif

#ifdef OHOS_EX_TOPCONTROLS
int RenderViewHostDelegateView::GetTopControlsHeight() {
#else
int RenderViewHostDelegateView::GetTopControlsHeight() const {
#endif
  return 0;
}

int RenderViewHostDelegateView::GetTopControlsMinHeight() const {
  return 0;
}

int RenderViewHostDelegateView::GetBottomControlsHeight() const {
  return 0;
}

int RenderViewHostDelegateView::GetBottomControlsMinHeight() const {
  return 0;
}

bool RenderViewHostDelegateView::ShouldAnimateBrowserControlsHeightChanges()
    const {
  return false;
}

bool RenderViewHostDelegateView::DoBrowserControlsShrinkRendererSize() const {
  return false;
}

bool RenderViewHostDelegateView::OnlyExpandTopControlsAtPageTop() const {
  return false;
}

void RenderViewHostDelegateView::GestureEventAck(
    const blink::WebGestureEvent& event,
    blink::mojom::InputEventResultState ack_result) {}

#ifdef OHOS_DISPLAY_CUTOUT
void RenderViewHostDelegateView::OnSafeInsetsChange(int left,
                                                    int top,
                                                    int right,
                                                    int bottom) {}
#endif

}  //  namespace content
