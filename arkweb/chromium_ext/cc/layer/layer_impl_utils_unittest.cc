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

#include "arkweb/chromium_ext/cc/layer/layer_utils.h"
#include "cc/layers/layer_impl.h"
#include "cc/test/layer_tree_impl_test_base.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

class LayerImplUtilsTest : public LayerTreeImplTestBase, public testing::Test {
 public:
  using LayerTreeImplTestBase::LayerTreeImplTestBase;
  void SetUp() override {
    host_impl()->CreatePendingTree();
    layer_impl_ = EnsureRootLayerInPendingTree();
    utils_ = std::make_unique<LayerImplUtils>(layer_impl_);
  }
  LayerImpl* layer_impl_;
  std::unique_ptr<LayerImplUtils> utils_;
};

TEST_F(LayerImplUtilsTest, SetNativeRect001) {
  gfx::RectF original_rect(10, 20, 30, 40);
  gfx::RectF new_rect(50, 60, 70, 80);
  utils_->SetNativeRect(original_rect);
  utils_->SetNativeRect(new_rect);
  EXPECT_EQ(utils_->NativeRect(), new_rect);
}

TEST_F(LayerImplUtilsTest, SetNativeRect002) {
  gfx::RectF test_rect(10, 20, 30, 40);
  utils_->SetNativeRect(test_rect);
  utils_->SetNativeRect(test_rect);
  EXPECT_EQ(utils_->NativeRect(), test_rect);
}

TEST_F(LayerImplUtilsTest, SetInitScale001) {
  const float kOriginalScale = 1.0f;
  const float kNewScale = 2.0f;
  utils_->SetInitScale(kOriginalScale);
  utils_->SetInitScale(kNewScale);
  EXPECT_EQ(utils_->GetInitScale(), kNewScale);
}

TEST_F(LayerImplUtilsTest, SetInitScale002) {
  const float kTestScale = 1.5f;
  utils_->SetInitScale(kTestScale);
  utils_->SetInitScale(kTestScale);
  EXPECT_EQ(utils_->GetInitScale(), kTestScale);
}

TEST_F(LayerImplUtilsTest, NativeRect001) {
  gfx::RectF test_rect(5, 10, 15, 20);
  utils_->SetNativeRect(test_rect);
  EXPECT_EQ(utils_->NativeRect(), test_rect);
}

TEST_F(LayerImplUtilsTest, GetNativeRect001) {
  gfx::RectF test_rect(5, 10, 15, 20);
  utils_->SetNativeRect(test_rect);
  EXPECT_EQ(utils_->GetNativeRect(), test_rect);
}

}  // namespace
}  // namespace cc