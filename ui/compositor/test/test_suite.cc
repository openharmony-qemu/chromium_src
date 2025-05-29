// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/compositor/test/test_suite.h"

#include "base/command_line.h"
#include "build/build_config.h"
#include "ui/compositor/compositor.h"
#include "ui/compositor/compositor_switches.h"
#include "ui/compositor/layer.h"
#include "ui/gl/test/gl_surface_test_support.h"

#if BUILDFLAG(IS_OZONE)
#include "ui/ozone/public/ozone_platform.h"
#endif

#if BUILDFLAG(IS_WIN)
#include "ui/display/win/dpi.h"
#endif

#if BUILDFLAG(ARKWEB_UNITTESTS)
#include "base/command_line.h"
#include "content/browser/scheduler/browser_io_thread_delegate.h"
#include "content/browser/scheduler/browser_task_executor.h"
#include "content/browser/scheduler/browser_task_priority.h"
#include "content/browser/scheduler/browser_ui_thread_scheduler.h"
#include "components/viz/test/test_gpu_service_holder.h"
#include "mojo/core/embedder/embedder.h"
#include "ui/base/ui_base_features.h"
#include "ui/gl/init/gl_factory.h"
#endif

#if BUILDFLAG(IS_APPLE)
// gn check complains on other platforms, because //gpu/ipc/service:service
// is added to dependencies only for mac.
#include "gpu/ipc/service/image_transport_surface.h"  // nogncheck
#endif

#if BUILDFLAG(IS_FUCHSIA)
#include "ui/platform_window/fuchsia/initialize_presenter_api_view.h"
#endif

namespace ui {
namespace test {

CompositorTestSuite::CompositorTestSuite(int argc, char** argv)
    : TestSuite(argc, argv) {}

CompositorTestSuite::~CompositorTestSuite() {}

void CompositorTestSuite::Initialize() {
  base::TestSuite::Initialize();
#if BUILDFLAG(ARKWEB_UNITTESTS)
  gl::init::InitializeGLNoExtensionsOneOff(
    /*init_bindings=*/true, /*gpu_preference=*/gl::GpuPreference::kDefault);
#else
  gl::GLSurfaceTestSupport::InitializeOneOff();
#endif

#if BUILDFLAG(IS_OZONE)
  OzonePlatform::InitParams params;
  params.single_process = true;
#if BUILDFLAG(ARKWEB_UNITTESTS)
  auto ui_sequence_manager_ =
      base::sequence_manager::CreateUnboundSequenceManager(
          base::sequence_manager::SequenceManager::Settings::Builder()
          .SetPrioritySettings(content::internal::CreateBrowserTaskPrioritySettings())
          .Build());
  auto browser_ui_thread_scheduler =
      content::BrowserUIThreadScheduler::CreateForTesting(ui_sequence_manager_.get());
  content::BrowserTaskExecutor::CreateForTesting(
      std::move(browser_ui_thread_scheduler),
      std::make_unique<content::BrowserIOThreadDelegate>());
#endif
  OzonePlatform::InitializeForUI(params);
#endif

#if BUILDFLAG(IS_WIN)
  display::win::SetDefaultDeviceScaleFactor(1.0f);
#endif

#if BUILDFLAG(IS_FUCHSIA)
  ui::fuchsia::IgnorePresentCallsForTest();
#endif  // BUILDFLAG(IS_FUCHSIA)
}

}  // namespace test
}  // namespace ui
