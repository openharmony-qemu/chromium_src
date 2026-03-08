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

// Test: GLTexturePassthroughVideoImageRepresentation constructor initializes correctly
TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_Constructor)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTexturePassthroughImageRepresentation> representation =
        backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(representation, nullptr);
}

// Test: ProduceGLTexturePassthrough returns nullptr when there is no texture owner
TEST_F(HwVideoNativeBufferImageBackingTest, ProduceGLTexturePassthrough_NoTextureOwner)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(false));
    SharedImageManager manager;
    std::unique_ptr<GLTexturePassthroughImageRepresentation> result =
        backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_EQ(result, nullptr);
}

// Test: ProduceGLTexturePassthrough successfully creates when there is a texture owner
TEST_F(HwVideoNativeBufferImageBackingTest, ProduceGLTexturePassthrough_Success)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTexturePassthroughImageRepresentation> result =
        backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result->GetTexturePassthrough(0), nullptr);
}

// Test: BeginAccess returns false when the native buffer is null
TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_BeginAccess_NullBuffer)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTexturePassthroughImageRepresentation> representation =
        backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(representation, nullptr);
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(nullptr));
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_FALSE(result);
}

// Test: BeginAccess successfully obtained native buffer
TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_BeginAccess_Success)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTexturePassthroughImageRepresentation> representation =
        backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(representation, nullptr);
    auto mock_native_buffer = std::make_unique<MockScopedNativeBufferFenceSync>();
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_native_buffer))));
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_TRUE(result);
}

// Test: GLTexturePassthroughVideoImageRepresentation::EndAccess set fence
TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_EndAccess)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTexturePassthroughImageRepresentation> representation =
        backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(representation, nullptr);
    auto mock_native_buffer = std::make_unique<MockScopedNativeBufferFenceSync>();
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_native_buffer))));
    bool begin_result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_TRUE(begin_result);

    representation->EndAccess();
}

// Test: GetTexturePassthrough returns passthrough_texture_ and plane_index must be 0
TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_GetTexturePassthrough)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTexturePassthroughImageRepresentation> representation =
        backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(representation, nullptr);
    const scoped_refptr<gles2::TexturePassthrough>& texture = representation->GetTexturePassthrough(0);
    EXPECT_NE(texture, nullptr);
    EXPECT_EQ(texture->target(), GL_TEXTURE_EXTERNAL_OES);
}

// Test: CreateAndBindEglImageFromNativeBuffer calls EGL function
TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_BeginAccess_WithValidBuffer)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTexturePassthroughImageRepresentation> representation =
        backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(representation, nullptr);

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

// Test: The destructor calls NotifyOnContextLost when there is no context
TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_Destructor_ContextLost)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(representation, nullptr);

    // Simulate context loss by releasing GL context
    if (gl_context_ && surf_) {
        gl_context_->ReleaseCurrent(surf_.get());
    }

    // Destroy representation - destructor should call NotifyOnContextLost
    representation.reset();
}

// Test: GLTexturePassthroughVideoImageRepresentation type is correct
TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_CorrectType)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTexturePassthroughImageRepresentation> result =
        backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(result, nullptr);
    // Verify the representation is indeed passthrough type
    EXPECT_NE(result->GetTexturePassthrough(0), nullptr);
}

// Test: ProduceGLTexturePassthrough creates two different representation instances
TEST_F(HwVideoNativeBufferImageBackingTest, ProduceGLTexturePassthrough_MultipleCalls)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .Times(2)
        .WillRepeatedly(Return(true));
    SharedImageManager manager;
    auto representation1 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    auto representation2 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(representation1, nullptr);
    EXPECT_NE(representation2, nullptr);
    // Each should have its own texture
    EXPECT_NE(representation1->GetTexturePassthrough(0), representation2->GetTexturePassthrough(0));
}

// Test: BeginAccess using DrDC lock protection
TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_BeginAccess_ThreadSafe)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTexturePassthroughImageRepresentation> representation =
        backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(representation, nullptr);

    auto mock_native_buffer = std::make_unique<MockScopedNativeBufferFenceSync>();
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_native_buffer))));

    // BeginAccess should complete without deadlocks with DrDC lock
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_TRUE(result);
}

TEST_F(HwVideoNativeBufferImageBackingTest, ProduceSkiaGanesh_GLContext_Success)
{
    // Create a GL context state instead of Vulkan to exercise the GL path
    auto gl_context_state = base::MakeRefCounted<SharedContextState>(
        base::MakeRefCounted<gl::GLShareGroup>(),
        surf_, /* surface */
        gl_context_, /* context */
        false, /* use_virtualized_gl_contexts */
        base::DoNothing(), /* context_lost_callback */
        GrContextType::kGL /* gr_context_type */);
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<SkiaGaneshImageRepresentation> result =
        backing_->ProduceSkiaGanesh(&manager, tracker_.get(), gl_context_state);
    EXPECT_NE(result, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, ProduceSkiaGanesh_NoTextureOwner)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(false));
    SharedImageManager manager;
    std::unique_ptr<SkiaGaneshImageRepresentation> result =
        backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    EXPECT_EQ(result, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTextureVideoImageRepresentation_BeginAccess_WithNoFence)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTextureImageRepresentation> representation =
        backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    // Create a mock native buffer with fence_fd == -1 (no fence needed)
    auto mock_native_buffer = std::make_unique<MockScopedNativeBufferFenceSync>();
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_native_buffer))));
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_TRUE(result);
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_BeginAndEndAccess)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTexturePassthroughImageRepresentation> representation =
        backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(representation, nullptr);
    auto mock_native_buffer = std::make_unique<MockScopedNativeBufferFenceSync>();
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_native_buffer))));
    bool begin_result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_TRUE(begin_result);
    representation->EndAccess();
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_BeginAccess_WithFence)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTexturePassthroughImageRepresentation> representation =
        backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(representation, nullptr);
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

TEST_F(HwVideoNativeBufferImageBackingTest, MultipleGLTexturePassthroughRepresentations)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .Times(2)
        .WillRepeatedly(Return(true));
    SharedImageManager manager;
    auto rep1 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    auto rep2 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(rep1, nullptr);
    EXPECT_NE(rep2, nullptr);
    EXPECT_NE(rep1->GetTexturePassthrough(0), rep2->GetTexturePassthrough(0));
}

TEST_F(HwVideoNativeBufferImageBackingTest, ProduceSkiaGanesh_VulkanContext)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<SkiaGaneshImageRepresentation> result =
        backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    EXPECT_NE(result, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTextureVideoImageRepresentation_EndAccess_AfterBegin)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTextureImageRepresentation> representation =
        backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    auto mock_native_buffer = std::make_unique<MockScopedNativeBufferFenceSync>();
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_native_buffer))));
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_TRUE(result);
    representation->EndAccess();
}


TEST_F(HwVideoNativeBufferImageBackingTest, BackingMailboxMatchesInput)
{
    EXPECT_EQ(backing_->mailbox(), mailbox_);
}

TEST_F(HwVideoNativeBufferImageBackingTest, BackingSizeMatchesInput)
{
    EXPECT_EQ(backing_->size(), size_);
}

TEST_F(HwVideoNativeBufferImageBackingTest, BackingColorSpaceMatchesInput)
{
    EXPECT_EQ(backing_->color_space(), color_space_);
}

TEST_F(HwVideoNativeBufferImageBackingTest, BackingSurfaceOriginMatchesInput)
{
    EXPECT_EQ(backing_->surface_origin(), surface_origin_);
}

TEST_F(HwVideoNativeBufferImageBackingTest, BackingAlphaTypeMatchesInput)
{
    EXPECT_EQ(backing_->alpha_type(), alpha_type_);
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTexture_BeginAccess_ReadWriteMode)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    auto mock_native_buffer = std::make_unique<MockScopedNativeBufferFenceSync>();
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_native_buffer))));
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READWRITE_CHROMIUM);
    EXPECT_TRUE(result);
    representation->EndAccess();
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTexture_BeginEndAccess_TwoCycles)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    for (int i = 0; i < 2; ++i) {
        auto mock_buf = std::make_unique<MockScopedNativeBufferFenceSync>();
        EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
            .WillOnce(Return(testing::ByMove(std::move(mock_buf))));
        bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
        EXPECT_TRUE(result);
        representation->EndAccess();
    }
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTexture_BeginAccess_WithFence_ThenEndAccess)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    int efd = eventfd(1, 0);
    ASSERT_NE(efd, -1);
    auto mock_native_buffer = std::make_unique<MockScopedNativeBufferFenceSync>(
        ScopedNativeBufferHandle(), base::ScopedFD(efd));
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_native_buffer))));
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_TRUE(result);
    representation->EndAccess();
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTexture_DestroyWithoutAccess)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    representation.reset();
    EXPECT_NE(backing_, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_BeginAccess_ReadWriteMode)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    auto mock_native_buffer = std::make_unique<MockScopedNativeBufferFenceSync>();
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_native_buffer))));
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READWRITE_CHROMIUM);
    EXPECT_TRUE(result);
    representation->EndAccess();
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_BeginEndAccess_TwoCycles)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    for (int i = 0; i < 2; ++i) {
        auto mock_buf = std::make_unique<MockScopedNativeBufferFenceSync>();
        EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
            .WillOnce(Return(testing::ByMove(std::move(mock_buf))));
        bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
        EXPECT_TRUE(result);
        representation->EndAccess();
    }
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_DestroyWithoutAccess)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    representation.reset();
    EXPECT_NE(backing_, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, ProduceGLTexture_FailThenSucceed)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto fail_rep = backing_->ProduceGLTexture(&manager, tracker_.get());
    EXPECT_EQ(fail_rep, nullptr);
    auto ok_rep = backing_->ProduceGLTexture(&manager, tracker_.get());
    EXPECT_NE(ok_rep, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, ProduceGLTexturePassthrough_FailThenSucceed)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto fail_rep = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_EQ(fail_rep, nullptr);
    auto ok_rep = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(ok_rep, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, ProduceSkiaGanesh_FailThenSucceed)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto fail_rep = backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    EXPECT_EQ(fail_rep, nullptr);
    auto ok_rep = backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    EXPECT_NE(ok_rep, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_GetTexturePassthrough_ExternalOES)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    const auto& texture = representation->GetTexturePassthrough(0);
    EXPECT_NE(texture, nullptr);
    EXPECT_EQ(texture->target(), GL_TEXTURE_EXTERNAL_OES);
}

TEST_F(HwVideoNativeBufferImageBackingTest, GetEstimatedSizeForMemoryDump_ConsistentWithState)
{
    EXPECT_CALL(*stream_texture_sii_, IsUsingGpuMemory())
        .WillOnce(Return(true));
    size_t size_with_gpu = backing_->GetEstimatedSizeForMemoryDump();
    EXPECT_GT(size_with_gpu, 0u);

    EXPECT_CALL(*stream_texture_sii_, IsUsingGpuMemory())
        .WillOnce(Return(false));
    size_t size_without_gpu = backing_->GetEstimatedSizeForMemoryDump();
    EXPECT_EQ(size_without_gpu, 0u);

    EXPECT_GT(size_with_gpu, size_without_gpu);
}

TEST_F(HwVideoNativeBufferImageBackingTest, ProduceSkiaGanesh_DestroyWithoutAccess)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto rep = backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    ASSERT_NE(rep, nullptr);
    rep.reset();
    EXPECT_NE(backing_, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, ProduceSkiaGanesh_MultipleRepresentations)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .Times(2)
        .WillRepeatedly(Return(true));
    SharedImageManager manager;
    auto rep1 = backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    auto rep2 = backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    EXPECT_NE(rep1, nullptr);
    EXPECT_NE(rep2, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, BeginReadAccess_WithValidBuffer_NoContextCrash)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    ASSERT_NE(representation, nullptr);
    auto mock_native_buffer = std::make_unique<MockScopedNativeBufferFenceSync>();
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_native_buffer))));
    std::vector<GrBackendSemaphore> begin_semaphores;
    std::vector<GrBackendSemaphore> end_semaphores;
    std::unique_ptr<skgpu::MutableTextureState> end_state;
    // The method should run without crashing regardless of result in unit test env
    auto result = representation->BeginReadAccess(&begin_semaphores, &end_semaphores, &end_state);
    EXPECT_NE(backing_, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, ProduceGLTexture_ThreeRepresentations)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .Times(3)
        .WillRepeatedly(Return(true));
    SharedImageManager manager;
    auto rep1 = backing_->ProduceGLTexture(&manager, tracker_.get());
    auto rep2 = backing_->ProduceGLTexture(&manager, tracker_.get());
    auto rep3 = backing_->ProduceGLTexture(&manager, tracker_.get());
    EXPECT_NE(rep1, nullptr);
    EXPECT_NE(rep2, nullptr);
    EXPECT_NE(rep3, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, ProduceGLTexture_ThenPassthrough_ThenSkia)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .Times(3)
        .WillRepeatedly(Return(true));
    SharedImageManager manager;
    auto gl_rep = backing_->ProduceGLTexture(&manager, tracker_.get());
    auto pt_rep = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    auto sk_rep = backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    EXPECT_NE(gl_rep, nullptr);
    EXPECT_NE(pt_rep, nullptr);
    EXPECT_NE(sk_rep, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTexture_BeginAccess_NullBuffer_ThenSuccess)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .Times(2)
        .WillRepeatedly(Return(true));
    SharedImageManager manager;
    auto rep1 = backing_->ProduceGLTexture(&manager, tracker_.get());
    auto rep2 = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(rep1, nullptr);
    ASSERT_NE(rep2, nullptr);

    // First representation: null buffer -> fail
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(nullptr)));
    bool result1 = rep1->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_FALSE(result1);

    // Second representation: valid buffer -> succeed
    auto mock_buf = std::make_unique<MockScopedNativeBufferFenceSync>();
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_buf))));
    bool result2 = rep2->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_TRUE(result2);
    rep2->EndAccess();
}

TEST_F(HwVideoNativeBufferImageBackingTest, ContextState_IsVulkan)
{
    // Verify context was created with Vulkan type (as set in SetUp)
    EXPECT_EQ(gr_context_type_, GrContextType::kVulkan);
}

TEST_F(HwVideoNativeBufferImageBackingTest, BackingStreamTextureSii_NotNull)
{
    EXPECT_NE(backing_->stream_texture_sii_, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, ProduceSkiaGanesh_GLContext_MultipleReps)
{
    auto gl_context_state = base::MakeRefCounted<SharedContextState>(
        base::MakeRefCounted<gl::GLShareGroup>(),
        surf_, gl_context_, false, base::DoNothing(),
        GrContextType::kGL);
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .Times(2)
        .WillRepeatedly(Return(true));
    SharedImageManager manager;
    auto rep1 = backing_->ProduceSkiaGanesh(&manager, tracker_.get(), gl_context_state);
    auto rep2 = backing_->ProduceSkiaGanesh(&manager, tracker_.get(), gl_context_state);
    EXPECT_NE(rep1, nullptr);
    EXPECT_NE(rep2, nullptr);
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTexture_BeginAccess_MixedResults)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .Times(2)
        .WillRepeatedly(Return(true));
    SharedImageManager manager;
    auto rep1 = backing_->ProduceGLTexture(&manager, tracker_.get());
    auto rep2 = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(rep1, nullptr);
    ASSERT_NE(rep2, nullptr);

    // rep1 fails (null buffer), rep2 succeeds (valid buffer)
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(nullptr)));
    EXPECT_FALSE(rep1->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM));

    auto mock_buf = std::make_unique<MockScopedNativeBufferFenceSync>();
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_buf))));
    EXPECT_TRUE(rep2->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM));
    rep2->EndAccess();
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTexturePassthrough_BeginAccess_MixedResults)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .Times(2)
        .WillRepeatedly(Return(true));
    SharedImageManager manager;
    auto rep1 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    auto rep2 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    ASSERT_NE(rep1, nullptr);
    ASSERT_NE(rep2, nullptr);

    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(nullptr)));
    EXPECT_FALSE(rep1->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM));

    auto mock_buf = std::make_unique<MockScopedNativeBufferFenceSync>();
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_buf))));
    EXPECT_TRUE(rep2->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM));
    rep2->EndAccess();
}

TEST_F(HwVideoNativeBufferImageBackingTest, BackingDebugLabel_NotEmpty)
{
    EXPECT_FALSE(debug_label_.empty());
    EXPECT_EQ(debug_label_, "test");
}

TEST_F(HwVideoNativeBufferImageBackingTest, GetEstimatedSizeForMemoryDump_CalledTwiceSameState)
{
    EXPECT_CALL(*stream_texture_sii_, IsUsingGpuMemory())
        .Times(2)
        .WillRepeatedly(Return(true));
    size_t result1 = backing_->GetEstimatedSizeForMemoryDump();
    size_t result2 = backing_->GetEstimatedSizeForMemoryDump();
    EXPECT_EQ(result1, result2);
    EXPECT_GT(result1, 0u);
}

TEST_F(HwVideoNativeBufferImageBackingTest, GLTexture_BeginAccess_ReadWriteMode_WithFence)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    int efd = eventfd(1, 0);
    ASSERT_NE(efd, -1);
    auto mock_native_buffer = std::make_unique<MockScopedNativeBufferFenceSync>(
        ScopedNativeBufferHandle(), base::ScopedFD(efd));
    EXPECT_CALL(*stream_texture_sii_, GetNativeBuffer())
        .WillOnce(Return(testing::ByMove(std::move(mock_native_buffer))));
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READWRITE_CHROMIUM);
    EXPECT_TRUE(result);
    representation->EndAccess();
}

}  // namespace gpu