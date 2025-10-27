/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
#include "cef/libcef/browser/osr/browser_platform_delegate_osr.h"

#include <utility>

#include "arkweb/build/features/features.h"
#include "base/task/current_thread.h"
#include "cef/libcef/browser/image_impl.h"
#include "cef/libcef/browser/osr/osr_accessibility_util.h"
#include "cef/libcef/browser/osr/render_widget_host_view_osr.h"
#include "cef/libcef/browser/osr/touch_selection_controller_client_osr.h"
#include "cef/libcef/browser/osr/web_contents_view_osr.h"
#include "cef/libcef/browser/views/view_util.h"
#include "cef/libcef/common/drag_data_impl.h"
#include "components/input/render_widget_host_input_event_router.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/render_view_host.h"
#include "ui/display/screen.h"
#include "ui/events/base_event_utils.h"

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#include "cef/ohos_cef_ext/include/arkweb_dialog_handler_ext.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
#include "cef/ohos_cef_ext/include/arkweb_render_handler_ext.h"
#include "cef/ohos_cef_ext/libcef/browser/osr/arkweb_render_widget_host_view_osr_ext.h"
#include "cef/ohos_cef_ext/libcef/browser/osr/arkweb_web_contents_view_osr_ext.h"
#endif

#include "cef/ohos_cef_ext/libcef/browser/osr/browser_platform_delegate_osr_ext.h"
#include "cef/ohos_cef_ext/libcef/browser/osr/browser_platform_delegate_osr_utils.h"


void CefBrowserPlatformDelegateOsrUtils::InitializeAndUpdateRenderView(){
    CefRenderWidgetHostViewOSR* view = cefBrowserPlatformDelegateOsr->GetOSRHostView();
    #if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
    if (view) {
        view->AsArkWebRenderWidgetHostViewOSRExt()->OnSafeInsetsChange(
            cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->safe_insets_);
    }
    #endif

    #if BUILDFLAG(ARKWEB_FOCUS)
    if (cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()
            ->is_view_focus_failed_ && view) {
        LOG(INFO) << "RenderViewCreated request focus after failed:"
                << cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->focus_status_;
        view->SetFocus(cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->focus_status_);
        cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->is_view_focus_failed_ = false;
    }
    #endif  // BUILDFLAG(ARKWEB_FOCUS)
    #if BUILDFLAG(ARKWEB_AI)
    if (view) {
        view->AsArkWebRenderWidgetHostViewOSRExt()->OnFoldStatusChanged(
            cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->fold_status_);
    }
    #endif  // BUILDFLAG(ARKWEB_AI)
    #if BUILDFLAG(ARKWEB_SYNC_RENDER)
    cefBrowserPlatformDelegateOsr->SetDrawMode(
        cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->drawMode_);
    #endif
    #if BUILDFLAG(ARKWEB_SAME_LAYER)
    cefBrowserPlatformDelegateOsr->SetNativeInnerWeb(
        cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->isInnerWeb_);
    #endif
}

void CefBrowserPlatformDelegateOsrUtils::RedistributeKeyEventIfUrlEmpty(const CefKeyEvent& event){
    // If the url content is empty when loadurl is loaded,
    // the key information is thrown to other components.
    CefRefPtr<CefKeyboardHandler> handler;
    if (cefBrowserPlatformDelegateOsr->browser_ && 
            cefBrowserPlatformDelegateOsr->browser_->client()) {
      handler = cefBrowserPlatformDelegateOsr->browser_
                    ->client()->GetKeyboardHandler();
    }
    if (handler) {
      handler->KeyboardReDispatch(event, false);
    }
}

void CefBrowserPlatformDelegateOsrUtils::CancelTouchpadFlingOnMouseClick(
    CefRenderWidgetHostViewOSR* view, const CefMouseEvent& event){
    blink::WebGestureEvent fling_cancel =
        cefBrowserPlatformDelegateOsr->native_delegate_->TranslateTouchpadFlingEvent(event);
    fling_cancel.data.fling_start.target_viewport = false;
    fling_cancel.SetType(blink::WebInputEvent::Type::kGestureFlingCancel);
    view->AsArkWebRenderWidgetHostViewOSRExt()->SendTouchpadFlingEvent(
        fling_cancel);
}

void CefBrowserPlatformDelegateOsrUtils::AdjustMouseEventCoordinates(
    CefRenderWidgetHostViewOSR* view,
    const CefMouseEvent& mouseEvent,
    blink::WebMouseEvent& event) {
    if (view->IsPointerLocked()) {
      event.SetPositionInScreen(
          {mouseEvent.x + mouseEvent.raw_x, mouseEvent.y + mouseEvent.raw_y});
      event.movement_x = mouseEvent.raw_x;
      event.movement_y = mouseEvent.raw_y;
      event.is_raw_movement_event = true;
    }
}

void CefBrowserPlatformDelegateOsrUtils::CancelTouchpadFlingMouseWheel(
    CefRenderWidgetHostViewOSR* view, const CefMouseEvent& event) {
    content::WebContentsImpl* web_contents =
        static_cast<content::WebContentsImpl*>(cefBrowserPlatformDelegateOsr->web_contents_);
    if (!web_contents) {
        return;
    }
    auto* frame = web_contents->GetFocusedFrame();
    if (!frame || !frame->GetRenderWidgetHost()) {
        LOG(ERROR) << "get focused rwh failed";
        return;
    }
    if (frame->GetRenderWidgetHost()->IsAutoscrollInProgress()) {
        LOG(DEBUG) << "When the current frame is auto scroll, kGestureFlingCancel event not send";
        return;
    }
    blink::WebGestureEvent fling_cancel =
        cefBrowserPlatformDelegateOsr->native_delegate_->TranslateTouchpadFlingEvent(event);
    fling_cancel.data.fling_start.target_viewport = false;
    fling_cancel.SetType(blink::WebInputEvent::Type::kGestureFlingCancel);
    view->AsArkWebRenderWidgetHostViewOSRExt()->SendTouchpadFlingEvent(
        fling_cancel);
}

void CefBrowserPlatformDelegateOsrUtils::AdjustAndSendTouchEvent(
    CefRenderWidgetHostViewOSR* view, const CefTouchEvent& event){
    if (!view) {
        return;
    }

    CefTouchEvent event_adjust = event;
    #if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
    if (event.type == CEF_TET_PRESSED) {
        cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->shrink_viewport_height_ = view->GetShrinkViewportHeight();
    } else if (event.type == CEF_TET_CANCELLED) {
        cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->shrink_viewport_height_ = 0;
    }
    event_adjust.y -= cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->shrink_viewport_height_;
    if (event.type == CEF_TET_RELEASED) {
        cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->shrink_viewport_height_ = 0;
    }
    #endif

    #if BUILDFLAG(ARKWEB_SAME_LAYER)
    if (view->AsArkWebRenderWidgetHostViewOSRExt()) {
        view->AsArkWebRenderWidgetHostViewOSRExt()->SetNativeEmbedMode(
            cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->native_embed_mode_);
        view->AsArkWebRenderWidgetHostViewOSRExt()->SetEnableCustomVideoPlayer(
            cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->custom_video_player_enable_);
    }
    #endif

    view->SendTouchEvent(event_adjust);

    #if BUILDFLAG(ARKWEB_INPUT_EVENTS)
    if (event.type == CEF_TET_PRESSED) {
        cefBrowserPlatformDelegateOsr->SendTouchEventToRender(event_adjust);
    }
    #endif
}

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void CefBrowserPlatformDelegateOsrUtils::UpdateNativeEmbedMode(CefRenderWidgetHostViewOSR* view) {
    if (view->AsArkWebRenderWidgetHostViewOSRExt()) {
        view->AsArkWebRenderWidgetHostViewOSRExt()->SetNativeEmbedMode(
            cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->native_embed_mode_);
    }
}

void CefBrowserPlatformDelegateOsrUtils::SetEnableCustomVideoPlayer(CefRenderWidgetHostViewOSR* view) {
    if (view->AsArkWebRenderWidgetHostViewOSRExt()) {
        view->AsArkWebRenderWidgetHostViewOSRExt()->SetEnableCustomVideoPlayer(
            cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->custom_video_player_enable_);
    }
}
#endif

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void CefBrowserPlatformDelegateOsrUtils::UpdateBypassVsyncCondition(CefRenderWidgetHostViewOSR* view) {
    if (view->AsArkWebRenderWidgetHostViewOSRExt()) {
        view->AsArkWebRenderWidgetHostViewOSRExt()->SetBypassVsyncCondition(
            cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->condition_);
    }
}
#endif

void CefBrowserPlatformDelegateOsrUtils::SetFocusAndUpdateStatus(
    bool setFocus, CefRenderWidgetHostViewOSR* view){
  cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->focus_status_ = setFocus;
  if (view) {
    view->SetFocus(setFocus);
    cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->is_view_focus_failed_ = false;
  } else {
    LOG(WARNING) << "CefBrowserPlatformDelegateOsrUtils::SetFocus failed.";
    cefBrowserPlatformDelegateOsr->AsCefBrowserPlatformDelegateOsrExt()->is_view_focus_failed_ = true;
  }
}

CefImageImpl* CefBrowserPlatformDelegateOsrUtils::CreateDragImage(const gfx::ImageSkia& image){
    CefImageImpl* image_impl = nullptr;
    if (image.size().IsEmpty()) {
      // An empty drag image is possible if the Javascript sets an empty drag
      // image on purpose. Create a dummy 1x1 pixel image to avoid crashes when
      // converting to cef image.
      LOG(INFO) << "drag image is empty";
      image_impl = new (std::nothrow) CefImageImpl();
      if (image_impl) {
        SkBitmap dummy_bitmap;
        dummy_bitmap.allocN32Pixels(1, 1);
        dummy_bitmap.eraseColor(0);
        image_impl->AddBitmap(1.0, dummy_bitmap);
      }
    } else {
      image_impl = new (std::nothrow) CefImageImpl(image);
    }
    return image_impl;
}

void CefBrowserPlatformDelegateOsrUtils::RestoreTextHandlesAfterDrag(){
  CefRenderWidgetHostViewOSR* view = cefBrowserPlatformDelegateOsr->GetOSRHostView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()
        ->SetTextHandlesTemporarilyHiddenByDrag(false, false);
  }
}