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

class MockLayerBoundsChangeCallback {
 public:
  MOCK_METHOD(void, OnLayerBoundsChanged, (const gfx::Rect&));
};

}  // namespace

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

  void ResetLayerRemovedVisibilityCallback() {
    if (!layer_->layer_removed_visibility_callback_.is_null()) {
      layer_->layer_removed_visibility_callback_.Reset();
    }
  }

  SurfaceLayer::RectChangeCallback GetLayerRectUpdateCallback() {
    return layer_->rect_change_callback_;
  }

  SurfaceLayer::RectVisibilityChangeCallback GetLayerRectVisibilityChangeCallback() {
    return layer_->rect_visibility_change_callback_;
  }

  SurfaceLayer::LayerRemovedVisibilityCallback GetLayerRemovedVisibilityCallback() {
    return layer_->layer_removed_visibility_callback_;
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

TEST_F(SurfaceLayerForIncludeTest, OnLayerRectVisibilityChange_IsNull) {
  layer_->ResetLayerRectVisibilityChangeCallback();
  layer_->OnLayerRectVisibilityChange(true);
  ASSERT_TRUE(GetLayerRectVisibilityChangeCallback().is_null());
}

TEST_F(SurfaceLayerForIncludeTest, ResetLayerRectUpdateCallback) {
  layer_->ResetLayerRectUpdateCallback();
}

TEST_F(SurfaceLayerForIncludeTest, ResetLayerRectUpdateCallback_IsNull) {
  layer_->ResetLayerRectUpdateCallback();
  layer_->ResetLayerRectUpdateCallback();
  ASSERT_TRUE(GetLayerRectUpdateCallback().is_null());
}

TEST_F(SurfaceLayerForIncludeTest, ResetLayerRectVisibilityChangeCallback) {
  layer_->ResetLayerRectVisibilityChangeCallback();
}

TEST_F(SurfaceLayerForIncludeTest, ResetLayerRectVisibilityChangeCallback_IsNull) {
  layer_->ResetLayerRectVisibilityChangeCallback();
  layer_->ResetLayerRectVisibilityChangeCallback();
  ASSERT_TRUE(GetLayerRectVisibilityChangeCallback().is_null());
}

TEST_F(SurfaceLayerForIncludeTest, CleanupVisibilityForRemovedLayer) {
  EXPECT_CALL(callbacks_, OnLayerRemoved(false));
  layer_->CleanupVisibilityForRemovedLayer(false);
}

TEST_F(SurfaceLayerForIncludeTest, CleanupVisibilityForRemovedLayer_IsNull) {
  ResetLayerRemovedVisibilityCallback();
  layer_->CleanupVisibilityForRemovedLayer(false);
  ASSERT_TRUE(GetLayerRemovedVisibilityCallback().is_null());
}

TEST_F(SurfaceLayerForIncludeTest, OnLayerBoundsUpdate) {
  gfx::Rect test_rect(10, 20, 30, 40);
  layer_->OnLayerBoundsUpdate(test_rect);
}

TEST_F(SurfaceLayerForIncludeTest, OnLayerBoundsUpdate_WithMockCallback) {
  gfx::Rect new_bounds(10, 20, 30, 40);
  MockLayerBoundsChangeCallback mock_callback;
  EXPECT_CALL(mock_callback, OnLayerBoundsChanged(new_bounds));

  layer_->SetLayerBoundsChangeCallback(
      base::BindRepeating(&MockLayerBoundsChangeCallback::OnLayerBoundsChanged,
                          base::Unretained(&mock_callback)));

  layer_->OnLayerBoundsUpdate(new_bounds);
}

}  // namespace cc