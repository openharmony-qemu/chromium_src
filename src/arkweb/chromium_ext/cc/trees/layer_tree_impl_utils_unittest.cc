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

#include "cc/layers/layer_impl.h"
#include "cc/test/layer_tree_impl_test_base.h"
#include "cc/test/fake_video_frame_provider.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "layer_tree_impl_utils.h"
#undef private

using testing::_;

namespace cc {
namespace{

class LayerTreeImplUtilsTest : public testing::Test {
 protected:
  void SetUp() override {
    LayerTreeImplTestBase impl;
    utils_ = std::make_unique<LayerTreeImplUtils>(impl.host_impl()->active_tree());
  }

  std::unique_ptr<LayerTreeImplUtils> utils_;
};

TEST_F(LayerTreeImplUtilsTest, RegisterClippedVisualViewportSelectionBounds001) {
  utils_->RegisterClippedVisualViewportSelectionBounds(gfx::Rect(10, 20, 30, 40));
  utils_->RegisterClippedVisualViewportSelectionBounds(gfx::Rect(10, 20, 30, 40));
  EXPECT_EQ(gfx::Rect(10, 20, 30, 40), 
            utils_->GetClippedVisualViewportSelectionBounds());
}

TEST_F(LayerTreeImplUtilsTest, RegisterClippedVisualViewportSelectionBounds002) {
  utils_->RegisterClippedVisualViewportSelectionBounds(gfx::Rect(5, 5, 5, 5));
  utils_->RegisterClippedVisualViewportSelectionBounds(gfx::Rect(10, 10, 20, 20));
  EXPECT_EQ(gfx::Rect(10, 10, 20, 20),
            utils_->GetClippedVisualViewportSelectionBounds());
}

TEST_F(LayerTreeImplUtilsTest, ViewportRectForTilePriority001) {
  gfx::Rect input_rect(100, 200, 300, 400);
  EXPECT_EQ(input_rect, utils_->ViewportRectForTilePriority(input_rect));
}

}  // namespace
}  // namespace cc