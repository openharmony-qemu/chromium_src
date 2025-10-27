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

#ifndef CEF_LIBCEF_BROWSER_OSR_TOUCH_SELECTION_CONTROLLER_CLIENT_OSR_EXT_H_
#define CEF_LIBCEF_BROWSER_OSR_TOUCH_SELECTION_CONTROLLER_CLIENT_OSR_EXT_H_

#include "arkweb/build/features/features.h"
#include "cef/libcef/browser/osr/touch_selection_controller_client_osr.h"

class ArkWebTouchSelectionControllerClientOSRExt
    : public CefTouchSelectionControllerClientOSR {
 public:
  ArkWebTouchSelectionControllerClientOSRExt*
  AsArkWebTouchSelectionControllerClientOSRExt() override {
    return this;
  }
  explicit ArkWebTouchSelectionControllerClientOSRExt(
      CefRenderWidgetHostViewOSR* rwhv);
  void CloseQuickMenuAndHideHandles();
  // Called when touch scroll starts/completes to hide/show touch handles and
  // the quick menu.
  void OnScrollStarted() override;
  void OnScrollCompleted() override;
  bool HandleContextMenu(const content::ContextMenuParams& params) override;
#if BUILDFLAG(ARKWEB_MENU)
  // CefTouchSelectionControllerClientOSR:
  void SetTemporarilyHidden(bool hidden);

  void MouseSelectMenuShow(bool show);
  void ChangeVisibilityOfQuickMenu();
  bool IsQuickMenuShow();
  void UpdateClientClippedSelectionBounds(
      const gfx::Rect& clipped_selection_bounds) override;
  bool NeedPopupInsertTouchHandleQuickMenu();
#endif
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  void SelectionTextNotEmpty(bool has_selection);
#endif
#if BUILDFLAG(ARKWEB_VIBRATE)
  bool IsInsertHandleShow();
#endif  // BUILDFLAG(ARKWEB_VIBRATE)
// ui::TouchSelectionControllerClient:
void OnSelectionEvent(ui::SelectionEventType event) override;
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  void HideHandleAndQuickMenuIfNecessary(bool hide_handles) override;
#endif
#if BUILDFLAG(ARKWEB_MENU)
  // CefTouchSelectionControllerClientOSR:
  void NotifyTouchSelectionChanged(bool need_report);

  bool IsVaildSelectionHandleMove();
  void ExecuteCommandMouse(int command_id, int event_flags);
  void MouseMayUpdateClientClippedSelectionBounds(
      const gfx::Rect& clipped_selection_bounds);
#endif  // BUILDFLAG(ARKWEB_MENU)

#if BUILDFLAG(ARKWEB_MENU)
  void SelectBetweenCoordinatesV2(const gfx::PointF& point,
                                  bool is_base) final;
#endif
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  // CefTouchSelectionControllerClientOSR:
  void UpdateQuickMenuByHandlesHidden();
  void SetSelectAllClicked(int command_id);
#endif
  void ClearSpecialSelectedTagTemporarily();
  void CloseQuickMenu() override;
  void ShowQuickMenu() override;
  void UpdateQuickMenu() override;
  void TemporarilyCloseQuickMenu() override;
  bool IsCommandIdEnabled(int command_id) const override;
  void ExecuteCommand(int command_id, int event_flags) override;
#if BUILDFLAG(ARKWEB_MENU)
  void NotifyShowMagnifier() override;
  void ConvertClientClippedSelectionBounds(
    gfx::Rect& clipped_selection_bounds) override;
  bool IsShowHandle() override;
  void SetQuickMenuRequested(bool is_visible);
#endif  // BUILDFLAG(ARKWEB_MENU)

#if BUILDFLAG(ARKWEB_PDF)
void ResetResponsePendingInputEvent() override;
#endif  // BUILDFLAG(ARKWEB_PDF)
 private:
  // // Not owned, non-null for the lifetime of this object.
  // raw_ptr<CefRenderWidgetHostViewOSR> rwhv_;
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  bool handles_hidden_by_selection_ui_ = false;
#endif

#if BUILDFLAG(ARKWEB_MENU)
  base::TimeTicks select_handle_move_timer_;
  bool mouse_quick_menu_running_ = false;
#endif  // BUILDFLAG(ARKWEB_MENU)
#if BUILDFLAG(ARKWEB_MENU_HANDLE)
  bool isCopy_ = false;
  bool isSelectAll_ = false;
#endif // ARKWEB_MENU_HANDLE
#if BUILDFLAG(ARKWEB_AI)
  bool isSelectionNotEmptyForAI_;
#endif
  base::WeakPtrFactory<ArkWebTouchSelectionControllerClientOSRExt>
      weak_ptr_factory_;
};
#endif
