/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "base/test/task_environment.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "ohos/adapter/test/mock_app_window_adapter.h"
#include "ohos_event_source.h"
#include "ohos_toplevel_window.h"
#include "ui/ozone/platform/ohos/common/ohos_util.h"

namespace ui {
using ::testing::_;
using ::testing::DoAll;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SetArgPointee;

using ohos::adapter::window::WindowRect;
using ohos::adapter::xcomponent::RectChangeReason;
using ohos::adapter::xcomponent::WindowEvent;
using NativeEventType = ohos::adapter::xcomponent::EventType;

class MockPlatformWindowDelegate : public PlatformWindowDelegate {
 public:
  MockPlatformWindowDelegate() = default;
  MockPlatformWindowDelegate(const MockPlatformWindowDelegate&) = delete;
  MockPlatformWindowDelegate& operator=(const MockPlatformWindowDelegate&) =
      delete;

  ~MockPlatformWindowDelegate() override = default;

  MOCK_METHOD1(OnBoundsChanged, void(const BoundsChange&));
  MOCK_METHOD1(OnDamageRect, void(const gfx::Rect&));
  MOCK_METHOD1(DispatchEvent, void(Event* event));
  MOCK_METHOD0(OnCloseRequest, void());
  MOCK_METHOD0(OnClosed, void());
  MOCK_METHOD2(OnWindowStateChanged,
               void(PlatformWindowState, PlatformWindowState));
  MOCK_METHOD0(OnFullscreenStateChanged, void());
  MOCK_METHOD0(OnLostCapture, void());
  MOCK_METHOD1(OnAcceleratedWidgetAvailable, void(gfx::AcceleratedWidget));
  MOCK_METHOD0(OnWillDestroyAcceleratedWidget, void());
  MOCK_METHOD0(OnAcceleratedWidgetDestroyed, void());
  MOCK_METHOD1(OnActivationChanged, void(bool));
  MOCK_METHOD0(OnMouseEnter, void());
  MOCK_METHOD1(SetSurfaceId, void(uint64_t));
  MOCK_CONST_METHOD0(GetMaximumSizeForWindow, std::optional<gfx::Size>());
  MOCK_CONST_METHOD0(GetMinimumSizeForWindow, std::optional<gfx::Size>());
  MOCK_METHOD1(OnOcclusionStateChanged, void(PlatformWindowOcclusionState));
};

class OhosWindowTest : public ::testing::Test {
 public:
  OhosWindowTest()
      : task_env_(std::make_unique<base::test::TaskEnvironment>(
            base::test::TaskEnvironment::MainThreadType::UI)) {}
  OhosWindowTest(const OhosWindowTest&) = delete;
  OhosWindowTest& operator=(const OhosWindowTest&) = delete;
  ~OhosWindowTest() override = default;

 protected:
  void SetUp() override {
    MockAppWindowAdapter::SetInstance(&app_window_adapter_);
    window_manager_ = std::make_unique<OhosWindowManager>();
    window_drag_manager_ = std::make_unique<OhosWindowDragManager>();
    event_source_ = std::make_unique<OhosEventSource>(
        window_manager_.get(), window_drag_manager_.get());
  }

  void TearDown() override { MockAppWindowAdapter::ClearInstance(); }

  std::unique_ptr<OhosToplevelWindow> CreateToplevelWindow(
      PlatformWindowDelegate* delegate,
      const gfx::Rect& bounds) {
    PlatformWindowInitProperties init_params(bounds);
    auto window =
        std::make_unique<OhosToplevelWindow>(delegate, window_manager_.get());
    window->Initialize(std::move(init_params));
    return window;
  }

  NiceMock<MockAppWindowAdapter> app_window_adapter_;
  NiceMock<MockPlatformWindowDelegate> delegate_;
  std::unique_ptr<base::test::TaskEnvironment> task_env_;
  std::unique_ptr<OhosWindowManager> window_manager_;
  std::unique_ptr<OhosWindowDragManager> window_drag_manager_;
  std::unique_ptr<OhosEventSourceBase> event_source_;
};

TEST_F(OhosWindowTest, ToplevelWindowCreate) {
  gfx::Rect bounds = gfx::Rect(100, 200, 1920, 1080);
  auto window1 = CreateToplevelWindow(&delegate_, bounds);
  EXPECT_EQ(window1->GetBoundsInPixels(), bounds);
  EXPECT_EQ(window1->GetPlatformWindowState(), PlatformWindowState::kNormal);

  auto window2 = CreateToplevelWindow(&delegate_, bounds);
  // Different windows should have different unique ids.
  EXPECT_NE(window1->GetWidget(), window2->GetWidget());
  EXPECT_EQ(util::ConvertWidgetIdToWindowId(window1->GetWidget()),
            window1->GetWindowUniqueId());
  EXPECT_EQ(util::ConvertWidgetIdToWindowId(window2->GetWidget()),
            window2->GetWindowUniqueId());
}

TEST_F(OhosWindowTest, ToplevelWindowBounds) {
  gfx::Rect bounds = gfx::Rect(100, 200, 1920, 1080);
  auto window = CreateToplevelWindow(&delegate_, bounds);
  EXPECT_EQ(window->GetBoundsInPixels(), bounds);

  // 1) Test setting bounds by PlatformWindow
  gfx::Rect new_bounds = gfx::Rect(150, 250, 860, 640);
  EXPECT_CALL(app_window_adapter_,
              SetBounds(window->GetWidget(), _)).Times(1);
  window->SetBoundsInPixels(new_bounds);

  // 2) Test bounds changed by ArkUI
  gfx::Rect size_change = gfx::Rect(bounds.origin(), gfx::Size(3840, 2160));
  auto event = std::make_shared<WindowRectChangeEvent>();
  event->top = size_change.y();
  event->left = size_change.x();
  event->width = size_change.width();
  event->height = size_change.height();
  event->reason = RectChangeReason::DRAG;
  
  EXPECT_CALL(delegate_, OnBoundsChanged(_)).Times(1);
  window->HandleEvent(event);
  EXPECT_EQ(window->GetBoundsInPixels(), size_change);

  gfx::Rect origin_change = gfx::Rect(gfx::Point(0, 0), size_change.size());
  event = std::make_shared<WindowRectChangeEvent>();
  event->top = origin_change.y();
  event->left = origin_change.x();
  event->width = origin_change.width();
  event->height = origin_change.height();
  event->reason = RectChangeReason::DRAG;

  EXPECT_CALL(delegate_, OnBoundsChanged(_)).Times(1);
  window->HandleEvent(event);
  EXPECT_EQ(window->GetBoundsInPixels(), origin_change);
}

TEST_F(OhosWindowTest, ToplevelWindowFullscreen) {
  gfx::Rect bounds = gfx::Rect(100, 200, 1920, 1080);
  auto window = CreateToplevelWindow(&delegate_, bounds);
  int64_t display_id = 1;

  EXPECT_CALL(app_window_adapter_, SetFullscreen(window->GetWidget())).Times(1);
  window->SetFullscreen(true, display_id);
}

TEST_F(OhosWindowTest, ToplevelWindowState) {
  gfx::Rect bounds = gfx::Rect(100, 200, 1920, 1080);
  auto window = CreateToplevelWindow(&delegate_, bounds);

  gfx::Rect maximize_bounds = gfx::Rect(0, 0, 3840, 2160);
  auto maximize_event = std::make_shared<WindowRectChangeEvent>();
  maximize_event->top = maximize_bounds.y();
  maximize_event->left = maximize_bounds.x();
  maximize_event->width = maximize_bounds.width();
  maximize_event->height = maximize_bounds.height();
  maximize_event->reason = RectChangeReason::MAXIMIZE;
  EXPECT_CALL(delegate_, OnWindowStateChanged(_, _)).Times(1);
  window->HandleEvent(maximize_event);
  EXPECT_EQ(window->GetBoundsInPixels(), maximize_bounds);
  EXPECT_EQ(window->GetPlatformWindowState(), PlatformWindowState::kMaximized);
  testing::Mock::VerifyAndClearExpectations(&delegate_);

  auto restore_event = std::make_shared<WindowRectChangeEvent>();
  restore_event->top = bounds.y();
  restore_event->left = bounds.x();
  restore_event->width = bounds.width();
  restore_event->height = bounds.height();
  restore_event->reason = RectChangeReason::RECOVER;
  EXPECT_CALL(delegate_, OnWindowStateChanged(_, _)).Times(1);
  window->HandleEvent(restore_event);
  EXPECT_EQ(window->GetBoundsInPixels(), bounds);
  EXPECT_EQ(window->GetPlatformWindowState(), PlatformWindowState::kNormal);
  testing::Mock::VerifyAndClearExpectations(&delegate_);

  auto minimize_event =
      std::make_shared<WindowEvent>(WindowEventType::WINDOW_HIDDEN);
  EXPECT_CALL(delegate_, OnWindowStateChanged(_, _)).Times(1);
  window->HandleEvent(minimize_event);
  EXPECT_EQ(window->GetPlatformWindowState(), PlatformWindowState::kMinimized);
  testing::Mock::VerifyAndClearExpectations(&delegate_);

  auto show_event =
      std::make_shared<WindowEvent>(WindowEventType::WINDOW_SHOWN);
  EXPECT_CALL(delegate_, OnWindowStateChanged(_, _)).Times(1);
  window->HandleEvent(show_event);
  EXPECT_EQ(window->GetPlatformWindowState(), PlatformWindowState::kNormal);
  testing::Mock::VerifyAndClearExpectations(&delegate_);

  auto occluded_event =
      std::make_shared<WindowEvent>(WindowEventType::WINDOW_OCCLUDED);
  EXPECT_CALL(delegate_, OnOcclusionStateChanged(
                             ui::PlatformWindowOcclusionState::kOccluded))
      .Times(1);
  window->HandleEvent(occluded_event);
  testing::Mock::VerifyAndClearExpectations(&delegate_);

  auto visiable_event =
      std::make_shared<WindowEvent>(WindowEventType::WINDOW_VISIBLE);
  EXPECT_CALL(delegate_, OnOcclusionStateChanged(
                             ui::PlatformWindowOcclusionState::kVisible))
      .Times(1);
  window->HandleEvent(visiable_event);
}

TEST_F(OhosWindowTest, ToplevelWindowFocus) {
  gfx::Rect bounds = gfx::Rect(100, 200, 1920, 1080);
  auto window = CreateToplevelWindow(&delegate_, bounds);

  auto blur_event = std::make_shared<SurfaceEvent>(NativeEventType::ET_SURFACE_BLUR);
  EXPECT_CALL(delegate_, OnActivationChanged(false)).Times(1);
  window->HandleEvent(blur_event);
  testing::Mock::VerifyAndClearExpectations(&delegate_);

  auto focus_event =
      std::make_shared<SurfaceEvent>(NativeEventType::ET_SURFACE_FOCUS);
  EXPECT_CALL(delegate_, OnActivationChanged(true)).Times(1);
  window->HandleEvent(focus_event);
}

TEST_F(OhosWindowTest, ToplevelWindowCaption) {
  gfx::Rect bounds = gfx::Rect(100, 200, 1920, 1080);
  auto window = CreateToplevelWindow(&delegate_, bounds);

  auto caption_event = std::make_shared<WindowCaptionButtonRectChangeEvent>();
  caption_event->top = 0;
  caption_event->right = 50;
  caption_event->width = 200;
  caption_event->height = 80;
  window->HandleEvent(caption_event);

  gfx::PointF not_hit = gfx::PointF(0, 75);
  EXPECT_FALSE(window->IsHitCaptionButton(not_hit));

  gfx::PointF hit = gfx::PointF(1850, 50);
  EXPECT_TRUE(window->IsHitCaptionButton(hit));
}
}  // namespace ui
