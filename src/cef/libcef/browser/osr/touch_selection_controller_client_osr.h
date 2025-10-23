// Copyright 2022 The Chromium Embedded Framework Authors.
// Portions copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_OSR_TOUCH_SELECTION_CONTROLLER_CLIENT_OSR_H_
#define CEF_LIBCEF_BROWSER_OSR_TOUCH_SELECTION_CONTROLLER_CLIENT_OSR_H_

#include <memory>

#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/observer_list.h"
#include "base/timer/timer.h"
#include "content/common/content_export.h"
#include "content/public/browser/touch_selection_controller_client_manager.h"
#include "ui/touch_selection/touch_selection_controller.h"
#include "ui/touch_selection/touch_selection_menu_runner.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
#include "cef/include/cef_context_menu_handler.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#endif
namespace content {
struct ContextMenuParams;
}
#if BUILDFLAG(IS_ARKWEB)
class CefRenderWidgetHostViewOSR;
class ArkWebTouchSelectionControllerClientOSRExt;
class ArkWebCefTouchHandleDrawableOSRExt;
#endif
class CefRunQuickMenuCallbackImpl : public CefRunQuickMenuCallback {
 public:
  using Callback = base::OnceCallback<void(int, int)>;

  explicit CefRunQuickMenuCallbackImpl(Callback callback)
      : callback_(std::move(callback)) {}

  CefRunQuickMenuCallbackImpl(const CefRunQuickMenuCallbackImpl&) = delete;
  CefRunQuickMenuCallbackImpl& operator=(const CefRunQuickMenuCallbackImpl&) =
      delete;

  ~CefRunQuickMenuCallbackImpl() override;

  void Continue(int command_id, cef_event_flags_t event_flags) override;

  void Cancel() override;

  void Disconnect();

 private:
  static void RunNow(Callback callback,
                     int command_id,
                     cef_event_flags_t event_flags);

  Callback callback_;

  IMPLEMENT_REFCOUNTING(CefRunQuickMenuCallbackImpl);
};

// An implementation of |TouchSelectionControllerClient| to be used in OSR's
// implementation of touch selection for contents.
// Copied from TouchSelectionControllerClientAura.
class CefTouchSelectionControllerClientOSR
    : public ui::TouchSelectionControllerClient,
      public ui::TouchSelectionMenuClient,
      public content::TouchSelectionControllerClientManager {
 public:
  virtual ArkWebTouchSelectionControllerClientOSRExt*
  AsArkWebTouchSelectionControllerClientOSRExt() {
    return nullptr;
  }
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  friend class ArkWebTouchSelectionControllerClientOSRExt;
#endif
  explicit CefTouchSelectionControllerClientOSR(
      CefRenderWidgetHostViewOSR* rwhv);

  CefTouchSelectionControllerClientOSR(
      const CefTouchSelectionControllerClientOSR&) = delete;
  CefTouchSelectionControllerClientOSR& operator=(
      const CefTouchSelectionControllerClientOSR&) = delete;

  ~CefTouchSelectionControllerClientOSR() override;

  void CloseQuickMenuAndHideHandles();

  void OnWindowMoved();

  // Called on first touch down/last touch up to hide/show the quick menu.
  void OnTouchDown();

  void OnTouchUp();
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  virtual void OnScrollStarted();
  virtual void OnScrollCompleted();
#else
  // Called when touch scroll starts/completes to hide/show touch handles and
  // the quick menu.
  void OnScrollStarted();

  void OnScrollCompleted();
#endif
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  virtual bool HandleContextMenu(const content::ContextMenuParams& params);
#else
  // Gives an opportunity to the client to handle context menu request and show
  // the quick menu instead, if appropriate. Returns |true| to indicate that no
  // further handling is needed.
  // TODO(mohsen): This is to match Chrome on Android behavior. However, it is
  // better not to send context menu request from the renderer in this case and
  // instead decide in the client about showing the quick menu in response to
  // selection events. (http://crbug.com/548245)
  virtual bool HandleContextMenu(const content::ContextMenuParams& params);
#endif
  void UpdateClientSelectionBounds(const gfx::SelectionBound& start,
                                   const gfx::SelectionBound& end);

  // TouchSelectionControllerClientManager.
  void DidStopFlinging() override;
  void OnSwipeToMoveCursorBegin() override;
  void OnSwipeToMoveCursorEnd() override;
  void OnClientHitTestRegionUpdated(
      ui::TouchSelectionControllerClient* client) override;
  void UpdateClientSelectionBounds(
      const gfx::SelectionBound& start,
      const gfx::SelectionBound& end,
      ui::TouchSelectionControllerClient* client,
      ui::TouchSelectionMenuClient* menu_client) override;
  void InvalidateClient(ui::TouchSelectionControllerClient* client) override;
  ui::TouchSelectionController* GetTouchSelectionController() override;
  void AddObserver(
      TouchSelectionControllerClientManager::Observer* observer) override;
  void RemoveObserver(
      TouchSelectionControllerClientManager::Observer* observer) override;

 private:
  class EnvEventObserver;

  bool IsQuickMenuAvailable() const;

#if BUILDFLAG(IS_ARKWEB)
  virtual void CloseQuickMenu();
  virtual void ShowQuickMenu();
  virtual void UpdateQuickMenu();
  virtual void TemporarilyCloseQuickMenu();
#else
  void CloseQuickMenu();
  void ShowQuickMenu();
  void UpdateQuickMenu();
  void TemporarilyCloseQuickMenu();
#endif
  // ui::TouchSelectionControllerClient:
  bool SupportsAnimation() const override;
  void SetNeedsAnimate() override;
  void MoveCaret(const gfx::PointF& position) override;
  void MoveRangeSelectionExtent(const gfx::PointF& extent) override;
  void SelectBetweenCoordinates(const gfx::PointF& base,
                                const gfx::PointF& extent) override;
  void OnSelectionEvent(ui::SelectionEventType event) override;
  void OnDragUpdate(const ui::TouchSelectionDraggable::Type type,
                    const gfx::PointF& position) override;
  std::unique_ptr<ui::TouchHandleDrawable> CreateDrawable() override;
  void DidScroll() override;

  // ui::TouchSelectionMenuClient:
  bool IsCommandIdEnabled(int command_id) const override;
  void ExecuteCommand(int command_id, int event_flags) override;
  void RunContextMenu() override;
  bool ShouldShowQuickMenu() override;
  std::u16string GetSelectedText() override;

  // Not owned, non-null for the lifetime of this object.
  raw_ptr<CefRenderWidgetHostViewOSR> rwhv_;

  class InternalClient final : public ui::TouchSelectionControllerClient {
   public:
    explicit InternalClient(CefRenderWidgetHostViewOSR* rwhv) : rwhv_(rwhv) {}
    ~InternalClient() final = default;

    bool SupportsAnimation() const final;
    void SetNeedsAnimate() final;
    void MoveCaret(const gfx::PointF& position) final;
    void MoveRangeSelectionExtent(const gfx::PointF& extent) final;
    void SelectBetweenCoordinates(const gfx::PointF& base,
                                  const gfx::PointF& extent) final;
#if BUILDFLAG(ARKWEB_MENU)
    void SelectBetweenCoordinatesV2(const gfx::PointF& point,
                                  bool is_base) final;
#endif
    void OnSelectionEvent(ui::SelectionEventType event) final;
    void OnDragUpdate(const ui::TouchSelectionDraggable::Type type,
                      const gfx::PointF& position) final;
    std::unique_ptr<ui::TouchHandleDrawable> CreateDrawable() final;
    void DidScroll() override;

   private:
    raw_ptr<CefRenderWidgetHostViewOSR> rwhv_;
  } internal_client_;

  // Keep track of which client interface to use.
  raw_ptr<TouchSelectionControllerClient> active_client_;
  raw_ptr<TouchSelectionMenuClient> active_menu_client_;
  gfx::SelectionBound manager_selection_start_;
  gfx::SelectionBound manager_selection_end_;

  base::ObserverList<TouchSelectionControllerClientManager::Observer>
      observers_;

  base::RetainingOneShotTimer quick_menu_timer_;
  bool quick_menu_requested_ = false;
  bool quick_menu_running_ = false;
  bool touch_down_ = false;
  bool scroll_in_progress_ = false;
  bool handle_drag_in_progress_ = false;
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  bool isSelectionTextNotEmpty_ = false;
#endif

  gfx::Rect clipped_selection_bounds_;

  base::WeakPtrFactory<CefTouchSelectionControllerClientOSR> weak_ptr_factory_;
};

#endif
