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

#include "arkweb/chromium_ext/cc/layer/video_layer_impl_utils.h"
#include "cc/test/fake_video_frame_provider.h"
#include "cc/test/layer_tree_impl_test_base.h"
#include "media/base/video_frame.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "cc/layers/video_layer_impl.h"
#undef private

using testing::_;

namespace cc {
namespace{

class VideoLayerImplUtilsTest : public testing::Test {
 protected:
  void SetUp() override {
    scoped_refptr<media::VideoFrame> video_frame = media::VideoFrame::CreateFrame(
      media::PIXEL_FORMAT_I420, gfx::Size(10, 10), gfx::Rect(10, 10),
      gfx::Size(10, 10), base::TimeDelta());
    FakeVideoFrameProvider provider;
    provider.set_frame(video_frame);
    video_layer_impl_ = impl_.AddLayerInActiveTree<VideoLayerImpl>(
        &provider, media::VIDEO_ROTATION_0);
    video_layer_impl_->SetBounds(gfx::Size(100, 100));
    video_layer_impl_->SetDrawsContent(true);
    video_layer_impl_->set_visible_layer_rect(gfx::Rect(1000, 1000));
    CopyProperties(impl_.root_layer(), video_layer_impl_);
    impl_.CalcDrawProps(gfx::Size(1000, 1000));
    utils_ = std::make_unique<VideoLayerImplUtils>(video_layer_impl_);
  }

  LayerTreeImplTestBase impl_;
  VideoLayerImpl* video_layer_impl_;
  std::unique_ptr<VideoLayerImplUtils> utils_;

  class MockLayerTreeImplUtils {
   public:
    MOCK_METHOD(void, OnLayerRectVisibilityChange, (int, bool));
    MOCK_METHOD(void, OnLayerRectUpdate, (int, const gfx::Rect&));
  };
  MockLayerTreeImplUtils mock_tree_utils_;
};

TEST_F(VideoLayerImplUtilsTest, VisbilityChange001) {
  video_layer_impl_->set_may_contain_video(true);
  EXPECT_CALL(mock_tree_utils_, OnLayerRectVisibilityChange(_, _)).Times(0);
  utils_->VisibilityChange();
}

TEST_F(VideoLayerImplUtilsTest, VisbilityChange002) {
  video_layer_impl_->layer_impl_utils()->set_may_contain_native(false);
  EXPECT_CALL(mock_tree_utils_, OnLayerRectVisibilityChange(_, _)).Times(0);
  utils_->VisibilityChange();
}

TEST_F(VideoLayerImplUtilsTest, VisbilityChange003) {
  video_layer_impl_->set_may_contain_video(false);
  video_layer_impl_->layer_impl_utils()->set_may_contain_native(true);
  video_layer_impl_->draw_properties().visible_layer_rect = gfx::Rect(0, 0);
  utils_->VisibilityChange();
}

TEST_F(VideoLayerImplUtilsTest, LayerRectUpdate001) {
  video_layer_impl_->set_may_contain_video(true);
  EXPECT_CALL(mock_tree_utils_, OnLayerRectUpdate(_, _)).Times(0);
  utils_->LayerRectUpdate();
}

TEST_F(VideoLayerImplUtilsTest, LayerRectUpdate002) {
  video_layer_impl_->layer_impl_utils()->set_may_contain_native(false);
  EXPECT_CALL(mock_tree_utils_, OnLayerRectUpdate(_, _)).Times(0);
  utils_->LayerRectUpdate();
}

TEST_F(VideoLayerImplUtilsTest, LayerRectUpdate003) {
  video_layer_impl_->set_may_contain_video(false);
  video_layer_impl_->layer_impl_utils()->set_may_contain_native(true);
  gfx::Rect new_bounds(50, 50, 200, 200);
  utils_->LayerRectUpdate();
}

}  // namespace
}  // namespace cc