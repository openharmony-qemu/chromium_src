// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ARKWEB_WEB_CONTENTS_VIEW_OSR_EXT_H_
#define ARKWEB_WEB_CONTENTS_VIEW_OSR_EXT_H_

#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "cef/ohos_cef_ext/libcef/browser/osr/arkweb_render_widget_host_view_osr_ext.h"
#include "content/browser/renderer_host/render_view_host_delegate_view.h"
#include "content/browser/web_contents/web_contents_view.h"
#include "third_party/skia/include/core/SkColor.h"
#include "cef/libcef/browser/osr/web_contents_view_osr.h"

// An implementation of CefWebContentsViewOSR for off-screen rendering.
class ArkWebCefWebContentsViewOSRExt : public CefWebContentsViewOSR {
 public:
  explicit ArkWebCefWebContentsViewOSRExt(SkColor background_color,
                                 bool use_shared_texture,
                                 bool use_external_begin_frame);

  ArkWebCefWebContentsViewOSRExt(const ArkWebCefWebContentsViewOSRExt&) = delete;
  ArkWebCefWebContentsViewOSRExt& operator=(const ArkWebCefWebContentsViewOSRExt&) = delete;

  ~ArkWebCefWebContentsViewOSRExt() override;

  ArkWebCefWebContentsViewOSRExt *AsArkWebCefWebContentsViewOSRExt() override { return this; }

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
  void WebContentsMaybeCreatePageNode();
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  void SetTextHandlesTemporarilyHiddenByDrag();
#endif

#if BUILDFLAG(ARKWEB_AI)
  void CloseImageOverlaySelection() override;
  void OnOverlayZoomChanged() override;
  void OnOverlayStateChanged(const gfx::Rect& image_rect) override;
#endif  // BUILDFLAG(ARKWEB_AI)

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  gfx::Rect GetVisibleRectToWeb() override;
#endif

#if BUILDFLAG(ARKWEB_AI)
  void CreateOverlay(const gfx::ImageSkia& image,
                     const gfx::Rect& image_rect,
                     const gfx::Point& touch_point) override;
#endif

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  void OnSafeInsetsChange(const gfx::Insets& safe_insets) override;
#endif
#if BUILDFLAG(ARKWEB_MENU)
  void MouseSelectMenuShow(bool show) override;
  void ChangeVisibilityOfQuickMenu() override;
  bool IsQuickMenuShow() override;
#endif

#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
  virtual content::WebContents* GetWebContents() override;
  void DidStopRefresh() override;
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  int GetTopControlsHeight() override;
  bool DoBrowserControlsShrinkRendererSize() const override;
  void UpdateBrowserControlsHeight(int height, bool animate) override;
#endif

#if BUILDFLAG(USE_EXTERNAL_POPUP_MENU)
  void ShowPopupMenu(
      content::RenderFrameHost* render_frame_host,
      mojo::PendingRemote<blink::mojom::PopupMenuClient> popup_client,
      const gfx::Rect& bounds,
      int item_height,
      double item_font_size,
      int selected_item,
      std::vector<blink::mojom::MenuItemPtr> menu_items,
      bool right_aligned,
      bool allow_multiple_selection) override;
#endif
};

#endif  // ARKWEB_WEB_CONTENTS_VIEW_OSR_EXT_H_
