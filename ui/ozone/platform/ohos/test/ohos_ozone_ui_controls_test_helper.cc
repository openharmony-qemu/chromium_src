// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/test/ohos_ozone_ui_controls_test_helper.h"

#include "ui/aura/window_tree_host.h"
#include "ui/events/base_event_utils.h"
#include "ui/events/event.h"
#include "ui/events/event_constants.h"
#include "ui/events/keycodes/dom/keycode_converter.h"
#include "ui/events/keycodes/keyboard_codes_posix.h"
#include "ui/events/ozone/layout/keyboard_layout_engine.h"
#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/events/pointer_details.h"
#include "ui/events/types/event_type.h"
#include "ui/ozone/platform/ohos/host/ohos_event_source_base.h"

namespace ui {
// Mask of the buttons currently down.
// just set it to 1 for test on the ohos platform
constexpr unsigned kButtonDownMask = 1;

OhosOzoneUIControlsTestHelper::OhosOzoneUIControlsTestHelper() = default;
OhosOzoneUIControlsTestHelper::~OhosOzoneUIControlsTestHelper() = default;

void OhosOzoneUIControlsTestHelper::Reset() {}

bool OhosOzoneUIControlsTestHelper::SupportsScreenCoordinates() const {
  return true;
}

unsigned OhosOzoneUIControlsTestHelper::ButtonDownMask() const {
  return kButtonDownMask;
}

void OhosOzoneUIControlsTestHelper::SendKeyEvents(gfx::AcceleratedWidget widget,
                                                  ui::KeyboardCode key,
                                                  int key_event_types,
                                                  int accelerator_state,
                                                  base::OnceClosure closure) {
  DomCode dom_code = KeycodeConverter::NativeKeycodeToDomCode(key);
  DomKey dom_key;
  KeyboardCode key_code = VKEY_UNKNOWN;
  auto* layout_engine = KeyboardLayoutEngineManager::GetKeyboardLayoutEngine();
  if (layout_engine == nullptr ||
      !layout_engine->Lookup(dom_code, key_event_types, &dom_key, &key_code)) {
    LOG(WARNING) << "[multiinput]failed to decode key_code: " << key_code;
    return;
  }
  auto* event_source =
      reinterpret_cast<OhosEventSourceBase*>(PlatformEventSource::GetInstance());

  KeyEvent event(EventType::kKeyPressed, ui::VKEY_MENU, dom_code, key_event_types,
                 dom_key, ui::EventTimeForNow());
  event_source->OnKeyEvent(widget, event);

  KeyEvent event_key(EventType::kKeyPressed, key_code, dom_code, key_event_types,
                     dom_key, ui::EventTimeForNow());
  event_source->OnKeyEvent(widget, event_key);
}

void OhosOzoneUIControlsTestHelper::SendMouseMotionNotifyEvent(
    gfx::AcceleratedWidget widget,
    const gfx::Point& mouse_loc,
    const gfx::Point& mouse_loc_in_screen,
    base::OnceClosure closure) {
  auto* host = aura::WindowTreeHost::GetForAcceleratedWidget(widget);
  gfx::Point mouse_loc_in_screen_px = mouse_loc_in_screen;
  // TODO(crbug.com/1430805): fix this conversion.
  host->ConvertDIPToPixels(&mouse_loc_in_screen_px);
}

void OhosOzoneUIControlsTestHelper::SendMouseEvent(
    gfx::AcceleratedWidget widget,
    ui_controls::MouseButton type,
    int button_state,
    int accelerator_state,
    const gfx::Point& mouse_loc,
    const gfx::Point& mouse_loc_in_screen,
    base::OnceClosure closure) {
  auto* host = aura::WindowTreeHost::GetForAcceleratedWidget(widget);
  gfx::Point mouse_loc_in_screen_px = mouse_loc_in_screen;
  // TODO(crbug.com/1430805): fix this conversion.
  host->ConvertDIPToPixels(&mouse_loc_in_screen_px);
}

void OhosOzoneUIControlsTestHelper::RunClosureAfterAllPendingUIEvents(
    base::OnceClosure closure) {}

bool OhosOzoneUIControlsTestHelper::MustUseUiControlsForMoveCursorTo() {
  return false;
}

OzoneUIControlsTestHelper* CreateOzoneUIControlsTestHelperOhos() {
  return new OhosOzoneUIControlsTestHelper();
}

}  // namespace ui
