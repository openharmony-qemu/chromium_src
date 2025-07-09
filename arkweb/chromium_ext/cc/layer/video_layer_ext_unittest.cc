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

#include "arkweb/chromium_ext/cc/layer/video_layer_ext.h"
#include "cc/test/layer_tree_impl_test_base.h"
#include "cc/test/fake_video_frame_provider.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using testing::_;

namespace cc {
namespace{

class MockCallbacks {
 public:
  MOCK_METHOD(void, OnVisibilityChange, (bool), ());
  MOCK_METHOD(void, OnLayerRemoved, (bool), ());
  MOCK_METHOD(void, OnRectChange, (const gfx::Rect&), ());
};

class VideoLayerExtTest : public testing::Test {
 protected:
  void SetUp() override {
    FakeVideoFrameProvider provider;
    layer_ = VideoLayerExt::Create(
        &provider, media::VIDEO_ROTATION_0,
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

TEST_F(VideoLayerExtTest, OnLayerRectUpdate) {
  gfx::Rect test_rect(5, 5, 100, 100);
  EXPECT_CALL(callbacks_, OnRectChange(test_rect));
  layer_->OnLayerRectUpdate(test_rect);
}

TEST_F(VideoLayerExtTest, OnLayerRectVisibilityChange) {
  EXPECT_CALL(callbacks_, OnVisibilityChange(true));
  layer_->OnLayerRectVisibilityChange(true);
}

TEST_F(VideoLayerExtTest, CleanupVisibilityForRemovedLayer) {
  EXPECT_CALL(callbacks_, OnLayerRemoved(false));
  layer_->CleanupVisibilityForRemovedLayer(false);
}

TEST_F(VideoLayerExtTest, ResetLayerRectCallback_ResetsAllCallbacks) {
  {
    EXPECT_CALL(callbacks_, OnRectChange(gfx::Rect(1, 1, 10, 10)));
    layer_->OnLayerRectUpdate(gfx::Rect(1, 1, 10, 10));
  }
  {
    EXPECT_CALL(callbacks_, OnVisibilityChange(false));
    layer_->OnLayerRectVisibilityChange(false);
  }
  {
    EXPECT_CALL(callbacks_, OnLayerRemoved(true));
    layer_->CleanupVisibilityForRemovedLayer(true);
  }

  layer_->ResetLayerRectCallback();

  EXPECT_CALL(callbacks_, OnRectChange(_)).Times(0);
  EXPECT_CALL(callbacks_, OnVisibilityChange(_)).Times(0);
  EXPECT_CALL(callbacks_, OnLayerRemoved(_)).Times(0);

  layer_->OnLayerRectUpdate(gfx::Rect(2, 2, 20, 20));
  layer_->OnLayerRectVisibilityChange(true);
  layer_->CleanupVisibilityForRemovedLayer(false);
}

}  // namespace
}  // namespace cc