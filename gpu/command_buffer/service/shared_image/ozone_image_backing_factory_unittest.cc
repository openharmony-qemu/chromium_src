/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gpu/command_buffer/service/shared_image/ozone_image_backing_factory.h"

#include <dawn/dawn_proc_table.h>
#include <dawn/native/DawnNative.h>
#include <memory>
#include "build/build_config.h"
#include "build/chromecast_buildflags.h"
#include "build/chromeos_buildflags.h"
#include "gmock/gmock.h"
#include "gpu/command_buffer/service/shared_memory_region_wrapper.h"
#include "third_party/googletest/src/googletest/include/gtest/gtest.h"
#include "ui/base/ime/input_method.h"
#include "ui/display/types/native_display_delegate.h"
#include "ui/gfx/gpu_memory_buffer.h"
#include "ui/gl/buildflags.h"
#include "ui/ozone/public/ozone_platform.h"
#include "ui/ozone/public/platform_screen.h"
#include "ui/ozone/public/system_input_injector.h"
#define protected public
#include "ui/ozone/public/surface_factory_ozone.h"
#undef protected
#include "ui/platform_window/platform_window_delegate.h"
#include "ui/platform_window/platform_window_init_properties.h"

namespace gpu {
class MockOzonePlatform : public ui::OzonePlatform {
 public:
  MOCK_METHOD(ui::SurfaceFactoryOzone*, GetSurfaceFactoryOzone, (), (override));
  MOCK_METHOD(ui::OverlayManagerOzone*, GetOverlayManager, (), (override));
  MOCK_METHOD(ui::CursorFactory*, GetCursorFactory, (), (override));
  MOCK_METHOD(ui::InputController*, GetInputController, (), (override));
  MOCK_METHOD(ui::GpuPlatformSupportHost*,
              GetGpuPlatformSupportHost,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<ui::SystemInputInjector>,
              CreateSystemInputInjector,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<ui::PlatformWindow>,
              CreatePlatformWindow,
              (ui::PlatformWindowDelegate * delegate,
               ui::PlatformWindowInitProperties properties),
              (override));
  MOCK_METHOD(std::unique_ptr<display::NativeDisplayDelegate>,
              CreateNativeDisplayDelegate,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<ui::PlatformScreen>,
              CreateScreen,
              (),
              (override));
  MOCK_METHOD(void, InitScreen, (ui::PlatformScreen * screen), (override));
  MOCK_METHOD(std::unique_ptr<ui::InputMethod>,
              CreateInputMethod,
              (ui::ImeKeyEventDispatcher * ime_key_event_dispatcher,
               gfx::AcceleratedWidget widget),
              (override));
  MOCK_METHOD(bool, InitializeUI, (const InitParams& params), (override));
  MOCK_METHOD(void, InitializeGPU, (const InitParams& params), (override));
};

class OzoneImageBackingFactoryTest : public testing::Test {
 protected:
  void SetUp() override {
    ozone_ptr = std::make_unique<OzoneImageBackingFactory>(
        shared_context_state, workarounds, gpu_preferences);
  }
  void TearDown() override { ozone_ptr.reset(); }
  SharedContextState* shared_context_state;
  const GpuDriverBugWorkarounds workarounds;
  const GpuPreferences gpu_preferences;
  std::unique_ptr<OzoneImageBackingFactory> ozone_ptr;
  Mailbox mailbox;
  gfx::GpuMemoryBufferHandle handle;
  gfx::BufferFormat buffer_format = gfx::BufferFormat::BGR_565;
  gfx::BufferPlane plane = gfx::BufferPlane::DEFAULT;
  gfx::Size size = gfx::Size(10, 10);
  gfx::ColorSpace color_space;
  GrSurfaceOrigin surface_origin = GrSurfaceOrigin::kBottomLeft_GrSurfaceOrigin;
  SkAlphaType alpha_type = SkAlphaType::kLastEnum_SkAlphaType;
  uint32_t usage = 1;
  uint64_t window_buffer[8] = {0};
};

#if BUILDFLAG(ENABLE_HEIF_DECODER)
TEST_F(OzoneImageBackingFactoryTest, CreateSharedImage) {
  std::unique_ptr<MockOzonePlatform> mock_zonep_plat_ptr =
      std::make_unique<MockOzonePlatform>();
  std::unique_ptr<ui::SurfaceFactoryOzone> surface_ptr =
      std::make_unique<ui::SurfaceFactoryOzone>();
  EXPECT_CALL(*mock_zonep_plat_ptr, GetSurfaceFactoryOzone())
      .WillRepeatedly(testing::Return(surface_ptr.get()));
  std::unique_ptr<ui::OzonePlatform> mock_zone_ptr =
      std::move(mock_zonep_plat_ptr);
  auto result = ozone_ptr->CreateSharedImage(
      mailbox, std::move(handle), buffer_format, plane, size, color_space,
      surface_origin, alpha_type, usage, window_buffer);
  ASSERT_FALSE(result);
}
#endif

}  // namespace gpu
