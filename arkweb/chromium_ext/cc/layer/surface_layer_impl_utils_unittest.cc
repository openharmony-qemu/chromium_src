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

#include "arkweb/build/features/features.h"
#include "ui/gfx/geometry/rect.h"
#define private public
#include "arkweb/chromium_ext/cc/layer/surface_layer_impl_utils.h"
#undef private
#include "cc/test/layer_tree_impl_test_base.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "cc/layers/surface_layer_impl.h"
#undef private

using testing::_;

namespace cc {
namespace {

static constexpr viz::FrameSinkId kArbitraryFrameSinkId(1, 1);

class SurfaceLayerImplUtilsTest : public testing::Test {
 protected:
  void SetUp() override {
    surface_layer_impl_ = impl_.AddLayerInActiveTree<SurfaceLayerImpl>();
    surface_layer_impl_->SetBounds(gfx::Size(100, 100));
    surface_layer_impl_->SetDrawsContent(true);
    viz::SurfaceId surface_id(kArbitraryFrameSinkId,
                              viz::LocalSurfaceId(9, base::UnguessableToken::Create()));
    surface_layer_impl_->SetRange(viz::SurfaceRange(std::nullopt, surface_id), std::nullopt);
    CopyProperties(impl_.root_layer(), surface_layer_impl_);
    impl_.CalcDrawProps(gfx::Size(1000, 1000));
    utils_ = std::make_unique<SurfaceLayerImplUtils>(surface_layer_impl_);
  }

  LayerTreeImplTestBase impl_;
  SurfaceLayerImpl* surface_layer_impl_;
  std::unique_ptr<SurfaceLayerImplUtils> utils_;

  class MockLayerTreeImplUtils {
   public:
    MOCK_METHOD(void, OnLayerRectVisibilityChange, (int, bool));
    MOCK_METHOD(void, OnLayerRectUpdate, (int, const gfx::Rect&));
  };
  MockLayerTreeImplUtils mock_tree_utils_;
};

TEST_F(SurfaceLayerImplUtilsTest, VisbilityChange001) {
  surface_layer_impl_->set_may_contain_video(true);
  EXPECT_CALL(mock_tree_utils_, OnLayerRectVisibilityChange(_, _)).Times(0);
  utils_->VisbilityChange();
}

TEST_F(SurfaceLayerImplUtilsTest, VisbilityChange002) {
  surface_layer_impl_->layer_impl_utils()->set_may_contain_native(false);
  EXPECT_CALL(mock_tree_utils_, OnLayerRectVisibilityChange(_, _)).Times(0);
  utils_->VisbilityChange();
}

TEST_F(SurfaceLayerImplUtilsTest, VisbilityChange003) {
  surface_layer_impl_->set_may_contain_video(false);
  surface_layer_impl_->layer_impl_utils()->set_may_contain_native(true);
  surface_layer_impl_->draw_properties().visible_layer_rect = gfx::Rect(0, 0);
  utils_->VisbilityChange();
}

TEST_F(SurfaceLayerImplUtilsTest, VisbilityChange004) {
  surface_layer_impl_->set_may_contain_video(false);
  surface_layer_impl_->layer_impl_utils()->set_may_contain_native(true);
  surface_layer_impl_->draw_properties().visible_layer_rect = gfx::Rect(0, 0);
  utils_->visibility_ = true;
  utils_->VisbilityChange();
  EXPECT_FALSE(utils_->visibility_);
}

TEST_F(SurfaceLayerImplUtilsTest, VisbilityChange005) {
  surface_layer_impl_->set_may_contain_video(false);
  surface_layer_impl_->layer_impl_utils()->set_may_contain_native(true);
  surface_layer_impl_->draw_properties().visible_layer_rect = gfx::Rect(50, 50);
  utils_->visibility_ = true;
  utils_->VisbilityChange();
  EXPECT_TRUE(utils_->visibility_);
}

TEST_F(SurfaceLayerImplUtilsTest, VisbilityChange006) {
  surface_layer_impl_->set_may_contain_video(false);
  surface_layer_impl_->layer_impl_utils()->set_may_contain_native(true);
  surface_layer_impl_->draw_properties().visible_layer_rect = gfx::Rect(50, 50);
  utils_->visibility_ = false;
  utils_->VisbilityChange();
  EXPECT_TRUE(utils_->visibility_);
}

TEST_F(SurfaceLayerImplUtilsTest, LayerRectUpdate001) {
  surface_layer_impl_->set_may_contain_video(true);
  EXPECT_CALL(mock_tree_utils_, OnLayerRectUpdate(_, _)).Times(0);
  utils_->LayerRectUpdate();
}

TEST_F(SurfaceLayerImplUtilsTest, LayerRectUpdate002) {
  surface_layer_impl_->layer_impl_utils()->set_may_contain_native(false);
  EXPECT_CALL(mock_tree_utils_, OnLayerRectUpdate(_, _)).Times(0);
  utils_->LayerRectUpdate();
}

TEST_F(SurfaceLayerImplUtilsTest, LayerRectUpdate003) {
  surface_layer_impl_->set_may_contain_video(false);
  surface_layer_impl_->layer_impl_utils()->set_may_contain_native(true);
  gfx::Rect new_bounds(50, 50, 200, 200);
  utils_->LayerRectUpdate();
}

}  // namespace
}  // namespace cc