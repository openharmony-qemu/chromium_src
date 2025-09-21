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

#include "base/containers/flat_set.h"
#define private public
#define protected public
#include "gpu/command_buffer/service/ohos/ohos_video_image_backing.h"
#undef private
#undef protected

#include "base/functional/callback_helpers.h"
#include "base/memory/raw_ptr.h"
#include "base/synchronization/lock.h"
#include "base/test/scoped_feature_list.h"
#include "base/test/task_environment.h"
#include "gpu/command_buffer/common/mailbox.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "gpu/command_buffer/service/ohos/ohos_image_backing.h"
#include "gpu/command_buffer/service/shared_image/shared_image_backing.h"
#include "gpu/command_buffer/service/stream_texture_shared_image_interface.h"
#include "gpu/command_buffer/service/abstract_texture_ohos.h"
#include "gpu/config/gpu_finch_features.h"
#include "gpu/ipc/service/gpu_channel.h"
#include "gpu/ipc/service/gpu_channel_manager.h"
#include "gpu/ipc/service/gpu_channel_test_common.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_surface.h"
#include "ui/gl/gl_utils.h"
#include "ui/gl/init/gl_factory.h"
#include "ui/gl/test/gl_surface_test_support.h"
#include "ui/gl/test/gl_test_support.h"

using namespace gpu;
using testing::_;
namespace gpu {
constexpr int SIZE = 256;
class MockStreamTextureSharedImageInterface
    : public StreamTextureSharedImageInterface {
public:
    MockStreamTextureSharedImageInterface() = default;
    ~MockStreamTextureSharedImageInterface() override = default;
    MOCK_METHOD(void, ReleaseResources, (), (override));
    MOCK_METHOD(bool, IsUsingGpuMemory, (), (const, override));
    MOCK_METHOD(void, UpdateAndBindTexImage, (GLuint service_id), (override));
    MOCK_METHOD(bool, HasTextureOwner, (), (const, override));
    MOCK_METHOD(TextureBase*, GetTextureBase, (), (const, override));
    MOCK_METHOD(void, NotifyOverlayPromotion, (bool promotion, const gfx::Rect& bounds), (override));
    MOCK_METHOD(bool, RenderToOverlay, (), (override));
    MOCK_METHOD(bool, TextureOwnerBindsTextureOnUpdate, (), (override));
#if BUILDFLAG(ARKWEB_SAME_LAYER)
    MOCK_METHOD(std::unique_ptr<ScopedNativeBufferFenceSync>, GetNativeBuffer, (), (override));
#endif
};

class OhosVideoImageBackingTest : public ::testing::Test {
public:
    OhosVideoImageBackingTest() : task_environment_() {}

    void SetUp() override {
        gl::init::InitializeGLNoExtensionsOneOff(
            /*init_bindings=*/true, /*gpu_preference=*/gl::GpuPreference::kDefault);
        auto display = gl::GLTestSupport::InitializeGL(std::nullopt);
        ASSERT_NE(display, nullptr);
        auto surf = gl::init::CreateOffscreenGLSurface(display, gfx::Size());
        ASSERT_NE(surf, nullptr);
        auto context = gl::init::CreateGLContext(nullptr, surf.get(), gl::GLContextAttribs());
        ASSERT_NE(context, nullptr);
        tracker_ = std::make_unique<MemoryTypeTracker>(nullptr);
        mailbox_ = Mailbox::Generate();
        size_ = gfx::Size(SIZE, SIZE);
        color_space_ = gfx::ColorSpace::CreateSRGB();
        surface_origin_ = kTopLeft_GrSurfaceOrigin;
        alpha_type_ = kPremul_SkAlphaType;
        gr_context_type_ = GrContextType::kVulkan;
        debug_label_ = "test";
        texture_owner_mode_ = gl::ohos::TextureOwnerMode::kSameLayerNativeBuffer;
        stream_texture_sii_ = base::MakeRefCounted<MockStreamTextureSharedImageInterface>();
        // Create a minimal context state for testing
        context_state_ = base::MakeRefCounted<SharedContextState>(
            base::MakeRefCounted<gl::GLShareGroup>(),
            surf, /* surface */
            context, /* context */
            false, /* use_virtualized_gl_contexts */
            base::DoNothing(), /* context_lost_callback */
            gr_context_type_ /* gr_context_type */);
        drdc_lock_ = base::MakeRefCounted<RefCountedLock>();
    }

    void TearDown() override {
        backing_.reset();
    }

protected:
    gpu::Mailbox mailbox_;
    gfx::Size size_;
    gfx::ColorSpace color_space_;
    GrSurfaceOrigin surface_origin_;
    SkAlphaType alpha_type_;
    GrContextType gr_context_type_;
    std::string debug_label_;
    gl::ohos::TextureOwnerMode texture_owner_mode_;
    scoped_refptr<MockStreamTextureSharedImageInterface> stream_texture_sii_;
    scoped_refptr<SharedContextState> context_state_;
    scoped_refptr<RefCountedLock> drdc_lock_;
    std::unique_ptr<MemoryTypeTracker> tracker_;
    std::unique_ptr<OhosVideoImageBacking> backing_;
    base::test::TaskEnvironment task_environment_{};
};

TEST_F(OhosVideoImageBackingTest, OhosVideoImageBackingTest001)
{
    texture_owner_mode_ = gl::ohos::TextureOwnerMode::kSameLayerNativeBuffer;
    auto result = OhosVideoImageBacking::Create(mailbox_, size_, color_space_,
        surface_origin_, alpha_type_, debug_label_, texture_owner_mode_,
        stream_texture_sii_, context_state_, drdc_lock_);
    EXPECT_NE(result, nullptr);
    texture_owner_mode_ = gl::ohos::TextureOwnerMode::kHwVideoZeroCopyNativeBuffer;
    result = OhosVideoImageBacking::Create(mailbox_, size_, color_space_,
        surface_origin_, alpha_type_, debug_label_, texture_owner_mode_,
        stream_texture_sii_, context_state_, drdc_lock_);
    EXPECT_NE(result, nullptr);
    texture_owner_mode_ = gl::ohos::TextureOwnerMode::kNone;
    result = OhosVideoImageBacking::Create(mailbox_, size_, color_space_,
        surface_origin_, alpha_type_, debug_label_, texture_owner_mode_,
        stream_texture_sii_, context_state_, drdc_lock_);
    EXPECT_NE(result, nullptr);
    auto info = OhosVideoImageBacking::GetYcbcrInfo(nullptr, nullptr);
    EXPECT_EQ(info, absl::nullopt);
}
}  // namespace gpu