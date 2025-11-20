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

#include "cc/test/layer_tree_impl_test_base.h"
#include "cc/trees/layer_tree_host.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "cc/layers/surface_layer.h"
#include "cc/layers/surface_layer_impl.h"
#undef private

namespace cc {
namespace {

TEST(SurfaceLayerImplTest, OnLayerBoundsUpdate) {
  LayerTreeImplTestBase impl;
  SurfaceLayerImpl* layer = impl.AddLayerInActiveTree<SurfaceLayerImpl>();

  layer->layer_bounds_ = gfx::Rect(10, 10, 100, 100);

  gfx::Rect visible_rect1(10, 10, 100, 100);
  layer->OnLayerBoundsUpdate(visible_rect1);

  gfx::Rect visible_rect2(20, 20, 200, 200);
  layer->OnLayerBoundsUpdate(visible_rect2);

  EXPECT_NE(layer->layer_bounds_, visible_rect2);
}

TEST(SurfaceLayerImplTest, OnLayerRectUpdate) {
  LayerTreeImplTestBase impl;
  SurfaceLayerImpl* layer = impl.AddLayerInActiveTree<SurfaceLayerImpl>();

  layer->visible_quad_rect_ = gfx::Rect(10, 10, 100, 100);

  layer->set_may_contain_video(false);
  layer->OnLayerRectUpdate(gfx::Rect(20, 20, 200, 200));

  layer->set_may_contain_video(true);
  layer->OnLayerRectUpdate(gfx::Rect(10, 10, 100, 100));

  gfx::Rect new_rect(30, 30, 300, 300);
  layer->OnLayerRectUpdate(new_rect);

  EXPECT_NE(layer->layer_bounds_, new_rect);
}

}  // namespace
}  // namespace cc