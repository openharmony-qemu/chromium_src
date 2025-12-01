/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cef/ohos_cef_ext/libcef/browser/osr/arkweb_render_widget_host_view_osr_utils.h"

#include "cef/libcef/browser/osr/render_widget_host_view_osr.h"
#include "arkweb/chromium_ext/ui/compositor/compositor_utils.h"
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
#include "cef/ohos_cef_ext/libcef/common/soc_perf_util.h"
#endif

std::unordered_map<gfx::AcceleratedWidget, ui::Compositor*>
    ArkWebRenderWidgetHostViewOSRUtils::compositor_map_;

std::unordered_map<gfx::AcceleratedWidget, uint32_t>
    ArkWebRenderWidgetHostViewOSRUtils::accelerate_widget_map_;

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
const int SOC_PERF_WEB_GESTURE_ID = 10012;
const int SOC_PERF_WEB_SLIDE_SCROLL  = 10097;
const int TOUCH_DOWN_DELAY_TIME = 200;
const int TOUCH_UP_DURATION_TIME = 100;
#endif

ArkWebRenderWidgetHostViewOSRUtils::ArkWebRenderWidgetHostViewOSRUtils(
    CefRenderWidgetHostViewOSR* view)
    : view_(view) {
  DCHECK(view);
}

ArkWebRenderWidgetHostViewOSRUtils::~ArkWebRenderWidgetHostViewOSRUtils() =
    default;

display::mojom::ScreenOrientation
ArkWebRenderWidgetHostViewOSRUtils::ConvertOrientationType(
    cef_screen_orientation_type_t type) {
  switch (type) {
    case cef_screen_orientation_type_t::UNDEFINED:
      return display::mojom::ScreenOrientation::kUndefined;
    case cef_screen_orientation_type_t::PORTRAIT_PRIMARY:
      return display::mojom::ScreenOrientation::kPortraitPrimary;
    case cef_screen_orientation_type_t::LANDSCAPE_PRIMARY:
      return display::mojom::ScreenOrientation::kLandscapePrimary;
    case cef_screen_orientation_type_t::PORTRAIT_SECONDARY:
      return display::mojom::ScreenOrientation::kPortraitSecondary;
    case cef_screen_orientation_type_t::LANDSCAPE_SECONDARY:
      return display::mojom::ScreenOrientation::kLandscapeSecondary;
    default:
      return display::mojom::ScreenOrientation::kUndefined;
  }
}

void ArkWebRenderWidgetHostViewOSRUtils::UpdateScreenInfoForArkweb(
    display::ScreenInfo& screenInfo,
    const CefScreenInfo& src) {
  screenInfo.orientation_angle = src.angle;
  screenInfo.orientation_type = ConvertOrientationType(src.orientation);
}

void ArkWebRenderWidgetHostViewOSRUtils::HandleCompositorCreation(
    base::SingleThreadTaskRunner* task_runner,
    bool use_external_begin_frame) {
  DCHECK(view_);
  auto context_factory = GetContextFactory();
  LOG(INFO) << "compositor construct, widget = "
            << static_cast<uint32_t>(
                   view_->browser_impl()->GetAcceleratedWidget(
                       view_->is_popup_));
  ui::Compositor* compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
      view_->browser_impl()->GetAcceleratedWidget(view_->is_popup_));
  accelerate_widget_map_[view_->browser_impl()->GetAcceleratedWidget(
      view_->is_popup_)]++;
  if (!compositor) {
    compositor = new ui::Compositor(
        context_factory->AllocateFrameSinkId(), context_factory, task_runner,
        false /* enable_pixel_canvas */, use_external_begin_frame);
    compositor->SetAcceleratedWidget(
        view_->browser_impl()->GetAcceleratedWidget(view_->is_popup_));
    ArkWebRenderWidgetHostViewOSRUtils::AddCompositor(
        view_->browser_impl()->GetAcceleratedWidget(view_->is_popup_),
        compositor);
  }
}

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
void ArkWebRenderWidgetHostViewOSRUtils::HandleCompositeRenderRelease() {
  DCHECK(view_);
#ifdef DISABLE_GPU
  if (!view_->compositor_) {
    return;  // already released
  }
#else
  if (!view_->browser_impl_) {
    return;
  }
  auto it1 = accelerate_widget_map_.find(
      view_->browser_impl_->GetAcceleratedWidget(view_->is_popup_));
  if (it1 == accelerate_widget_map_.end()) {
    return;
  }
#endif
  // Marking the DelegatedFrameHost as removed from the window hierarchy is
  // necessary to remove all connections to its old ui::Compositor.
  if (view_->delegated_frame_host_) {
    if (view_->is_showing_) {
      view_->delegated_frame_host_->WasHidden(
          content::DelegatedFrameHost::HiddenCause::kOther);
    }
    view_->delegated_frame_host_->DetachFromCompositor();
    view_->delegated_frame_host_.reset(nullptr);
  }

#ifdef DISABLE_GPU
  view_->compositor_.reset(nullptr);
#else
  LOG(INFO) << "ReleaseCompositor";
  auto com = compositor_map_.find(
      view_->browser_impl_->GetAcceleratedWidget(view_->is_popup_));
  if (--accelerate_widget_map_[view_->browser_impl_->GetAcceleratedWidget(
          view_->is_popup_)] == 0) {
    if (!view_->browser_impl_) {
      return;
    }
    LOG(INFO) << "ReleaseCompositor, widget = "
              << static_cast<uint32_t>(
                     view_->browser_impl_->GetAcceleratedWidget(
                         view_->is_popup_));
    if (com != compositor_map_.end()) {
      if (com->second != nullptr) {
        delete com->second;
      }
      compositor_map_.erase(com);
    }
    accelerate_widget_map_.erase(
        view_->browser_impl_->GetAcceleratedWidget(view_->is_popup_));
  } else {
    if (com != compositor_map_.end()) {
      if (com->second->delegate() == view_) {
        com->second->SetDelegate(nullptr);
      }
    }
  }
#endif  // DISABLE_GPU
#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
  if (view_->overscroll_controller_) {
    view_->overscroll_controller_.reset();
  }
#endif  // ARKWEB_PULL_TO_REFRESH
}
#endif  // ARKWEB_COMPOSITE_RENDER

void ArkWebRenderWidgetHostViewOSRUtils::SetupCompositor(
    ui::Compositor* compositor) {
  DCHECK(view_);
  compositor->SetDelegate(view_);
  compositor->SetRootLayer(view_->root_layer_.get());

  content::RenderWidgetHostImpl* render_widget_host_impl =
      content::RenderWidgetHostImpl::From(view_->render_widget_host_);
  if (render_widget_host_impl) {
    render_widget_host_impl->SetCompositorForFlingScheduler(compositor);
  }
  LOG(INFO) << "CefRenderWidgetHostViewOSR::ShowWithVisibility compositor";
}

void ArkWebRenderWidgetHostViewOSRUtils::HandleInvalidLocalSurfaceId() {
  // If the viz::LocalSurfaceId is invalid, we may have been evicted,
  // and no other visual properties have since been changed. Allocate a new id
  // and start synchronizing.
  DCHECK(view_);
  if (!view_->GetLocalSurfaceId().is_valid()) {
    view_->AllocateLocalSurfaceId();
    view_->SynchronizeVisualProperties(cc::DeadlinePolicy::UseDefaultDeadline(),
                                       view_->GetLocalSurfaceId());
  }
}

std::u16string ArkWebRenderWidgetHostViewOSRUtils::TruncateTooltipText(
    const std::u16string& tooltip_text) {
  const size_t kMaxTooltipLength = 1024;
  std::u16string truncated_text =
      gfx::TruncateString(tooltip_text, kMaxTooltipLength, gfx::WORD_BREAK);
  return truncated_text;
}

gfx::Rect ArkWebRenderWidgetHostViewOSRUtils::CalculateViewBounds(
    CefRefPtr<CefRenderHandler> handler,
    CefRect& rc) {
  DCHECK(view_);
  gfx::Rect view_bounds;
  double screen_x = 0;
  double screen_y = 0;

  auto browser_impl = view_->browser_impl_;

  if (browser_impl && browser_impl->GetClient() && handler &&
      handler->AsArkWebRenderHandler()) {
    handler->AsArkWebRenderHandler()->GetScreenOffset(browser_impl.get(),
                                                      screen_x, screen_y);
  }

  if (handler->GetRootScreenRect(browser_impl.get(), rc)) {
    view_bounds =
        gfx::Rect(rc.x + screen_x, rc.y + screen_y, rc.width, rc.height);
  } else {
    gfx::Rect view_bounds_inner = view_->GetViewBounds();
    view_bounds = gfx::Rect(
        view_bounds_inner.x() + screen_x, view_bounds_inner.y() + screen_y,
        view_bounds_inner.width(), view_bounds_inner.height());
  }
  return view_bounds;
}

void ArkWebRenderWidgetHostViewOSRUtils::ImeCommitTextEx(
    const CefString& text,
    const gfx::Range& range,
    int relative_cursor_pos) {
  if (view_->text_input_manager_ &&
      view_->text_input_manager_->GetActiveWidget()) {
    view_->text_input_manager_->GetActiveWidget()->ImeCommitText(
        text, std::vector<ui::ImeTextSpan>(), range, relative_cursor_pos);
  }
}

void ArkWebRenderWidgetHostViewOSRUtils::ImeSetCompositionEx(
    const CefString& text,
    const std::vector<ui::ImeTextSpan>& web_underlines,
    const gfx::Range& range,
    int selection_start,
    int selection_end) {
  if (view_->text_input_manager_ &&
      view_->text_input_manager_->GetActiveWidget()) {
    view_->text_input_manager_->GetActiveWidget()->ImeSetComposition(
        text, web_underlines, range, selection_start, selection_end);
  }
}

void ArkWebRenderWidgetHostViewOSRUtils::ImeCancelCompositionEx() {
  if (view_->text_input_manager_ &&
      view_->text_input_manager_->GetActiveWidget()) {
    view_->text_input_manager_->GetActiveWidget()->ImeCancelComposition();
  }
}

void ArkWebRenderWidgetHostViewOSRUtils::SendTouchEventEx(
    const CefTouchEvent& event) {
#if BUILDFLAG(IS_ARKWEB) && BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  if (event.type == CEF_TET_PRESSED) {
    view_->is_editable_node_ = false;
    auto compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
        view_->browser_impl_->GetAcceleratedWidget(view_->is_popup_));
    if (compositor) {
      compositor->Utils()->SetCurrentFrameSinkId(view_->GetFrameSinkId());
    } else {
      LOG(ERROR) << "compositor is null when send touch event";
    }
  }
#endif
}

void ArkWebRenderWidgetHostViewOSRUtils::SetCompositorVSyncParameters(
    int frame_rate_threshold_us) {
  auto compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
      view_->browser_impl_->GetAcceleratedWidget(view_->is_popup_));
  if (compositor) {
    compositor->SetDisplayVSyncParameters(
        base::TimeTicks::Now(), base::Microseconds(frame_rate_threshold_us));
  }
}

void ArkWebRenderWidgetHostViewOSRUtils::SetupCompositorWithRootLayerSize() {
  auto compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
      view_->browser_impl_->GetAcceleratedWidget(view_->is_popup_));
  if (compositor) {
    view_->compositor_local_surface_id_allocator_.GenerateId();
    compositor->SetScaleAndSize(view_->GetDeviceScaleFactor(),
                                view_->SizeInPixels(),
                                view_->compositor_local_surface_id_allocator_
                                    .GetCurrentLocalSurfaceId());
  }
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
gfx::Size GetVisibleViewportSize(AlloyBrowserHostImpl* browser) {
  if (!browser) {
    return gfx::Size(0, 0);
  }

  CefRect rc;
  CefRefPtr<ArkWebRenderHandlerExt> handler =
      browser->GetClient()->GetRenderHandler();
  CHECK(handler);

  handler->GetVisibleViewportRect(browser, rc);
  CHECK_GT(rc.width, 0);
  CHECK_GT(rc.height, 0);

  return gfx::Size(rc.width, rc.height);
}

bool ArkWebRenderWidgetHostViewOSRUtils::SetVisibleViewportSize() {
  // This method should not be called while the resize hold is active.
  DCHECK(!view_->hold_resize_);

  // Popup bounds are set in InitAsPopup.
  if (view_->IsPopupWidget()) {
    return false;
  }
  const gfx::Size& visible_view_bounds =
      ::GetVisibleViewportSize(view_->browser_impl_.get());
  const bool size_in_pixel_changed = SetCurrentSizeInPixel();
  if (visible_view_bounds == view_->current_visible_view_bounds_) {
    return size_in_pixel_changed;
  }

  view_->current_visible_view_bounds_ = visible_view_bounds;
  return true;
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

bool ArkWebRenderWidgetHostViewOSRUtils::SetRootLayerSizeEx(
    bool force,
    bool* visible_changed) {
  const bool screen_info_changed = view_->SetScreenInfo();
  const bool view_bounds_changed = view_->SetViewBounds();
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  const bool visible_view_bounds_changed = SetVisibleViewportSize();
  if (visible_changed) {
    *visible_changed = visible_view_bounds_changed;
  }

  if (!force && !screen_info_changed && !view_bounds_changed &&
      !visible_view_bounds_changed) {
    return false;
  }

  view_->GetRootLayer()->SetBounds(
      gfx::Rect(view_->GetPhysicalViewBounds().size()));
#endif

#ifdef DISABLE_GPU
  if (view_->compositor_) {
    view_->compositor_local_surface_id_allocator_.GenerateId();
    view_->compositor_->SetScaleAndSize(
        view_->GetDeviceScaleFactor(), view_->SizeInPixels(),
        view_->compositor_local_surface_id_allocator_
            .GetCurrentLocalSurfaceId());
  }
#else
  auto compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
      view_->browser_impl_->GetAcceleratedWidget(view_->is_popup_));
  if (compositor) {
    view_->compositor_local_surface_id_allocator_.GenerateId();
    compositor->SetScaleAndSize(view_->GetDeviceScaleFactor(),
                                view_->SizeInPixels(),
                                view_->compositor_local_surface_id_allocator_
                                    .GetCurrentLocalSurfaceId());
  }
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  // We only need to notify the screen information change, the visual window
  // adjustment is done by CefRenderWidgetHostViewOSR::WasResized.
  return view_bounds_changed;
#endif

#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
  if (view_->overscroll_controller_) {
    view_->overscroll_controller_->Disable();
  }
#endif
}

bool ArkWebRenderWidgetHostViewOSRUtils::ResizeRootLayerEx(
    bool isKeyboard,
    bool& visible_changed) {
  if (!view_->hold_resize_) {
    // The resize hold is not currently active.
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
    if (SetRootLayerSizeEx(false /* force */, &visible_changed)) {
#endif
      // The size has changed. Avoid resizing again until ReleaseResizeHold() is
      // called.
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
      setReleaseResizeHoldDelayedTask_.Reset(
          base::BindOnce(&CefRenderWidgetHostViewOSR::ReleaseResizeHold,
                         view_->weak_ptr_factory_.GetWeakPtr()));
      content::GetUIThreadTaskRunner({})->PostDelayedTask(
          FROM_HERE, setReleaseResizeHoldDelayedTask_.callback(),
          base::Milliseconds(400));
      TRACE_EVENT0(
          "base",
          "CefRenderWidgetHostViewOSR::ResizeRootLayer, trigger delay task.");
#endif
      view_->hold_resize_ = true;
      view_->cached_scale_factor_ = view_->GetDeviceScaleFactor();
      return true;
    }
  } else if (!view_->pending_resize_) {
    // The resize hold is currently active. Another resize will be triggered
    // from ReleaseResizeHold().
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
    view_->isKeyboardResized_ = isKeyboard;
#endif
    view_->pending_resize_ = true;
  }
  return false;
}

void ArkWebRenderWidgetHostViewOSRUtils::ReleaseResizeHoldEx() {
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  setReleaseResizeHoldDelayedTask_.Cancel();
  DCHECK(view_->hold_resize_);
  view_->hold_resize_ = false;
  view_->cached_scale_factor_ = -1;
  if (view_->pending_resize_) {
    view_->pending_resize_ = false;
    if (view_->isKeyboardResized_) {
      view_->isKeyboardResized_ = false;
      CEF_POST_TASK(
          CEF_UIT,
          base::BindOnce(&CefRenderWidgetHostViewOSR::WasKeyboardResizedEx,
                         view_->weak_ptr_factory_.GetWeakPtr()));
    } else {
      CEF_POST_TASK(CEF_UIT,
                    base::BindOnce(&CefRenderWidgetHostViewOSR::WasResized,
                                   view_->weak_ptr_factory_.GetWeakPtr()));
    }
  }

  if (view_->browser_impl_) {
    CefRefPtr<CefRenderHandler> handler =
        view_->browser_impl_->client()->GetRenderHandler();
    CHECK(handler);
  }
#endif

#if BUILDFLAG(ARKWEB_DRAG_RESIZE)
  bool isDragResized =
      OHOS::NWeb::NWebResizeHelper::GetInstance().IsDragResizeStart();
  if (isDragResized) {
    OHOS::NWeb::NWebResizeHelper::GetInstance().SetDragResizeStart(false);
  }
#endif
}

void ArkWebRenderWidgetHostViewOSRUtils::OnScrollOffsetChangedEx(
    CefRefPtr<CefRenderHandler> handler) {
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  auto browser = view_->browser_impl_->GetBrowser();
  if (browser != nullptr && browser->GetHost() != nullptr) {
    float ratio = browser->GetHost()->GetVirtualPixelRatio();
    if (ratio <= 0) {
      LOG(ERROR) << "get ratio invalid: " << ratio;
      return;
    }
    float x = view_->last_scroll_offset_.x() / ratio;
    float y = view_->last_scroll_offset_.y() / ratio;
    handler->OnScrollOffsetChanged(view_->browser_impl_.get(), std::round(x),
                                   std::round(y));
  }
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
}

void ArkWebRenderWidgetHostViewOSRUtils::SynchronizeVisualPropertiesEx(
    bool resized) {
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  if (resized) {
    OHOS::NWeb::ResSchedClientAdapter::ReportScene(
        OHOS::NWeb::ResSchedStatusAdapter::WEB_SCENE_ENTER,
        OHOS::NWeb::ResSchedSceneAdapter::RESIZE);
  }
#endif

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  if (resized && view_->should_wait_) {
    if (auto compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
            view_->browser_impl_->GetAcceleratedWidget(view_->is_popup_))) {
      compositor->Utils()->SetShouldFrameSubmissionBeforeDraw(true);
    }
  }
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
}

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
void ArkWebRenderWidgetHostViewOSRUtils::StopBoosting() {
  if (view_->is_fling_) {
    return;
  }
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  if (view_->browser_impl_) {
    view_->browser_impl_->SetIsFling(false);
  }
#endif
  int socPerfId = SOC_PERF_WEB_GESTURE_ID;
#if BUILDFLAG(IS_OHOS)
  if (base::ohos::IsPcDevice() || base::ohos::IsTabletDevice()) {
    socPerfId = SOC_PERF_WEB_SLIDE_SCROLL;
  }
#endif
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigByIdEx(socPerfId, false);
}

void ArkWebRenderWidgetHostViewOSRUtils::OnTouchDown() {
  if (view_->pointer_state_.GetPointerCount() == 0) {
    has_touch_point_ = false;
    if (view_->isBoosting_) {
      view_->isBoosting_ = false;
      CEF_POST_DELAYED_TASK(
          CEF_UIT,
          base::BindOnce(&ArkWebRenderWidgetHostViewOSRUtils::StopBoosting,
                         weak_ptr_factory_.GetWeakPtr()),
          TOUCH_UP_DURATION_TIME);
      auto* host = content::GpuProcessHost::Get();
      if (host && host->gpu_host()) {
        host->gpu_host()->SetHasTouchPoint(false);
      }
    }
    return;
  }
  if (view_->isBoosting_) {
    int socPerfId = SOC_PERF_WEB_GESTURE_ID;
  #if BUILDFLAG(IS_OHOS)
    if (base::ohos::IsPcDevice() || base::ohos::IsTabletDevice()) {
      socPerfId = SOC_PERF_WEB_SLIDE_SCROLL;
    }
  #endif
    OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .CreateSocPerfClientAdapter()
        ->ApplySocPerfConfigByIdEx(socPerfId, true);
    LOG(DEBUG) << "hwtlog:CefRenderWidgetHostViewOSR::OnTouchDown";
    CEF_POST_DELAYED_TASK(
        CEF_UIT,
        base::BindOnce(&ArkWebRenderWidgetHostViewOSRUtils::OnTouchDown,
                       weak_ptr_factory_.GetWeakPtr()),
        TOUCH_DOWN_DELAY_TIME);
  }
#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  OHOS::NWeb::ResSchedClientAdapter::ReportScene(
      OHOS::NWeb::ResSchedStatusAdapter::WEB_SCENE_ENTER,
      OHOS::NWeb::ResSchedSceneAdapter::SLIDE);
#endif
  if (!has_touch_point_) {
    if (auto* host = content::GpuProcessHost::Get()) {
      if (auto* host_impl = host->gpu_host()) {
        host_impl->SetHasTouchPoint(true);
      }
    }
  }
  has_touch_point_ = true;
}
#endif

void ArkWebRenderWidgetHostViewOSRUtils::HandleRawKeyDownEvent(
    const input::NativeWebKeyboardEvent& event) {
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  if (event.GetType() == blink::WebInputEvent::Type::kRawKeyDown) {
    OHOS::NWeb::ResSchedClientAdapter::ReportScene(
        OHOS::NWeb::ResSchedStatusAdapter::WEB_SCENE_ENTER,
        OHOS::NWeb::ResSchedSceneAdapter::KEYBOARD_CLICK);
  }
#endif
}

void ArkWebRenderWidgetHostViewOSRUtils::
    SetDoubleTapSupportForPlatformEnabledEx() {
  bool excludable_devices = base::CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kDoubleTapSupportForPlatformEnabled);
  view_->gesture_provider_.SetDoubleTapSupportForPlatformEnabled(
      excludable_devices);
}

void ArkWebRenderWidgetHostViewOSRUtils::HideEx() {
  if (view_->GetTextInputManager()) {
    view_->GetTextInputManager()->RemoveObserver(view_);
  }
  if (view_->overscroll_controller_) {
    view_->overscroll_controller_->Disable();
  }
}

void ArkWebRenderWidgetHostViewOSRUtils::ImeFinishComposingTextEx(bool keep_selection) {
  if (view_->text_input_manager_ && view_->text_input_manager_->GetActiveWidget()) {
    view_->text_input_manager_->GetActiveWidget()->ImeFinishComposingText(
        keep_selection);
  }
}

void ArkWebRenderWidgetHostViewOSRUtils::SendMouseEventEx(
    const blink::WebMouseEvent& event) {
  if (view_->selection_controller_client_ &&
      event.GetType() == blink::WebMouseEvent::Type::kMouseDown) {
    view_->selection_controller_client_->CloseQuickMenuAndHideHandles();
  }
}

void ArkWebRenderWidgetHostViewOSRUtils::SendMouseWheelEventEx(
    blink::WebMouseWheelEvent& mouse_wheel_event) {
  bool shouldRoute = view_->ShouldRouteEvents();
  view_->mouse_wheel_phase_handler_.SendWheelEndForTouchpadScrollingIfNeeded(
      shouldRoute);
  view_->mouse_wheel_phase_handler_.AddPhaseIfNeededAndScheduleEndEvent(
      mouse_wheel_event, shouldRoute, /*is_fling_capable=*/false);
}

void ArkWebRenderWidgetHostViewOSRUtils::SetFocusEx() {
  if (view_->selection_controller_client_) {
    view_->selection_controller_client_->SetTemporarilyHidden(false);
  }
  view_->OnFocusInternal();
}

void ArkWebRenderWidgetHostViewOSRUtils::AddCompositor(gfx::AcceleratedWidget widget,
                                               ui::Compositor* compositor) {
  ArkWebRenderWidgetHostViewOSRUtils::compositor_map_.emplace(widget, compositor);
}

ui::Compositor* ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
    gfx::AcceleratedWidget widget) {
  auto it = ArkWebRenderWidgetHostViewOSRUtils::compositor_map_.find(widget);
  if (it == ArkWebRenderWidgetHostViewOSRUtils::compositor_map_.end()) {
    return nullptr;
  }
  return it->second;
}

#if BUILDFLAG(ARKWEB_DSS)
bool ArkWebRenderWidgetHostViewOSRUtils::SetCurrentSizeInPixel() {
  gfx::Size size_in_pixel = view_->SizeInPixels();
  if (size_in_pixel == current_size_in_pixel_) {
    return false;
  }
  current_size_in_pixel_ = size_in_pixel;
  return true;
}
#endif
