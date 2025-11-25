/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
