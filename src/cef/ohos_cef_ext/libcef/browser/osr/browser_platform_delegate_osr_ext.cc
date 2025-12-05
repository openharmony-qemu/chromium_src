/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
#include "cef/libcef/browser/osr/browser_platform_delegate_osr.h"

#include <utility>

#include "arkweb/build/features/features.h"
#include "base/task/current_thread.h"
#include "base/hash/hash.h"
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
#include "cef/ohos_cef_ext/libcef/browser/osr/browser_platform_delegate_osr_ext.h"
#include "cef/ohos_cef_ext/libcef/browser/osr/browser_platform_delegate_osr_utils.h"
#endif

#if BUILDFLAG(ARKWEB_PIP)
#include "content/browser/media/media_web_contents_observer.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"

namespace {
const base::TimeDelta kPictureInPictureDelta = base::Seconds(15);
const base::TimeDelta kPictureInPicturePauseDelta = base::Milliseconds(10);
}
#endif

#if BUILDFLAG(ARKWEB_HTML_SELECT)
namespace {
void ConvertSelectPopupItem(const blink::mojom::MenuItemPtr& menu_ptr,
                            CefSelectPopupItem& menu_item)
{
  CefString label = CefString(menu_ptr->label.value_or(""));
  CefString tool_tip = CefString(menu_ptr->tool_tip.value_or(""));
  cef_string_set(label.c_str(), label.length(), &(menu_item.label), true);
  cef_string_set(tool_tip.c_str(), tool_tip.length(), &(menu_item.tool_tip),
                 true);
  menu_item.action = menu_ptr->action;
  menu_item.enabled = menu_ptr->enabled;
  menu_item.checked = menu_ptr->checked;
  menu_item.type = static_cast<cef_select_popup_item_type_t>(menu_ptr->type);
  menu_item.text_direction =
      static_cast<cef_text_direction_t>(menu_ptr->text_direction);
  menu_item.has_text_direction_override = menu_ptr->has_text_direction_override;
}

void ConvertAutofillPopupItem(const autofill::Suggestion& suggestion,
                              CefAutofillPopupItem& menu_item)
{
  CefString label = CefString(base::UTF16ToUTF8(suggestion.main_text.value));
  // TODO:In src/components/autofill/core/browser/suggestions/suggestion.h, 
  // minor_text has been changed to minor_texts, 
  // and the corresponding implementations need to be updated.
  // CefString sublabel = CefString(suggestion.minor_text.value);
  cef_string_set(label.c_str(), label.length(), &(menu_item.label), true);
  // cef_string_set(sublabel.c_str(), sublabel.length(), &(menu_item.sublabel),
  //                true);
}
}  // namespace

class CefSelectPopupCallbackImpl : public CefSelectPopupCallback {
 public:
  explicit CefSelectPopupCallbackImpl(
      mojo::PendingRemote<blink::mojom::PopupMenuClient> popup_client)
  {
    popup_client_.Bind(std::move(popup_client));
  }

  void Continue(const std::vector<int>& indices) override
  {
    if (popup_client_) {
      popup_client_->DidAcceptIndices(indices);
    }
  }

  void Cancel() override
  {
    if (popup_client_) {
      popup_client_->DidCancel();
    }
  }

 private:
  mojo::Remote<blink::mojom::PopupMenuClient> popup_client_;
  IMPLEMENT_REFCOUNTING(CefSelectPopupCallbackImpl);
};
#endif  // #if BUILDFLAG(ARKWEB_HTML_SELECT)


CefBrowserPlatformDelegateOsrExt::CefBrowserPlatformDelegateOsrExt(
    std::unique_ptr<CefBrowserPlatformDelegateNative> native_delegate,
    bool use_shared_texture,
    bool use_external_begin_frame)
    : CefBrowserPlatformDelegateOsr(std::move(native_delegate),
                                    use_shared_texture,
                                    use_external_begin_frame) {}

#if BUILDFLAG(ARKWEB_HTML_SELECT)
void CefBrowserPlatformDelegateOsrExt::ShowPopupMenu(
    mojo::PendingRemote<blink::mojom::PopupMenuClient> popup_client,
    const gfx::Rect& bounds,
    double item_font_size,
    int selected_item,
    std::vector<blink::mojom::MenuItemPtr> menu_items,
    bool right_aligned,
    bool allow_multiple_selection)
{
  CefRefPtr<CefDialogHandler> handler =
      browser_->GetClient()->GetDialogHandler();
  if (handler.get()) {
    std::vector<CefSelectPopupItem> item_list;
    for (unsigned long i = 0; i < menu_items.size(); i++) {
      CefSelectPopupItem menu_item;
      ConvertSelectPopupItem(menu_items[i], menu_item);
      item_list.push_back(menu_item);
    }
    CefRefPtr<CefSelectPopupCallback> callback =
        new CefSelectPopupCallbackImpl(std::move(popup_client));
    handler->AsArkDialogHandler()->OnSelectPopupMenu(
        browser_->GetBrowser(),
        CefRect(bounds.x(), bounds.y(), bounds.width(), bounds.height()),
        item_font_size, selected_item, item_list, right_aligned,
        allow_multiple_selection, callback);
  }
}
#endif  // #if BUILDFLAG(ARKWEB_HTML_SELECT)

#if BUILDFLAG(ARKWEB_DATALIST)
void CefBrowserPlatformDelegateOsrExt::OnShowAutofillPopup(
    const gfx::RectF& element_bounds,
    bool is_rtl,
    const std::vector<autofill::Suggestion>& suggestions,
    bool is_password_popup_type)
{
  CefRefPtr<CefDialogHandler> handler =
      browser_->GetClient()->GetDialogHandler();
  if (handler.get()) {
    std::vector<CefAutofillPopupItem> item_list;
    for (auto& item : suggestions) {
      CefAutofillPopupItem menu_item;
      ConvertAutofillPopupItem(item, menu_item);
      item_list.push_back(menu_item);
    }

    float shrink_viewport_height = 0;
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
    if (GetOSRHostView()) {
      shrink_viewport_height = GetOSRHostView()->GetShrinkViewportHeight();
    }
#endif

    LOG(INFO) << "element is screen bounds x:" << element_bounds.x()
              << ", y: " << element_bounds.y()
              << ", element_bounds width: " << element_bounds.width()
              << ", element_bounds height:" << element_bounds.height();
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO) << "element is screen bounds x:" << element_bounds.x()
                       << ", y: " << element_bounds.y()
                       << ", element_bounds width: " << element_bounds.width()
                       << ", element_bounds height:" << element_bounds.height();
#endif

    handler->AsArkDialogHandler()->OnShowAutofillPopup(
        browser_->GetBrowser(),
        CefRect(element_bounds.x(), element_bounds.y() + shrink_viewport_height,
                element_bounds.width(), element_bounds.height()),
        is_rtl, item_list, is_password_popup_type);
  }
}
void CefBrowserPlatformDelegateOsrExt::OnHideAutofillPopup()
{
  CefRefPtr<CefDialogHandler> handler =
      browser_->GetClient()->GetDialogHandler();
  if (handler.get()) {
    handler->AsArkDialogHandler()->OnHideAutofillPopup();
  }
}
#endif

#if BUILDFLAG(ARKWEB_TOUCHPAD_FLING)
void CefBrowserPlatformDelegateOsrExt::SendTouchpadFlingEvent(
    const CefMouseEvent& event,
    double vx,
    double vy)
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (!view) {
    return;
  }

  cef_browser_platform_delegate_osr_utils_->CancelTouchpadFlingMouseWheel(view, event);

  blink::WebGestureEvent fling_start =
      native_delegate_->TranslateTouchpadFlingEvent(event);
  fling_start.SetType(blink::WebInputEvent::Type::kGestureFlingStart);
  fling_start.data.fling_start.velocity_x = vx;
  fling_start.data.fling_start.velocity_y = vy;
  fling_start.data.fling_start.target_viewport = false;
  fling_start.SetSourceDevice(blink::mojom::GestureDevice::kTouchpad);

  view->AsArkWebRenderWidgetHostViewOSRExt()->SendTouchpadFlingEvent(
      fling_start);
}
#endif
#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
void CefBrowserPlatformDelegateOsrExt::NotifyScreenInfoChangedV2()
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->OnScreenInfoChangedV2();
  }
}
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void CefBrowserPlatformDelegateOsrExt::AdvanceFocusForIME(int focusType)
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->AdvanceFocusForIME(focusType);
  }
}

void CefBrowserPlatformDelegateOsrExt::SendTouchEventList(
    const std::vector<CefTouchEvent>& event_list)
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (!view) {
    return;
  }

  std::vector<CefTouchEvent> event_adjust_list = event_list;

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  for (auto& event_adjust : event_adjust_list) {
    if (event_adjust.type == CEF_TET_PRESSED) {
      shrink_viewport_height_ = view->GetShrinkViewportHeight();
    } else if (event_adjust.type == CEF_TET_CANCELLED) {
      shrink_viewport_height_ = 0;
    }
    event_adjust.y -= shrink_viewport_height_;
    if (event_adjust.type == CEF_TET_RELEASED) {
      shrink_viewport_height_ = 0;
    }
  }
#endif

#if BUILDFLAG(ARKWEB_FIT_CONTENT)
  view->AsArkWebRenderWidgetHostViewOSRExt()->SetFitContentMode(
      fit_content_mode_);
#endif
  view->AsArkWebRenderWidgetHostViewOSRExt()->SendTouchEventList(
      event_adjust_list);

  for (auto& event_adjust : event_adjust_list) {
    if (event_adjust.type == CEF_TET_PRESSED) {
      SendTouchEventToRender(event_adjust);  //
    }
  }
}

void CefBrowserPlatformDelegateOsrExt::SetScrollable(bool enable)
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view)
    view->AsArkWebRenderWidgetHostViewOSRExt()->SetScrollable(enable);
}

void CefBrowserPlatformDelegateOsrExt::ScrollBy(float delta_x, float delta_y)
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->ScrollBy(delta_x, delta_y);
  }
}

void CefBrowserPlatformDelegateOsrExt::UpdateSecurityLayer(bool isNeedSecurityLayer) {
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->UpdateSecurityLayer(isNeedSecurityLayer);
  }
}

void CefBrowserPlatformDelegateOsrExt::UpdateTextFieldStatus(bool isShowKeyboard, bool isAttachIME) {
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->UpdateTextFieldStatus(isShowKeyboard, isAttachIME);
  }
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void CefBrowserPlatformDelegateOsrExt::SetBypassVsyncCondition(int32_t condition)
{
  condition_ = condition;
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void CefBrowserPlatformDelegateOsrExt::ScrollFocusedEditableNodeIntoView()
{
  content::WebContentsImpl* contents =
      static_cast<content::WebContentsImpl*>(web_contents_);
  if (!contents) {
    LOG(ERROR) << "CefBrowserPlatformDelegateOsrExt::"
                  "ScrollFocusedEditableNodeIntoView web_contents is null";
    return;
  }

  auto* input_handler = contents->GetFocusedFrameWidgetInputHandler();
  if (!input_handler) {
    LOG(ERROR) << "CefBrowserPlatformDelegateOsrExt::"
                  "ScrollFocusedEditableNodeIntoView input_handler is null";
    return;
  }
  input_handler->ScrollFocusedEditableNodeIntoView();
}

void CefBrowserPlatformDelegateOsrExt::ScaleGestureChangeV2(int type,
    float scale, float originScale, float centerX, float centerY)
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->ScaleGestureChangeV2(
        type, scale, originScale, centerX, centerY);
  }
}
#endif
#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
void CefBrowserPlatformDelegateOsrExt::WasOccluded(bool occluded)
{
  // The WebContentsImpl will notify the OSR view.
  content::WebContentsImpl* web_contents =
      static_cast<content::WebContentsImpl*>(web_contents_);
  if (web_contents) {
    if (occluded) {
      web_contents->WasOccluded();
    } else {
      web_contents->WasShown();
    }
  }
}
#endif
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
void CefBrowserPlatformDelegateOsrExt::WasKeyboardResized()
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->WasKeyboardResized();
  }
}

void CefBrowserPlatformDelegateOsrExt::SetDrawMode(int mode)
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  drawMode_ = mode;
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->SetDrawMode(drawMode_);
  }
}

void CefBrowserPlatformDelegateOsrExt::SetFitContentMode(int mode)
{
  fit_content_mode_ = mode;
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->SetFitContentMode(mode);
  }
}

bool CefBrowserPlatformDelegateOsrExt::GetPendingSizeStatus()
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    return view->AsArkWebRenderWidgetHostViewOSRExt()->GetPendingSizeStatus();
  }
  return false;
}

void CefBrowserPlatformDelegateOsrExt::SetDrawRect(int x,
    int y, int width, int height)
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    drawRect_ = gfx::Rect(x, y, width, height);
    LOG(DEBUG) << "CefBrowserPlatformDelegateOsrExt::SetDrawRect, drawRect:"
               << drawRect_.ToString().c_str();
    view->AsArkWebRenderWidgetHostViewOSRExt()->SetDrawRect(drawRect_);
  }
}

void CefBrowserPlatformDelegateOsrExt::SetShouldFrameSubmissionBeforeDraw(
    bool should)
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view)
    view->AsArkWebRenderWidgetHostViewOSRExt()->SetShouldFrameSubmissionBeforeDraw(should);
}

std::string CefBrowserPlatformDelegateOsrExt::GetCurrentLanguage()
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (!view) {
    return "";
  }
  return view->AsArkWebRenderWidgetHostViewOSRExt()->GetCurrentLanguage();
}
#endif
#if BUILDFLAG(ARKWEB_SAME_LAYER)
void CefBrowserPlatformDelegateOsrExt::OnNativeEmbedLifecycleChange(
    const ArkWebRenderHandlerExt::CefNativeEmbedData& info)
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->OnNativeEmbedLifecycleChange(
        info);
  }
}

void CefBrowserPlatformDelegateOsrExt::OnNativeEmbedFirstFramePaint(
    const content::NativeEmbedFirstPaintEvent& event)
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->OnNativeEmbedFirstFramePaint(
        event);
  }
}

void CefBrowserPlatformDelegateOsrExt::SetNativeEmbedMode(bool flag)
{
  native_embed_mode_ = flag;
}

void CefBrowserPlatformDelegateOsrExt::OnNativeEmbedVisibilityChange(
    const std::string& embed_id,
    bool visibility)
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->OnNativeEmbedVisibilityChange(
        embed_id, visibility);
  }
}

void CefBrowserPlatformDelegateOsrExt::SetNativeInnerWeb(bool isInnerWeb) {
  isInnerWeb_ = isInnerWeb;
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->SetNativeInnerWeb(isInnerWeb);
  }
}

void CefBrowserPlatformDelegateOsrExt::SetEnableCustomVideoPlayer(bool flag){
  custom_video_player_enable_ = flag;
}

void CefBrowserPlatformDelegateOsrExt::OnNativeEmbedObjectParamChange(
    const ArkWebRenderHandlerExt::CefNativeParamData& native_param_data)
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->OnNativeEmbedObjectParamChange(
        native_param_data);
  }
}
#endif
#if BUILDFLAG(ARKWEB_AI)
void CefBrowserPlatformDelegateOsrExt::OnTextSelected(bool flag)
{
  if (CefRenderWidgetHostViewOSR* view = GetOSRHostView()) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->OnTextSelected(flag);
  }
}

void CefBrowserPlatformDelegateOsrExt::OnDestroyImageAnalyzerOverlay()
{
  if (CefRenderWidgetHostViewOSR* view = GetOSRHostView()) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->OnDestroyImageAnalyzerOverlay();
  }
}

void CefBrowserPlatformDelegateOsrExt::OnFoldStatusChanged(uint32_t foldStatus)
{
  fold_status_ = foldStatus;
  if (CefRenderWidgetHostViewOSR* view = GetOSRHostView()) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->OnFoldStatusChanged(foldStatus);
  }
}

float CefBrowserPlatformDelegateOsrExt::GetPageScaleFactor()
{
  if (CefRenderWidgetHostViewOSR* view = GetOSRHostView()) {
    return view->AsArkWebRenderWidgetHostViewOSRExt()->GetPageScaleFactor();
  }
  return 1;
}

std::string CefBrowserPlatformDelegateOsrExt::GetDataDetectorSelectText()
{
  if (CefRenderWidgetHostViewOSR* view = GetOSRHostView()) {
    return view->AsArkWebRenderWidgetHostViewOSRExt()->GetDataDetectorSelectText();
  }
  return std::string();
}

void CefBrowserPlatformDelegateOsrExt::OnDataDetectorSelectText()
{
  if (CefRenderWidgetHostViewOSR* view = GetOSRHostView()) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->OnDataDetectorSelectText();
  }
}
#endif
#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
void CefBrowserPlatformDelegateOsrExt::OnSafeInsetsChange(int left,
    int top, int right, int bottom)
{
  gfx::Insets safe_insets = gfx::Insets::TLBR(top, left, bottom, right);
  if (safe_insets_ == safe_insets) {
    return;
  }
  safe_insets_ = safe_insets;

  if (CefRenderWidgetHostViewOSR* view = GetOSRHostView()) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->OnSafeInsetsChange(
        safe_insets_);
  }
}
#endif
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
int CefBrowserPlatformDelegateOsrExt::GetTopControlsOffset()
{
  if (CefRenderWidgetHostViewOSR* view = GetOSRHostView()) {
    return view->GetTopControlsOffset();
  }
  return 0;
}

int CefBrowserPlatformDelegateOsrExt::GetShrinkViewportHeight()
{
  if (CefRenderWidgetHostViewOSR* view = GetOSRHostView()) {
    return view->GetShrinkViewportHeight();
  }
  return 0;
}
#endif
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
bool CefBrowserPlatformDelegateOsrExt::WebPageSnapshot(
    const char* id,
    int width,
    int height,
    cef_web_snapshot_callback_t callback)
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    return view->AsArkWebRenderWidgetHostViewOSRExt()->WebPageSnapshot(
        id, width, height, std::move(callback));
  }
  return false;
}
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void CefBrowserPlatformDelegateOsrExt::SetVirtualKeyBoardArg(int32_t width,
    int32_t height, double keyboard)
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->SetVirtualKeyBoardArg(
        width, height, keyboard);
  }
}

bool CefBrowserPlatformDelegateOsrExt::ShouldVirtualKeyboardOverlay()
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view &&
      view->AsArkWebRenderWidgetHostViewOSRExt()->GetVirtualKeyboardMode() ==
          ui::mojom::VirtualKeyboardMode::kOverlaysContent) {
    return true;
  }
  return false;
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
void CefBrowserPlatformDelegateOsrExt::MaximizeResize()
{
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (view && view->AsArkWebRenderWidgetHostViewOSRExt()) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->MaximizeResize();
  }
}
#endif  // ARKWEB_MAXIMIZE_RESIZE
#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
void CefBrowserPlatformDelegateOsrExt::OnBeforeUnloadFired(bool proceed)
{
  CefRefPtr<CefDialogHandler> handler =
      browser_->GetClient()->GetDialogHandler();
  if (handler.get()) {
    handler->AsArkDialogHandler()->OnBeforeUnloadFired(browser_->GetBrowser(),
                                                       proceed);
  }
}
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
void CefBrowserPlatformDelegateOsrExt::OnShareFile(
    const std::string& file_path,
    const std::string& utd_type_id)
{
  CefRefPtr<CefDialogHandler> handler =
      browser_->GetClient()->GetDialogHandler();
  if (handler.get()) {
    handler->OnShareFile(file_path, utd_type_id);
  }
}
#endif
#if BUILDFLAG(ARKWEB_ADBLOCK)
void CefBrowserPlatformDelegateOsrExt::OnAdsBlocked(
    const std::string& main_frame_url,
    const std::map<std::string, int32_t>& subresource_blocked,
    bool is_site_first_report)
{
  CefRefPtr<CefDialogHandler> handler =
      browser_->GetClient()->GetDialogHandler();

  if (handler.get()) {
    std::map<CefString, CefString> adsBlocked_str;
    for (auto item : subresource_blocked) {
      adsBlocked_str.insert(
          {CefString(item.first), base::NumberToString(item.second)});
    }

    handler->AsArkDialogHandler()->OnAdsBlocked(
        browser_->GetBrowser(), CefString(main_frame_url), adsBlocked_str,
        is_site_first_report);
  }
}

bool CefBrowserPlatformDelegateOsrExt::TrigAdBlockEnabledForSiteFromUi(
    const std::string& main_frame_url,
    int main_frame_tree_node_id)
{
  CefRefPtr<CefDialogHandler> handler =
      browser_->GetClient()->GetDialogHandler();

  if (handler.get()) {
    return handler->AsArkDialogHandler()->TrigAdBlockEnabledForSiteFromUi(
        browser_->GetBrowser(), CefString(main_frame_url),
        main_frame_tree_node_id);
  }

  return false;
}
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

#if BUILDFLAG(ARKWEB_DRAG_DROP)
bool CefBrowserPlatformDelegateOsrExt::GetCurRWH(
    content::WebContentsImpl* web_contents,
    const gfx::PointF& client_pt,
    gfx::PointF* transformed_pt)
{
  auto event_router = web_contents->GetInputEventRouter();
  if (!event_router) {
    LOG(WARNING) << "DragDrop Get event router failed";
    return false;
  }
  auto rvh = web_contents->GetRenderViewHost();
  if (!rvh) {
    LOG(WARNING) << "DragDrop Get render view host failed";
    return false;
  }
  auto rvh_widget = rvh->GetWidget();
  if (!rvh_widget) {
    LOG(WARNING) << "DragDrop Get render widget failed";
    return false;
  }
  auto root_view = rvh_widget->GetView();
  if (!root_view) {
    LOG(WARNING) << "DragDrop Get view failed";
    return false;
  }
  auto* view = event_router->GetRenderWidgetHostViewInputAtPoint(
      root_view, gfx::PointF(client_pt), transformed_pt);
  if (!view) {
    return false;
  }
  auto* current_rwh_for_drag = content::RenderWidgetHostImpl::From(
      static_cast<content::RenderWidgetHostViewBase*>(view)
          ->GetRenderWidgetHost());
  if (!current_rwh_for_drag) {
    LOG(WARNING) << "DragDrop Get render widget host failed ";
    return false;
  }

  current_rwh_for_drag_ = current_rwh_for_drag->GetWeakPtr();
  return true;
}
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)

#if BUILDFLAG(ARKWEB_PIP)
void CefBrowserPlatformDelegateOsrExt::OnPip(int status,
                                          int delegate_id,
                                          int child_id,
                                          int frame_routing_id,
                                          int width,
                                          int height) {
  CefRefPtr<ArkWebDisplayHandlerExt> handler =
      browser_->GetClient()->GetDisplayHandler();
  if (handler.get()) {
    handler->OnPip(browser_->GetBrowser(), status, delegate_id, child_id,
                   frame_routing_id, width, height);
  }
}

void CefBrowserPlatformDelegateOsrExt::OnPipEvent(int event) {
  LOG(INFO) << __func__ << " event:" << event;
  CefRefPtr<CefDialogHandlerExt> handler =
      browser_->GetClient()->GetDialogHandler();
  if (handler.get()) {
    handler->AsArkDialogHandler()->OnPipEvent(browser_->GetBrowser(), event);
  }
}

void CefBrowserPlatformDelegateOsrExt::SetPipNativeWindow(
    int delegate_id,
    int child_id,
    int frame_routing_id,
    cef_native_window_t window) {
  LOG(INFO) << __func__
            << " [hash: "<< std::hex << base::FastHash(base::byte_span_from_ref(window)) << "]"
            << " delegate_id:" << delegate_id << " child:" << child_id
            << " frame_routing_id:" << frame_routing_id;
  if (window == nullptr) {
    LOG(ERROR) << "Pip window is nullptr";
    return;
  }

  content::WebContentsImpl* web_contents_impl =
      static_cast<content::WebContentsImpl*>(web_contents_);

  if (!web_contents_impl) {
    return;
  }
  bool status = false;
  int32_t surface_widget =
      NWebNativeWindowTracker::Get()->AddNativeWindow(window);
  auto it = web_contents_impl->AsWebContentsImplExt()->GetMediaPlayerId(delegate_id,
                                                child_id,
                                                frame_routing_id,
                                                status);
  if (status) {
    LOG(INFO) << "PIC playId:" << it.player_id; 
    auto observer = web_contents_impl->media_web_contents_observer();
    if (web_contents_impl->media_web_contents_observer()) {
      if (observer) {
        observer->GetMediaPlayerRemote(it)->PipEnable(true);
        observer->GetMediaPlayerRemote(it)->SetVideoSurface(surface_widget);
      }
    }
  }

}

void CefBrowserPlatformDelegateOsrExt::SendPipEvent(
    int delegate_id,
    int child_id,
    int frame_routing_id,
    int event) {
  content::WebContentsImpl* web_contents_impl =
      static_cast<content::WebContentsImpl*>(web_contents_);
  LOG(INFO) << "Pip " << __func__ << " " << delegate_id << " " << child_id
            << " " << frame_routing_id << " event:" << event;
  if (!web_contents_impl || !web_contents_impl->AsWebContentsImplExt()) {
    return;
  }
  bool status = false;
  auto it = web_contents_impl->AsWebContentsImplExt()->GetMediaPlayerId(delegate_id,
                                                child_id,
                                                frame_routing_id,
                                                status);
  auto observer = web_contents_impl->media_web_contents_observer();
  if (!status || !observer || !observer->GetMediaPlayerRemote(it)) {
    LOG(ERROR) << "Pip cann't find mediaplayer status or observer is null.";
    return;
  }
  switch(event) {
    case content::PIP_STATE_PAUSE:
      observer->GetMediaPlayerRemote(it)->PipDown(false);
      observer->GetMediaPlayerRemote(it)->RequestPause(false);
      web_contents_impl->AsWebContentsImplExt()->OnPipEvent(event);
      break;
    case content::PIP_STATE_PLAY:
      observer->GetMediaPlayerRemote(it)->PipDown(false);
      observer->GetMediaPlayerRemote(it)->RequestPlay();
      web_contents_impl->AsWebContentsImplExt()->OnPipEvent(event);
      break;
    case content::PIP_STATE_FAST_FORWARD:
      observer->GetMediaPlayerRemote(it)->RequestSeekForward(kPictureInPictureDelta);
      break;
    case content::PIP_STATE_FAST_BACKWARD:
      observer->GetMediaPlayerRemote(it)->RequestSeekBackward(kPictureInPictureDelta);
      break;
    case content::PIP_STATE_EXIT: {
      PipExit(delegate_id, child_id, frame_routing_id, observer, web_contents_impl, it);
      web_contents_impl->AsWebContentsImplExt()->OnPipEvent(event);
      break;
    }
    case content::PIP_STATE_PAGE_CLOSE: {
      observer->GetMediaPlayerRemote(it)->RequestMute(true);
      observer->GetMediaPlayerRemote(it)->RequestExitPictureInPicture();
      observer->GetMediaPlayerRemote(it)->RequestPause(false);
      web_contents_impl->AsWebContentsImplExt()->SetUpdateSurface(false);
      web_contents_impl->AsWebContentsImplExt()->OnPipEvent(content::PIP_STATE_EXIT);
      break;
    }
    case content::PIP_STATE_RESTORE:
      observer->GetMediaPlayerRemote(it)->RequestExitPictureInPicture();
      observer->GetMediaPlayerRemote(it)->PipRequestPlay();
      web_contents_impl->AsWebContentsImplExt()->OnPipEvent(event);
      web_contents_impl->AsWebContentsImplExt()->SetUpdateSurface(false);
      break;
    case content::PIP_STATE_RESIZE:
      observer->GetMediaPlayerRemote(it)->NotifyPipResize();
      break;
    default:
      LOG(INFO) << "Pip other event:" << event;
  }
}

void CefBrowserPlatformDelegateOsrExt::PipExit(
    int delegate_id,
    int child_id,
    int frame_routing_id,
    content::MediaWebContentsObserver* observer,
    content::WebContentsImpl* web_contents_impl,
    content::MediaPlayerId& id) {
  delegate_id_ = delegate_id;
  child_id_ = child_id;
  frame_routing_id_ = frame_routing_id;
  if (!observer || !observer->GetMediaPlayerRemote(id) ||
      !web_contents_impl || !web_contents_impl->AsWebContentsImplExt()) {
    LOG(ERROR) << "observer is null";
    return;
  }
  observer->GetMediaPlayerRemote(id)->RequestExitPictureInPicture();
  if (!web_contents_impl->IsFullscreen() &&
      !web_contents_impl->AsWebContentsImplExt()->IsUpdateSurface()) {
    observer->GetMediaPlayerRemote(id)->RequestPause(false);
  }
  else if ((web_contents_impl->IsFullscreen() ||
       web_contents_impl->AsWebContentsImplExt()->IsUpdateSurface())) {
    observer->GetMediaPlayerRemote(id)->PipRequestPlay();
  }
  else if (!web_contents_impl->IsFullscreen() &&
       web_contents_impl->AsWebContentsImplExt()->IsUpdateSurface()) {
    base::TimeDelta duration = kPictureInPicturePauseDelta;
    if (!pause_timer_) {
       pause_timer_ = std::make_unique<base::OneShotTimer>();
    }
    pause_timer_->Start(FROM_HERE, duration, this,
                        &CefBrowserPlatformDelegateOsrExt::Pause);
  } else {
    LOG(ERROR) << "PipExit error";
  }
  web_contents_impl->AsWebContentsImplExt()->SetUpdateSurface(false);
}

void CefBrowserPlatformDelegateOsrExt::Pause() {
  content::WebContentsImpl* web_contents_impl =
      static_cast<content::WebContentsImpl*>(web_contents_);
  if (!web_contents_impl) {
    LOG(ERROR) << "Pip " << __func__ << " " << delegate_id_
               << " " << child_id_ << " " << frame_routing_id_;
    return;
  }
  bool status = false;
  auto it = web_contents_impl->AsWebContentsImplExt()->GetMediaPlayerId(
      delegate_id_, child_id_, frame_routing_id_, status);
  if (status && !web_contents_impl->IsFullscreen()) {
    auto observer = web_contents_impl->media_web_contents_observer();
    if (!observer || !observer->GetMediaPlayerRemote(it)) {
      LOG(ERROR) << "Pip get media_web_contents_observer is null";
      return;
    }
    observer->GetMediaPlayerRemote(it)->RequestPause(false);
  } else {
    LOG(ERROR) << "Pip cann't find mediaplayer: " << delegate_id_
               << " " << child_id_ << " " << frame_routing_id_;
  }
}
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
CefRefPtr<CefDragData> CefBrowserPlatformDelegateOsrExt::GetDropData() {
  return last_drag_data_;
}
#endif // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

#if BUILDFLAG(ARKWEB_PDF)
void CefBrowserPlatformDelegateOsrExt::OnPdfScrollAtBottom(const std::string& url) {
  CHECK(browser_);
  CHECK(browser_->GetClient());
  CefRefPtr<ArkWebLoadHandlerExt> handler =
      browser_->GetClient()->GetLoadHandler();
  if (handler.get()) {
    handler->OnPdfScrollAtBottom(url);
  }
}

void CefBrowserPlatformDelegateOsrExt::OnPdfLoadEvent(int32_t result, const std::string& url) {
  CHECK(browser_);
  CHECK(browser_->GetClient());
  CefRefPtr<ArkWebLoadHandlerExt> handler =
      browser_->GetClient()->GetLoadHandler();
  if (handler.get()) {
    handler->OnPdfLoadEvent(result, url);
  }
}
#endif  // BUILDFLAG(ARKWEB_PDF)

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
bool CefBrowserPlatformDelegateOsrExt::OnStartBackgroundTask(
    int32_t type,
    const std::string& message) {
  if (!browser_ || !browser_->GetClient().get() ||
      !browser_->GetClient()->AsArkWebClient().get()) {
    LOG(ERROR) << "has nullptr, default return true";
    return true;
  }
  return browser_->GetClient()->AsArkWebClient()->OnStartBackgroundTask(
      type, message);
}
#endif  // ARKWEB_PERFORMANCE_PERSISTENT_TASK

#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
SkColor CefBrowserPlatformDelegateOsrExt::GetBackgroundColor() const {
  if (background_color_) {
    return background_color_.value();
  }
  return CefBrowserPlatformDelegateOsr::GetBackgroundColor();
}

void CefBrowserPlatformDelegateOsrExt::UpdateBackgroundColor(SkColor color) {
  background_color_ = color;
}
#endif  // ARKWEB_BACKGROUND_COLOR

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void CefBrowserPlatformDelegateOsrExt::SendMouseClickEvent(
    const CefMouseEvent& event,
    CefBrowserHost::MouseButtonType type,
    bool mouseUp,
    int clickCount) {
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (!view) {
    LOG(ERROR) << "SendMouseClickEvent drop mouse event!!";
    return;
  }

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  cef_browser_platform_delegate_osr_utils_->UpdateNativeEmbedMode(view);
  cef_browser_platform_delegate_osr_utils_->SetEnableCustomVideoPlayer(view);
#endif

  CefMouseEvent mouseEvent = event;
  blink::WebMouseEvent web_event = native_delegate_->TranslateWebClickEvent(
      mouseEvent, type, mouseUp, clickCount);
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  cef_browser_platform_delegate_osr_utils_->AdjustMouseEventCoordinates(view, mouseEvent, web_event);
#endif

  view->SendMouseEvent(web_event);
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  cef_browser_platform_delegate_osr_utils_->CancelTouchpadFlingOnMouseClick(view, event);
#endif
}

void CefBrowserPlatformDelegateOsrExt::SendMouseMoveEvent(
    const CefMouseEvent& event,
    bool mouseLeave) {
  CefRenderWidgetHostViewOSR* view = GetOSRHostView();
  if (!view) {
    LOG(ERROR) << "SendMouseMoveEvent drop mouse event!!";
    return;
  }
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  cef_browser_platform_delegate_osr_utils_->UpdateNativeEmbedMode(view);
#endif

  CefMouseEvent mouseEvent = event;
  blink::WebMouseEvent web_event =
      native_delegate_->TranslateWebMoveEvent(mouseEvent, mouseLeave);
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  cef_browser_platform_delegate_osr_utils_->AdjustMouseEventCoordinates(view, mouseEvent, web_event);
#endif
  view->SendMouseEvent(web_event);
}
#endif