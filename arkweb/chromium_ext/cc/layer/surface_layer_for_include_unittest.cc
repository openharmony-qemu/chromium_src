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

#include "cc/layers/surface_layer.h"
#include "cc/layers/surface_layer_impl.h"
#include "cc/trees/layer_tree_host.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace cc {
namespace {

class MockCallbacks {
 public:
  MOCK_METHOD(void, OnVisibilityChange, (bool), ());
  MOCK_METHOD(void, OnLayerRemoved, (bool), ());
  MOCK_METHOD(void, OnRectChange, (const gfx::Rect&), ());
};

class SurfaceLayerForIncludeTest : public testing::Test {
 protected:
  void SetUp() override {
    layer_ = SurfaceLayer::Create(
        UpdateSubmissionStateCB(),
        base::BindRepeating(&MockCallbacks::OnRectChange, 
                           base::Unretained(&callbacks_)),
        base::BindRepeating(&MockCallbacks::OnVisibilityChange,
                           base::Unretained(&callbacks_)),
        base::BindRepeating(&MockCallbacks::OnLayerRemoved,
                           base::Unretained(&callbacks_)));
  }

  scoped_refptr<SurfaceLayer> layer_;
  testing::StrictMock<MockCallbacks> callbacks_;
};

TEST_F(SurfaceLayerForIncludeTest, OnLayerRectUpdate001) {
  MockCallbacks video_callbacks;
  layer_->SetVideoRectChangeCallback(
      base::BindRepeating(&MockCallbacks::OnRectChange, 
                         base::Unretained(&video_callbacks)));
  
  gfx::Rect test_rect(10, 20, 30, 40);
  layer_->OnLayerRectUpdate(test_rect);
}

TEST_F(SurfaceLayerForIncludeTest, OnLayerRectUpdate002) {
  gfx::Rect test_rect(5, 5, 100, 100);
  EXPECT_CALL(callbacks_, OnRectChange(test_rect));
  layer_->OnLayerRectUpdate(test_rect);
}

TEST_F(SurfaceLayerForIncludeTest, OnLayerRectUpdate003) {
  layer_->ResetLayerRectUpdateCallback();
  layer_->OnLayerRectUpdate(gfx::Rect());
}

TEST_F(SurfaceLayerForIncludeTest, OnLayerRectVisibilityChange) {
  EXPECT_CALL(callbacks_, OnVisibilityChange(true));
  layer_->OnLayerRectVisibilityChange(true);
}

TEST_F(SurfaceLayerForIncludeTest, ResetLayerRectUpdateCallback) {
  layer_->ResetLayerRectUpdateCallback();
}

TEST_F(SurfaceLayerForIncludeTest, ResetLayerRectVisibilityChangeCallback) {
  layer_->ResetLayerRectVisibilityChangeCallback();
}

TEST_F(SurfaceLayerForIncludeTest, CleanupVisibilityForRemovedLayer) {
  EXPECT_CALL(callbacks_, OnLayerRemoved(false));
  layer_->CleanupVisibilityForRemovedLayer(false);
}

}  // namespace
}  // namespace cc