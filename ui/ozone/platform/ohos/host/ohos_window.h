// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_OHOS_WINDOW_H_
#define UI_OZONE_PLATFORM_OHOS_OHOS_WINDOW_H_

#include "base/memory/weak_ptr.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "ohos/adapter/window/window_common.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ohos/adapter/xcomponent/event/window_event_common.h"
#include "ui/display/display.h"
#include "ui/display/types/display_constants.h"
#include "ui/events/event_target.h"
#include "ui/events/platform/platform_event_dispatcher.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/ozone/common/bitmap_cursor.h"
#include "ui/ozone/platform/ohos/drag/ohos_drag_manager.h"
#include "ui/platform_window/platform_window.h"
#include "ui/platform_window/platform_window_delegate.h"
#include "ui/platform_window/platform_window_init_properties.h"

namespace ui {

using namespace ohos::adapter::window;
using namespace ohos::adapter::xcomponent;

class OhosPopup;
class OhosToplevelWindow;
class OhosPipWindow;
class OhosWindowManager;

class OhosWindow : public PlatformWindow,
                   public PlatformEventDispatcher,
                   public EventTarget,
                   public OhosDragManager::Delegate {
 public:
  enum class ActivationState {
    kUnknown,
    kActive,
    kInactive,
  };

  using XCEvent = ohos::adapter::xcomponent::Event;
  using XCEventType = ohos::adapter::xcomponent::EventType;

  OhosWindow(OhosWindow&&) = delete;
  OhosWindow& operator=(OhosWindow&&) = delete;
  OhosWindow(PlatformWindowDelegate* delegate,
             OhosWindowManager* manager);

  OhosWindow(const OhosWindow&) = delete;
  OhosWindow& operator=(const OhosWindow&) = delete;

  ~OhosWindow() override;

  // A factory method that can create any of the derived types of OhosWindow
  // (OhosToplevelWindow and OhosPopup).
  static std::unique_ptr<OhosWindow> Create(
      PlatformWindowDelegate* delegate,
      OhosWindowManager* manager,
      PlatformWindowInitProperties properties);

  // Initializes the OhosWindow with supplied properties.
  virtual bool Initialize(PlatformWindowInitProperties properties);

  void set_parent_window(OhosWindow* parent_window) {
    parent_window_ = parent_window;
  }
  OhosWindow* parent_window() const { return parent_window_; }

  void SetWidget(gfx::AcceleratedWidget new_widget);
  gfx::AcceleratedWidget GetWidget() const;

  // Set a child of this window.
  void set_child_window(OhosWindow* window) { child_window_ = window; }
  OhosWindow* child_window() const { return child_window_; }

  // Returns current type of the window.
  PlatformWindowType type() const { return type_; }

  void OnSurfaceCreated();
  void OnSurfaceDestoryed();

  void Applied(const gfx::Rect& origin_bounds,
               const gfx::Rect& bounds,
               bool force = false);

  // PlatformWindow:
  void Show(bool inactive) override;
  void Hide() override;
  void Close() override;
  bool IsVisible() const override;
  void PrepareForShutdown() override;
  void SetBoundsInPixels(const gfx::Rect& bounds) override;
  gfx::Rect GetBoundsInPixels() const override;
  void SetBoundsInDIP(const gfx::Rect& bounds) override;
  gfx::Rect GetBoundsInDIP() const override;
  void SetTitle(const std::u16string& title) override;
  void SetCapture() override;
  void ReleaseCapture() override;
  void SetFullscreen(bool fullscreen, int64_t target_display_id) override;
  bool HasCapture() const override;
  void Maximize() override;
  void Minimize() override;
  void Restore() override;
  PlatformWindowState GetPlatformWindowState() const override;
  void Activate() override;
  void Deactivate() override;
  std::string GetWindowUniqueId() const override;
  void SetUseNativeFrame(bool use_native_frame) override;
  bool ShouldUseNativeFrame() const override;
  void SetCursor(scoped_refptr<PlatformCursor> cursor) override;
  void MoveCursorTo(const gfx::Point& location) override;
  void ConfineCursorToBounds(const gfx::Rect& bounds) override;
  void SetRestoredBoundsInDIP(const gfx::Rect& bounds) override;
  gfx::Rect GetRestoredBoundsInDIP() const override;
  void SetWindowIcons(const gfx::ImageSkia& window_icon,
                      const gfx::ImageSkia& app_icon) override;
  void SizeConstraintsChanged() override;

  // PlatformEventDispatcher
  bool CanDispatchEvent(const PlatformEvent& event) override;
  uint32_t DispatchEvent(const PlatformEvent& native_event) override;

  // EventTarget:
  bool CanAcceptEvent(const Event& event) override;
  EventTarget* GetParentTarget() override;
  std::unique_ptr<EventTargetIterator> GetChildIterator() const override;
  EventTargeter* GetEventTargeter() override;

  void OnWindowLostCapture();
  OhosWindow* GetRootParentWindow();

  void OnActivateEvent();
  void SetSurfaceId(uint64_t surface_id);
  void OnPointerFocusChanged(const bool focused);

  virtual OhosPopup* AsOhosPopup();
  virtual OhosToplevelWindow* AsOhosToplevelWindow();
  virtual WindowInitParameter BuildWindowInitParameter();

  scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner() {
    return ui_task_runner_;
  }

  base::WeakPtr<OhosWindow> AsWeakPtr() {
    return weak_ptr_factory_.GetWeakPtr();
  }

  ActivationState activation_state() const { return activation_state_;}

  PlatformWindowDelegate* delegate() const { return delegate_; }
  OhosWindowManager* window_manager() const { return window_manager_; }
  OhosDragManager* GetDragManager() const { return drag_manager_.get(); }

  // OhosDragManager::Delegate
  void SetDragSourceWidget() override;
  void ClearDragSourceWidget() override;
  void EndDrag() override;

  void SetFocus(bool focus) { has_focus_ = focus; }
  bool HasFocus() const { return has_focus_; }

  void SetCurrentDisplayId(int64_t display_id) {
    current_display_id_ = display_id;
    ohos::adapter::xcomponent::WindowAdapter::GetInstance().SetCurrentDisplayId(display_id);
  }
  int64_t GetCurrentDisplayId() {
    return current_display_id_;
  }
  display::Display PrepareDisplayForNewWindow();

  virtual int32_t GetOriginWindowId();
  virtual display::Display GetCurrentDisplay();

  bool is_visible_{true};

  // The bounds of our window before the window was maximized.
  gfx::Rect restored_bounds_in_pixels_;

  // The bounds of window
  gfx::Rect bounds_in_pixels_;

  // keep the previous set of bounds
  gfx::Rect previous_bounds_in_pixels_;

  // Contains the current state of the window.
  PlatformWindowState state_ = PlatformWindowState::kUnknown;

 protected:
  void BindNodeHandle();

 private:
  uint32_t DispatchEventToDelegate(const PlatformEvent& native_event);

  // Additional initialization of derived classes.
  virtual void OnInitialize(PlatformWindowInitProperties properties) {}

  virtual bool OnCreateWindow(WindowInitParameter parameter) { return true; }

  virtual void HandleEvent(std::shared_ptr<XCEvent> event) {}

  virtual bool IsHitCaptionButton(const gfx::PointF& point) {
    return false;
  }

  void RegistWindowEvent();
  void UnRegistWindowEvent();

  void UpdateCursorShape(scoped_refptr<BitmapCursor> cursor);

  virtual bool HasInitDone();

  raw_ptr<PlatformWindowDelegate> delegate_;
  raw_ptr<OhosWindowManager> window_manager_;
  raw_ptr<OhosWindow> parent_window_ = nullptr;
  raw_ptr<OhosWindow> child_window_ = nullptr;

  std::unique_ptr<OhosDragManager> drag_manager_;

  // AcceleratedWidget for this window.
  gfx::AcceleratedWidget widget_;

  // Unique ID for this window
  std::string window_unique_id_;

  ActivationState activation_state_ = ActivationState::kUnknown;
  
  PlatformWindowType type_ = PlatformWindowType::kWindow;

  scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner_;

  // The current cursor bitmap
  scoped_refptr<BitmapCursor> cursor_;

  bool has_focus_ = false;
  bool has_pointer_focus_ = false;
  int64_t current_display_id_ = display::kInvalidDisplayId;
  // whether the node handle xcomponent already bound with node content in ability
  bool is_ability_bound_ = false;

  base::WeakPtrFactory<OhosWindow> weak_ptr_factory_{this};
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_OHOS_WINDOW_H_
