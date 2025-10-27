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

#include "cef/ohos_cef_ext/libcef/browser/osr/arkweb_touch_selection_controller_client_osr_ext.h"

#include <cmath>
#include <set>

#include "base/functional/bind.h"
#include "base/ohos/sys_info_utils_ext.h"
#include "cef/libcef/browser/osr/render_widget_host_view_osr.h"
#include "cef/libcef/browser/osr/touch_handle_drawable_osr.h"
#include "cef/ohos_cef_ext/include/arkweb_render_handler_ext.h"
#include "cef/ohos_cef_ext/libcef/browser/osr/arkweb_render_widget_host_view_osr_ext.h"
#include "content/browser/renderer_host/render_widget_host_delegate.h"
#include "content/browser/renderer_host/render_widget_host_impl.h"
#include "content/public/browser/context_menu_params.h"
#include "content/public/browser/render_view_host.h"
#include "ui/base/clipboard/clipboard.h"
#include "ui/base/data_transfer_policy/data_transfer_endpoint.h"
#include "ui/base/mojom/menu_source_type.mojom-forward.h"
#include "ui/gfx/geometry/point_conversions.h"
#include "ui/gfx/geometry/size_conversions.h"
#include "ui/touch_selection/touch_editing_controller.h"
#include "cef/ohos_cef_ext/libcef/browser/alloy/alloy_browser_host_impl_ext.h"
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
#include "base/base_switches.h"
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#endif

namespace {
#if BUILDFLAG(ARKWEB_MENU)
#if BUILDFLAG(ARKWEB_DRAG_DROP)
constexpr int kQuickMenuDelayInMs = 0;
#else
constexpr int kQuickMenuDelayInMs = 100;
#endif
constexpr int kSelectHandleMoveDelayMixInMs = 400;
#endif

#if BUILDFLAG(ARKWEB_MENU)
constexpr cef_quick_menu_edit_state_flags_t kMenuCommands[] = {
    QM_EDITFLAG_CAN_ELLIPSIS, QM_EDITFLAG_CAN_CUT, QM_EDITFLAG_CAN_COPY,
    QM_EDITFLAG_CAN_PASTE, QM_EDITFLAG_CAN_SELECT_ALL};

void ConvertTouchHandleState(const std::unique_ptr<ui::TouchHandle>& handle,
                             CefTouchHandleState& state) {
  if (handle == nullptr) {
    state.enabled = 0;
    return;
  }

  state.enabled = handle->AsTouchHandleExt()->GetEnabled();
#if BUILDFLAG(ARKWEB_MENU)
  gfx::PointF origin;
  if (handle->focus_bottom().y() < handle->AsTouchHandleExt()->focus_top().y()) {
    origin = handle->AsTouchHandleExt()->focus_top();
    state.edge_height = handle->AsTouchHandleExt()->focus_top().y() - handle->focus_bottom().y();
  } else {
    origin = handle->focus_bottom();
    state.edge_height = handle->focus_bottom().y() - handle->AsTouchHandleExt()->focus_top().y();
  }
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  state.view_port = {handle->AsTouchHandleExt()->viewport().x(),
                     handle->AsTouchHandleExt()->viewport().y()};
  state.origin = {origin.x() + handle->AsTouchHandleExt()->viewport().x(),
                  origin.y() + handle->AsTouchHandleExt()->viewport().y()};
#else
  state.origin = {origin.x(), origin.y()};
#endif
#else
  state.origin = {handle->focus_bottom().x(), handle->focus_bottom().y()};
  state.edge_height = handle->focus_bottom().y() - handle->focus_top().y();
#endif

  state.alpha = handle->alpha();

  cef_horizontal_alignment_t orientation;
  switch (handle->orientation()) {
    case ui::TouchHandleOrientation::LEFT:
      orientation = CEF_HORIZONTAL_ALIGNMENT_LEFT;
      break;
    case ui::TouchHandleOrientation::CENTER:
      orientation = CEF_HORIZONTAL_ALIGNMENT_CENTER;
      break;
    case ui::TouchHandleOrientation::RIGHT:
      orientation = CEF_HORIZONTAL_ALIGNMENT_RIGHT;
      break;
    default:
      orientation = CEF_HORIZONTAL_ALIGNMENT_UNDEFINED;
  }
  state.orientation = orientation;
}
#endif  // BUILDFLAG(ARKWEB_MENU)
}  // namespace

ArkWebTouchSelectionControllerClientOSRExt::
    ArkWebTouchSelectionControllerClientOSRExt(CefRenderWidgetHostViewOSR* rwhv)
    : CefTouchSelectionControllerClientOSR(rwhv),
      weak_ptr_factory_(this) {}

void ArkWebTouchSelectionControllerClientOSRExt::OnScrollStarted() {
  scroll_in_progress_ = true;
  rwhv_->selection_controller()->SetTemporarilyHidden(true);
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  if (handles_hidden_by_selection_ui_) {
    LOG(INFO) << "scroll starts when the handle menu is hidden";
    AsArkWebTouchSelectionControllerClientOSRExt()->UpdateQuickMenuByHandlesHidden();
    return;
  }
#endif
  UpdateQuickMenu();
}

void ArkWebTouchSelectionControllerClientOSRExt::OnScrollCompleted() {
  scroll_in_progress_ = false;
  active_client_->DidScroll();
  rwhv_->selection_controller()->SetTemporarilyHidden(false);
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  if (handles_hidden_by_selection_ui_) {
    LOG(INFO) << "scroll completed when the handle menu is hidden";
    AsArkWebTouchSelectionControllerClientOSRExt()->UpdateQuickMenuByHandlesHidden();
    return;
  }
#endif
  UpdateQuickMenu();
}

bool ArkWebTouchSelectionControllerClientOSRExt::HandleContextMenu(
    const content::ContextMenuParams& params) {
#if BUILDFLAG(ARKWEB_AI)
  isSelectionNotEmptyForAI_ = false;
  rwhv_->AsArkWebRenderWidgetHostViewOSRExt()->SetDataDetectorSelectText(
      std::u16string());
#endif
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  bool is_browser = base::CommandLine::ForCurrentProcess() &&
                    base::CommandLine::ForCurrentProcess()->HasSwitch(
                        switches::kEnableNwebExFreeCopy);
  bool has_select = !rwhv_->GetSelectedText().empty();
  if (is_browser && !has_select && params.selection_text.empty() &&
      !params.is_editable) {
    quick_menu_requested_ = false;
    return false;
  }
#endif
  if ((params.source_type == ui::mojom::MenuSourceType::kLongPress ||
       params.source_type == ui::mojom::MenuSourceType::kLongTap) &&
      params.is_editable && params.selection_text.empty() &&
      IsQuickMenuAvailable()) {
    quick_menu_requested_ = true;
    UpdateQuickMenu();
    return true;
  }

  bool from_touch =
      params.source_type == ui::mojom::MenuSourceType::kLongPress ||
      params.source_type == ui::mojom::MenuSourceType::kLongTap ||
      params.source_type == ui::mojom::MenuSourceType::kTouch;
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  if (is_browser) {
    from_touch = from_touch || params.source_type ==
                                   ui::mojom::MenuSourceType::kSelectAndCopy;
  }
#endif
  if (from_touch && !params.selection_text.empty()) {
#if BUILDFLAG(ARKWEB_AI)
    isSelectionNotEmptyForAI_ =
        rwhv_->AsArkWebRenderWidgetHostViewOSRExt()->SetDataDetectorSelectText(
            params.selection_text);
#endif
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
    if (is_browser) {
      if (params.source_type == ui::mojom::MenuSourceType::kSelectAndCopy) {
        rwhv_->AsArkWebRenderWidgetHostViewOSRExt()->SetLastSelectedTextFromContextParam(params.selection_text);
        quick_menu_requested_ = true;
      }
      AsArkWebTouchSelectionControllerClientOSRExt()->SelectionTextNotEmpty(!params.selection_text.empty());
    }
#endif
    return true;
  }

  rwhv_->selection_controller()->HideAndDisallowShowingAutomatically();
  return false;
}

void ArkWebTouchSelectionControllerClientOSRExt::CloseQuickMenuAndHideHandles() {
  LOG(INFO) << "Close Quick Menu And Hide Hanles.";
  CloseQuickMenu();
#if BUILDFLAG(ARKWEB_MENU)
  auto controller = rwhv_->selection_controller();
  if (controller) {
    if (!controller->AsTouchSelectionControllerExt()->GetInsertHandle() ||
        !controller->AsTouchSelectionControllerExt()
             ->GetInsertHandle()
             ->AsTouchHandleExt()
             ->GetEnabled()) {
      controller->HideAndDisallowShowingAutomatically();
    } else if (controller->AsTouchSelectionControllerExt()
                   ->GetInsertHandle()
                   ->AsTouchHandleExt()
                   ->GetEnabled()) {
      controller->SetTemporarilyHidden(true);
      AsArkWebTouchSelectionControllerClientOSRExt()->NotifyTouchSelectionChanged(true);
    }
  }
#else
  rwhv_->selection_controller()->HideAndDisallowShowingAutomatically();
#endif  // #if BUILDFLAG(ARKWEB_MENU)
}

#if BUILDFLAG(ARKWEB_DRAG_DROP)
void ArkWebTouchSelectionControllerClientOSRExt::
    UpdateQuickMenuByHandlesHidden() {
  if (!rwhv_) {
    return;
  }
  auto browser = rwhv_->browser_impl();
  if (!browser || !browser->web_contents()) {
    return;
  }
  if (!browser->web_contents()->IsShowingContextMenu()) {
    LOG(INFO) << "Current Quick Menu IS Not Showing";
    UpdateQuickMenu();
  } else {
    NotifyTouchSelectionChanged(true);
  }
}
#endif

#if BUILDFLAG(ARKWEB_MENU)
void ArkWebTouchSelectionControllerClientOSRExt::NotifyTouchSelectionChanged(
    bool need_report) {
  ui::TouchSelectionController* controller = GetTouchSelectionController();
  if (rwhv_ && controller) {
    CefTouchHandleState insert_handle;
    CefTouchHandleState start_selection_handle;
    CefTouchHandleState end_selection_handle;
    ConvertTouchHandleState(
        controller->AsTouchSelectionControllerExt()->GetInsertHandle(),
        insert_handle);
    ConvertTouchHandleState(
        controller->AsTouchSelectionControllerExt()->GetStartSelectionHandle(),
        start_selection_handle);
    ConvertTouchHandleState(
        controller->AsTouchSelectionControllerExt()->GetEndSelectionHandle(),
        end_selection_handle);
    rwhv_->AsArkWebRenderWidgetHostViewOSRExt()->OnTouchSelectionChanged(
        insert_handle, start_selection_handle, end_selection_handle,
        need_report);
  }
}

bool ArkWebTouchSelectionControllerClientOSRExt::
    NeedPopupInsertTouchHandleQuickMenu() {
  if (ShouldShowQuickMenu()) {
    ShowQuickMenu();
    return true;
  }
  return false;
}

void ArkWebTouchSelectionControllerClientOSRExt::SetTemporarilyHidden(
    bool hidden) {
  if (rwhv_ && rwhv_->selection_controller()) {
    rwhv_->selection_controller()->SetTemporarilyHidden(hidden);
    NotifyTouchSelectionChanged(false);
  }
}

void ArkWebTouchSelectionControllerClientOSRExt::OnSelectionEvent(
    ui::SelectionEventType event) {
  // This function (implicitly) uses active_menu_client_, so we don't go to the
  // active view for this.
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  LOG(INFO) << "Selection Event Value = " << static_cast<int32_t>(event)
            << ", handles_hidden_by_selection_ui = "
            << handles_hidden_by_selection_ui_;
#else
  LOG(INFO) << "Selection Event Value = " << static_cast<int32_t>(event);
#endif
  if (!rwhv_) {
    LOG(ERROR) << "Fatal error: rwhv_ is null";
    return;
  }
  SetTemporarilyHidden(false);
  auto browser = rwhv_->browser_impl();
  ui::TouchSelectionController* controller = GetTouchSelectionController();
  switch (event) {
    case ui::SELECTION_HANDLES_SHOWN:
      if (handles_hidden_by_selection_ui_) {
        LOG(INFO) << "Selection Event handle shows, but handles hidden by ui.";
        NotifyTouchSelectionChanged(true);
        rwhv_->ResetGestureDetection(false);
        break;
      }
      quick_menu_requested_ = true;
      NotifyTouchSelectionChanged(false);
      UpdateQuickMenu();
      {
        auto rwhvOSRExt = rwhv_->AsArkWebRenderWidgetHostViewOSRExt();
        if (rwhvOSRExt &&
            rwhvOSRExt->GetGestureProvider().GetCurrentDownEvent()) {
          rwhv_->ResetGestureDetection(false);
        }
      }
      break;
    case ui::INSERTION_HANDLE_SHOWN:
#if BUILDFLAG(ARKWEB_MENU_HANDLE)
      AsArkWebTouchSelectionControllerClientOSRExt()->ClearSpecialSelectedTagTemporarily();
      isSelectAll_ = false;
      isCopy_ = false;
#endif // ARKWEB_MENU_HANDLE
      if (rwhv_->browser_impl()) {
        quick_menu_requested_ =
            rwhv_->browser_impl()->AsAlloyBrowserHostImplExt()->GetTouchInsertHandleMenuShow();
      }
      NotifyTouchSelectionChanged(true);
      if (quick_menu_requested_) {
        if (controller && !controller->AsTouchSelectionControllerExt()->IsTapEvent() &&
            controller->AsTouchSelectionControllerExt()->IsLongPressEvent()) {
          if (auto client = browser->client()) {
            if (auto render = client->GetRenderHandler()) {
              controller->AsTouchSelectionControllerExt()
                  ->ResetLongPressEvent();
            }
          }
          ShowQuickMenu();
        }
      }
      break;
    case ui::SELECTION_HANDLES_CLEARED:
#if BUILDFLAG(ARKWEB_MENU_HANDLE)
      isSelectAll_ = false;
#endif // ARKWEB_MENU_HANDLE
      [[fallthrough]];
    case ui::INSERTION_HANDLE_CLEARED:
#if BUILDFLAG(ARKWEB_MENU_HANDLE)
      isCopy_ = false;
#endif // ARKWEB_MENU_HANDLE
      quick_menu_requested_ = false;
      NotifyTouchSelectionChanged(true);
      UpdateQuickMenu();
      break;
    case ui::SELECTION_HANDLE_DRAG_STARTED:
#if BUILDFLAG(ARKWEB_MENU_HANDLE)
      isSelectAll_ = false;
#endif // ARKWEB_MENU_HANDLE
      [[fallthrough]];
    case ui::INSERTION_HANDLE_DRAG_STARTED:
      handle_drag_in_progress_ = true;
      if (controller && controller->AsTouchSelectionControllerExt()
                            ->IsLongPressDragSelectionActive()) {
        LOG(INFO) << "Selection Event Long Press Drag Selection Is Active.";
        UpdateQuickMenu();
      }
      break;
    case ui::SELECTION_HANDLE_DRAG_STOPPED:
#if BUILDFLAG(ARKWEB_MENU_HANDLE)
      if (isCopy_) {
        isCopy_ = false;
        ShowQuickMenu();
      }
#endif // ARKWEB_MENU_HANDLE
      [[fallthrough]];
    case ui::INSERTION_HANDLE_DRAG_STOPPED:
      handle_drag_in_progress_ = false;
#if BUILDFLAG(ARKWEB_MENU)
      if (browser && browser->client()) {
        auto handler = browser->client()->GetContextMenuHandler();
        if (handler) {
          handler->HideMagnifier();
        }
      }
#endif
      if (browser) {
        if (auto client = browser->client()) {
          if (auto render = client->GetRenderHandler()) {
            render->NotifySelectAllClicked(false);
          }
        }
      }
      break;
    case ui::INSERTION_HANDLE_MOVED:
#if BUILDFLAG(ARKWEB_DRAG_DROP)
      if (handles_hidden_by_selection_ui_) {
        LOG(INFO) << "Current Selection Handle Menu Hidden";
        NotifyTouchSelectionChanged(true);
        return;
      }
#endif
      if (quick_menu_requested_) {
        quick_menu_requested_ = false;
      }
      [[fallthrough]];
    case ui::SELECTION_HANDLES_MOVED:
#if BUILDFLAG(ARKWEB_DRAG_DROP)
      if (handles_hidden_by_selection_ui_) {
        LOG(INFO) << "Current Selection Handle Menu Hidden";
        NotifyTouchSelectionChanged(true);
        return;
      }
#endif
      if (!handle_drag_in_progress_) {
        if (!IsVaildSelectionHandleMove()
#if BUILDFLAG(ARKWEB_MENU_HANDLE)
            || (rwhv_->render_widget_host()->IsOrientationChange())
#endif // ARKWEB_MENU_HANDLE           
           ) {
          LOG(DEBUG) << "Current Selection Handle Move Event Is Invalid";
          NotifyTouchSelectionChanged(true);
          return;
        }
        UpdateQuickMenu();
      }
      NotifyTouchSelectionChanged(true);
      break;
    case ui::INSERTION_HANDLE_TAPPED:
#if !BUILDFLAG(ARKWEB_MENU)
      quick_menu_requested_ = !quick_menu_requested_;
      if (quick_menu_requested_) {
#else
      if (!quick_menu_requested_) {
#endif
        bool handle_drag_in_progress = handle_drag_in_progress_;
        handle_drag_in_progress_ = false;
#if !BUILDFLAG(ARKWEB_MENU)
        UpdateQuickMenu();
#else
        ShowQuickMenu();
        quick_menu_requested_ = !quick_menu_requested_;
#endif
        handle_drag_in_progress_ = handle_drag_in_progress;
      } else {
#if BUILDFLAG(ARKWEB_MENU)
        ChangeVisibilityOfQuickMenu();
#else
        CloseQuickMenu();
        NotifyTouchSelectionChanged(true);
#endif
      }
      break;
    case ui::SELECTION_HANDLES_UPDATEMENU:
#if BUILDFLAG(ARKWEB_DRAG_DROP)
#if BUILDFLAG(ARKWEB_MENU_HANDLE)
      isSelectAll_ = false;
#endif // ARKWEB_MENU_HANDLE
      if (controller) {
        controller->AsTouchSelectionControllerExt()->ResetLongPressEvent();
      }
      if (handles_hidden_by_selection_ui_) {
        LOG(INFO) << "Current Selection Handle Menu Hidden";
        return;
      }
#endif
      if (controller &&
          controller->AsTouchSelectionControllerExt()->GetInsertHandle() &&
          controller->AsTouchSelectionControllerExt()
              ->GetInsertHandle()
              ->alpha()) {
        LOG(INFO) << "Selection handles is show";
        return;
      }
      if (browser && !browser->web_contents()->IsShowingContextMenu()) {
        LOG(INFO) << "Current Quick Menu IS Not Showing";
        UpdateQuickMenu();
      }
      NotifyTouchSelectionChanged(true);
      break;
  }
}

void ArkWebTouchSelectionControllerClientOSRExt::
    UpdateClientClippedSelectionBounds(
        const gfx::Rect& clipped_selection_bounds) {
  clipped_selection_bounds_ = clipped_selection_bounds;
  MouseMayUpdateClientClippedSelectionBounds(clipped_selection_bounds);
}

bool ArkWebTouchSelectionControllerClientOSRExt::IsVaildSelectionHandleMove() {
  base::TimeDelta duration = base::TimeTicks::Now() - select_handle_move_timer_;
  select_handle_move_timer_ = base::TimeTicks::Now();
  LOG(DEBUG) << "Selection Handle Move Vaild Time = "
             << duration.InMilliseconds();
  if (kSelectHandleMoveDelayMixInMs > duration.InMilliseconds() &&
      duration.InMilliseconds() > kQuickMenuDelayInMs) {
    return false;
  }
  return true;
}

void ArkWebTouchSelectionControllerClientOSRExt::ExecuteCommandMouse(
    int command_id,
    int event_flags) {
  SetSelectAllClicked(command_id);
  if (command_id != QM_EDITFLAG_CAN_ELLIPSIS &&
      command_id != QM_EDITFLAG_CAN_SELECT_ALL) {
    rwhv_->selection_controller()->HideAndDisallowShowingAutomatically();
  }
  content::RenderWidgetHostDelegate* host_delegate = rwhv_->host()->delegate();
  if (!host_delegate) {
    return;
  }
  auto browser = rwhv_->browser_impl();
  if (browser && browser->client()) {
    auto handler = browser->client()->GetContextMenuHandler();
    if (handler && handler->OnQuickMenuCommand(
                       browser.get(), browser->GetFocusedFrame(), command_id,
                       static_cast<cef_event_flags_t>(event_flags))) {
      return;
    }
  }

  absl::optional<std::u16string> value;
  LOG(INFO) << "mouse quick menu command id = " << command_id;
  switch (command_id) {
    case QM_EDITFLAG_CAN_CUT:
      host_delegate->Cut();
      MouseSelectMenuShow(false);
      break;
    case QM_EDITFLAG_CAN_COPY:
      host_delegate->Copy();
      browser->web_contents()->CollapseSelection();
      MouseSelectMenuShow(false);
      break;
    case QM_EDITFLAG_CAN_PASTE:
      host_delegate->Paste();
      MouseSelectMenuShow(false);
      break;
    case QM_EDITFLAG_CAN_SELECT_ALL:
      host_delegate->SelectAll();
      MouseSelectMenuShow(false);
      MouseSelectMenuShow(true);
      break;
    case QM_EDITFLAG_CAN_ELLIPSIS:
      MouseSelectMenuShow(false);
      RunContextMenu();
      browser->web_contents()->CollapseSelection();
      break;
    default:
      MouseSelectMenuShow(false);
      browser->web_contents()->CollapseSelection();
      break;
  }
}

void ArkWebTouchSelectionControllerClientOSRExt::
    MouseMayUpdateClientClippedSelectionBounds(
        const gfx::Rect& clipped_selection_bounds) {
  if (!mouse_quick_menu_running_) {
    return;
  }
  auto browser = rwhv_->browser_impl();
  if (!browser || !browser->client()) {
    return;
  }
  auto handler = browser->client()->GetContextMenuHandler();
  if (!handler) {
    return;
  }
  handler->UpdateClippedSelectionBounds(
      browser, browser->GetFocusedFrame(),
      {clipped_selection_bounds_.x(), clipped_selection_bounds_.y(),
       clipped_selection_bounds_.width(), clipped_selection_bounds_.height()});
}

void ArkWebTouchSelectionControllerClientOSRExt::MouseSelectMenuShow(
    bool show) {
  auto browser = rwhv_->browser_impl();
  if (!browser || !browser->client()) {
    return;
  }
  auto handler = browser->client()->GetContextMenuHandler();
  if (!handler) {
    return;
  }
  if (!show) {
    if (mouse_quick_menu_running_) {
      mouse_quick_menu_running_ = false;
      handler->OnQuickMenuDismissed(browser, browser->GetFocusedFrame(), true);
      if (browser->web_contents()) {
        browser->web_contents()->SetShowingContextMenu(false);
      }
    }
    return;
  }

  int quickmenuflags = 0;
  if (active_menu_client_) {
    for (const auto& command : kMenuCommands) {
      if (active_menu_client_->IsCommandIdEnabled(command)) {
        quickmenuflags |= command;
      }
    }
  }
  LOG(INFO) << "Mouse Quick Menu Flags Is = " << quickmenuflags;
  CefRefPtr<CefRunQuickMenuCallbackImpl> callbackImpl(
      new CefRunQuickMenuCallbackImpl(base::BindOnce(
          &ArkWebTouchSelectionControllerClientOSRExt::ExecuteCommandMouse,
          weak_ptr_factory_.GetWeakPtr())));
  ui::TouchSelectionController* controller = GetTouchSelectionController();
  bool isLongPressSelectionActive = false;
  if (controller) {
    isLongPressSelectionActive = controller->AsTouchSelectionControllerExt()
                                     ->IsLongPressDragSelectionActive();
  }
  if (!handler->RunQuickMenu(
          browser, browser->GetFocusedFrame(), {0, 0}, {0, 0},
          {clipped_selection_bounds_.x(), clipped_selection_bounds_.y(),
           clipped_selection_bounds_.width(),
           clipped_selection_bounds_.height()},
          static_cast<CefContextMenuHandlerExt::QuickMenuEditStateFlags>(
              quickmenuflags),
          callbackImpl, true, isLongPressSelectionActive)) {
    callbackImpl->Disconnect();
    auto render = browser->client()->GetRenderHandler();
    if (render) {
      render->NotifySelectAllClicked(false);
    }
    return;
  }
  LOG(INFO) << "Show Mouse Handle Quick Menu Success";
  mouse_quick_menu_running_ = true;
  if (browser->web_contents()) {
    browser->web_contents()->SetShowingContextMenu(true);
  }
}

void ArkWebTouchSelectionControllerClientOSRExt::ChangeVisibilityOfQuickMenu() {
  if (!rwhv_) {
    return;
  }
#if BUILDFLAG(ARKWEB_MENU_HANDLE)
  if (isCopy_) {
    isCopy_ = false;
    ShowQuickMenu();
    return;
  }
#endif // ARKWEB_MENU_HANDLEs
  auto browser = rwhv_->browser_impl();
  if (!browser || !browser->client()) {
    return;
  }
  auto handler = browser->client()->GetContextMenuHandler();
  if (!handler) {
    return;
  }
  handler->ChangeVisibilityOfQuickMenu();
}

bool ArkWebTouchSelectionControllerClientOSRExt::IsQuickMenuShow() {
  if (!rwhv_) {
    return false;
  }
  auto browser = rwhv_->browser_impl();
  if (!browser || !browser->client()) {
    return false;
  }
  auto handler = browser->client()->GetContextMenuHandler();
  if (!handler) {
    return false;
  }
  return handler->IsQuickMenuShow();
}
#endif

#if BUILDFLAG(ARKWEB_VIBRATE)
bool ArkWebTouchSelectionControllerClientOSRExt::IsInsertHandleShow() {
  ui::TouchSelectionController* controller = GetTouchSelectionController();
  if (controller &&
      controller->AsTouchSelectionControllerExt()->GetInsertHandle() &&
      controller->AsTouchSelectionControllerExt()->GetInsertHandle()->alpha()) {
    return true;
  }
  return false;
}
#endif  // BUILDFLAG(ARKWEB_VIBRATE)

#if BUILDFLAG(ARKWEB_DRAG_DROP)
void ArkWebTouchSelectionControllerClientOSRExt::
    HideHandleAndQuickMenuIfNecessary(bool hide_handles) {
  if (handles_hidden_by_selection_ui_ == hide_handles) {
    return;
  }
  handles_hidden_by_selection_ui_ = hide_handles;
  if (rwhv_) {
    auto browser = rwhv_->browser_impl();
    if (browser && browser->client()) {
      auto handler = browser->client()->GetContextMenuHandler();
      if (handler) {
        handler->HideHandleAndQuickMenuIfNecessary(hide_handles);
      }
    }
  }
}

bool ArkWebTouchSelectionControllerClientOSRExt::IsShowHandle() {
  if (!rwhv_) {
    return false;
  }
  auto browser = rwhv_->browser_impl();
  if (browser && browser->client()) {
    auto handler = browser->client()->GetContextMenuHandler();
    if (handler) {
      return handler->IsShowHandle();
    }
  }
  return false;
}
#endif
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
void ArkWebTouchSelectionControllerClientOSRExt::SelectionTextNotEmpty(
    bool has_selection) {
  isSelectionTextNotEmpty_ = has_selection;
}
#endif
#if BUILDFLAG(ARKWEB_DRAG_DROP)
void ArkWebTouchSelectionControllerClientOSRExt::SetSelectAllClicked(int command_id) {
  if (!rwhv_) {
    return;
  }
  auto browser = rwhv_->browser_impl();
  if (!browser) {
    return;
  }
  auto client = browser->client();
  if (!client) {
    return;
  }
  auto render = client->GetRenderHandler();
  if (!render) {
    return;
  }
  if (command_id == QM_EDITFLAG_CAN_SELECT_ALL) {
    render->NotifySelectAllClicked(true);
  } else {
    render->NotifySelectAllClicked(false);
  }
}
#endif  // #ifdef ARKWEB_DRAG_DROP
#if BUILDFLAG(ARKWEB_MENU)
void ArkWebTouchSelectionControllerClientOSRExt::SelectBetweenCoordinatesV2(
    const gfx::PointF& point,
    bool is_base) {
  active_client_->SelectBetweenCoordinatesV2(point, is_base);
}

void CefTouchSelectionControllerClientOSR::InternalClient::
    SelectBetweenCoordinatesV2(const gfx::PointF& point, bool is_base) {
  if (auto host_delegate = rwhv_->host()->delegate()) {
    host_delegate->SelectRangeV2(gfx::ToRoundedPoint(point), is_base);
  }
}
#endif
void ArkWebTouchSelectionControllerClientOSRExt::TemporarilyCloseQuickMenu() {
  if (!quick_menu_running_) {
    return;
  }

  quick_menu_running_ = false;
#if BUILDFLAG(ARKWEB_MENU_HANDLE)
  rwhv_->render_widget_host()->SetOrientationChange(false);
#endif // ARKWEB_MENU_HANDLE

  auto browser = rwhv_->browser_impl();
  if (auto handler = browser->client()->GetContextMenuHandler()) {
    handler->AsCefContextMenuHandlerExt()->OnQuickMenuDismissed(
        browser.get(), browser->GetFocusedFrame(), false);
#if BUILDFLAG(ARKWEB_DRAG_DROP)
    handles_hidden_by_selection_ui_ = false;
    AsArkWebTouchSelectionControllerClientOSRExt()->NotifyTouchSelectionChanged(false);
#endif
  }
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  LOG(INFO) << "Close Quick Menu Now.";
  if (browser->web_contents()) {
    browser->web_contents()->SetShowingContextMenu(false);
  }
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)
}

void ArkWebTouchSelectionControllerClientOSRExt::ClearSpecialSelectedTagTemporarily() {
#if BUILDFLAG(ARKWEB_AI)
  if(!rwhv_ || !rwhv_->AsArkWebRenderWidgetHostViewOSRExt()) {
    return;
  }
  isSelectionNotEmptyForAI_ = false;
  rwhv_->AsArkWebRenderWidgetHostViewOSRExt()->SetDataDetectorSelectText(std::u16string());
#endif
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  if (base::CommandLine::ForCurrentProcess() &&
      base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebExFreeCopy)) {
    AsArkWebTouchSelectionControllerClientOSRExt()->SelectionTextNotEmpty(false);
    rwhv_->AsArkWebRenderWidgetHostViewOSRExt()->SetLastSelectedTextFromContextParam(std::u16string());
  }
#endif
}

void ArkWebTouchSelectionControllerClientOSRExt::CloseQuickMenu() {
  TemporarilyCloseQuickMenu();
  AsArkWebTouchSelectionControllerClientOSRExt()->ClearSpecialSelectedTagTemporarily();
}

void ArkWebTouchSelectionControllerClientOSRExt::ShowQuickMenu() {
  auto browser = rwhv_->browser_impl();
  if (auto handler = browser->client()->GetContextMenuHandler()) {
    gfx::RectF rect =
        rwhv_->selection_controller()->GetVisibleRectBetweenBounds();

    gfx::PointF origin = rect.origin();
    gfx::PointF bottom_right = rect.bottom_right();
    auto client_bounds = gfx::RectF(rwhv_->GetViewBounds());
    origin.SetToMax(client_bounds.origin());
    bottom_right.SetToMin(client_bounds.bottom_right());
#if !BUILDFLAG(ARKWEB_CLIPBOARD)
    if (origin.x() > bottom_right.x() || origin.y() > bottom_right.y()) {
      return;
    }
#endif  // !BUILDFLAG(ARKWEB_CLIPBOARD)
    gfx::Vector2dF diagonal = bottom_right - origin;
    gfx::SizeF size(diagonal.x(), diagonal.y());

#if BUILDFLAG(ARKWEB_MENU_HANDLE)
    rwhv_->render_widget_host()->SetOrientationChange(false);
#endif // ARKWEB_MENU_HANDLE
    int quickmenuflags = 0;
    for (const auto& command : kMenuCommands) {
      if (active_menu_client_->IsCommandIdEnabled(command)) {
        quickmenuflags |= command;
      }
    }
    LOG(INFO) << "Quick Menu Flags Is = " << quickmenuflags;
    CefRefPtr<CefRunQuickMenuCallbackImpl> callbackImpl(
        new CefRunQuickMenuCallbackImpl(base::BindOnce(
            &CefTouchSelectionControllerClientOSR::ExecuteCommand,
            weak_ptr_factory_.GetWeakPtr())));

    quick_menu_running_ = true;
#if BUILDFLAG(ARKWEB_DRAG_DROP)
    handles_hidden_by_selection_ui_ = false;
#endif
#if BUILDFLAG(ARKWEB_VIBRATE)
    ui::TouchSelectionController* controller = GetTouchSelectionController();
    bool isLongPressSelectionActive = false;
    if (controller) {
      isLongPressSelectionActive = controller->AsTouchSelectionControllerExt()
                                       ->IsLongPressDragSelectionActive();
      LOG(INFO) << "The selection long press active is " << isLongPressSelectionActive;
    }
#if BUILDFLAG(ARKWEB_MENU)
    auto handle_visible_callback = base::BindRepeating(
        &ArkWebTouchSelectionControllerClientOSRExt::SetQuickMenuRequested,
        weak_ptr_factory_.GetWeakPtr());
    handler->SetHandleVisibleCallback(handle_visible_callback);
#endif
    if (!handler->AsCefContextMenuHandlerExt()->RunQuickMenu(
            browser, browser->GetFocusedFrame(),
            {static_cast<int>(std::round(origin.x())),
             static_cast<int>(std::round(origin.y()))},
            {static_cast<int>(std::round(size.width())),
             static_cast<int>(std::round(size.height()))},
            {clipped_selection_bounds_.x(), clipped_selection_bounds_.y(),
             clipped_selection_bounds_.width(),
             clipped_selection_bounds_.height()},
            static_cast<CefContextMenuHandler::QuickMenuEditStateFlags>(
                quickmenuflags),
            callbackImpl, false, isLongPressSelectionActive)) {
      callbackImpl->Disconnect();
      if (browser) {
        if (auto client = browser->client()) {
          if (auto render = client->GetRenderHandler()) {
            render->NotifySelectAllClicked(false);
          }
        }
      }
      CloseQuickMenu();
#endif  // BUILDFLAG(ARKWEB_VIBRATE)
#if BUILDFLAG(ARKWEB_CLIPBOARD)
    } else {
      LOG(INFO) << "Show Handle Quick Menu Success";
      if (!browser) {
        return;
      }
      if (browser->web_contents()) {
        browser->web_contents()->SetShowingContextMenu(true);
      }
      browser->AsAlloyBrowserHostImplExt()->SetTouchInsertHandleMenuShow(false);
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)
    }
  }
}
void ArkWebTouchSelectionControllerClientOSRExt::UpdateQuickMenu() {
  // Hide the quick menu if there is any. This should happen even if the menu
  // should be shown again, in order to update its location or content.
  if (quick_menu_running_) {
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
    TemporarilyCloseQuickMenu();
#else
    CloseQuickMenu();
#endif
  } else {
    quick_menu_timer_.Stop();
  }

  // Start timer to show quick menu if necessary.
  if (ShouldShowQuickMenu()) {
    quick_menu_timer_.Reset();
  }
}
bool ArkWebTouchSelectionControllerClientOSRExt::IsCommandIdEnabled(
    int command_id) const {
  bool editable = rwhv_->GetTextInputType() != ui::TEXT_INPUT_TYPE_NONE;
  bool readable = rwhv_->GetTextInputType() != ui::TEXT_INPUT_TYPE_PASSWORD;
  bool has_selection = !rwhv_->GetSelectedText().empty();
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  bool has_text =
      !rwhv_->AsArkWebRenderWidgetHostViewOSRExt()->GetText().empty();
#endif
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebExFreeCopy)) {
    has_selection = has_selection || isSelectionTextNotEmpty_;
  }
#endif
#if BUILDFLAG(ARKWEB_AI)
  has_selection = has_selection || isSelectionNotEmptyForAI_;
#endif
  switch (command_id) {
    case QM_EDITFLAG_CAN_ELLIPSIS:
      return true;  // Always allowed to show the ellipsis button.
    case QM_EDITFLAG_CAN_CUT:
      return editable && readable && has_selection;
    case QM_EDITFLAG_CAN_COPY:
      return readable && has_selection;
    case QM_EDITFLAG_CAN_PASTE: {
#if BUILDFLAG(ARKWEB_CLIPBOARD)
      bool can_paste = false;
      if (!editable) {
        LOG(INFO) << "This area is not editable.";
        return can_paste;
      }
      can_paste = ui::Clipboard::GetForCurrentThread()->HasPasteData();
      return can_paste;
#else
      std::u16string result;
      ui::DataTransferEndpoint data_dst = ui::DataTransferEndpoint(
          ui::EndpointType::kDefault, /*notify_if_restricted=*/false);
      ui::Clipboard::GetForCurrentThread()->ReadText(
          ui::ClipboardBuffer::kCopyPaste, &data_dst, &result);
      return editable && !result.empty();
#endif  // if BUILDFLAG(ARKWEB_CLIPBOARD)
    }
#if BUILDFLAG(ARKWEB_CLIPBOARD)
    case QM_EDITFLAG_CAN_SELECT_ALL:
#if BUILDFLAG(ARKWEB_MENU_HANDLE)
      if (isSelectAll_) {
        return false;
      }
#endif // ARKWEB_MENU_HANDLE
      if (!editable && readable) {
        return true;
      }
      if (editable && has_text) {
        return true;
      }
      return false;
#endif  // #if BUILDFLAG(ARKWEB_CLIPBOARD)
    default:
      return false;
  }
}
void ArkWebTouchSelectionControllerClientOSRExt::ExecuteCommand(
    int command_id,
    int event_flags) {
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  AsArkWebTouchSelectionControllerClientOSRExt()->SetSelectAllClicked(
      command_id);
#endif

#if !BUILDFLAG(ARKWEB_CLIPBOARD)
  if (command_id == kInvalidCommandId) {
    LOG(ERROR) << "Quick menu command id is invaild";
    return;
  }
#endif  // #if BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_MENU)
  if (command_id != QM_EDITFLAG_CAN_ELLIPSIS &&
      command_id != QM_EDITFLAG_CAN_SELECT_ALL
#if BUILDFLAG(ARKWEB_MENU_HANDLE)
      && command_id != QM_EDITFLAG_CAN_COPY
#endif // ARKWEB_MENU_HANDLE
      ) {
#else
  if (command_id != QM_EDITFLAG_CAN_ELLIPSIS) {
#endif  // BUILDFLAG(ARKWEB_MENU)
    rwhv_->selection_controller()->HideAndDisallowShowingAutomatically();
  }

  content::RenderWidgetHostDelegate* host_delegate = rwhv_->host()->delegate();
  if (!host_delegate) {
    return;
  }

  auto browser = rwhv_->browser_impl();
  if (auto handler = browser->client()->GetContextMenuHandler()) {
    if (handler->OnQuickMenuCommand(
            browser.get(), browser->GetFocusedFrame(), command_id,
            static_cast<cef_event_flags_t>(event_flags))) {
      return;
    }
  }
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  absl::optional<std::u16string> value;
  LOG(INFO) << "Quick menu command id = " << command_id;
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

  switch (command_id) {
    case QM_EDITFLAG_CAN_CUT:
      host_delegate->Cut();
      break;
    case QM_EDITFLAG_CAN_COPY:
      host_delegate->Copy();
#if BUILDFLAG(ARKWEB_MENU_HANDLE)
      isCopy_ = true;
#endif // ARKWEB_MENU_HANDLE
#if BUILDFLAG(ARKWEB_NAVIGATION)
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)
      break;
    case QM_EDITFLAG_CAN_PASTE:
      host_delegate->Paste();
      break;
#if BUILDFLAG(ARKWEB_CLIPBOARD)
    case QM_EDITFLAG_CAN_SELECT_ALL:
      host_delegate->SelectAll();
#if BUILDFLAG(ARKWEB_MENU_HANDLE)
      isSelectAll_ = true;
#endif // ARKWEB_MENU_HANDLE
      ShowQuickMenu();
      break;
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)
    case QM_EDITFLAG_CAN_ELLIPSIS:
      CloseQuickMenu();
      RunContextMenu();
#if BUILDFLAG(ARKWEB_CLIPBOARD)
      browser->web_contents()->CollapseSelection();
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)
      break;
    default:
      // Invalid command, do nothing.
      // Also reached when callback is destroyed/cancelled.
#if BUILDFLAG(ARKWEB_CLIPBOARD)
      browser->web_contents()->CollapseSelection();
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)
      break;
  }
}

#if BUILDFLAG(ARKWEB_MENU)
void ArkWebTouchSelectionControllerClientOSRExt::NotifyShowMagnifier() {
  if (!rwhv_) {
    return;
  }
  auto browser = rwhv_->browser_impl();
  if (browser && browser->client()) {
    auto handler = browser->client()->GetContextMenuHandler();
    if (handler) {
      handler->ShowMagnifier();
    }
  }
}

void ArkWebTouchSelectionControllerClientOSRExt::
    ConvertClientClippedSelectionBounds(gfx::Rect& clipped_selection_bounds) {
  if(rwhv_ && rwhv_->AsArkWebRenderWidgetHostViewOSRExt()) {
    CefRect converted_rect(
        clipped_selection_bounds.x(), clipped_selection_bounds.y(),
        clipped_selection_bounds.width(), clipped_selection_bounds.height());
    rwhv_->AsArkWebRenderWidgetHostViewOSRExt()->OnSelectAreaChanged(
        converted_rect, true);
    clipped_selection_bounds = {converted_rect.x, converted_rect.y,
                                converted_rect.width, converted_rect.height};
  }
}

void ArkWebTouchSelectionControllerClientOSRExt::SetQuickMenuRequested(bool is_visible)
{
  quick_menu_requested_ = is_visible;
}
#endif

#if BUILDFLAG(ARKWEB_PDF)
void ArkWebTouchSelectionControllerClientOSRExt::ResetResponsePendingInputEvent()
{
  ui::TouchSelectionController* controller = GetTouchSelectionController();
  if (controller) {
    controller->AsTouchSelectionControllerExt()->ResetResponsePendingInputEvent();
  }
}
#endif  // BUILDFLAG(ARKWEB_PDF)