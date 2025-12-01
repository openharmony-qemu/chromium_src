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

#include "cef/ohos_cef_ext/libcef/browser/osr/arkweb_render_widget_host_view_osr_ext.h"

#include <stdint.h>

#include <memory>
#include <utility>

#include "arkweb/build/features/features.h"
#include "base/command_line.h"
#include "base/functional/callback_helpers.h"
#include "base/memory/ptr_util.h"
#include "base/memory/raw_ptr.h"
#include "base/strings/utf_string_conversions.h"
#include "cc/base/switches.h"
#include "cef/libcef/browser/alloy/alloy_browser_host_impl.h"
#include "cef/libcef/browser/osr/osr_util.h"
#include "cef/libcef/browser/osr/render_widget_host_view_osr.h"
#include "cef/libcef/browser/osr/synthetic_gesture_target_osr.h"
#include "cef/libcef/browser/osr/touch_selection_controller_client_osr.h"
#include "cef/libcef/browser/osr/video_consumer_osr.h"
#include "cef/libcef/browser/thread_util.h"
#include "cef/libcef/browser/osr/render_widget_host_view_osr.h"
#include "cef/libcef/browser/native/cursor_util.h"
#include "cef/libcef/browser/browser_event_util.h"
#include "components/viz/common/features.h"
#include "components/viz/common/frame_sinks/begin_frame_args.h"
#include "components/viz/common/frame_sinks/copy_output_request.h"
#include "components/viz/common/frame_sinks/delay_based_time_source.h"
#include "components/viz/common/surfaces/frame_sink_id_allocator.h"
#include "components/viz/common/switches.h"
#include "content/browser/bad_message.h"
#include "content/browser/gpu/gpu_data_manager_impl.h"
#include "content/browser/renderer_host/delegated_frame_host.h"
#include "content/browser/renderer_host/dip_util.h"
#include "content/browser/renderer_host/input/motion_event_web.h"
#include "content/browser/renderer_host/input/synthetic_gesture_target_base.h"
#include "content/browser/renderer_host/render_frame_host_impl.h"
#include "content/browser/renderer_host/render_widget_host_delegate.h"
#include "content/browser/renderer_host/render_widget_host_impl.h"
#include "content/browser/renderer_host/render_frame_host_impl.h"
#include "base/ohos/sys_info_utils_ext.h"
#include "cef/libcef/browser/osr/render_widget_host_view_osr.h"
#include "cef/ohos_cef_ext/include/arkweb_render_handler_ext.h"
#include "cef/ohos_cef_ext/include/cef_web_client_extension_handler.h"
#include "cef/ohos_cef_ext/libcef/browser/osr/arkweb_touch_selection_controller_client_osr_ext.h"
#include "cef/ohos_cef_ext/libcef/browser/osr/arkweb_render_widget_host_view_osr_utils.h"
#include "components/input/render_widget_host_input_event_router.h"
#include "content/browser/renderer_host/frame_tree.h"
#include "content/browser/renderer_host/render_view_host_delegate_view.h"
#include "content/browser/renderer_host/render_widget_host_view_base.h"
#include "content/common/content_switches_internal.h"
#include "content/common/native_embed_first_paint_event.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/context_factory.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/common/content_switches.h"
#include "libcef/browser/web_client_extension/alloy_native_embed_first_frame_paint_event.h"
#include "media/base/video_frame.h"
#include "media/capture/mojom/video_capture_buffer.mojom.h"
// #include "third_party/blink/renderer/platform/widget/input/input_handler_proxy.h"  // Removed for Chromium 141 compatibility
#include "ui/compositor/compositor.h"
#include "ui/events/blink/blink_event_util.h"
#include "ui/events/gesture_detection/gesture_provider_config_helper.h"
#include "ui/events/velocity_tracker/motion_event.h"
#include "ui/gfx/geometry/dip_util.h"
#include "ui/gfx/geometry/size_conversions.h"
#include "ui/touch_selection/touch_selection_controller.h"

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
#include "cef/ohos_cef_ext/libcef/common/soc_perf_util.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
#include "cef/ohos_cef_ext/include/arkweb_client_ext.h"
#include "content/browser/renderer_host/render_view_host_delegate_view.h"
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
const size_t kMaxGestureQueueSize = 10;
const size_t KFirstRecordingTimes = 3;
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
constexpr int32_t PINCH_START_TYPE = 1;
constexpr int32_t PINCH_UPDATE_TYPE = 3;
constexpr int32_t PINCH_END_TYPE = 2;
const int32_t DEFAULT_PINCH_FINGER = 2;
const int32_t TAP_TWICE = 2;
#endif

#if BUILDFLAG(ARKWEB_AI)
const size_t kMaxDataDetectorTextLength = 1000;
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
#include <window_manager/oh_window.h>

#include "arkweb/ohos_adapter_ndk/window_manager_adapter/window_manager_adapter_impl.h"
#include "cef/include/internal/cef_types.h"
#include "ui/base/ime/text_input_flags.h"
#include "ui/events/blink/did_overscroll_params.h"
#include "ui/events/types/event_type.h"
#endif //BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_AI)
#include "cef/libcef/browser/image_impl.h"
#endif

#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
#include "arkweb/chromium_ext/content/browser/ohos/overscroll_controller_ohos.h"
#include "arkweb/chromium_ext/ui/ohos/overscroll_refresh.h"
#include "components/security_interstitials/content/security_interstitial_tab_helper.h"
#endif

#include "arkweb/chromium_ext/ui/compositor/compositor_utils.h"

#if BUILDFLAG(IS_ARKWEB)
const int SCALE_FACTOR_CONVERT_RATIO = 100;
const int SOC_PERF_WEB_SLIDE_SCROLL = 10097;
#endif

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
static float DEFAULT_PAGE_SCROLL_OFFSET = 50.0f;
static float DEFAULT_SCROLL_OFFSET_Y = -1.0f;
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
const int32_t NOT_NATIVE_ID = 100;
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
// NOTE: The following callback implementation classes are disabled for Chromium 141
// due to incompatible changes in blink internal APIs (InputHandlerProxyUtils).
// The required types GestureEventCallback and MouseEventCallback are no longer
// available in the new architecture.
//
// TODO: Re-implement this functionality using the new Chromium 141 input event
// system when the ARKWEB_SAME_LAYER feature needs to be supported.

// class CefGestureEventCallbackImpl : public CefGestureEventCallback {
//  public:
//   using CallbackType = blink::InputHandlerProxyUtils::GestureEventCallback;  // Not available in Chromium 141
//
//   CefGestureEventCallbackImpl(CallbackType callback, int32_t fingerId)
//       : callback_(std::move(callback)), fingerId_(fingerId) {}
//   ~CefGestureEventCallbackImpl() override {
//     if (!callback_.is_null()) {
//       // The callback is still pending. Cancel it now.
//       if (CEF_CURRENTLY_ON_UIT()) {
//         CancelNow(std::move(callback_), fingerId_);
//       } else {
//         CEF_POST_TASK(CEF_UIT,
//                       base::BindOnce(&CefGestureEventCallbackImpl::CancelNow,
//                                      std::move(callback_), fingerId_));
//       }
//     }
//   }
//
//   void ContinueTask(bool user_input, bool stopPropagation) override {
//     if (CEF_CURRENTLY_ON_UIT()) {
//       if (!callback_.is_null()) {
//         std::move(callback_).Run(user_input, stopPropagation, fingerId_);
//       }
//     } else {
//       CEF_POST_TASK(CEF_UIT,
//                     base::BindOnce(&CefGestureEventCallbackImpl::ContinueTask,
//                                    this, user_input, stopPropagation));
//     }
//   }
//
//   [[nodiscard]] CallbackType Disconnect() { return std::move(callback_); }
//
//  private:
//   static void CancelNow(CallbackType callback, int32_t fingerId) {
//     CEF_REQUIRE_UIT();
//     std::move(callback).Run(false, true, fingerId);
//   }
//
//   CallbackType callback_;
//   int32_t fingerId_ = 0;
//
//   IMPLEMENT_REFCOUNTING(CefGestureEventCallbackImpl);
// };
//
// class CefMouseEventCallbackImpl : public CefMouseEventCallback {
//  public:
//   using CallbackType = blink::InputHandlerProxyUtils::MouseEventCallback;  // Not available in Chromium 141
//
//   CefMouseEventCallbackImpl(CallbackType callback)
//       : callback_(std::move(callback)) {}
//   ~CefMouseEventCallbackImpl() override {
//     if (!callback_.is_null()) {
//       // The callback is still pending. Cancel it now.
//       if (CEF_CURRENTLY_ON_UIT()) {
//         CancelNow(std::move(callback_));
//       } else {
//         CEF_POST_TASK(CEF_UIT,
//                       base::BindOnce(&CefMouseEventCallbackImpl::CancelNow,
//                                      std::move(callback_)));
//       }
//     }
//   }
//
//   void ContinueTask(bool user_input, bool stopPropagation) override {
//     if (CEF_CURRENTLY_ON_UIT()) {
//       if (!callback_.is_null()) {
//         std::move(callback_).Run(user_input, stopPropagation);
//       }
//     } else {
//       CEF_POST_TASK(CEF_UIT,
//                     base::BindOnce(&CefMouseEventCallbackImpl::ContinueTask, this,
//                                    user_input, stopPropagation));
//     }
//   }
//
//
//   [[nodiscard]] CallbackType Disconnect() { return std::move(callback_); }
//
//  private:
//   static void CancelNow(CallbackType callback) {
//     CEF_REQUIRE_UIT();
//     std::move(callback).Run(false, true);
//   }
//
//   CallbackType callback_;
//
//   IMPLEMENT_REFCOUNTING(CefMouseEventCallbackImpl);
// };

#endif // BUILDFLAG(ARKWEB_SAME_LAYER)

ArkWebRenderWidgetHostViewOSRExt::ArkWebRenderWidgetHostViewOSRExt(
    SkColor background_color,
    bool use_shared_texture,
    bool use_external_begin_frame,
    content::RenderWidgetHost* widget,
    CefRenderWidgetHostViewOSR* parent_host_view)
    : CefRenderWidgetHostViewOSR(background_color,
                                 use_shared_texture,
                                 use_external_begin_frame,
                                 widget,
                                 parent_host_view),
#if BUILDFLAG(IS_ARKWEB)
      enable_nweb_ex_(base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebEx)),
#endif
      weak_ptr_factory_(this) {
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  OnTopControlsHeightChanged();
#endif

#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
  CreateOverscrollControllerIfPossible();
#endif
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void ArkWebRenderWidgetHostViewOSRExt::WasKeyboardResized() {
  // Only one resize will be in-flight at a time.
  if (hold_resize_) {
    isKeyboardResized_ = true;
    if (!pending_resize_)
      pending_resize_ = true;
    return;
  }

  bool isKeyboardResized = true;
  SynchronizeVisualProperties(cc::DeadlinePolicy::UseExistingDeadline(),
                              absl::nullopt, isKeyboardResized);
}

gfx::Size ArkWebRenderWidgetHostViewOSRExt::GetPhysicalVisibleViewportSize() {
  if (current_visible_view_bounds_.width() == 0 && current_visible_view_bounds_.height() == 0) {
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
    return GetPhysicalViewBounds().size();
#else
    return GetViewBounds().size();
#endif
  }

  if (IsPopupWidget()) {
    return popup_position_.size();
  }
  return current_visible_view_bounds_;
}

gfx::Size ArkWebRenderWidgetHostViewOSRExt::GetVisibleViewportSize() {
  gfx::Size bounds = GetPhysicalVisibleViewportSize();
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  bounds.set_height(bounds.height() - GetShrinkViewportHeight());
#endif
  return bounds;
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
ui::LatencyInfo CreateLatencyInfoEx(const blink::WebInputEvent& event) {
  ui::LatencyInfo latency_info;
  // The latency number should only be added if the timestamp is valid.
  base::TimeTicks time = event.TimeStamp();
  if (!time.is_null()) {
    latency_info.AddLatencyNumberWithTimestamp(
        ui::INPUT_EVENT_LATENCY_ORIGINAL_COMPONENT, time);
  }
  return latency_info;
}

void ArkWebRenderWidgetHostViewOSRExt::SendTouchGestureEvent(blink::WebTouchEvent& touch_event) {
  TRACE_EVENT0("cef", "ArkWebRenderWidgetHostViewOSRExt::SendTouchGestureEvent");
  ui::LatencyInfo latency_info = CreateLatencyInfoEx(touch_event);
  if (ShouldRouteEvents()) {
    render_widget_host_->delegate()->GetInputEventRouter()->RouteTouchEvent(
        this, &touch_event, latency_info);
  } else {
    render_widget_host_->GetRenderInputRouter()->ForwardTouchEventWithLatencyInfo(touch_event,
                                                          latency_info);
  }
}

void ArkWebRenderWidgetHostViewOSRExt::SendGestureEvent(
    const ui::GestureEventData& gesture) {
#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
  // Sending a gesture that may trigger overscroll should resume the effect.
  if (overscroll_controller_) {
    overscroll_controller_->Enable();
  }

  blink::WebGestureEvent web_event =
      ui::CreateWebGestureEventFromGestureEventData(gesture);

  // without this check, forwarding gestures does not work!
  if (web_event.GetType() == blink::WebInputEvent::Type::kUndefined) {
    return;
  }

#if BUILDFLAG(ARKWEB_MENU)
  // We let the touch selection controller see gesture events here, since they
  // may be routed and not make it to FilterInputEvent().
  if (selection_controller_ &&
      web_event.SourceDevice() == blink::WebGestureDevice::kTouchscreen) {
    switch (web_event.GetType()) {
      case blink::WebInputEvent::Type::kGestureLongPress:
        selection_controller_->HandleLongPressEvent(
            web_event.TimeStamp(), web_event.PositionInWidget());
        break;
      case blink::WebInputEvent::Type::kGestureScrollBegin:
        selection_controller_->OnScrollBeginEvent();
        break;
      case blink::WebInputEvent::Type::kGestureTapDown:
        break;
      case blink::WebInputEvent::Type::kGestureShowPress:
        break;
      case blink::WebInputEvent::Type::kGestureTap:
        if (web_event.TapCount() == TAP_TWICE) {
          is_tap_down_twice_ = true;
        }
        break;
      default:
        break;
    }
  }
#endif

  ui::LatencyInfo latency_info = CreateLatencyInfoEx(web_event);
  if (ShouldRouteEvents()) {
    render_widget_host_->delegate()->GetInputEventRouter()->RouteGestureEvent(
        this, &web_event, latency_info);
  } else {
    render_widget_host_->GetRenderInputRouter()->ForwardGestureEventWithLatencyInfo(web_event,
                                                            latency_info);
  }
#endif //BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
}

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
void ArkWebRenderWidgetHostViewOSRExt::EvictFrameBackBuffers() {
  TRACE_EVENT0("base", "ArkWebRenderWidgetHostViewOSRExt::EvictFrameBackBuffers");

  if (browser_impl_.get() && browser_impl_->GetAcceleratedWidget(is_popup_)) {
    ui::Compositor* compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
      browser_impl_->GetAcceleratedWidget(is_popup_));
    if (compositor) {
      compositor->Utils()->EvictFrameBackBuffers();
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
void ArkWebRenderWidgetHostViewOSRExt::OnTouchMove() {
  if(pointer_state_.GetPointerCount() == 0 || isBoosting_ == true) {
    return;
  }
  isBoosting_ = true;
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  if (browser_impl_) {
    browser_impl_->SetIsFling(true);
  }
#endif
  BoostingPreiodly();
}

void ArkWebRenderWidgetHostViewOSRExt::BoostingPreiodly() {
  if(pointer_state_.GetPointerCount() == 0) {
    return;
  }
  int socPerfId = SOC_PERF_WEB_GESTURE_ID;
  if (base::ohos::IsPcDevice() || base::ohos::IsTabletDevice()) {
    socPerfId = SOC_PERF_WEB_SLIDE_SCROLL;
  }
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
    .CreateSocPerfClientAdapter()
    ->ApplySocPerfConfigByIdEx(socPerfId, true);
  LOG(DEBUG) << "hwtlog:ArkWebRenderWidgetHostViewOSRExt::BoostingPreiodly";
  CEF_POST_DELAYED_TASK(CEF_UIT,
    base::BindOnce(&ArkWebRenderWidgetHostViewOSRExt::BoostingPreiodly,
    weak_ptr_factory_.GetWeakPtr()), TOUCH_DOWN_DELAY_TIME);
}
#endif

void ArkWebRenderWidgetHostViewOSRExt::SendTouchEventList(const std::vector<CefTouchEvent>& event_list) {
  TRACE_EVENT0("base", "ArkWebRenderWidgetHostViewOSRExt::SendTouchEventList");
 
  for (const auto& event : event_list) {
#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
    if (event.type == CEF_TET_PRESSED) {
      is_editable_node_ = false;
      auto compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
          browser_impl_->GetAcceleratedWidget(is_popup_));
      if (compositor) {
        compositor->Utils()->SetCurrentFrameSinkId(GetFrameSinkId());
      } else {
        LOG(ERROR) << "compositor is null when send touch event";
      }
    }
#endif

    if (!IsPopupWidget() && popup_host_view_) {
      if (!forward_touch_to_popup_ && event.type == CEF_TET_PRESSED &&
          pointer_state_.GetPointerCount() == 0) {
        forward_touch_to_popup_ =
            popup_host_view_->popup_position_.Contains(event.x, event.y);
      }
 
      if (forward_touch_to_popup_) {
        CefTouchEvent popup_event(event);
        popup_event.x -= popup_host_view_->popup_position_.x();
        popup_event.y -= popup_host_view_->popup_position_.y();
        popup_host_view_->SendTouchEvent(popup_event);
        return;
      }
    }
  }
 
  bool had_no_pointer = true;
  std::vector<CefTouchEvent> filtered_event_list;
  for (const auto& event : event_list) {
    // Update the touch event first.
#if BUILDFLAG(ARKWEB_CLIPBOARD)
    had_no_pointer = had_no_pointer && !pointer_state_.GetPointerCount();
    pointer_state_.SetFromOverlay(event.from_overlay);
#endif  // #if BUILDFLAG(ARKWEB_CLIPBOARD)
    if (!pointer_state_.OnTouch(event)) {
      continue;
    }
#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
    OnTouchMove();
#endif
    if (selection_controller_->WillHandleTouchEvent(pointer_state_)) {
      pointer_state_.CleanupRemovedTouchPoints(event);
      continue;
    }
    filtered_event_list.emplace_back(event);
  }
  if (filtered_event_list.empty()) {
    return;
  }
 
  ui::FilteredGestureProvider::TouchHandlingResult result =
      gesture_provider_.OnTouchEvent(pointer_state_);
 
  blink::WebTouchEvent touch_event = ui::CreateWebTouchEventFromMotionEvent(
      pointer_state_, result.moved_beyond_slop_region, false);
 
  for (const auto& event : filtered_event_list) {
    pointer_state_.CleanupRemovedTouchPoints(event);
 
    // Set unchanged touch point to StateStationary for touchmove and
    // touchcancel to make sure only send one ack per WebTouchEvent.
    if (!result.succeeded)
      pointer_state_.MarkUnchangedTouchPointsAsStationary(&touch_event, event);
  }
 
  if (!render_widget_host_)
    return;
 
  SendTouchGestureEvent(touch_event);
 
  bool touch_end =
      touch_event.GetType() == blink::WebInputEvent::Type::kTouchEnd ||
      touch_event.GetType() == blink::WebInputEvent::Type::kTouchCancel;
 
  if (touch_end && IsPopupWidget() && parent_host_view_ &&
      parent_host_view_->popup_host_view_ == this) {
    parent_host_view_->forward_touch_to_popup_ = false;
  }
}

void ArkWebRenderWidgetHostViewOSRExt::OnUpdateTextInputStateCalled(
    content::TextInputManager* text_input_manager,
    content::RenderWidgetHostViewBase* updated_view,
    bool did_update_state) {
  LOG(DEBUG) << "ArkWebRenderWidgetHostViewOSRExt::OnUpdateTextInputStateCalled ";
  ArkWebRenderHandlerExt::TextInputMode mode = CEF_TEXT_INPUT_MODE_NONE;
  ArkWebRenderHandlerExt::TextInputType type = CEF_TEXT_INPUT_TYPE_NONE;
  ArkWebRenderHandlerExt::TextInputAction action = CEF_TEXT_INPUT_ACTION_DEFAULT;
  ArkWebRenderHandlerExt::TextInputFlags flags = CEF_TEXT_INPUT_FLAG_NONE;
  bool show_keyboard = false;
  std::map<CefString, CefString> text_input_attributes;
  const auto state = text_input_manager->GetTextInputState();
  if (state) {
    for (const auto& atrribute : state->input_element_attributes) {
      text_input_attributes.insert(std::make_pair<CefString, CefString>(CefString(atrribute.first),
        CefString(atrribute.second)));
    }
  }

  CefRefPtr<ArkWebRenderHandlerExt> handler =
      browser_impl_->GetClient()->GetRenderHandler();
  CHECK(handler);
#if BUILDFLAG(ARKWEB_SCROLLBAR)
  if (AsArkWebRenderWidgetHostViewOSRExt()->UpdateEditBounds()) {
    auto processedOffset = AsArkWebRenderWidgetHostViewOSRExt()->HandleCursorOffset();
    handler->OnCursorUpdate(browser_impl_->GetBrowser(),
                            CefRect(processedOffset.first, processedOffset.second,
                                    AsArkWebRenderWidgetHostViewOSRExt()->focus_rect_width_,
                                    AsArkWebRenderWidgetHostViewOSRExt()->focus_rect_height_));
  }
#endif
  bool is_need_reset_ime_listener = false;
  if (state) {
    int32_t current_node_id = state->node_id;
    LOG(DEBUG) << "CefRenderWidgetHostViewOSR:: node->ID " << state->node_id <<
      ", node_id_ " << node_id_;
    if (current_node_id != node_id_) {
      is_need_reset_ime_listener = true;
      node_id_ = current_node_id;
    }
  }
  if (state && state->type != ui::TEXT_INPUT_TYPE_NONE) {
    // The last definition of the enumeration in src/cef/include/internal/cef_types.h was modified to 
    // clearly indicate "the number of values," making the semantics clearer. However, the corresponding 
    // enumeration in src/ui/base/ime/text_input_mode.h has not been modified, 
    // so it is necessary to subtract one to match the difference.
    static_assert(
        static_cast<int>(ArkWebRenderHandlerExt::TextInputMode::CEF_TEXT_INPUT_MODE_NUM_VALUES) - 1 ==
            static_cast<int>(ui::TextInputMode::TEXT_INPUT_MODE_MAX),
        "Enum values in cef_text_input_mode_t must match ui::TextInputMode");
    static_assert(
        static_cast<int>(ArkWebRenderHandlerExt::TextInputType::CEF_TEXT_INPUT_TYPE_MAX) ==
            static_cast<int>(ui::TextInputType::TEXT_INPUT_TYPE_MAX),
        "Enum values in cef_text_input_type_t must match ui::TextInputType");
    static_assert(
        static_cast<int>(ArkWebRenderHandlerExt::TextInputAction::CEF_TEXT_INPUT_ACTION_MAX) ==
            static_cast<int>(ui::TextInputAction::kMaxValue),
        "Enum values in cef_text_input_action_t must match ui::TextInputAction");
    static_assert(
        static_cast<int>(ArkWebRenderHandlerExt::TextInputFlags::CEF_TEXT_INPUT_FLAG_VERTICAL) ==
            static_cast<int>(ui::TextInputFlags::TEXT_INPUT_FLAG_VERTICAL),
        "Enum values in cef_text_input_flags_t must match ui::TextInputFlags");
    mode = static_cast<ArkWebRenderHandlerExt::TextInputMode>(state->mode);
    type = state->flags & ui::TEXT_INPUT_FLAG_HAS_BEEN_PASSWORD
               ? CEF_TEXT_INPUT_TYPE_PASSWORD
               : static_cast<ArkWebRenderHandlerExt::TextInputType>(state->type);
    action = static_cast<ArkWebRenderHandlerExt::TextInputAction>(state->action);
    // flags is used as bitfield 
    // state->flags may contains multiple ArkWebRenderHandlerExt::TextInputFlag
    flags = static_cast<ArkWebRenderHandlerExt::TextInputFlags>(state->flags);
    show_keyboard = state->show_ime_if_needed;
  }
  if (state && !state->show_ime_if_needed && did_update_state) {
    LOG(INFO) << "Autofocus requires a keyboard to be bound, "
                 "but there is no need to pull up the keyboard";
    // TODO(OHOS) Specific implementation needs to be completed
    LOG(DEBUG) << "CefRenderWidgetHostViewOSR:: OnVirtualKeyboardRequested update state";
    ArkWebRenderHandlerExt::TextInputInfo input_info = {
        .node_id = state->node_id,
        .show_keyboard = state->show_ime_if_needed,
        .input_mode = mode,
        .input_type = type,
        .input_action = action,
        .input_flags = flags,
        .always_hide_ime = state->always_hide_ime};
    handler->OnVirtualKeyboardRequestedEx(browser_impl_->GetBrowser(), input_info,
                                        is_need_reset_ime_listener,
                                        text_input_attributes);
    OnUpdateTextInputStateCalledInner(state);
    return;
  }
  if (!show_keyboard) {
    last_key_code_ = -1;
  }

  if (state && state->show_ime_if_needed) {
    LOG(DEBUG) << "CefRenderWidgetHostViewOSR:: OnVirtualKeyboardRequested show_ime_if_needed";
    // TODO(OHOS) Specific implementation needs to be completed
    ArkWebRenderHandlerExt::TextInputInfo input_info = {
        .node_id = state->node_id,
        .show_keyboard = show_keyboard,
        .input_mode = mode,
        .input_type = type,
        .input_action = action,
        .input_flags = flags,
        .always_hide_ime = state->always_hide_ime};
    handler->OnVirtualKeyboardRequestedEx(browser_impl_->GetBrowser(), input_info,
                                        is_need_reset_ime_listener,
                                        text_input_attributes);
  } else if ((!state || state->node_id == 0) && (HasFocus())) {
    LOG(DEBUG) << "CefRenderWidgetHostViewOSR:: OnVirtualKeyboardRequested not editable";
    ArkWebRenderHandlerExt::TextInputInfo input_info;
    handler->OnVirtualKeyboardRequestedEx(browser_impl_->GetBrowser(), input_info,
                                        is_need_reset_ime_listener,
                                        text_input_attributes);
  }
  OnUpdateTextInputStateCalledInner(state);
}

void ArkWebRenderWidgetHostViewOSRExt::KeyboardReDispatch(const input::NativeWebKeyboardEvent& event, bool isUsed) {
  if (browser_impl_ && browser_impl_->GetClient()) {
    CefKeyEvent cef_event;
    GetCefKeyEvent(event, cef_event);
    CefRefPtr<CefKeyboardHandler> handler =
        browser_impl_->GetClient()->GetKeyboardHandler();
    if (handler.get()) {
      handler->KeyboardReDispatch(cef_event, isUsed);
    }
  }
}

void ArkWebRenderWidgetHostViewOSRExt::UpdateSecurityLayer(bool isNeedSecurityLayer) {
  if (browser_impl_ && browser_impl_->GetClient()) {
    CefRefPtr<ArkWebRenderHandlerExt> handler =
        browser_impl_->GetClient()->GetRenderHandler();
    if (handler.get()) {
      handler->UpdateSecurityLayer(isNeedSecurityLayer);
    }
  }
}

void ArkWebRenderWidgetHostViewOSRExt::UpdateTextFieldStatus(bool isShowKeyboard, bool isAttachIME) {
  if (browser_impl_ && browser_impl_->GetClient()) {
    CefRefPtr<ArkWebRenderHandlerExt> handler =
        browser_impl_->GetClient()->GetRenderHandler();
    if (handler.get()) {
      handler->UpdateTextFieldStatus(isShowKeyboard, isAttachIME);
    }
  }
}
#endif //BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_ZOOM)
bool ArkWebRenderWidgetHostViewOSRExt::RequiresDoubleTapGestureEvents() const {
  return base::CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kDoubleTapSupportForPlatformEnabled);
}
#endif

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
#if BUILDFLAG(ARKWEB_SYNC_RENDER)
void ArkWebRenderWidgetHostViewOSRExt::SetDrawRect(const gfx::Rect& rect) {
  if (auto compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
          browser_impl_->GetAcceleratedWidget(is_popup_))) {
    compositor->Utils()->SetDrawRect(rect);
    UpdateDrawRect(rect);
  }
}

void ArkWebRenderWidgetHostViewOSRExt::UpdateDrawRect(const gfx::Rect& rect) {
  if (!software_compositor_) {
    LOG(ERROR) << "software compositor is null when DrawRect";
    return;
  }
  software_compositor_->DrawRect(rect);
}

void ArkWebRenderWidgetHostViewOSRExt::SetDrawMode(int mode) {
  if (auto compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
          browser_impl_->GetAcceleratedWidget(is_popup_))) {
    compositor->Utils()->SetDrawMode(mode);
  }
}
#endif
void ArkWebRenderWidgetHostViewOSRExt::SetFitContentMode(int mode) {
  is_fit_content_ = mode;
}

bool ArkWebRenderWidgetHostViewOSRExt::GetPendingSizeStatus() {
  return false;
}

void ArkWebRenderWidgetHostViewOSRExt::SetShouldFrameSubmissionBeforeDraw(
    bool should) {
  TRACE_EVENT0(
      "base",
      "ArkWebRenderWidgetHostViewOSRExt::SetShouldFrameSubmissionBeforeDraw");
  should_wait_ = should;
}

void ArkWebRenderWidgetHostViewOSRExt::SendCurrentLanguage(
    const std::string& ans) {
  language_ = ans;
}
#endif

#if BUILDFLAG(IS_ARKWEB)
void ArkWebRenderWidgetHostViewOSRExt::SetDoubleTapSupportEnabled(
    bool enabled) {
  gesture_provider_.SetDoubleTapSupportForPlatformEnabled(enabled);
}

void ArkWebRenderWidgetHostViewOSRExt::SetMultiTouchZoomSupportEnabled(
    bool enabled) {
  gesture_provider_.SetMultiTouchZoomSupportEnabled(enabled);
}
#endif

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
void ArkWebRenderWidgetHostViewOSRExt::WasOccluded() {
  Hide();
}
 
void ArkWebRenderWidgetHostViewOSRExt::SetEnableLowerFrameRate(bool enabled) {
  if (browser_impl_.get() && browser_impl_->GetAcceleratedWidget(is_popup_)) {
    ui::Compositor* compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
        browser_impl_->GetAcceleratedWidget(is_popup_));
    if (compositor) {
      compositor->Utils()->SetEnableLowerFrameRate(enabled);
    }
  }
}

void ArkWebRenderWidgetHostViewOSRExt::SetEnableHalfFrameRate(bool enabled) {
  if (browser_impl_.get() && browser_impl_->GetAcceleratedWidget(is_popup_)) {
    ui::Compositor* compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
        browser_impl_->GetAcceleratedWidget(is_popup_));
    if (compositor) {
      compositor->Utils()->SetEnableHalfFrameRate(enabled);
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
void ArkWebRenderWidgetHostViewOSRExt::OnSafeInsetsChange(
    const gfx::Insets& safe_insets) {
  float ratio = 1.f;
  auto browser = browser_impl()->GetBrowser();
  if (browser != nullptr && browser->GetHost() != nullptr) {
    ratio = browser->GetHost()->GetVirtualPixelRatio();
    if (ratio <= 0) {
      LOG(ERROR) << __func__ << " get ratio invalid: " << ratio;
      return;
    }
  }
  if (auto rvh_delegate_view = host()->delegate()->GetDelegateView()) {
    rvh_delegate_view->OnSafeInsetsChange(
        gfx::ScaleToCeiledInsets(safe_insets, 1.f / ratio));
  }
}
#endif

#if BUILDFLAG(ARKWEB_AI)
std::vector<int8_t> ArkWebRenderWidgetHostViewOSRExt::GetWordSelection(
    const std::string& text,
    int8_t offset) {
  CefPoint temp(-1, -1);
  if (browser_impl_.get()) {
    CefRefPtr<ArkWebRenderHandlerExt> handler =
        browser_impl_->client()->GetRenderHandler();
    CHECK(handler);
    handler->GetWordSelection(browser_impl_.get(), text, offset, temp);
  }
  std::vector<int8_t> select = {temp.x, temp.y};
  return select;
}

void ArkWebRenderWidgetHostViewOSRExt::CreateOverlay(
    const gfx::ImageSkia& image,
    const gfx::Rect& image_rect,
    const gfx::Point& touch_point) {
  if (is_scrolling_) {
    OnDestroyImageAnalyzerOverlay();
    return;
  }
  CefRefPtr<ArkWebRenderHandlerExt> handler =
      browser_impl_->client()->GetRenderHandler();

  if (handler.get()) {
    CefRefPtr<CefImage> cef_image(new CefImageImpl(image));
    CefRect cef_image_rect(image_rect.x(), image_rect.y(), image_rect.width(),
                           image_rect.height());
    CefPoint cef_touch_point(touch_point.x(), touch_point.y());
    LOG(INFO) << "ArkWebRenderWidgetHostViewOSRExt::CreateOverlay";
    handler->CreateOverlay(browser_impl_, cef_image, cef_image_rect,
                           cef_touch_point);
  }
}

void ArkWebRenderWidgetHostViewOSRExt::OnTextSelected(bool flag) {
  if (flag) {
    gesture_provider_.OnAITextSelected();
  }
  if (render_widget_host_) {
    render_widget_host_->AsRenderWidgetHostImplExt()->OnTextSelected(flag);
    overlay_in_progress_ = flag;
  }
}

void ArkWebRenderWidgetHostViewOSRExt::OnDestroyImageAnalyzerOverlay() {
  overlay_in_progress_ = false;
  if (render_widget_host_) {
    render_widget_host_->AsRenderWidgetHostImplExt()->OnDestroyImageAnalyzerOverlay();
  }
}

float ArkWebRenderWidgetHostViewOSRExt::GetPageScaleFactor() {
  return page_scale_factor_;
}

void ArkWebRenderWidgetHostViewOSRExt::OnFoldStatusChanged(
    uint32_t foldstatus) {
  if (render_widget_host_) {
    render_widget_host_->AsRenderWidgetHostImplExt()->OnFoldStatusChanged(foldstatus);
  }
}

void ArkWebRenderWidgetHostViewOSRExt::CloseImageOverlaySelection() {
  if (!overlay_in_progress_)
    return;
  if (browser_impl_ && browser_impl_->GetClient()) {
    CefRefPtr<CefContextMenuHandlerExt> handler =
        browser_impl_->GetClient()->GetContextMenuHandler();
    if (handler) {
      bool result = handler->CloseImageOverlaySelection();
      if (result) {
        LOG(INFO) << "CefRenderWidgetHostViewOSR::CloseImageOverlaySelection success";
        OnDestroyImageAnalyzerOverlay();
      }
    }
  }
  return false;
}

void ArkWebRenderWidgetHostViewOSRExt::OnOverlayStateChanged(const gfx::Rect& image_rect) {
  LOG(DEBUG) << "ArkWebRenderWidgetHostViewOSRExt::OnOverlayStateChanged";
  if (browser_impl_.get()) {
    CefRefPtr<ArkWebRenderHandlerExt> handler =
        browser_impl_->client()->GetRenderHandler();
    if (handler && render_widget_host_ && overlay_in_progress_) {
      CefRect cef_image_rect(image_rect.x(), image_rect.y(), image_rect.width(), image_rect.height());
      if (cef_image_rect.IsEmpty()) {
        overlay_in_progress_ = false;
      }
      handler->OnOverlayStateChanged(browser_impl_.get(), cef_image_rect);
    }
  }
}

void ArkWebRenderWidgetHostViewOSRExt::NotifyOverlayStateChanged() {
  LOG(DEBUG) << "ArkWebRenderWidgetHostViewOSRExt::NotifyOverlayStateChanged";
  if (render_widget_host_ && overlay_in_progress_) {
    render_widget_host_->AsRenderWidgetHostImplExt()->NotifyOverlayStateChanged();
  }
}

bool ArkWebRenderWidgetHostViewOSRExt::SetDataDetectorSelectText(const std::u16string& text) {
  if (text.empty()) {
    data_detector_select_text_ = std::u16string();
    return true;
  }
  bool ret = false;
  if (browser_impl_ && browser_impl_->client()) {
    CefRefPtr<ArkWebRenderHandlerExt> handler =
        browser_impl_->client()->GetRenderHandler();
    if (handler) {
      ret = handler->GetDataDetectorEnable();
    }
  }
  ret = ret && text.size() <= kMaxDataDetectorTextLength;
  data_detector_select_text_ = ret ? text : std::u16string();
  return ret;
}

std::string ArkWebRenderWidgetHostViewOSRExt::GetDataDetectorSelectText() {
  return base::UTF16ToUTF8(data_detector_select_text_);
}

void ArkWebRenderWidgetHostViewOSRExt::OnDataDetectorSelectText() {
  LOG(DEBUG) << "ArkWebRenderWidgetHostViewOSRExt::OnDataDetectorSelectText";
  if (render_widget_host_) {
    render_widget_host_->AsRenderWidgetHostImplExt()->OnDataDetectorSelectText();
  }
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
void ArkWebRenderWidgetHostViewOSRExt::UpdateVSyncFrequency() {
  if (browser_impl_ && browser_impl_->GetAcceleratedWidget(is_popup)) {
    ui::Compositor* compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
        browser_impl_->GetAcceleratedWidget(is_popup));
    if (compositor) {
      compositor->Utils()->UpdateVSyncFrequency();
    }
  }
}

void ArkWebRenderWidgetHostViewOSRExt::ResetVSyncFrequency() {
  if (browser_impl_ && browser_impl_->GetAcceleratedWidget(is_popup)) {
    ui::Compositor* compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
        browser_impl_->GetAcceleratedWidget(is_popup));
    if (compositor) {
      compositor->Utils()->ResetVSyncFrequency();
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_MENU)
void ArkWebRenderWidgetHostViewOSRExt::OnTouchSelectionChanged(
    const CefTouchHandleState& insert_handle,
    const CefTouchHandleState& start_selection_handle,
    const CefTouchHandleState& end_selection_handle,
    bool need_report) {
  if (!browser_impl_) {
    return;
  }
  CefRefPtr<CefRenderHandler> handler =
      browser_impl_->client()->GetRenderHandler();
  if (handler) {
    handler->OnTouchSelectionChanged(insert_handle, start_selection_handle,
                                     end_selection_handle, need_report);
  }
}

bool ArkWebRenderWidgetHostViewOSRExt::NeedPopupInsertTouchHandleQuickMenu() {
  if (selection_controller_client_) {
    selection_controller_client_->AsArkWebTouchSelectionControllerClientOSRExt()
        ->NeedPopupInsertTouchHandleQuickMenu();
  }
  return false;
}

std::u16string ArkWebRenderWidgetHostViewOSRExt::GetSelectedText() {
  if (text_input_manager_) {
    return text_input_manager_->GetTextSelection(this)->selected_text();
  }
  return std::u16string();
}

std::u16string ArkWebRenderWidgetHostViewOSRExt::GetText() {
  if (text_input_manager_) {
    return text_input_manager_->GetTextSelection(this)->text();
  }
  return std::u16string();
}

void ArkWebRenderWidgetHostViewOSRExt::ResetGestureDetection(
    bool is_lost_focus) {
  const ui::MotionEvent* current_down_event =
      gesture_provider_.GetCurrentDownEvent();
  if (!current_down_event) {
    // A hard reset ensures prevention of any timer-based events that might fire
    // after a touch sequence has ended.
    gesture_provider_.ResetDetection();
    return;
  }

  std::unique_ptr<ui::MotionEvent> cancel_event = current_down_event->Cancel();
  cancel_event->SetCancelByLostFocus(is_lost_focus);
  if (gesture_provider_.OnTouchEvent(*cancel_event).succeeded) {
    bool causes_scrolling = false;
    // 114 ui::LatencyInfo latency_info(ui::SourceEventType::TOUCH);
    ui::LatencyInfo latency_info{};
    latency_info.AddLatencyNumber(ui::INPUT_EVENT_LATENCY_UI_COMPONENT);
    blink::WebTouchEvent web_event = ui::CreateWebTouchEventFromMotionEvent(
        *cancel_event, causes_scrolling /* may_cause_scrolling */,
        false /* hovering */);
    if (ShouldRouteEvents()) {
      host()->delegate()->GetInputEventRouter()->RouteTouchEvent(
          this, &web_event, latency_info);
    } else {
      host()->AsRenderWidgetHostImplExt()->ForwardTouchEventWithLatencyInfo(web_event, latency_info);
    }
  }
}

void ArkWebRenderWidgetHostViewOSRExt::OnTextSelectionChanged(
    content::TextInputManager* text_input_manager,
    RenderWidgetHostViewBase* updated_view) {
  if (!text_input_manager || !updated_view) {
    LOG(ERROR) << "OnTextSelectionChanged text is null";
    return;
  }
  const content::TextInputManager::TextSelection& selection =
      *text_input_manager->GetTextSelection(updated_view);
  if (!browser_impl_ || !browser_impl_->GetClient()) {
    LOG(ERROR) << "OnTextSelectionChanged get client failed";
    return;
  }
  CefRefPtr<CefRenderHandler> handler =
      browser_impl_->GetClient()->GetRenderHandler();
  CHECK(handler);
  if (!selection.selected_text().empty() && !selection.range().is_empty()) {
    handler->OnTextSelectionChanged(
        browser_impl_.get(), selection.selected_text(),
        CefRange(selection.range().start(), selection.range().end()));
  } else {
    LOG(INFO) << "OnTextSelectionChanged selected_text is null";
  }
}

void ArkWebRenderWidgetHostViewOSRExt::OnSelectAreaChanged(
    CefRect& select_area,
    bool need_report) {
  if (!browser_impl_ || !browser_impl_->GetClient()) {
    return;
  }
  CefRefPtr<ArkWebRenderHandlerExt> handler =
    browser_impl_->GetClient()->GetRenderHandler();
  if (handler) {
    handler->OnSelectAreaChanged(select_area);
  }
}
#endif

#if BUILDFLAG(IS_ARKWEB)
void ArkWebRenderWidgetHostViewOSRExt::
    OnRenderFrameMetadataChangedBeforeActivation(
        const cc::RenderFrameMetadata& metadata) {
#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
  if (overscroll_controller_) {
    overscroll_controller_->OnFrameMetadataUpdated(
        metadata.page_scale_factor, metadata.device_scale_factor,
        metadata.scrollable_viewport_size, metadata.root_layer_size,
        metadata.root_scroll_offset.value_or(gfx::PointF()),
        metadata.root_overflow_y_hidden);
  }
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  float top_content_offset = metadata.top_controls_height *
                             metadata.top_controls_shown_ratio /
                             metadata.device_scale_factor;
  float top_controls_offset =
      top_content_offset -
      metadata.top_controls_height / metadata.device_scale_factor;

  if (!pull_to_refreshing_ && (top_content_offset != top_content_offset_ ||
                               top_controls_offset != top_controls_offset_)) {
    top_content_offset_ = top_content_offset;
    top_controls_offset_ = top_controls_offset;
    OnTopControlsChanged(top_controls_offset_, top_content_offset_);
  }

  if (enable_nweb_ex_) {
    // Set parameters for adaptive handle orientation.
    gfx::SizeF viewport_size(metadata.scrollable_viewport_size);
    viewport_size.Scale(page_scale_factor_);
    gfx::RectF viewport_rect(0.0f,
                             metadata.top_controls_height *
                                 metadata.top_controls_shown_ratio /
                                 metadata.device_scale_factor,
                             viewport_size.width(), viewport_size.height());
    selection_controller_->OnViewportChanged(viewport_rect);
  }
#endif
  gesture_provider_.SetDoubleTapSupportForPageEnabled(
      !metadata.is_mobile_optimized);
}

void ArkWebRenderWidgetHostViewOSRExt::
    OnRenderFrameMetadataChangedAfterActivation(
        base::TimeTicks activation_time) {
  auto metadata =
      host_->render_frame_metadata_provider()->LastRenderFrameMetadata();

  if (video_consumer_) {
    // Need to wait for the first frame of the new size before calling
    // SizeChanged. Otherwise, the video frame will be letterboxed.
    video_consumer_->SizeChanged(metadata.viewport_size_in_pixels);
  }

  gfx::PointF root_scroll_offset;
  if (metadata.root_scroll_offset) {
    root_scroll_offset = *metadata.root_scroll_offset;
  }
  if (root_scroll_offset != last_scroll_offset_) {
    last_scroll_offset_ = root_scroll_offset;

    if (!is_scroll_offset_changed_pending_) {
      is_scroll_offset_changed_pending_ = true;

      // Send the notification asynchronously.
      CEF_POST_TASK(
          CEF_UIT,
          base::BindOnce(&CefRenderWidgetHostViewOSR::OnScrollOffsetChanged,
                         weak_ptr_factory_.GetWeakPtr()));
    }
  }

#if BUILDFLAG(IS_ARKWEB)
  gfx::SizeF root_layer_size = metadata.root_layer_size;
  if (root_layer_size != root_layer_size_) {
    root_layer_size_ = root_layer_size;

    // Send the notification asynchronously.
#ifdef DISABLE_GPU
    CEF_POST_TASK(
        CEF_UIT, base::BindOnce(&ArkWebRenderWidgetHostViewOSRExt::OnRootLayerChanged,
                                weak_ptr_factory_.GetWeakPtr()));
#else
    CEF_POST_TASK(
        CEF_UIT, base::BindOnce(&ArkWebRenderWidgetHostViewOSRExt::OnRootLayerChanged,
                                weak_ptr_factory_.GetWeakPtr()));
#endif
  }
  bool size_changed = false;
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  gfx::SizeF scrollable_viewport_size = metadata.scrollable_viewport_size;
  if (scrollable_viewport_size != scrollable_viewport_size_) {
    scrollable_viewport_size_ = scrollable_viewport_size;
    size_changed = true;
  }
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  if (size_changed && needFocusViewport_ > 0) {
    CefRefPtr<ArkWebRenderHandlerExt> handler =
        browser_impl_->GetClient()->GetRenderHandler();
    if (handler) {
      needFocusViewport_--;
      handler->OnResizeScrollableViewport(browser_impl_->GetBrowser());
    }
  }
#endif

  gfx::Size viewport_size_in_pixels = metadata.viewport_size_in_pixels;
  float device_scale_factor = metadata.device_scale_factor;
  if (viewport_size_in_pixels != viewport_size_in_pixels_ ||
      device_scale_factor != device_scale_factor_) {
    TRACE_EVENT2("cef",
                 "CefRenderWidgetHostViewOSR::"
                 "OnRenderFrameMetadataChangedAfterActivation",
                 "viewport_size_in_pixels", viewport_size_in_pixels.ToString(),
                 "device_scale_factor", device_scale_factor);
    viewport_size_in_pixels_ = viewport_size_in_pixels;
    device_scale_factor_ = device_scale_factor;
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&CefRenderWidgetHostViewOSR::ReleaseResizeHold,
                                 weak_ptr_factory_.GetWeakPtr()));
  }

  if (!page_scale_factor_) {
    // set init page scale factor.
    page_scale_factor_ = metadata.page_scale_factor;
#if BUILDFLAG(ARKWEB_SCROLLBAR)
    if (browser_impl_.get()) {
      CefRefPtr<CefDisplayHandler> handler =
          browser_impl_->client()->GetDisplayHandler();
      CHECK(handler);
      handler->OnScaleInited(
          browser_impl_.get(),
          std::round(page_scale_factor_ * SCALE_FACTOR_CONVERT_RATIO));
    }
    return;
#endif
  }
  float new_page_scale_factor = metadata.page_scale_factor;
  if (new_page_scale_factor != page_scale_factor_) {
    float old_page_scale_factor = page_scale_factor_;
    page_scale_factor_ = new_page_scale_factor;
    // Send the notification asynchronously.
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&ArkWebRenderWidgetHostViewOSRExt::OnScaleChanged,
                                 weak_ptr_factory_.GetWeakPtr(),
                                 old_page_scale_factor, new_page_scale_factor));
  }
#endif

  if (metadata.selection.start != selection_start_ ||
      metadata.selection.end != selection_end_ ||
      metadata.selection.start.type() == gfx::SelectionBound::CENTER) {
    selection_start_ = metadata.selection.start;
    selection_end_ = metadata.selection.end;
    selection_controller_client_->UpdateClientSelectionBounds(selection_start_,
                                                              selection_end_);
  }

#if BUILDFLAG(ARKWEB_MENU)
  if (clipped_selection_bounds_ != metadata.clipped_selection_bounds) {
    clipped_selection_bounds_ = metadata.clipped_selection_bounds;
    selection_controller_client_->UpdateClientClippedSelectionBounds(
        clipped_selection_bounds_);
  }
#endif
}
#endif  // BUILDFLAG(IS_ARKWEB)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void ArkWebRenderWidgetHostViewOSRExt::SelectionChanged(
    const std::u16string& text,
    size_t offset,
    const gfx::Range& range) {
  RenderWidgetHostViewBase::SelectionChanged(text, offset, range);

  if (!browser_impl_.get()) {
    return;
  }
  CefRefPtr<CefRenderHandler> handler =
      browser_impl_->GetClient()->GetRenderHandler();
  CHECK(handler);

  CefRange cef_range(range.start(), range.end());

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  handler->AsArkWebRenderHandler()->OnSelectionChanged(browser_impl_.get(),
                                                       text, cef_range);
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

  CefString selected_text;
  if (!range.is_empty() && !text.empty()) {
    size_t pos = range.GetMin() - offset;
    size_t n = range.length();
    if (pos + n <= text.length()) {
      selected_text = text.substr(pos, n);
    }
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
    is_select_text_ = n - pos > 0;
    if (n > 0 && is_event_from_touch_ && !is_tap_down_twice_) {
      handler->AsArkWebRenderHandler()->StartVibraFeedback("longPress.light");
    }
    is_tap_down_twice_ = false;
  } else {
    is_select_text_ = false;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
  }

  handler->OnTextSelectionChanged(browser_impl_.get(), selected_text,
                                  cef_range);
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
blink::mojom::PointerLockResult ArkWebRenderWidgetHostViewOSRExt::LockPointer(
    bool request_unadjusted_movement) {
  if (IsPointerLocked()) {
    return blink::mojom::PointerLockResult::kAlreadyLocked;
  }

  if (!HasFocus()) {
    LOG(ERROR) << "SetPointerLock failed because WrongDocument";
    return blink::mojom::PointerLockResult::kWrongDocument;
  }

  CefRefPtr<ArkWebDisplayHandlerExt> handler =
      browser_impl_->client()->GetDisplayHandler();

  if (!handler) {
    return blink::mojom::PointerLockResult::kPermissionDenied;
  }

  int32_t res = OHOS::NWeb::WindowManagerAdapterImpl::LockPointer(handler->GetWindowId());
  switch (res) {
    case WindowManager_ErrorCode::OK: {
      LOG(INFO) << "SetPointerLock unadjust mouse movement is "
                << (request_unadjusted_movement ? "on" : "off");
      is_pointer_locked_ = true;
      CefCursorInfo cursor_info;
      handler->OnCursorChange(browser_impl_->GetBrowser(), nullptr, CT_LOCK,
                              cursor_info);
      is_request_unadjusted_movement_ = request_unadjusted_movement;
      return blink::mojom::PointerLockResult::kSuccess;
    }
    case WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_NO_PERMISSION:
      return blink::mojom::PointerLockResult::kPermissionDenied;
    case WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED:
    case WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_STATE_ABNORMAL:
    case WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_SYSTEM_ABNORMAL:
    default:
      return blink::mojom::PointerLockResult::kUnknownError;
  }
}

blink::mojom::PointerLockResult
ArkWebRenderWidgetHostViewOSRExt::ChangePointerLock(
    bool request_unadjusted_movement) {
  LOG(INFO) << "ChangePointerLock unadjust mouse movement to "
            << (request_unadjusted_movement ? "on" : "off")
            << ", Lock state is " << is_pointer_locked_;
  if (is_pointer_locked_) {
    is_request_unadjusted_movement_ = request_unadjusted_movement;
    return blink::mojom::PointerLockResult::kSuccess;
  }
  return blink::mojom::PointerLockResult::kWrongDocument;
}

void ArkWebRenderWidgetHostViewOSRExt::UnlockPointer() {
  if (!is_pointer_locked_) {
    return;
  }
  is_pointer_locked_ = false;
  is_request_unadjusted_movement_ = false;
  CefRefPtr<ArkWebDisplayHandlerExt> handler =
      browser_impl_->client()->GetDisplayHandler();

  if (!handler) {
    LOG(ERROR)<<"UnLockPointer failed, no displayHandler";
    return;
  }

  OHOS::NWeb::WindowManagerAdapterImpl::UnlockPointer(handler->GetWindowId());
  CefCursorInfo cursor_info;
  handler->OnCursorChange(browser_impl_->GetBrowser(), nullptr, CT_UNLOCK,
                          cursor_info);

  if (render_widget_host_) {
    render_widget_host_->LostPointerLock();
  }
}

bool ArkWebRenderWidgetHostViewOSRExt::IsPointerLocked() {
  return is_pointer_locked_;
}
#endif

void ArkWebRenderWidgetHostViewOSRExt::SelectionBoundsChanged(
    const gfx::Rect& anchor_rect,
    base::i18n::TextDirection anchor_dir,
    const gfx::Rect& focus_rect,
    base::i18n::TextDirection focus_dir,
    const gfx::Rect& bounding_box,
    bool is_anchor_first) {
  if (!browser_impl_) {
    LOG(ERROR) << "browser_impl_ is nullptr";
    return;
  }

  focus_rect_x_ = focus_rect.x();
  focus_rect_y_ = focus_rect.y();
  focus_rect_width_ = focus_rect.width();
  focus_rect_height_ = focus_rect.height();

  CefRefPtr<CefRenderHandler> handler =
      browser_impl_->GetClient()->GetRenderHandler();
  CHECK(handler);

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  handler->AsArkWebRenderHandler()->SelectionBoundsChanged(
      CefRect(anchor_rect.x(), anchor_rect.y(), anchor_rect.width(),
              anchor_rect.height()),
      CefRect(focus_rect.x(), focus_rect.y(), focus_rect.width(),
              focus_rect.height()),
      is_anchor_first);
#endif

  if (!is_select_text_) {
    handler->AsArkWebRenderHandler()->OnCursorUpdate(
        browser_impl_->GetBrowser(),
        CefRect(focus_rect_x_, focus_rect_y_, focus_rect_width_,
                focus_rect_height_));
    return;
  }

  if (UpdateEditBounds()) {
    auto processedOffset = HandleCursorOffset();
    handler->AsArkWebRenderHandler()->OnCursorUpdate(
        browser_impl_->GetBrowser(),
        CefRect(processedOffset.first, processedOffset.second,
                focus_rect_width_, focus_rect_height_));
  }
}

bool ArkWebRenderWidgetHostViewOSRExt::UpdateEditBounds() {
  if (text_input_manager_ && text_input_manager_->GetTextInputState()) {
    auto state = text_input_manager_->GetTextInputState();
    if (!state || !state->edit_context_control_bounds) {
      return false;
    }
    CHECK(state);
    CHECK(state->edit_context_control_bounds);
    edit_bounds_x_ = state->edit_context_control_bounds->x();
    edit_bounds_y_ = state->edit_context_control_bounds->y();
    edit_bounds_width_ = state->edit_context_control_bounds->width();
    edit_bounds_height_ = state->edit_context_control_bounds->height();
    return true;
  }
  return false;
}
void ArkWebRenderWidgetHostViewOSRExt::SendInternalBeginFrame() {
  auto compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
      browser_impl_->GetAcceleratedWidget(is_popup_));
  if (compositor) {
    compositor->Utils()->SendInternalBeginFrame();
  }
}

void ArkWebRenderWidgetHostViewOSRExt::FocusedNodeChanged(
    bool is_editable_node,
    const gfx::Rect& node_bounds_in_screen) {
  CefRefPtr<CefRenderHandler> handler =
      browser_impl_->GetClient()->GetRenderHandler();
  CHECK(handler);
  handler->AsArkWebRenderHandler()->OnEditableChanged(browser_impl_.get(),
                                                      is_editable_node);
  is_editable_node_ = is_editable_node;
}

void ArkWebRenderWidgetHostViewOSRExt::DidOverscroll(
    const ui::DidOverscrollParams& params) {
  if (browser_impl_.get()) {
    CefRefPtr<CefRenderHandler> handler =
        browser_impl_->client()->GetRenderHandler();
    CHECK(handler);
    float x = params.latest_overscroll_delta.x();
    float y = params.latest_overscroll_delta.y();
    handler->AsArkWebRenderHandler()->OnOverscroll(browser_impl_.get(), x, y);

    float fling_velocity_x = params.current_fling_velocity.x();
    float fling_velocity_y = params.current_fling_velocity.y();
    bool is_fling = true;
    if (fling_velocity_x == 0 && fling_velocity_y == 0) {
      fling_velocity_x = x;
      fling_velocity_y = y;
      is_fling = false;
    }
    fling_velocity_x =
        params.accumulated_overscroll.x() == 0 ? 0 : fling_velocity_x;
    fling_velocity_y =
        params.accumulated_overscroll.y() == 0 ? 0 : fling_velocity_y;
    handler->AsArkWebRenderHandler()->OnOverScrollFlingVelocity(
        browser_impl_.get(), fling_velocity_x, fling_velocity_y, is_fling);

#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
    if (overscroll_controller_) {
      overscroll_controller_->OnOverscrolled(params);
    }
#endif
  }
}

void ArkWebRenderWidgetHostViewOSRExt::DidStopFlinging() {
  LOG(DEBUG) << "CefRenderWidgetHostViewOSR::DidStopFlinging";
  if (browser_impl_.get()) {
    CefRefPtr<CefRenderHandler> handler =
        browser_impl_->client()->GetRenderHandler();
    CHECK(handler);
    handler->AsArkWebRenderHandler()->OnOverScrollFlingEnd(browser_impl_.get());
#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
    is_fling_ = false;
#endif
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
    if (browser_impl_) {
      browser_impl_->SetIsFling(false);
    }
#endif
  }
}
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void ArkWebRenderWidgetHostViewOSRExt::OnUpdateTextInputStateCalledInner(
    const ui::mojom::TextInputState* state) {
  if (browser_impl_.get()) {
    CefRefPtr<ArkWebRenderHandlerExt> handler =
        browser_impl_->GetClient()->GetRenderHandler();
    CHECK(handler);
    if (state != nullptr) {
      LOG(DEBUG)
          << "ArkWebRenderWidgetHostViewOSRExt:: state->show_ime_if_needed "
          << state->show_ime_if_needed;
      std::u16string whole_text = state->value.value_or(std::u16string());
      CefRange selection_range(state->selection.start(),
                               state->selection.end());
      CefRange compositon_range = CefRange::InvalidRange();
      if (state->composition) {
        gfx::Range range = state->composition.value();
        compositon_range.Set(range.start(), range.end());
      }
      handler->OnUpdateTextInputStateCalled(browser_impl_.get(), whole_text, selection_range, compositon_range);
    } else {
      CefRenderWidgetHostViewOSR::ImeFinishComposingText(false);
      LOG(DEBUG) << "ArkWebRenderWidgetHostViewOSRExt:: status is nullptr, "
                    "finish compositon";
    }
  }
}

void ArkWebRenderWidgetHostViewOSRExt::GestureEventAck(
    const blink::WebGestureEvent& event,
    blink::mojom::InputEventResultSource ack_source,
    blink::mojom::InputEventResultState ack_result) {
  LOG(INFO)
      << "ArkWebRenderWidgetHostViewOSRExt::GestureEventAck ack_result:"
      << (int)ack_result;

  if (overscroll_controller_) {
    overscroll_controller_->OnGestureEventAck(event, ack_result);
  }

  StopFlingingIfNecessary(event, ack_result);

  ForwardTouchpadZoomEventIfNecessary(event, ack_result);
}
#endif

#if BUILDFLAG(ARKWEB_SCROLLBAR)
blink::mojom::InputEventResultState
ArkWebRenderWidgetHostViewOSRExt::FilterInputEvent(
    const blink::WebInputEvent& input_event) {
  blink::mojom::EventType inputType = input_event.GetType();    
  LOG(DEBUG) << "CefRenderWidgetHostViewOSR::FilterInputEvent type: " << inputType;

  if (input_event.IsTouchEventType(inputType)) {
    is_event_from_touch_ = true;
  } else if (inputType == blink::WebInputEvent::Type::kMouseDown) {
    is_event_from_touch_ = false;
  }

#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
  if (FilterInputEventForPullToRefresh(input_event)) {
    return blink::mojom::InputEventResultState::kConsumed;
  }
#endif

  if (!scroll_enabled_ &&
      input_event.GetType() ==
          blink::WebInputEvent::Type::kGestureScrollUpdate) {
    LOG(DEBUG) << "can not GestureScroll, scroll is disabled";
    return blink::mojom::InputEventResultState::kConsumed;
  }

  if (input_event.GetType() == blink::WebInputEvent::Type::kMouseWheel) {
    is_mouse_wheel_scroll_ = true;
  }

  if (browser_impl_.get() && input_event.IsGestureScroll()) {
    CefRefPtr<CefRenderHandler> handler =
        browser_impl_->client()->GetRenderHandler();
    CHECK(handler);
    blink::WebGestureEvent gesture_event =
        static_cast<const blink::WebGestureEvent&>(input_event);
    if (input_event.GetType() ==
        blink::WebInputEvent::Type::kGestureScrollBegin) {
#if BUILDFLAG(ARKWEB_AI)
      is_scrolling_ = true;
#endif
      is_scroll_consumed_ = false;
      selection_controller_client_->OnScrollStarted();
      handler->AsArkWebRenderHandler()->OnScrollState(browser_impl_.get(),
                                                      true);
      handler->AsArkWebRenderHandler()->OnScrollStart(
          browser_impl_.get(), gesture_event.data.scroll_begin.delta_x_hint,
          gesture_event.data.scroll_begin.delta_y_hint);
    } else if (input_event.GetType() ==
               blink::WebInputEvent::Type::kGestureScrollEnd) {
#if BUILDFLAG(ARKWEB_AI)
      is_scrolling_ = false;
#endif
      is_scroll_consumed_ = false;
      selection_controller_client_->OnScrollCompleted();
      handler->AsArkWebRenderHandler()->OnScrollState(browser_impl_.get(),
                                                      false);
#if BUILDFLAG(ARKWEB_AI)
      NotifyOverlayStateChanged();
#endif
    } else if (input_event.GetType() ==
                   blink::WebInputEvent::Type::kGestureScrollUpdate &&
               is_mouse_wheel_scroll_) {
#if BUILDFLAG(ARKWEB_AI)
      is_scrolling_ = true;
#endif
      float x = 0.0f, y = 0.0f, xVelocity = 0.0f, yVelocity = 0.0f;
      bool isAvailable = false;
      x = gesture_event.data.scroll_update.delta_x;
      y = gesture_event.data.scroll_update.delta_y;
      is_scroll_consumed_ =
        handler->AsArkWebRenderHandler()->OnNestedScroll(browser_impl_.get(),
                                                         x,
                                                         y, xVelocity, yVelocity, isAvailable);
      
      if (!isAvailable) {
        is_scroll_consumed_ =
          handler->AsArkWebRenderHandler()->FilterScrollEvent(browser_impl_.get(),
                                                              gesture_event.data.scroll_update.delta_x,
                                                              gesture_event.data.scroll_update.delta_y, 0, 0);
      } else {
        const blink::WebGestureEvent& const_gesture_event = static_cast<const blink::WebGestureEvent&>(input_event);
        blink::WebGestureEvent& mutable_gesture = const_cast<blink::WebGestureEvent&>(const_gesture_event);
        if (x != gesture_event.data.scroll_update.delta_x)
          mutable_gesture.data.scroll_update.delta_x = x;
        if (y != gesture_event.data.scroll_update.delta_y)
          mutable_gesture.data.scroll_update.delta_y = y;
      }
      is_mouse_wheel_scroll_ = false;
    }
    return is_scroll_consumed_
               ? blink::mojom::InputEventResultState::kConsumed
               : blink::mojom::InputEventResultState::kNotConsumed;
  }

  return blink::mojom::InputEventResultState::kNotConsumed;
}
#endif

void ArkWebRenderWidgetHostViewOSRExt::OnScrollState(bool scroll_state) {
  if (browser_impl_.get()) {
    CefRefPtr<CefRenderHandler> handler =
        browser_impl_->client()->GetRenderHandler();
    CHECK(handler);
    handler->AsArkWebRenderHandler()->OnScrollState(browser_impl_.get(),
                                                    scroll_state);
  }
}

void ArkWebRenderWidgetHostViewOSRExt::AdvanceFocusForIME(int focusType) {
  LOG(DEBUG) << "CefRenderWidgetHostViewOSR::AdvanceFocusForIME focusType = "
             << focusType;
  content::RenderFrameHostImpl* frame_host = nullptr;
  if (render_widget_host() && render_widget_host()->frame_tree() &&
      render_widget_host()->frame_tree()->GetFocusedFrame()) {
    frame_host = render_widget_host()
                     ->frame_tree()
                     ->GetFocusedFrame()
                     ->current_frame_host();
  }

  if (frame_host && frame_host->GetAssociatedLocalFrame()) {
    frame_host->GetAssociatedLocalFrame()->AdvanceFocusForIME(
        static_cast<blink::mojom::FocusType>(focusType));
  }
}

std::pair<int, int> ArkWebRenderWidgetHostViewOSRExt::HandleCursorOffset() {
  int x = focus_rect_x_;
  int y = focus_rect_y_;
  x = x < edit_bounds_x_ ? edit_bounds_x_ : x;
  x = x > (edit_bounds_x_ + edit_bounds_width_)
          ? (edit_bounds_x_ + edit_bounds_width_)
          : x;
  y = y < edit_bounds_y_ ? edit_bounds_y_ : y;
  y = y > (edit_bounds_y_ + edit_bounds_height_)
          ? (edit_bounds_y_ + edit_bounds_height_)
          : y;
  return std::make_pair(x, y);
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
bool ArkWebRenderWidgetHostViewOSRExt::IsRequestUnadjustedMovement() {
  return is_request_unadjusted_movement_;
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

void ArkWebRenderWidgetHostViewOSRExt::SetScrollable(bool enable) {
  scroll_enabled_ = enable;
}

void ArkWebRenderWidgetHostViewOSRExt::ScrollBy(float delta_x, float delta_y) {
  if (!render_widget_host_) {
    return;
  }
#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  if (condition_) {
    OHOS::NWeb::ResSchedClientAdapter::ReportScene(
        OHOS::NWeb::ResSchedStatusAdapter::WEB_SCENE_ENTER, OHOS::NWeb::ResSchedSceneAdapter::SLIDE);
    float scroll_offset_y = DEFAULT_SCROLL_OFFSET_Y;

    auto browser = browser_impl_->GetBrowser();
    if (browser != nullptr && browser->GetHost() != nullptr) {
      float ratio = browser->GetHost()->GetVirtualPixelRatio();
      if (ratio > 0) {
        scroll_offset_y = std::round(last_scroll_offset_.y() / ratio);
      }
    }

    if (scroll_offset_y >= 0.0 && scroll_offset_y < DEFAULT_PAGE_SCROLL_OFFSET) {
      TRACE_EVENT1("cef", "ArkWebRenderWidgetHostViewOSRExt::Scrollby",
    "scroll_offset_y", scroll_offset_y);
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .CreateSocPerfClientAdapter()
      ->ApplySocPerfConfigByIdEx(OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_SLIDE_NORMAL_ID, true);
    }
  }
#endif
  render_widget_host_->input_router()->ScrollBy(delta_x, delta_y);
}

#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void ArkWebRenderWidgetHostViewOSRExt::SetBypassVsyncCondition(int32_t condition) {
  LOG(INFO) << "ArkWebRenderWidgetHostViewOSRExt::SetBypassVsyncCondition condition:"
            << condition;
  condition_ = condition;
  if (!render_widget_host_) {
    return;
  }
  render_widget_host_->input_router()->SetBypassVsyncCondition(condition);
  if (auto compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
            browser_impl_->GetAcceleratedWidget(is_popup_))) {
    compositor->Utils()->SetBypassVsyncCondition(condition);
  }
}
#endif
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
void ArkWebRenderWidgetHostViewOSRExt::DynamicFrameLossEvent(
    const std::string& sceneId,
    bool isStart) {
  if (browser_impl_.get()) {
    CefRefPtr<CefRenderHandler> handler =
        browser_impl_->client()->GetRenderHandler();
    CHECK(handler);
    handler->AsArkWebRenderHandler()->SendDynamicFrameLossEvent(
        browser_impl_.get(), sceneId, isStart);
  }
}
#endif

#if BUILDFLAG(ARKWEB_DSS)
gfx::Size ArkWebRenderWidgetHostViewOSRExt::SizeInPixels() {
  if (IsPopupWidget()) {
    return gfx::ScaleToCeiledSize(popup_position_.size(),
                                  GetDeviceScaleFactor());
  }

  CefSize size{};
  if (browser_impl_ && browser_impl_->GetClient() &&
      browser_impl_->GetClient()->GetRenderHandler()) {
    auto handler =
        browser_impl_->GetClient()->GetRenderHandler()->AsArkWebRenderHandler();
    CHECK(handler);
    handler->GetDevicePixelSize(browser_impl_.get(), size);
  } else {
    LOG(WARNING) << "cannot get device pixel size, return zero";
  }
  return gfx::Size(size.width, size.height);
}
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
bool ArkWebRenderWidgetHostViewOSRExt::WebPageSnapshot(
    const char* id,
    int width,
    int height,
    cef_web_snapshot_callback_t callback) {
  if (!software_compositor_) {
    LOG(ERROR) << "software compositor is null when get snapshot";
    return false;
  }

  if (!browser_impl_) {
    LOG(ERROR) << "browser is null when get snapshot";
    return false;
  }

  gfx::SizeF pageSize{0.0f, 0.0f};
  gfx::PointF pageOffsize{0.0f, 0.0f};
  if (browser_impl_->settings().record_whole_document) {
    pageSize = root_layer_size_;
    pageOffsize = last_scroll_offset_;
  } else {
    pageSize = scrollable_viewport_size_;
  }

  if (width < 0) {
    width = (std::abs(width) * pageSize.width()) / 100;
  }
  if (height < 0) {
    height = (std::abs(height) * pageSize.height()) / 100;
  }

  pageSize.Scale(page_scale_factor_);
  pageOffsize.Scale(page_scale_factor_);

  software_compositor_->DemandDrawSwAsync(id, width, height, pageSize,
                                          pageOffsize, std::move(callback));
  return true;
}
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
void ArkWebRenderWidgetHostViewOSRExt::OnVsync() {
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void ArkWebRenderWidgetHostViewOSRExt::OnGestureEvent(
    const ui::GestureEventData& gesture) {
  if ((gesture.type() == ui::EventType::kGesturePinchBegin ||
       gesture.type() == ui::EventType::kGesturePinchUpdate ||
       gesture.type() == ui::EventType::kGesturePinchEnd) &&
      !pinch_zoom_enabled_) {
    return;
  }
  FilterScrollEventImpl(gesture);
  SendGestureEvent(gesture);
  SetFocusOnGestureEvent(gesture);
}

void ArkWebRenderWidgetHostViewOSRExt::ScaleGestureChangeV2(int type,
                                                            float scale,
                                                            float originScale,
                                                            float centerX,
                                                            float centerY) {
  LOG(DEBUG) << "ArkWebRenderWidgetHostViewOSRExt::ScaleGestureChangeV2 type: "
             << type << " scale: " << scale << " originScale: " << originScale;
  auto event_type = ui::EventType::kUnknown;
  switch (type) {
    case PINCH_START_TYPE:
      event_type = ui::EventType::kGesturePinchBegin;
      break;
    case PINCH_UPDATE_TYPE:
      event_type = ui::EventType::kGesturePinchUpdate;
      break;
    case PINCH_END_TYPE:
      event_type = ui::EventType::kGesturePinchEnd;
      break;
    default:
      LOG(ERROR) << "ArkWebRenderWidgetHostViewOSRExt::ScaleGestureChangeV2 "
                    "type invalid.";
      return;
  }

  ui::GestureEventDetails details(event_type);
  details.set_device_type(ui::GestureDeviceType::DEVICE_TOUCHPAD);
  details.set_scale(scale);
  OnGestureEvent(ui::GestureEventData(details,
                                      0,
                                      ui::MotionEvent::ToolType::FINGER,
                                      base::TimeTicks::Now(),
                                      centerX,
                                      centerY,
                                      centerX,
                                      centerY,
                                      DEFAULT_PINCH_FINGER,
                                      gfx::RectF(),
                                      0,
                                      0U));
}

void ArkWebRenderWidgetHostViewOSRExt::NotifyVirtualKeyboardOverlayRect(
    const gfx::Rect& keyboard_rect) {
  content::RenderFrameHostImpl* frame_host =
      render_widget_host()->frame_tree()->GetMainFrame();
  if (!frame_host) {
    return;
  }
  frame_host->GetPage().NotifyVirtualKeyboardOverlayRect(keyboard_rect);
}

ui::mojom::VirtualKeyboardMode
ArkWebRenderWidgetHostViewOSRExt::GetVirtualKeyboardMode() {
  // overlaycontent flag can only be set from main frame.
  content::RenderFrameHostImpl* frame_host =
      render_widget_host()->frame_tree()->GetMainFrame();
  if (!frame_host) {
    return ui::mojom::VirtualKeyboardMode::kUnset;
  }
  return frame_host->GetPage().virtual_keyboard_mode();
}

void ArkWebRenderWidgetHostViewOSRExt::SetVirtualKeyBoardArg(int32_t width,
                                                             int32_t height,
                                                             double keyboard) {
  LOG(DEBUG)
      << "ArkWebRenderWidgetHostViewOSRExt::SetVirtualKeyBoardArg ; width = "
      << width << "height = " << height << "keyboard = " << keyboard;
  gfx::Rect keyboard_rect;
  keyboard_rect.set_x(0);
  keyboard_rect.set_y(0);
  keyboard_rect.set_width(width);
  keyboard_rect.set_height(keyboard);
  if (GetVirtualKeyboardMode() !=
      ui::mojom::VirtualKeyboardMode::kOverlaysContent) {
    return;
  }

  gfx::Rect keyboard_rect_with_scale;
  if (!keyboard_rect.IsEmpty()) {
    auto browser = browser_impl_->GetBrowser();
    if (browser != nullptr && browser->GetHost() != nullptr) {
      float ratio = browser->GetHost()->GetVirtualPixelRatio();
      if (ratio <= 0) {
        LOG(ERROR) << __func__ << " get ratio invalid: " << ratio;
        return;
      }
      keyboard_rect_with_scale = ScaleToEnclosedRect(keyboard_rect, 1 / ratio);
      keyboard_rect_with_scale.Intersect(GetViewBounds());
      LOG(DEBUG) << "CefRenderWidgetHostViewOSR::SetVirtualKeyBoardArg"
                 << ",keyboard_rect_with_scale.width"
                 << keyboard_rect_with_scale.width()
                 << ",keyboard_rect_with_scale.height"
                 << keyboard_rect_with_scale.height();
    }
  }
  NotifyVirtualKeyboardOverlayRect(keyboard_rect_with_scale);
}
#endif  // #if BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void ArkWebRenderWidgetHostViewOSRExt::DidNativeEmbedEvent(
    const blink::mojom::NativeEmbedTouchEventPtr& touchEvent) {
  if (browser_impl_.get()) {
    CefRefPtr<ArkWebRenderHandlerExt> handler =
        browser_impl_->client()->GetRenderHandler();
    CHECK(handler);
    ArkWebRenderHandlerExt::CefEmbedTouchEvent event{
        touchEvent->embedId,
        touchEvent->id,
        touchEvent->x,
        touchEvent->y,
        touchEvent->screenX,
        touchEvent->screenY,
        static_cast<ArkWebRenderHandlerExt::CefEmbedTouchType>(
            touchEvent->type),
        touchEvent->offsetX,
        touchEvent->offsetY};
    auto type = static_cast<ArkWebRenderHandlerExt::CefEmbedTouchType>(
        touchEvent->type);
    auto new_callback =
        base::BindOnce(&ArkWebRenderWidgetHostViewOSRExt::SetGestureEventResult,
                       weak_ptr_factory_.GetWeakPtr());
    if (touchEvent->id == NOT_NATIVE_ID) {
      new_callback.Reset();
    }
    // TODO: Re-implement gesture event callback handling for Chromium 141
    // CefRefPtr<CefGestureEventCallbackImpl> callbackPtr(
    //     new CefGestureEventCallbackImpl(std::move(new_callback), touchEvent->id));
    // handler->OnNativeEmbedGestureEvent(browser_impl_.get(), event,
    //                                    callbackPtr.get());
  }
}

void ArkWebRenderWidgetHostViewOSRExt::DidNativeEmbedMouseEvent(
    const blink::mojom::NativeEmbedMouseEventPtr& mouseEvent) {
  if (browser_impl_.get()) {
    CefRefPtr<ArkWebRenderHandlerExt> handler =
        browser_impl_->client()->GetRenderHandler();
    CHECK(handler);
    ArkWebRenderHandlerExt::CefEmbedMouseEvent event{
        mouseEvent->embedId,
        mouseEvent->x,
        mouseEvent->y,
        mouseEvent->screenX,
        mouseEvent->screenY,
        static_cast<ArkWebRenderHandlerExt::CefEmbedMouseType>(
            mouseEvent->type),
        static_cast<ArkWebRenderHandlerExt::CefEmbedMouseButton>(
            mouseEvent->button),
        mouseEvent->isHitNativeArea,
        mouseEvent->offsetX,
        mouseEvent->offsetY};
    auto new_callback =
        base::BindOnce(&ArkWebRenderWidgetHostViewOSRExt::SetMouseEventResult,
                       weak_ptr_factory_.GetWeakPtr());
    // TODO: Re-implement mouse event callback handling for Chromium 141
    // CefRefPtr<CefMouseEventCallbackImpl> callbackPtr(
    //     new CefMouseEventCallbackImpl(std::move(new_callback)));
    // handler->OnNativeEmbedMouseEvent(browser_impl_.get(), event,
    //                                  callbackPtr.get());
  }
}

void ArkWebRenderWidgetHostViewOSRExt::OnNativeEmbedLifecycleChange(
    const ArkWebRenderHandlerExt::CefNativeEmbedData& info) {
  if (browser_impl_.get()) {
    CefRefPtr<ArkWebRenderHandlerExt> handler =
        browser_impl_->client()->GetRenderHandler();
    CHECK(handler);
    handler->OnNativeEmbedLifecycleChange(browser_impl_.get(), info);
  }
}

void ArkWebRenderWidgetHostViewOSRExt::SetGestureEventResult(
    bool result,
    bool stopPropagation,
    int32_t fingerId) {
  if (!render_widget_host_ || !render_widget_host_->input_router()) {
    return;
  }
  render_widget_host_->input_router()->SetGestureEventResult(result,
                                                             stopPropagation,
                                                             fingerId);
  if (!browser_impl_ || browser_impl_->client()) {
    return;
  }
  CefRefPtr<ArkWebRenderHandlerExt> handler =
      browser_impl_->client()->GetRenderHandler();
  if (handler) {
    handler->SetGestureEventResult(result);
  }
}

void ArkWebRenderWidgetHostViewOSRExt::SetEnableCustomVideoPlayer(bool flag) {
  if (!render_widget_host_ || !render_widget_host_->input_router()) {
    return;
  }
  render_widget_host_->input_router()->SetEnableCustomVideoPlayer(flag);
}

void ArkWebRenderWidgetHostViewOSRExt::SetMouseEventResult(bool result, bool stopPropagation) {
  if (!render_widget_host_) {
    return;
  }
  render_widget_host_->input_router()->SetMouseEventResult(result, stopPropagation);
}

void ArkWebRenderWidgetHostViewOSRExt::SetNativeEmbedMode(bool flag) {
  if (!render_widget_host_) {
    return;
  }
  render_widget_host_->input_router()->SetNativeEmbedMode(flag);
}

void ArkWebRenderWidgetHostViewOSRExt::OnNativeEmbedFirstFramePaint(
    const content::NativeEmbedFirstPaintEvent& event) {
  if (!browser_impl_) {
    return;
  }
  CefRefPtr<CefWebClientExtensionHandler> handler =
      browser_impl_->client()->AsArkWebClient()->GetWebClientExtensionHandler();
  CefRefPtr<CefNativeEmbedFirstFramePaintEvent> request(
      new AlloyNativeEmbedFirstFramePaintEvent(event));
  if (handler) {
    handler->OnNativeEmbedFirstFramePaint(request);
  }
}

void ArkWebRenderWidgetHostViewOSRExt::OnNativeEmbedVisibilityChange(
    const std::string& embed_id,
    bool visibility) {
  if (browser_impl_.get()) {
    CefRefPtr<ArkWebRenderHandlerExt> handler =
        browser_impl_->client()->GetRenderHandler();
    CHECK(handler);
    handler->OnNativeEmbedVisibilityChange(embed_id, visibility);
  }
}

void ArkWebRenderWidgetHostViewOSRExt::SetNativeInnerWeb(bool isInnerWeb) {
  if (auto compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
            browser_impl_->GetAcceleratedWidget(is_popup_))) {
    compositor->Utils()->SetNativeInnerWeb(isInnerWeb);
  }
}

void ArkWebRenderWidgetHostViewOSRExt::OnNativeEmbedObjectParamChange(
    const ArkWebRenderHandlerExt::CefNativeParamData& native_param_data) {
  if (browser_impl_.get()) {
    CefRefPtr<ArkWebRenderHandlerExt> handler =
        browser_impl_->client()->GetRenderHandler();
    CHECK(handler);
    handler->OnNativeEmbedObjectParamChange(browser_impl_.get(), native_param_data);
  }
}
#endif

#if BUILDFLAG(ARKWEB_TOUCHPAD_FLING)
void ArkWebRenderWidgetHostViewOSRExt::SendTouchpadFlingEvent(
    blink::WebGestureEvent event) {
  TRACE_EVENT1("cef", "CefRenderWidgetHostViewOSR::SendTouchpadFlingEvent",
    "type", blink::WebInputEvent::GetName(event.GetType()));
  if (event.GetType() == blink::WebInputEvent::Type::kGestureFlingStart) {
    IgnorePendingWheelEndEvent();
  }

  if (!IsPopupWidget()) {
    if (popup_host_view_ &&
        popup_host_view_->popup_position_.Contains(event.PositionInWidget().x(),
                                                   event.PositionInWidget().y())) {
      blink::WebGestureEvent popup_touchpad_fling_event(event);
      popup_touchpad_fling_event.SetPositionInWidget(gfx::PointF(
          event.PositionInWidget().x() - popup_host_view_->popup_position_.x(),
          event.PositionInWidget().y() - popup_host_view_->popup_position_.y()));
      popup_touchpad_fling_event.SetPositionInScreen(
          gfx::PointF(popup_touchpad_fling_event.PositionInWidget().x(),
                      popup_touchpad_fling_event.PositionInWidget().y()));
      popup_host_view_->AsArkWebRenderWidgetHostViewOSRExt()->SendTouchpadFlingEvent(
          popup_touchpad_fling_event);
      { TRACE_EVENT0("cef", "SendTouchpadFlingEvent to popup_host_view_"); }
      return;
    } else if (!guest_host_views_.empty()) {
      for (auto guest_host_view : guest_host_views_) {
        if (!guest_host_view->render_widget_host_ ||
            !guest_host_view->render_widget_host_->GetView()) {
          continue;
        }
        const gfx::Rect &guest_bounds =
            guest_host_view->render_widget_host_->GetView()->GetViewBounds();
        if (guest_bounds.Contains(event.PositionInWidget().x(),
                                  event.PositionInWidget().y())) {
          blink::WebGestureEvent guest_touchpad_fling_event(event);
          guest_touchpad_fling_event.SetPositionInWidget(
              gfx::PointF(event.PositionInWidget().x() - guest_bounds.x(),
                          event.PositionInWidget().y() - guest_bounds.y()));
          guest_touchpad_fling_event.SetPositionInScreen(
              gfx::PointF(guest_touchpad_fling_event.PositionInWidget().x(),
                          guest_touchpad_fling_event.PositionInWidget().y()));
          guest_host_view->AsArkWebRenderWidgetHostViewOSRExt()->SendTouchpadFlingEvent(
              guest_touchpad_fling_event);
          { TRACE_EVENT0("cef", "SendTouchpadFlingEvent to guest_host_view"); }
          return;
        }
      }
    }
  }
  if (render_widget_host_ && render_widget_host_->GetView()) {
    if (ShouldRouteEvents()) {
      render_widget_host_->delegate()->GetInputEventRouter()->RouteGestureEvent(
          this, const_cast<blink::WebGestureEvent*>(&event), ui::LatencyInfo());
    } else {
      render_widget_host_->GetView()->ProcessGestureEvent(event,
                                                          ui::LatencyInfo());
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void ArkWebRenderWidgetHostViewOSRExt::FilterScrollEventImpl(
    const ui::GestureEventData& gesture) {
  blink::WebGestureEvent web_event =
      ui::CreateWebGestureEventFromGestureEventData(gesture);
  if (!browser_impl_.get()) {
    LOG(ERROR) << "FilterScrollEventImpl get browser_impl_ is nullptr.";
    return;
  }
  CefRefPtr<CefRenderHandler> handler =
      browser_impl_->client()->GetRenderHandler();
  if (!handler) {
    LOG(ERROR) << "FilterScrollEventImpl get handler is nullptr.";
    return;
  }
  float x = 0.0f, y = 0.0f, xVelocity = 0.0f, yVelocity = 0.0f;
  bool isAvailable = false;
  if (web_event.GetType() == blink::WebInputEvent::Type::kGestureScrollUpdate) {
    x = web_event.data.scroll_update.delta_x;
    y = web_event.data.scroll_update.delta_y;
    is_scroll_consumed_ =
      handler->AsArkWebRenderHandler()->OnNestedScroll(browser_impl_.get(),
                                                       x,
                                                       y, xVelocity, yVelocity, isAvailable);
    if (!isAvailable) {
      is_scroll_consumed_ =
        handler->AsArkWebRenderHandler()->FilterScrollEvent(browser_impl_.get(),
                                                            web_event.data.scroll_update.delta_x,
                                                            web_event.data.scroll_update.delta_y, 0, 0);
    } else {
      ui::GestureEventData& mutable_gesture = const_cast<ui::GestureEventData&>(gesture);
      if (x != web_event.data.scroll_update.delta_x)
        mutable_gesture.details.set_scroll_update_x(x);
      if (y != web_event.data.scroll_update.delta_y)
        mutable_gesture.details.set_scroll_update_y(y);
    }
  } else if (web_event.GetType() ==
             blink::WebInputEvent::Type::kGestureFlingStart) {
    xVelocity = web_event.data.fling_start.velocity_x;
    yVelocity = web_event.data.fling_start.velocity_y;
    is_scroll_consumed_ =
      handler->AsArkWebRenderHandler()->OnNestedScroll(browser_impl_.get(),
                                                       x,
                                                       y, xVelocity, yVelocity, isAvailable);
    if (!isAvailable) {
      is_scroll_consumed_ =
        handler->AsArkWebRenderHandler()->FilterScrollEvent(browser_impl_.get(),
                                                            0, 0, web_event.data.fling_start.velocity_x,
                                                            web_event.data.fling_start.velocity_y);
    } else {
      ui::GestureEventData& mutable_gesture = const_cast<ui::GestureEventData&>(gesture);
      if (xVelocity != web_event.data.fling_start.velocity_x)
        mutable_gesture.details.set_velocity_x(xVelocity);
      if (yVelocity != web_event.data.fling_start.velocity_y)
        mutable_gesture.details.set_velocity_y(yVelocity);
    }
#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
    is_fling_ = true;
#endif
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
    if (browser_impl_) {
      browser_impl_->SetIsFling(true);
    }
#endif
  }
}

void ArkWebRenderWidgetHostViewOSRExt::SetFocusOnGestureEvent(
    const ui::GestureEventData& gesture) {
  blink::WebGestureEvent web_event =
      ui::CreateWebGestureEventFromGestureEventData(gesture);
  if (!browser_impl_ || !browser_impl_->client()) {
    LOG(ERROR) << "get browser_impl client failed.";
    return;
  }
  if (!browser_impl_->settings().gesture_focus_mode) {
    return;
  }

  CefRefPtr<CefFocusHandler> handler =
      browser_impl_->client()->GetFocusHandler();
  if (!handler) {
    LOG(ERROR) << "get handler failed.";
    return;
  }
  if (web_event.GetType() == blink::WebInputEvent::Type::kGestureTap ||
      web_event.GetType() == blink::WebInputEvent::Type::kGestureLongPress) {
    LOG(INFO) << "set focus on the gesture event of "
              << (web_event.GetType() == blink::WebInputEvent::Type::kGestureTap
                      ? "gestureTap"
                      : "gestureLongPress");
    handler->OnSetFocus(browser_impl_.get(), FOCUS_SOURCE_GESTURE);
  }
}

void ArkWebRenderWidgetHostViewOSRExt::OnDidNavigateMainFrameToNewPage() {
  ResetGestureDetection(false);
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(IS_ARKWEB)
void ArkWebRenderWidgetHostViewOSRExt::OnRootLayerChanged() {
  if (browser_impl_.get()) {
    CefRefPtr<CefRenderHandler> handler =
        browser_impl_->client()->GetRenderHandler();
    CHECK(handler);
  }
}

void ArkWebRenderWidgetHostViewOSRExt::OnScaleChanged(
    float old_page_scale_factor,
    float new_page_scale_factor) {
  if (browser_impl_.get()) {
    CefRefPtr<ArkWebDisplayHandlerExt> handler =
        browser_impl_->client()->GetDisplayHandler();
    CHECK(handler);
    handler->OnScaleChanged(
        browser_impl_.get(),
        std::round(old_page_scale_factor * SCALE_FACTOR_CONVERT_RATIO),
        std::round(new_page_scale_factor * SCALE_FACTOR_CONVERT_RATIO));
  }
}
#endif
#if BUILDFLAG(ARKWEB_MENU)
void ArkWebRenderWidgetHostViewOSRExt::MouseSelectMenuShow(bool show) {
  if (selection_controller_client_) {
    selection_controller_client_->MouseSelectMenuShow(show);
  }
}

void ArkWebRenderWidgetHostViewOSRExt::ChangeVisibilityOfQuickMenu() {
  if (selection_controller_client_) {
    selection_controller_client_->ChangeVisibilityOfQuickMenu();
  }
}

bool ArkWebRenderWidgetHostViewOSRExt::IsQuickMenuShow() {
  if (selection_controller_client_) {
    return selection_controller_client_->IsQuickMenuShow();
  }
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
bool ArkWebRenderWidgetHostViewOSRExt::FilterInputEventForPullToRefresh(
    const blink::WebInputEvent& input_event) {
  if (overscroll_controller_ &&
      blink::WebInputEvent::IsGestureEventType(input_event.GetType())) {
    blink::WebGestureEvent gesture_event =
        static_cast<const blink::WebGestureEvent&>(input_event);
    if (overscroll_controller_->WillHandleGestureEvent(gesture_event)) {
      // Terminate an active fling when a GSU generated from the fling progress
      // (GSU with inertial state) is consumed by the overscroll_controller_ and
      // overscrolling mode is not |OVERSCROLL_NONE|. The early fling
      // termination generates a GSE which completes the overscroll action.
      if (gesture_event.GetType() ==
              blink::WebInputEvent::Type::kGestureScrollUpdate &&
          gesture_event.data.scroll_update.inertial_phase ==
              blink::WebGestureEvent::InertialPhaseState::kMomentum) {
        host_->StopFling();
      }
      return true;
    }
  }
  return false;
}

void ArkWebRenderWidgetHostViewOSRExt::CreateOverscrollControllerIfPossible() {
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebExPullToRefresh)) {
    return;
  }

  // an OverscrollController is already set
  if (overscroll_controller_) {
    return;
  }

  content::RenderWidgetHostDelegate* delegate = host()->delegate();
  if (!delegate) {
    return;
  }

  content::RenderViewHostDelegateView* delegate_view =
      delegate->GetDelegateView();
  // render_widget_host_unittest.cc uses an object called
  // MockRenderWidgetHostDelegate that does not have a DelegateView
  if (!delegate_view) {
    return;
  }

  if (has_parent_) {
    return;
  }

#ifdef DISABLE_GPU
  if (!compositor_) {
    return;
  }
#else
  auto compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
      browser_impl_->GetAcceleratedWidget(is_popup_));
  if (!compositor) {
    return;
  }
#endif

  overscroll_controller_ =
      std::make_unique<content::OverscrollControllerOHOS>(this);
}

void ArkWebRenderWidgetHostViewOSRExt::OnFocusInternal() {
  if (overscroll_controller_) {
    overscroll_controller_->Enable();
  }
}

void ArkWebRenderWidgetHostViewOSRExt::LostFocusInternal() {
  if (overscroll_controller_) {
    overscroll_controller_->Disable();
  }
}

void ArkWebRenderWidgetHostViewOSRExt::Focus() {
#if BUILDFLAG(ARKWEB_FOCUS)
  if (!render_widget_host_) {
    return;
  }
  content::RenderWidgetHostImpl* widget =
      content::RenderWidgetHostImpl::From(render_widget_host_);
  widget->GotFocus();
  widget->SetActive(true);
#endif
#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
  OnFocusInternal();
#endif
}

bool ArkWebRenderWidgetHostViewOSRExt::HasFocus() {
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  content::RenderWidgetHostImpl* target_host = render_widget_host_;
  if (render_widget_host_ && render_widget_host_->delegate()) {
    target_host = render_widget_host_->delegate()->GetFocusedRenderWidgetHost(
        render_widget_host_);
  }
  if (target_host && target_host->GetView()) {
    return target_host->is_focused();
  }
#endif

  return false;
}

void ArkWebRenderWidgetHostViewOSRExt::UpdateBackgroundColor() {
#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  if (SkColorGetA(background_color_) != SK_AlphaOPAQUE) {
#ifdef DISABLE_GPU
    if (compositor_) {
      compositor_->SetBackgroundColor(background_color_);
    }
#else
    auto compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
        browser_impl_->GetAcceleratedWidget(is_popup_));
    if (compositor) {
      compositor->SetBackgroundColor(background_color_);
    }
#endif
  }
#endif // BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
}

void ArkWebRenderWidgetHostViewOSRExt::UpdateCursor(const ui::Cursor& cursor) {
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  if (!browser_impl_) {
    LOG(ERROR) << "browser is null when update cursor";
    return;
  }
  cursor_util::OnCursorChange(browser_impl_->GetBrowser(), cursor);
#endif
}

viz::FrameSinkId ArkWebRenderWidgetHostViewOSRExt::GetRootFrameSinkId() {
  auto compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
      browser_impl_->GetAcceleratedWidget(is_popup_));
  return compositor ? compositor->frame_sink_id() : viz::FrameSinkId();
}

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
void ArkWebRenderWidgetHostViewOSRExt::OnRendererWidgetCreated() {
  software_compositor_ = std::make_unique<content::SoftwareCompositorHostOhos>(
      render_widget_host_);
}
#endif

#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
void ArkWebRenderWidgetHostViewOSRExt::SendAccessibilityEvent(
    int64_t accessibilityId,
    int32_t eventType,
    const std::string& argument) {
  CefRefPtr<ArkWebRenderHandlerExt> handler =
      browser_impl_->GetClient()->GetRenderHandler();
  CHECK(handler);
  handler->OnAccessibilityEvent(accessibilityId, eventType, argument);
}
#endif

bool ArkWebRenderWidgetHostViewOSRExt::IsDisplayingInterstitial() {
  if (!host() || !host()->delegate()) {
    return false;
  }

  auto rvh_delegate_view = host()->delegate()->GetDelegateView();
  if (!rvh_delegate_view || !rvh_delegate_view->GetWebContents()) {
    return false;
  }

  security_interstitials::SecurityInterstitialTabHelper*
      security_interstitial_tab_helper =
          security_interstitials::SecurityInterstitialTabHelper::
              FromWebContents(rvh_delegate_view->GetWebContents());
  return security_interstitial_tab_helper &&
         security_interstitial_tab_helper->IsDisplayingInterstitial();
}

bool ArkWebRenderWidgetHostViewOSRExt::PullToRefreshAction(
    ui::PullToRefreshAction action) {
  if (!browser_impl_.get() || !browser_impl_->GetClient().get() ||
      IsDisplayingInterstitial()) {
    return false;
  }

  if (action == ui::PullToRefreshAction::PULL_START ? pull_to_refreshing_
                                                    : !pull_to_refreshing_) {
    return false;
  }

  bool is_dark =
      (browser_impl_->settings().force_dark_mode_enabled == STATE_ENABLED &&
       browser_impl_->settings().dark_prefer_color_scheme_enabled ==
           STATE_ENABLED);
  bool result =
      browser_impl_->GetClient()->AsArkWebClient()->OnPullToRefreshAction(
          static_cast<int>(action));
  switch (action) {
    case ui::PullToRefreshAction::PULL_START: {
      pull_to_refreshing_ = result;
      root_layer_transform_ = GetRootLayer()->transform();
      pull_to_refresh_offset_x_ = pull_to_refresh_offset_y_ = 0;
      if (result && is_dark) {
        SetBackgroundColor(SK_ColorBLACK);
      }
      break;
    }
    case ui::PullToRefreshAction::PULL_REFRESH: {
      if (browser_impl_ && result) {
        browser_impl_->Reload();
      }
      break;
    }
    case ui::PullToRefreshAction::PULL_RESET: {
      pull_to_refreshing_ = false;
      pull_to_refresh_offset_x_ = pull_to_refresh_offset_y_ = 0;
      browser_impl_->GetClient()->AsArkWebClient()->OnPullToRefreshPull(0, 0);
      GetRootLayer()->SetTransform(root_layer_transform_);
      if (is_dark) {
        SetBackgroundColor(SK_ColorWHITE);
      }
      break;
    }
    case ui::PullToRefreshAction::UNKNOWN:
    case ui::PullToRefreshAction::PULL_CANCEL:
    case ui::PullToRefreshAction::PULL_RELEASE:
      break;
  }

  LOG(INFO) << __func__ << " [pulltorefresh] " << action << " " << result;
  return result;
}

void ArkWebRenderWidgetHostViewOSRExt::PullToRefreshUpdate(float x_delta,
                                                           float y_delta) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(&ArkWebRenderWidgetHostViewOSRExt::PullToRefreshUpdate,
                       weak_ptr_factory_.GetWeakPtr(), x_delta, y_delta));
    return;
  }
  if (!enable_nweb_ex_ || !browser_impl_.get() ||
      !browser_impl_->GetClient().get()) {
    return;
  }
  pull_to_refresh_offset_x_ += x_delta;
  pull_to_refresh_offset_y_ += y_delta;
  browser_impl_->GetClient()->AsArkWebClient()->OnPullToRefreshPull(
      pull_to_refresh_offset_x_, pull_to_refresh_offset_y_);

  gfx::Transform root_layer_transform = GetRootLayer()->transform();
  root_layer_transform.PostTranslate(gfx::Vector2dF(0, y_delta));
  GetRootLayer()->SetTransform(root_layer_transform);
  LOG(DEBUG) << __func__
             << " [pulltorefresh] y_offset:" << pull_to_refresh_offset_y_;
}

void ArkWebRenderWidgetHostViewOSRExt::DidStopRefresh() {
  if (overscroll_controller_) {
    overscroll_controller_->DidStopRefresh();
  }
}
#endif  // BUILDFLAG(ARKWEB_PULL_TO_REFRESH)

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
void ArkWebRenderWidgetHostViewOSRExt::TransformPointToRootSurface(
    gfx::PointF* point) {
  *point += gfx::Vector2d(0, GetShrinkViewportHeight());
}

int ArkWebRenderWidgetHostViewOSRExt::GetTopControlsOffset() const {
  return (enable_nweb_ex_ ? top_controls_offset_ : 0);
}

int ArkWebRenderWidgetHostViewOSRExt::GetShrinkViewportHeight() {
  int shrink_viewport_height = 0;
  if (!enable_nweb_ex_ || !host()->delegate()) {
    return shrink_viewport_height;
  }

  auto rvh_delegate_view = host()->delegate()->GetDelegateView();
  if (rvh_delegate_view->DoBrowserControlsShrinkRendererSize()) {
    gfx::Size shrink_size(0, rvh_delegate_view->GetTopControlsHeight());
    shrink_viewport_height =
        gfx::ScaleToFlooredSize(shrink_size, 1.f / GetDeviceScaleFactor())
            .height();
  }

  return shrink_viewport_height;
}

void ArkWebRenderWidgetHostViewOSRExt::OnTopControlsChanged(
    float top_controls_offset,
    float top_content_offset) {
  if (!enable_nweb_ex_ || !browser_impl_.get() ||
      !browser_impl_->GetClient().get()) {
    return;
  }
  bool is_dark =
      (browser_impl_->settings().force_dark_mode_enabled == STATE_ENABLED &&
       browser_impl_->settings().dark_prefer_color_scheme_enabled ==
           STATE_ENABLED);
  if (is_dark) {
    SetBackgroundColor(SK_ColorBLACK);
  } else {
    SetBackgroundColor(SK_ColorWHITE);
  }
  browser_impl_->GetClient()->AsArkWebClient()->OnTopControlsChanged(
      top_controls_offset, top_content_offset);
  gfx::Transform root_layer_transform;
  root_layer_transform.Translate(gfx::Vector2dF(0, top_content_offset));
  GetRootLayer()->SetTransform(root_layer_transform);
}

void ArkWebRenderWidgetHostViewOSRExt::OnTopControlsHeightChanged() {
  if (auto rvh_delegate_view = host()->delegate()->GetDelegateView()) {
    GetRootLayer()->SetTopControlsHeight(
        rvh_delegate_view->GetTopControlsHeight() / GetDeviceScaleFactor());
  }
}
#endif  // BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)

#if BUILDFLAG(ARKWEB_DRAG_DROP)
void ArkWebRenderWidgetHostViewOSRExt::SetTextHandlesTemporarilyHiddenByDrag(
    bool hide_handles,
    bool dragging) {
  LOG(INFO) << "SetTextHandlesTemporarilyHiddenByDrag hide_handles:"
            << hide_handles << ", dragging:" << dragging;
  if (!dragging && selection_controller_) {
    selection_controller_->AsTouchSelectionControllerExt()->ResetResponsePendingInputEvent();
  }

  if (selection_controller_client_) {
    selection_controller_client_->AsArkWebTouchSelectionControllerClientOSRExt()
        ->HideHandleAndQuickMenuIfNecessary(hide_handles);
  }
}
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
void ArkWebRenderWidgetHostViewOSRExt::MaximizeResize() {
  auto compositor = ArkWebRenderWidgetHostViewOSRExt::GetCompositor();
  if (compositor) {
    compositor->Utils()->DisableSwapUntilMaximized();
  }
}

void ArkWebRenderWidgetHostViewOSRExt::RestoreRenderFit() {
  if (!browser_impl_ || !browser_impl_->client()) {
    LOG(ERROR) << "RestoreRenderFit get client failed.";
    return;
  }
  CefRefPtr<CefRenderHandler> handler =
      browser_impl_->client()->GetRenderHandler();
  if (handler && handler->AsArkWebRenderHandler()) {
    handler->AsArkWebRenderHandler()->RestoreRenderFit();
  }
}
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
void ArkWebRenderWidgetHostViewOSRExt::OnScreenInfoChangedV2() {
  TRACE_EVENT0("cef", "ArkWebRenderWidgetHostViewOSRExt::OnScreenInfoChangedV2");
  if (!render_widget_host_) {
    return;
  }
  if (render_widget_host_->delegate()) {
    render_widget_host_->delegate()->SendScreenRects();
  } else {
    render_widget_host_->SendScreenRects();
  }
}
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)

#if BUILDFLAG(IS_OHOS)
ui::Compositor* ArkWebRenderWidgetHostViewOSRExt::GetCompositor() {
#ifdef DISABLE_GPU
  return nullptr;
#else
  if (browser_impl_) {
    return ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
        browser_impl_->GetAcceleratedWidget(is_popup_));
  }
  return nullptr;
#endif
}
#endif

#if BUILDFLAG(ARKWEB_PIP)
void ArkWebRenderWidgetHostViewOSRExt::SetPipActive(bool active) {
  if (browser_impl_.get() && browser_impl_->GetAcceleratedWidget(is_popup_)) {
    ui::Compositor* compositor = ArkWebRenderWidgetHostViewOSRUtils::GetCompositor(
        browser_impl_->GetAcceleratedWidget(is_popup_));
    if (compositor) {
      compositor->Utils()->SetPipActive(active);
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
void ArkWebRenderWidgetHostViewOSRExt::OnDetectedBlankScreen(
    const std::string& url,
    int32_t blankScreenReason,
    int32_t detectedContentfulNodesCount) {
  if (!browser_impl_.get() || !browser_impl_->GetClient().get()) {
    return;
  }
  CefRefPtr<ArkWebRenderHandlerExt> handler =
      browser_impl_->GetClient()->GetRenderHandler();
  CHECK(handler);
  handler->OnDetectedBlankScreen(url, blankScreenReason,
                                 detectedContentfulNodesCount);
}
#endif

#if BUILDFLAG(ARKWEB_AI)
ui::FilteredGestureProvider& ArkWebRenderWidgetHostViewOSRExt::GetGestureProvider() {
  return gesture_provider_;
}
#endif