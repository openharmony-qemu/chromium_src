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

#include "base/logging.h"
#include "base/test/task_environment.h"
#include "cc/layers/surface_layer.h"
#include "components/viz/common/surfaces/frame_sink_id.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/platform/graphics/surface_layer_bridge.h"

namespace blink {
class MockWebSurfaceLayerBridgeObserver : public WebSurfaceLayerBridgeObserver {
public:
  MOCK_METHOD(void, OnWebLayerUpdated, (), (override));
  MOCK_METHOD(void, RegisterContentsLayer, (cc::Layer*), (override));
  MOCK_METHOD(void, UnregisterContentsLayer, (cc::Layer*), (override));
};

class SurfaceLayerBridgeForIncludeTest : public testing::Test {
 public:
  void SetUp() override {
    task_environment_ = std::make_unique<base::test::TaskEnvironment>();
    viz::FrameSinkId parent_frame_sink_id(1, 1);
    update_submission_state_callback_ = base::BindRepeating([](bool is_visible, base::WaitableEvent* event) {});
    surface_layer_bridge_ = std::make_unique<SurfaceLayerBridge>(
        parent_frame_sink_id,
        WebSurfaceLayerBridge::ContainsVideo::kNo,
        nullptr,
        update_submission_state_callback_
    );
  }

  void TearDown() override {
    surface_layer_bridge_.reset();
    task_environment_.reset();
  }

  std::unique_ptr<base::test::TaskEnvironment> task_environment_;
  std::unique_ptr<SurfaceLayerBridge> surface_layer_bridge_;
  cc::UpdateSubmissionStateCB update_submission_state_callback_;
};

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
TEST_F(SurfaceLayerBridgeForIncludeTest, SetVideoRectChangeCallbackWithNullSurfaceLayer) {
  bool callback_called = false;
  cc::SurfaceLayer::RectChangeCallback callback = base::BindRepeating(
      [](bool* called, const gfx::Rect&) { *called = true; },
      &callback_called);
  surface_layer_bridge_->SetVideoRectChangeCallback(std::move(callback));
  EXPECT_FALSE(callback_called);
}

TEST_F(SurfaceLayerBridgeForIncludeTest, SetVideoRectChangeCallbackWithSurfaceLayer) {
  surface_layer_bridge_->CreateSurfaceLayer();
  bool callback_called = false;
  cc::SurfaceLayer::RectChangeCallback callback = base::BindRepeating(
      [](bool* called, const gfx::Rect&) { *called = true; },
      &callback_called);
  surface_layer_bridge_->SetVideoRectChangeCallback(std::move(callback));
  EXPECT_FALSE(callback_called);
}
#endif  // BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
TEST_F(SurfaceLayerBridgeForIncludeTest, SetLayerBoundsChangeCallbackWithNullSurfaceLayer) {
  bool callback_called = false;
  cc::SurfaceLayer::LayerBoundsChangeCallback callback = base::BindRepeating(
      [](bool* called, const gfx::Rect&) { *called = true; },
      &callback_called);
  surface_layer_bridge_->SetLayerBoundsChangeCallback(std::move(callback));
  EXPECT_FALSE(callback_called);
}

TEST_F(SurfaceLayerBridgeForIncludeTest, SetLayerBoundsChangeCallbackWithSurfaceLayer) {
  surface_layer_bridge_->CreateSurfaceLayer();
  bool callback_called = false;
  cc::SurfaceLayer::LayerBoundsChangeCallback callback = base::BindRepeating(
      [](bool* called, const gfx::Rect&) { *called = true; },
      &callback_called);
  surface_layer_bridge_->SetLayerBoundsChangeCallback(std::move(callback));
  EXPECT_FALSE(callback_called);
}
#endif  // BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
TEST_F(SurfaceLayerBridgeForIncludeTest, CreateSurfaceLayerWithObserver) {
  MockWebSurfaceLayerBridgeObserver mock_observer;
  viz::FrameSinkId parent_frame_sink_id(1, 1);
  surface_layer_bridge_ = std::make_unique<SurfaceLayerBridge>(
      parent_frame_sink_id,
      WebSurfaceLayerBridge::ContainsVideo::kNo,
      &mock_observer,
      update_submission_state_callback_
  );
  EXPECT_CALL(mock_observer, RegisterContentsLayer(testing::NotNull()))
      .Times(1);
  bool callback_called = false;
  cc::SurfaceLayer::RectChangeCallback rect_callback = base::BindRepeating(
      [](bool* called, const gfx::Rect&) { *called = true; },
      &callback_called);
  cc::SurfaceLayer::RectVisibilityChangeCallback visibility_callback = base::BindRepeating(
      [](bool visible) {});
  cc::SurfaceLayer::LayerRemovedVisibilityCallback removed_callback = base::BindRepeating(
      [](bool visible) {});
  scoped_refptr<cc::SurfaceLayer> result = surface_layer_bridge_->CreateSurfaceLayer(
      std::move(rect_callback), 
      std::move(visibility_callback), 
      std::move(removed_callback)
  );
  EXPECT_TRUE(result != nullptr);
  EXPECT_FALSE(callback_called);
}
#endif  // BUILDFLAG(ARKWEB_SAME_LAYER)
}  // namespace blink