// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cef/libcef/browser/osr/web_contents_view_osr.h"
#include "cef/ohos_cef_ext/libcef/browser/osr/arkweb_web_contents_view_osr_ext.h"

#include "arkweb/build/features/features.h"
#include "cef/libcef/browser/alloy/alloy_browser_host_impl.h"
#include "cef/libcef/browser/osr/render_widget_host_view_osr.h"
#include "cef/libcef/browser/osr/touch_selection_controller_client_osr.h"
#include "cef/libcef/common/drag_data_impl.h"
#include "cef/ohos_cef_ext/libcef/browser/osr/arkweb_render_widget_host_view_osr_ext.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/render_widget_host.h"
#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
#include "components/performance_manager/embedder/performance_manager_registry.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
#include "base/command_line.h"
#include "cef/ohos_cef_ext/include/arkweb_client_ext.h"
#include "content/public/common/content_switches.h"
#endif

#include "cef/ohos_cef_ext/libcef/browser/alloy/alloy_browser_host_impl_ext.h"

ArkWebCefWebContentsViewOSRExt::ArkWebCefWebContentsViewOSRExt(
    SkColor background_color,
    bool use_shared_texture,
    bool use_external_begin_frame)
    :CefWebContentsViewOSR(
        background_color,
        use_shared_texture,
        use_external_begin_frame) {}

ArkWebCefWebContentsViewOSRExt::~ArkWebCefWebContentsViewOSRExt() = default;

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
void ArkWebCefWebContentsViewOSRExt::WebContentsMaybeCreatePageNode()
{
  LOG(INFO) << "CefWebContentsViewOSR::WebContentsCreated";
  if (auto* registry =
          performance_manager::PerformanceManagerRegistry::GetInstance()) {
    registry->MaybeCreatePageNodeForWebContents(web_contents_);
  }
}
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
void ArkWebCefWebContentsViewOSRExt::SetTextHandlesTemporarilyHiddenByDrag()
{
    CefRenderWidgetHostViewOSR* view = GetView();
    if (view) {
      view->AsArkWebRenderWidgetHostViewOSRExt()
          ->SetTextHandlesTemporarilyHiddenByDrag(true, true);
    }
}
#endif

#if BUILDFLAG(ARKWEB_AI)
void ArkWebCefWebContentsViewOSRExt::CloseImageOverlaySelection()
{
  auto* rwhv = GetView();
  if (rwhv) {
    rwhv->AsArkWebRenderWidgetHostViewOSRExt()->CloseImageOverlaySelection();
  }
}

void ArkWebCefWebContentsViewOSRExt::OnOverlayStateChanged(const gfx::Rect& image_rect)
{
  auto* rwhv = GetView();
  if (rwhv) {
    rwhv->AsArkWebRenderWidgetHostViewOSRExt()->OnOverlayStateChanged(image_rect);
  }
}

void ArkWebCefWebContentsViewOSRExt::OnOverlayZoomChanged()
{
  auto* rwhv = GetView();
  if (rwhv) {
    rwhv->AsArkWebRenderWidgetHostViewOSRExt()->CloseImageOverlaySelection();
  }
}
#endif  // BUILDFLAG(ARKWEB_AI)

#if BUILDFLAG(ARKWEB_DRAG_DROP)
gfx::Rect ArkWebCefWebContentsViewOSRExt::GetVisibleRectToWeb()
{
  CefRefPtr<AlloyBrowserHostImpl> browser = GetBrowser();
  if (browser.get()) {
    return browser->GetVisibleRectToWeb();
  }
  return gfx::Rect();
}
#endif

#if BUILDFLAG(ARKWEB_AI)
void ArkWebCefWebContentsViewOSRExt::CreateOverlay(const gfx::ImageSkia& image,
                                                   const gfx::Rect& image_rect,
                                                   const gfx::Point& touch_point)
{
  ArkWebRenderWidgetHostViewOSRExt* view = GetView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->CreateOverlay(image, image_rect,
                                                              touch_point);
  }
}
#endif

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
void ArkWebCefWebContentsViewOSRExt::OnSafeInsetsChange(const gfx::Insets& safe_insets)
{
  if (web_contents_) {
    content::WebContentsImpl* web_contents_impl =
        static_cast<content::WebContentsImpl*>(web_contents_);
    if (web_contents_impl) {
      web_contents_impl->SetDisplayCutoutSafeArea(safe_insets);
    }
  }
}
#endif
#if BUILDFLAG(ARKWEB_MENU)
void ArkWebCefWebContentsViewOSRExt::MouseSelectMenuShow(bool show)
{
  auto* rwhv = GetView();
  if (rwhv) {
    rwhv->MouseSelectMenuShow(show);
  }
}

void ArkWebCefWebContentsViewOSRExt::ChangeVisibilityOfQuickMenu()
{
  auto* rwhv = GetView();
  if (rwhv) {
    rwhv->ChangeVisibilityOfQuickMenu();
  }
}

bool ArkWebCefWebContentsViewOSRExt::IsQuickMenuShow()
{
  auto* rwhv = GetView();
  if (rwhv) {
    return rwhv->IsQuickMenuShow();
  }
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
void ArkWebCefWebContentsViewOSRExt::DidStopRefresh()
{
  if (ArkWebRenderWidgetHostViewOSRExt* view = GetView()) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->DidStopRefresh();
  }
}

content::WebContents* ArkWebCefWebContentsViewOSRExt::GetWebContents()
{
  return web_contents_;
}
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
int ArkWebCefWebContentsViewOSRExt::GetTopControlsHeight()
{
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kEnableNwebExTopControls)) {
    return 0;
  }

  int top_controls_height = top_controls_height_;

  CefRefPtr<AlloyBrowserHostImpl> browser = GetBrowser();
  if (browser.get() && browser->GetClient()) {
    top_controls_height =
        browser->GetClient()->AsArkWebClient()->OnGetTopControlsHeight();
  }
  if (top_controls_height != top_controls_height_) {
    LOG(INFO) << "browser controls height changed:" << top_controls_height_
              << "->" << top_controls_height;
    top_controls_height_ = top_controls_height;
    if (CefRenderWidgetHostViewOSR* view = GetView()) {
      view->AsArkWebRenderWidgetHostViewOSRExt()->OnTopControlsHeightChanged();
    }
  }

  return top_controls_height_;
}

bool ArkWebCefWebContentsViewOSRExt::DoBrowserControlsShrinkRendererSize() const
{
  CefRefPtr<AlloyBrowserHostImpl> browser = GetBrowser();
  if (browser.get() && browser->GetClient()) {
    return browser->GetClient()
        ->AsArkWebClient()
        ->DoBrowserControlsShrinkRendererSize();
  }
  return false;
}

void ArkWebCefWebContentsViewOSRExt::UpdateBrowserControlsHeight(int height,
                                                                 bool animate)
{
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kEnableNwebExTopControls) ||
      height == top_controls_height_) {
    return;
  }

  LOG(INFO) << "browser controls height changed:" << top_controls_height_
            << "->" << height;
  top_controls_height_ = height;

  if (CefRenderWidgetHostViewOSR* view = GetView()) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->OnTopControlsHeightChanged();
    view->SynchronizeVisualProperties(cc::DeadlinePolicy::UseDefaultDeadline(),
                                      absl::nullopt);
  }
}
#endif  // BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)

#if BUILDFLAG(USE_EXTERNAL_POPUP_MENU)
void ArkWebCefWebContentsViewOSRExt::ShowPopupMenu(
    content::RenderFrameHost* render_frame_host,
    mojo::PendingRemote<blink::mojom::PopupMenuClient> popup_client,
    const gfx::Rect& bounds,
    int item_height,
    double item_font_size,
    int selected_item,
    std::vector<blink::mojom::MenuItemPtr> menu_items,
    bool right_aligned,
    bool allow_multiple_selection)
{
  CefRefPtr<AlloyBrowserHostImpl> browser = GetBrowser();
#if BUILDFLAG(ARKWEB_HTML_SELECT)
  if (browser.get()) {
    browser->AsAlloyBrowserHostImplExt()->ShowPopupMenu(std::move(popup_client), bounds, item_height,
                                                        item_font_size, selected_item, std::move(menu_items),
                                                        right_aligned, allow_multiple_selection);
  }
#endif  // #if BUILDFLAG(ARKWEB_HTML_SELECT)
}
#endif
