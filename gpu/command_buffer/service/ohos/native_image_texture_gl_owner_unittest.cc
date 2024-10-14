/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#define private public
#include "gpu/command_buffer/service/ohos/native_image_texture_gl_owner.h"
#undef private
#include "base/memory/scoped_refptr.h"

#include <memory>
#include "base/check_op.h"
#include "base/debug/alias.h"
#include "base/debug/dump_without_crashing.h"
#include "base/functional/bind.h"
#include "base/memory/ptr_util.h"
#include "base/metrics/histogram_macros.h"
#include "base/notreached.h"
#include "base/synchronization/waitable_event.h"
#include "base/task/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"
#include "gmock/gmock.h"
#include "gpu/command_buffer/service/abstract_texture_ohos.h"
#include "gpu/config/gpu_finch_features.h"
#include "gpu/ipc/service/gpu_channel.h"
#include "gpu/ipc/service/gpu_channel_manager.h"
#include "gpu/ipc/service/gpu_channel_test_common.h"
#include "gtest/gtest.h"
#include "media/base/scoped_async_trace.h"
#include "ui/gl/scoped_binders.h"
#include "ui/gl/scoped_make_current.h"
namespace gpu {
namespace {
std::unique_ptr<AbstractTextureOHOS> CreateTexture(
    SharedContextState* context_state) {
  DCHECK(context_state);

  gles2::FeatureInfo* feature_info = context_state->feature_info();
  if (feature_info && feature_info->is_passthrough_cmd_decoder()) {
    return AbstractTextureOHOS::CreateForPassthrough(gfx::Size());
  }

  return AbstractTextureOHOS::CreateForValidating(gfx::Size());
}
}  // namespace

class NativeImageTextureGlOwnerTest : public GpuChannelTestCommon {
 public:
  NativeImageTextureGlOwnerTest()
      : GpuChannelTestCommon(true /* use_stub_bindings */) {}
  ~NativeImageTextureGlOwnerTest() override = default;
  void SetUp() override {
    int32_t kClientId = 1;
    bool is_gpu_host = false;
    GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
    ContextResult result;
    auto context_state =
        channel->gpu_channel_manager()->GetSharedContextState(&result);
    auto texture = CreateTexture(context_state.get());
    texture_gl_owner = base::MakeRefCounted<gpu::NativeImageTextureGlOwner>(
        std::move(texture), std::move(context_state));
  }

  scoped_refptr<gpu::NativeImageTextureGlOwner> texture_gl_owner;
};

TEST_F(NativeImageTextureGlOwnerTest, UpdateNativeImage_001) {
  texture_gl_owner->UpdateNativeImage();
  ASSERT_NE(texture_gl_owner->native_image_, nullptr);
}

TEST_F(NativeImageTextureGlOwnerTest, GetSurfaceId_001) {
  uint64_t surface_id = 1;
  uint64_t* surface_id_ptr = &surface_id;
  texture_gl_owner->GetSurfaceId(surface_id_ptr);
  ASSERT_NE(texture_gl_owner->native_image_, nullptr);
}

TEST_F(NativeImageTextureGlOwnerTest, GetSurfaceId_002) {
  uint64_t surface_id = 1;
  uint64_t* surface_id_ptr = &surface_id;
  texture_gl_owner->native_image_ = nullptr;
  texture_gl_owner->GetSurfaceId(surface_id_ptr);
  ASSERT_EQ(texture_gl_owner->native_image_, nullptr);
}

TEST_F(NativeImageTextureGlOwnerTest, ReleaseNativeImage_001) {
  texture_gl_owner->ReleaseNativeImage();
  ASSERT_NE(texture_gl_owner->native_image_, nullptr);
}

TEST_F(NativeImageTextureGlOwnerTest, ReleaseNativeImage_002) {
  texture_gl_owner->native_image_ = nullptr;
  texture_gl_owner->ReleaseNativeImage();
  ASSERT_EQ(texture_gl_owner->native_image_, nullptr);
}

TEST_F(NativeImageTextureGlOwnerTest, AquireOhosNativeWindow_001) {
  auto result = texture_gl_owner->AquireOhosNativeWindow();
  ASSERT_NE(result, nullptr);
}

TEST_F(NativeImageTextureGlOwnerTest, AquireOhosNativeWindow_002) {
  texture_gl_owner->native_image_ = nullptr;
  auto result = texture_gl_owner->AquireOhosNativeWindow();
  ASSERT_EQ(result, nullptr);
}

TEST_F(NativeImageTextureGlOwnerTest, GetCodedSizeAndVisibleRect_001) {
  gfx::Size rotated_visible_size;
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  texture_gl_owner->native_image_ = nullptr;
  bool result = texture_gl_owner->GetCodedSizeAndVisibleRect(
      rotated_visible_size, coded_size, visible_rect);
  EXPECT_FALSE(result);
}

TEST_F(NativeImageTextureGlOwnerTest, GetCodedSizeAndVisibleRect_002) {
  gfx::Size rotated_visible_size(1, 1);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  bool result = texture_gl_owner->GetCodedSizeAndVisibleRect(
      rotated_visible_size, coded_size, visible_rect);
  EXPECT_TRUE(result);
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform001) {
  float mtx[16] = {0.0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  bool result = texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size,
                                                     coded_size, visible_rect);
  EXPECT_FALSE(result);
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform002) {
  float mtx[16] = {0.0};
  gfx::Size rotated_visible_size(1, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  bool result = texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size,
                                                     coded_size, visible_rect);
  EXPECT_TRUE(result);
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform003) {
  float mtx[16] = {0.0};
  gfx::Size rotated_visible_size(5, 1);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  bool result = texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size,
                                                     coded_size, visible_rect);
  EXPECT_TRUE(result);
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform004) {
  float mtx[16] = {0.0};
  gfx::Size rotated_visible_size(1, 1);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  bool result = texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size,
                                                     coded_size, visible_rect);
  EXPECT_TRUE(result);
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform005) {
  float mtx[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(true, texture_gl_owner->DecomposeTransform(
                      mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform006) {
  float mtx[16] = {1, 1, 3, 4, 0, 0, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(false, texture_gl_owner->DecomposeTransform(
                       mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform007) {
  float mtx[16] = {1, 0, 3, 4, 1, 0, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(false, texture_gl_owner->DecomposeTransform(
                       mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform008) {
  float mtx[16] = {1, 0, 3, 4, 0, 1, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(true, texture_gl_owner->DecomposeTransform(
                      mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform009) {
  float mtx[16] = {1, 0, 3, 4, 1, 1, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(true, texture_gl_owner->DecomposeTransform(
                      mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform010) {
  float mtx[16] = {1, 1, 3, 4, 0, 1, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(true, texture_gl_owner->DecomposeTransform(
                      mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform011) {
  float mtx[16] = {1, 1, 3, 4, 1, 0, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(false, texture_gl_owner->DecomposeTransform(
                       mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform012) {
  float mtx[16] = {1, 0, 3, 4, 0, 0, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(false, texture_gl_owner->DecomposeTransform(
                       mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform013) {
  float mtx[16] = {1, 1, 3, 4, 1, 1, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(true, texture_gl_owner->DecomposeTransform(
                      mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform014) {
  float mtx[16] = {-1, 1, 3, 4, 1, -1, 7, 8, 9, 10, 11, 2, 2, 2, 2, 2};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(true, texture_gl_owner->DecomposeTransform(
                      mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform015) {
  float mtx[16] = {0, 0, 3, 4, 0, 0, 7, 8, 9, 10, 11, 1, 0, 0, 1, 1};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(false, texture_gl_owner->DecomposeTransform(
                       mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform016) {
  float mtx[16] = {0, 1, 3, 4, 0, 0, 7, 8, 9, 10, 11, 1, 0, 1, 1, 1};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(false, texture_gl_owner->DecomposeTransform(
                       mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform017) {
  float mtx[16] = {0, 0, 3, 4, 1, 0, 7, 8, 9, 10, 11, 1, 1, 0, 1, 1};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(false, texture_gl_owner->DecomposeTransform(
                       mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform018) {
  float mtx[16] = {0.5, 1, 3, 4, 1, 0.5, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(true, texture_gl_owner->DecomposeTransform(
                      mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform019) {
  float mtx[16] = {0.0};
  gfx::Size rotated_visible_size(1, 1);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  bool result = texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size,
                                                     coded_size, visible_rect);
  EXPECT_TRUE(result);
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform020) {
  float mtx[16] = {0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(true, texture_gl_owner->DecomposeTransform(
                      mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform021) {
  float mtx[16] = {0, 1, 3, 4, 0, 0, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(false, texture_gl_owner->DecomposeTransform(
                       mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform022) {
  float mtx[16] = {0, 0, 3, 4, 1, 0, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(false, texture_gl_owner->DecomposeTransform(
                       mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform023) {
  float mtx[16] = {0, 0, 3, 4, 0, 1, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(true, texture_gl_owner->DecomposeTransform(
                      mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform024) {
  float mtx[16] = {0, 0, 3, 4, 1, 1, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(true, texture_gl_owner->DecomposeTransform(
                      mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform025) {
  float mtx[16] = {0, 1, 3, 4, 0, 1, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(true, texture_gl_owner->DecomposeTransform(
                      mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform026) {
  float mtx[16] = {0, 1, 3, 4, 1, 0, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(false, texture_gl_owner->DecomposeTransform(
                       mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform027) {
  float mtx[16] = {0, 0, 3, 4, 0, 0, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(false, texture_gl_owner->DecomposeTransform(
                       mtx, rotated_visible_size, coded_size, visible_rect));
}

TEST_F(NativeImageTextureGlOwnerTest, DecomposeTransform028) {
  float mtx[16] = {0, 1, 3, 4, 1, 1, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
  gfx::Size rotated_visible_size(5, 5);
  gfx::Size* coded_size = new gfx::Size(1, 1);
  gfx::Rect* visible_rect = new gfx::Rect(1, 1);
  mtx[0] = true;
  texture_gl_owner->DecomposeTransform(mtx, rotated_visible_size, coded_size,
                                       visible_rect);
  EXPECT_EQ(true, texture_gl_owner->DecomposeTransform(
                      mtx, rotated_visible_size, coded_size, visible_rect));
}

}  // namespace gpu
