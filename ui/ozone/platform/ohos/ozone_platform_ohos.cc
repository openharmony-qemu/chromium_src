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

#include "ui/ozone/platform/ohos/ozone_platform_ohos.h"

#include <memory>

#include "build/build_config.h"
#include "ohos/adapter/node_handle/node_handle_impl.h"
#include "ui/base/cursor/cursor_factory.h"
#include "ui/base/ime/ohos/input_method_ohos.h"
#include "ui/display/types/native_display_delegate.h"
#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"
#include "ui/events/ozone/layout/stub/stub_keyboard_layout_engine.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/ozone/common/base_keyboard_hook.h"
#include "ui/ozone/common/bitmap_cursor_factory.h"
#include "ui/ozone/common/stub_overlay_manager.h"
#include "ui/ozone/platform/ohos/drag/ohos_window_drag_manager.h"
#include "ui/ozone/platform/ohos/gpu/ohos_surface_factory.h"
#include "ui/ozone/platform/ohos/host/ohos_canvas_surface.h"
#include "ui/ozone/platform/ohos/host/ohos_event_source.h"
#include "ui/ozone/platform/ohos/host/ohos_event_source_node_handle.h"
#include "ui/ozone/platform/ohos/host/ohos_screen.h"
#include "ui/ozone/platform/ohos/host/ohos_window.h"
#include "ui/ozone/platform/ohos/host/ohos_window_manager.h"
#include "ui/ozone/public/gpu_platform_support_host.h"
#include "ui/ozone/public/input_controller.h"
#include "ui/ozone/public/ozone_platform.h"
#include "ui/ozone/public/platform_window_manager.h"
#include "ui/ozone/public/surface_ozone_canvas.h"
#include "ui/ozone/public/system_input_injector.h"
#include "ui/platform_window/platform_window_init_properties.h"

namespace ui {

namespace {
// OzonePlatform for ohos mode
class OzonePlatformOhos : public OzonePlatform {
 public:
  OzonePlatformOhos() = default;

  OzonePlatformOhos(const OzonePlatformOhos&) = delete;
  OzonePlatformOhos& operator=(const OzonePlatformOhos&) = delete;

  ~OzonePlatformOhos() override = default;

  bool IsWindowCompositingSupported() const override {
    return true;
  }

  // OzonePlatform:
  ui::SurfaceFactoryOzone* GetSurfaceFactoryOzone() override {
    return surface_factory_.get();
  }
  OverlayManagerOzone* GetOverlayManager() override {
    return overlay_manager_.get();
  }
  CursorFactory* GetCursorFactory() override { return cursor_factory_.get(); }
  InputController* GetInputController() override {
    return input_controller_.get();
  }
  GpuPlatformSupportHost* GetGpuPlatformSupportHost() override {
    return gpu_platform_support_host_.get();
  }
  std::unique_ptr<SystemInputInjector> CreateSystemInputInjector() override {
    return nullptr;  // no input injection support.
  }
  std::unique_ptr<PlatformWindow> CreatePlatformWindow(
      PlatformWindowDelegate* delegate,
      PlatformWindowInitProperties properties) override {
    LOG(INFO) << "[ohoswindow] CreatePlatformWindow enter, type is "
              << static_cast<int>(properties.type);
    return OhosWindow::Create(delegate, window_manager_.get(),
                              std::move(properties));
  }
  std::unique_ptr<display::NativeDisplayDelegate> CreateNativeDisplayDelegate()
      override {
    return nullptr;
  }
  std::unique_ptr<PlatformScreen> CreateScreen() override {
    return std::make_unique<OhosScreen>(
        window_manager_.get(), platform_event_source_.get());
  }
  void InitScreen(PlatformScreen* screen) override {}
  std::unique_ptr<InputMethod> CreateInputMethod(
      ImeKeyEventDispatcher* ime_key_event_dispatcher,
      gfx::AcceleratedWidget widget) override {
    return std::make_unique<InputMethodOHOS>(ime_key_event_dispatcher, widget);
  }

  bool InitializeUI(const InitParams& params) override {
    window_manager_ = std::make_unique<OhosWindowManager>();
    surface_factory_ = std::make_unique<OhosSurfaceFactory>();
    window_drag_manager_ = std::make_unique<OhosWindowDragManager>();
    // This unbreaks tests that create their own.
    if (!PlatformEventSource::GetInstance()) {
      if (ohos::adapter::nodeHandle::NodeHandleImpl::GetInstance()
              .IsSupportNodeHandle()) {
        platform_event_source_ = std::make_unique<OhosEventSourceNodeHandle>(
            window_manager_.get(), window_drag_manager_.get());
      } else {
        platform_event_source_ = std::make_unique<OhosEventSource>(
            window_manager_.get(), window_drag_manager_.get());
      }
    }
    keyboard_layout_engine_ = std::make_unique<StubKeyboardLayoutEngine>();
    KeyboardLayoutEngineManager::SetKeyboardLayoutEngine(
        keyboard_layout_engine_.get());

    overlay_manager_ = std::make_unique<StubOverlayManager>();
    input_controller_ = CreateStubInputController();
    cursor_factory_ = std::make_unique<BitmapCursorFactory>();
    gpu_platform_support_host_.reset(CreateStubGpuPlatformSupportHost());
    return true;
  }

  void InitializeGPU(const InitParams& params) override {
    if (!surface_factory_) {
      surface_factory_ = std::make_unique<OhosSurfaceFactory>();
    }
  }

  const PlatformProperties& GetPlatformProperties() override {
    static base::NoDestructor<OzonePlatform::PlatformProperties> properties;
    static bool initialised = false;
    if (!initialised) {
      properties->set_parent_for_non_top_level_windows = true;
      initialised = true;
    }
    return *properties;
  }

  std::unique_ptr<PlatformKeyboardHook> CreateKeyboardHook(
      PlatformKeyboardHookTypes type,
      base::RepeatingCallback<void(KeyEvent* event)> callback,
      absl::optional<base::flat_set<DomCode>> dom_codes,
      gfx::AcceleratedWidget accelerated_widget) override {
    switch (type) {
      case PlatformKeyboardHookTypes::kModifier:
        return std::make_unique<BaseKeyboardHook>(std::move(dom_codes),
                                                  std::move(callback));
      default:
        return nullptr;
    }
  }

  PlatformWindowManager* GetPlatformWindowManager() override {
    return window_manager_.get();
  }

 private:
  std::unique_ptr<KeyboardLayoutEngine> keyboard_layout_engine_;
  std::unique_ptr<OhosWindowManager> window_manager_;
  std::unique_ptr<OhosSurfaceFactory> surface_factory_;
  std::unique_ptr<PlatformEventSource> platform_event_source_;
  std::unique_ptr<CursorFactory> cursor_factory_;
  std::unique_ptr<InputController> input_controller_;
  std::unique_ptr<GpuPlatformSupportHost> gpu_platform_support_host_;
  std::unique_ptr<OverlayManagerOzone> overlay_manager_;
  std::unique_ptr<OhosWindowDragManager> window_drag_manager_;
};

}  // namespace

OzonePlatform* CreateOzonePlatformOhos() {
  return new OzonePlatformOhos();
}

}  // namespace ui
