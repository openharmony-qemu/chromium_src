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

#define protected public
#include "ui/compositor/compositor.h"

#include <gtest/gtest.h>

#include <memory>

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "base/test/task_environment.h"
#include "base/threading/thread.h"
#include "content/browser/browser_thread_impl.h"
#include "ui/compositor/test/test_context_factories.h"
#include "arkweb/chromium_ext/ui/compositor/compositor_utils.h"
namespace ui {
namespace {
std::shared_ptr<Compositor> g_compositor = nullptr;
}

class CompositorTest : public testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();
  scoped_refptr<base::SingleThreadTaskRunner> CreateTaskRunner();

 private:
  std::unique_ptr<TestContextFactories> context_factories_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
};

void CompositorTest::SetUpTestCase(void) {}

void CompositorTest::TearDownTestCase(void) {}

scoped_refptr<base::SingleThreadTaskRunner> CompositorTest::CreateTaskRunner() {
  base::test::TaskEnvironment task_environment;
  task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  return task_runner_;
}

void CompositorTest::SetUp(void) {
  context_factories_ = std::make_unique<TestContextFactories>(false);
  bool enable_pixel_canvas = false;
  bool use_external_begin_frame_control = false;
  bool force_software_compositor = false;
  bool enable_compositing_based_throttling = false;
  size_t memory_limit_when_visible_mb = 0;
  g_compositor = std::make_shared<Compositor>(
      context_factories_->GetContextFactory()->AllocateFrameSinkId(),
      context_factories_->GetContextFactory(), CreateTaskRunner(),
      enable_pixel_canvas, use_external_begin_frame_control,
      force_software_compositor, enable_compositing_based_throttling,
      memory_limit_when_visible_mb);
}

void CompositorTest::TearDown(void) {
  g_compositor = nullptr;
  context_factories_.reset();
}

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
TEST_F(CompositorTest, SetDrawRect) {
  ASSERT_NE(g_compositor, nullptr);
  gfx::Rect rect(10, 20, 30, 40);
  g_compositor->Utils()->SetDrawRect(rect);
}
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

TEST_F(CompositorTest, SetDrawMode) {
  ASSERT_NE(g_compositor, nullptr);
  g_compositor->Utils()->SetDrawMode(0);
  g_compositor->Utils()->SetDrawMode(1);
}

TEST_F(CompositorTest, SetNativeInnerWeb) {
  ASSERT_NE(g_compositor, nullptr);
  g_compositor->Utils()->SetNativeInnerWeb(true);
  g_compositor->Utils()->SetDrawMode(false);
}

TEST_F(CompositorTest, SetBypassVsyncCondition) {
  ASSERT_NE(g_compositor, nullptr);
  g_compositor->Utils()->SetBypassVsyncCondition(0);
  g_compositor->Utils()->SetBypassVsyncCondition(1);
}

TEST_F(CompositorTest, SetShouldFrameSubmissionBeforeDraw) {
  ASSERT_NE(g_compositor, nullptr);
  g_compositor->Utils()->SetShouldFrameSubmissionBeforeDraw(false);
  g_compositor->Utils()->SetShouldFrameSubmissionBeforeDraw(true);
}

TEST_F(CompositorTest, SendInternalBeginFrame) {
  ASSERT_NE(g_compositor, nullptr);
  g_compositor->Utils()->SendInternalBeginFrame();
}

TEST_F(CompositorTest, SetEnableLowerFrameRate) {
  ASSERT_NE(g_compositor, nullptr);
  g_compositor->Utils()->SetEnableLowerFrameRate(false);
  g_compositor->Utils()->SetEnableLowerFrameRate(true);
}

TEST_F(CompositorTest, SetEnableHalfFrameRate) {
  ASSERT_NE(g_compositor, nullptr);
  g_compositor->Utils()->SetEnableHalfFrameRate(false);
  g_compositor->Utils()->SetEnableHalfFrameRate(true);
}

TEST_F(CompositorTest, SetIsOfflineWebComponentInactive) {
  ASSERT_NE(g_compositor, nullptr);
  g_compositor->Utils()->SetIsOfflineWebComponentInactive(true);
  g_compositor->Utils()->SetIsOfflineWebComponentInactive(false);
}

TEST_F(CompositorTest, EvictFrameBackBuffers) {
  ASSERT_NE(g_compositor, nullptr);
  g_compositor->Utils()->EvictFrameBackBuffers();
}

TEST_F(CompositorTest, UpdateVSyncFrequency) {
  ASSERT_NE(g_compositor, nullptr);
  g_compositor->Utils()->UpdateVSyncFrequency();
}

TEST_F(CompositorTest, ResetVSyncFrequency) {
  ASSERT_NE(g_compositor, nullptr);
  g_compositor->Utils()->ResetVSyncFrequency();
}

TEST_F(CompositorTest, SetCurrentFrameSinkId) {
  ASSERT_NE(g_compositor, nullptr);
  const viz::FrameSinkId id(0, 0);
  g_compositor->Utils()->ResetVSyncFrequency();
}

TEST_F(CompositorTest, DisableSwapUntilMaximized) {
  ASSERT_NE(g_compositor, nullptr);
  g_compositor->Utils()->DisableSwapUntilMaximized();
}

TEST_F(CompositorTest, SetPipActive) {
  ASSERT_NE(g_compositor, nullptr);
  g_compositor->Utils()->SetPipActive(false);
  g_compositor->Utils()->SetPipActive(true);
}

}  // namespace ui
                  