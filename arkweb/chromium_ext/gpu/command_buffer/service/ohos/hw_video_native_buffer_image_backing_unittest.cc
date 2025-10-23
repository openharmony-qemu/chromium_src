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
#include "gpu/command_buffer/service/ohos/hw_video_native_buffer_image_backing.h"
#undef private
#undef protected

#include "base/ohos/scoped_native_buffer_handle.h"
#include "base/functional/callback_helpers.h"
#include "base/memory/raw_ptr.h"
#include "base/posix/eintr_wrapper.h"
#include "base/synchronization/lock.h"
#include "base/test/scoped_feature_list.h"
#include "base/test/test_mock_time_task_runner.h"
#include "base/test/task_environment.h"
#include "gpu/command_buffer/common/mailbox.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "gpu/command_buffer/service/ohos/native_image_texture_owner.h"
#include "gpu/command_buffer/service/stream_texture_shared_image_interface.h"
#include "gpu/command_buffer/service/abstract_texture_ohos.h"
#include "gpu/command_buffer/service/ohos/ohos_image_backing.h"
#include "gpu/command_buffer/service/ohos/ohos_video_image_backing.h"
#include "gpu/command_buffer/service/ohos/scoped_native_buffer_fence_sync.h"
#include "gpu/command_buffer/service/shared_context_state.h"
#include "gpu/command_buffer/service/shared_image/shared_image_backing.h"
#include "gpu/command_buffer/service/shared_image/test_image_backing.h"
#include "gpu/command_buffer/service/texture_manager.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "third_party/dawn/include/dawn/dawn_proc.h"
#include "third_party/skia/include/gpu/ganesh/GrBackendSurface.h"
#include "third_party/skia/include/gpu/ganesh/GrBackendSemaphore.h"
#include "third_party/skia/include/gpu/MutableTextureState.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_surface.h"
#include "ui/gl/gl_utils.h"
#include "ui/gl/init/gl_factory.h"
#include "ui/gl/test/gl_surface_test_support.h"
#include "ui/gl/test/gl_test_support.h"
#include "gpu/config/gpu_finch_features.h"
#include <sys/eventfd.h>

using namespace gpu;
using testing::_;
using testing::Return;
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

class MockScopedNativeBufferFenceSync : public ScopedNativeBufferFenceSync {
public:
    MockScopedNativeBufferFenceSync() : ScopedNativeBufferFenceSync() {}
    MockScopedNativeBufferFenceSync(ScopedNativeBufferHandle handle, base::ScopedFD fence_fd)
        : ScopedNativeBufferFenceSync(std::move(handle), std::move(fence_fd)) {}
    ~MockScopedNativeBufferFenceSync() override = default;
    MOCK_METHOD(void, SetReadFence, (base::ScopedFD fence_fd), (override));
};

class HwVideoNativeBufferImageBackingTest : public ::testing::Test {
public:
    HwVideoNativeBufferImageBackingTest() : task_environment_() {}
    
    void SetUp() override {
        gl::init::InitializeGLNoExtensionsOneOff(
            /*init_bindings=*/true, /*gpu_preference=*/gl::GpuPreference::kDefault);
        auto display = gl::GLTestSupport::InitializeGL(std::nullopt);
        ASSERT_NE(display, nullptr);
        surf_ = gl::init::CreateOffscreenGLSurface(display, gfx::Size());
        ASSERT_NE(surf_, nullptr);
        gl_context_ = gl::init::CreateGLContext(nullptr, surf_.get(), gl::GLContextAttribs());
        ASSERT_NE(gl_context_, nullptr);
        bool make_current_result = gl_context_->MakeCurrent(surf_.get());
        ASSERT_TRUE(make_current_result);
        tracker_ = std::make_unique<MemoryTypeTracker>(nullptr);
        mailbox_ = Mailbox::Generate();
        size_ = gfx::Size(SIZE, SIZE);
        color_space_ = gfx::ColorSpace::CreateSRGB();
        surface_origin_ = kTopLeft_GrSurfaceOrigin;
        alpha_type_ = kPremul_SkAlphaType;
        gr_context_type_ = GrContextType::kVulkan;
        debug_label_ = "test";
        stream_texture_sii_ = base::MakeRefCounted<MockStreamTextureSharedImageInterface>();
        context_state_ = base::MakeRefCounted<SharedContextState>(
            base::MakeRefCounted<gl::GLShareGroup>(),
            surf_, /* surface */
            gl_context_, /* context */
            false, /* use_virtualized_gl_contexts */
            base::DoNothing(), /* context_lost_callback */
            gr_context_type_ /* gr_context_type */);
        drdc_lock_ = base::MakeRefCounted<RefCountedLock>();
        backing_ = std::make_unique<HwVideoNativeBufferImageBacking>(mailbox_,
            size_, color_space_, surface_origin_, alpha_type_, debug_label_,
            stream_texture_sii_, context_state_, drdc_lock_);
    }

    void TearDown() override {
        if (gl_context_ && surf_) {
            gl_context_->ReleaseCurrent(surf_.get());
        }
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
    scoped_refptr<MockStreamTextureSharedImageInterface> stream_texture_sii_;
    scoped_refptr<SharedContextState> context_state_;
    scoped_refptr<RefCountedLock> drdc_lock_;
    std::unique_ptr<MemoryTypeTracker> tracker_;
    std::unique_ptr<HwVideoNativeBufferImageBacking> backing_;
    base::test::TaskEnvironment task_environment_{};
    scoped_refptr<gl::GLSurface> surf_;
    scoped_refptr<gl::GLContext> gl_context_;
};

TEST_F(HwVideoNativeBufferImageBackingTest, ProduceGLTexture_Fail)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(false));
    SharedImageManager manager;
    std::unique_ptr<GLTextureImageRepresentation> result =
        backing_->ProduceGLTexture(&manager, tracker_.get());
    EXPECT_EQ(result, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, ProduceGLTexture_Success)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTextureImageRepresentation> result =
        backing_->ProduceGLTexture(&manager, tracker_.get());
    EXPECT_NE(result, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, ProduceSkiaGanesh_Success)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<SkiaGaneshImageRepresentation> result =
        backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    EXPECT_NE(result, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, ProduceSkiaGanesh_Fail)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(false));
    SharedImageManager manager;
    std::unique_ptr<SkiaGaneshImageRepresentation> result =
        backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    EXPECT_EQ(result, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, GetEstimatedSizeForMemoryDump_UsingGpuMemory)
{
    EXPECT_CALL(*stream_texture_sii_, IsUsingGpuMemory())
        .WillOnce(Return(true));
    size_t estimated_size = backing_->GetEstimatedSizeForMemoryDump();
    EXPECT_GT(estimated_size, 0u);
}

TEST_F(HwVideoNativeBufferImageBackingTest, GetEstimatedSizeForMemoryDump_NotUsingGpuMemory)
{
    EXPECT_CALL(*stream_texture_sii_, IsUsingGpuMemory())
        .WillOnce(Return(false));
    size_t estimated_size = backing_->GetEstimatedSizeForMemoryDump();
    EXPECT_EQ(estimated_size, 0u);
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTextureVideoImageRepresentation_BeginAccess_Failure)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTextureImageRepresentation> representation =
        backing_->ProduceGLTexture(&manager, tracker_.get());
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(nullptr));
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_FALSE(result);
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTextureVideoImageRepresentation_BeginAccess_Success)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTextureImageRepresentation> representation =
        backing_->ProduceGLTexture(&manager, tracker_.get());
    auto mock_native_buffer = std::make_unique<MockScopedNativeBufferFenceSync>();
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_native_buffer))));
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_TRUE(result);
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTextureVideoImageRepresentation_BeginAccess_Success002)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTextureImageRepresentation> representation =
        backing_->ProduceGLTexture(&manager, tracker_.get());
    int efd = eventfd(1, 0);
    ASSERT_NE(efd, -1);
    auto mock_native_buffer = std::make_unique<MockScopedNativeBufferFenceSync>(
        ScopedNativeBufferHandle(),
        base::ScopedFD(efd)
    );
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_native_buffer))));
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_TRUE(result);
}

TEST_F(HwVideoNativeBufferImageBackingTest, BeginReadAccess_Fail)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<SkiaGaneshImageRepresentation> representation =
        backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    EXPECT_NE(representation, nullptr);
    std::vector<GrBackendSemaphore> begin_semaphores;
    std::vector<GrBackendSemaphore> end_semaphores;
    std::unique_ptr<skgpu::MutableTextureState> end_state;
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(nullptr));
    auto result = representation->BeginReadAccess(
        &begin_semaphores, &end_semaphores, &end_state);
    EXPECT_TRUE(result.empty());
}
}  // namespace gpu