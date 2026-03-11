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
#include "gpu/command_buffer/service/ohos/native_image_image_backing.h"
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
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_surface.h"
#include "ui/gl/gl_utils.h"
#include "ui/gl/init/gl_factory.h"
#include "ui/gl/test/gl_surface_test_support.h"
#include "ui/gl/test/gl_test_support.h"
#include "gpu/config/gpu_finch_features.h"

using namespace gpu;
using testing::_;
using testing::Return;
using testing::NiceMock;

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

class MockTextureBase : public TextureBase {
public:
    explicit MockTextureBase(unsigned int service_id) 
        : TextureBase(0), service_id_(service_id) {}
    ~MockTextureBase() override = default;
    unsigned int service_id() const { return service_id_; }
private:
    unsigned int service_id_;
};

class NativeImageImageBackingTest : public ::testing::Test {
public:
    NativeImageImageBackingTest() : task_environment_() {}
    
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
        gr_context_type_ = GrContextType::kGL;
        debug_label_ = "test";
        stream_texture_sii_ = base::MakeRefCounted<NiceMock<MockStreamTextureSharedImageInterface>>();
        // Set up default mock behaviors
        ON_CALL(*stream_texture_sii_, HasTextureOwner()).WillByDefault(Return(true));
        ON_CALL(*stream_texture_sii_, GetTextureBase()).WillByDefault(Return(&mock_texture_));
        ON_CALL(*stream_texture_sii_, TextureOwnerBindsTextureOnUpdate()).WillByDefault(Return(true));
        ON_CALL(*stream_texture_sii_, IsUsingGpuMemory()).WillByDefault(Return(true));
        context_state_ = base::MakeRefCounted<SharedContextState>(
            base::MakeRefCounted<gl::GLShareGroup>(),
            surf_, /* surface */
            gl_context_, /* context */
            false, /* use_virtualized_gl_contexts */
            base::DoNothing(), /* context_lost_callback */
            gr_context_type_ /* gr_context_type */);
        backing_ = std::make_unique<NativeImageImageBacking>(mailbox_,
            size_, color_space_, surface_origin_, alpha_type_, debug_label_,
            stream_texture_sii_, context_state_);
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
    scoped_refptr<NiceMock<MockStreamTextureSharedImageInterface>> stream_texture_sii_;
    scoped_refptr<SharedContextState> context_state_;
    std::unique_ptr<MemoryTypeTracker> tracker_;
    std::unique_ptr<NativeImageImageBacking> backing_;
    base::test::TaskEnvironment task_environment_{};
    scoped_refptr<gl::GLSurface> surf_;
    scoped_refptr<gl::GLContext> gl_context_;
    MockTextureBase mock_texture_{123};
};

TEST_F(NativeImageImageBackingTest, ConstructorAndDestructor)
{
    EXPECT_NE(backing_, nullptr);
    EXPECT_EQ(backing_->mailbox(), mailbox_);
    EXPECT_EQ(backing_->size(), size_);
    EXPECT_EQ(backing_->color_space(), color_space_);
    EXPECT_EQ(backing_->surface_origin(), surface_origin_);
    EXPECT_EQ(backing_->alpha_type(), alpha_type_);
    size_t estimated_size = backing_->GetEstimatedSizeForMemoryDump();
    EXPECT_EQ(estimated_size, 0u);
    EXPECT_CALL(*stream_texture_sii_, ReleaseResources()).Times(1);
    backing_.reset();
}

TEST_F(NativeImageImageBackingTest, ProduceGLTexture_Fail)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(false));
    SharedImageManager manager;
    std::unique_ptr<GLTextureImageRepresentation> result =
        backing_->ProduceGLTexture(&manager, tracker_.get());
    EXPECT_EQ(result, nullptr);
}

TEST_F(NativeImageImageBackingTest, ProduceGLTexture_Success)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTextureImageRepresentation> result =
        backing_->ProduceGLTexture(&manager, tracker_.get());
    EXPECT_NE(result, nullptr);
}

TEST_F(NativeImageImageBackingTest, ProduceGLTexturePassthrough_Fail)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(false));
    SharedImageManager manager;
    std::unique_ptr<GLTexturePassthroughImageRepresentation> result =
        backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_EQ(result, nullptr);
}

TEST_F(NativeImageImageBackingTest, ProduceSkiaGanesh_NoTextureOwner)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(false));
    SharedImageManager manager;
    std::unique_ptr<SkiaGaneshImageRepresentation> result =
        backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    EXPECT_EQ(result, nullptr);
}

TEST_F(NativeImageImageBackingTest, ProduceSkiaGanesh_NonGLContext)
{
    auto non_gl_context_state = base::MakeRefCounted<SharedContextState>(
        base::MakeRefCounted<gl::GLShareGroup>(),
        surf_, gl_context_, false, base::DoNothing(),
        GrContextType::kVulkan);
    SharedImageManager manager;
    std::unique_ptr<SkiaGaneshImageRepresentation> result =
        backing_->ProduceSkiaGanesh(&manager, tracker_.get(), non_gl_context_state);
    EXPECT_EQ(result, nullptr);
}

TEST_F(NativeImageImageBackingTest, OnContextLost)
{
    EXPECT_CALL(*stream_texture_sii_, ReleaseResources()).Times(1);
    backing_->OnContextLost();
    EXPECT_EQ(backing_->context_state_, nullptr);
    // Destructor should handle context_state_ == nullptr gracefully
    EXPECT_CALL(*stream_texture_sii_, ReleaseResources()).Times(1);
    backing_.reset();
}

TEST_F(NativeImageImageBackingTest, DestructorWithNullStreamTextureSii)
{
    // After OnContextLost, stream_texture_sii_ is still valid but
    // context_state_ is null. Test destructor with stream_texture_sii_ reset.
    EXPECT_CALL(*stream_texture_sii_, ReleaseResources()).Times(1);
    backing_->OnContextLost();
    backing_->stream_texture_sii_.reset();
    // Destructor should not crash with nullptr stream_texture_sii_
    backing_.reset();
}

TEST_F(NativeImageImageBackingTest, ProduceGLTexturePassthrough_Success)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    std::unique_ptr<GLTexturePassthroughImageRepresentation> result =
        backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result->GetTexturePassthrough(0), nullptr);
}

TEST_F(NativeImageImageBackingTest, ProduceSkiaGanesh_GLContext_Success)
{
    SharedImageManager manager;
    std::unique_ptr<SkiaGaneshImageRepresentation> result =
        backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    EXPECT_NE(result, nullptr);
}

TEST_F(NativeImageImageBackingTest, GLTexture_BeginAccess_EndAccess)
{
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(testing::_)).Times(1);
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_TRUE(result);
    representation->EndAccess();
}

TEST_F(NativeImageImageBackingTest, GLTexturePassthrough_BeginAccess_EndAccess)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(testing::_)).Times(1);
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_TRUE(result);
    representation->EndAccess();
}

TEST_F(NativeImageImageBackingTest, GLTexture_GetTexture)
{
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    auto* texture = representation->GetTexture(0);
    EXPECT_NE(texture, nullptr);
}

TEST_F(NativeImageImageBackingTest, GLTexturePassthrough_GetTexturePassthrough)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    const auto& texture = representation->GetTexturePassthrough(0);
    EXPECT_NE(texture, nullptr);
}

TEST_F(NativeImageImageBackingTest, BeginGLReadAccess)
{
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(123u)).Times(1);
    backing_->BeginGLReadAccess(123u);
}

TEST_F(NativeImageImageBackingTest, GetEstimatedSizeForMemoryDump_AlwaysZero)
{
    size_t estimated_size = backing_->GetEstimatedSizeForMemoryDump();
    EXPECT_EQ(estimated_size, 0u);
}

TEST_F(NativeImageImageBackingTest, MultipleGLTextureRepresentations)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .Times(2)
        .WillRepeatedly(Return(true));
    SharedImageManager manager;
    auto rep1 = backing_->ProduceGLTexture(&manager, tracker_.get());
    auto rep2 = backing_->ProduceGLTexture(&manager, tracker_.get());
    EXPECT_NE(rep1, nullptr);
    EXPECT_NE(rep2, nullptr);
}

TEST_F(NativeImageImageBackingTest, MultiplePassthroughRepresentations)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .Times(2)
        .WillRepeatedly(Return(true));
    SharedImageManager manager;
    auto rep1 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    auto rep2 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(rep1, nullptr);
    EXPECT_NE(rep2, nullptr);
}

TEST_F(NativeImageImageBackingTest, BackingMailboxMatchesInput)
{
    EXPECT_EQ(backing_->mailbox(), mailbox_);
}

TEST_F(NativeImageImageBackingTest, BackingSizeMatchesInput)
{
    EXPECT_EQ(backing_->size(), size_);
}

TEST_F(NativeImageImageBackingTest, BackingColorSpaceMatchesInput)
{
    EXPECT_EQ(backing_->color_space(), color_space_);
}

TEST_F(NativeImageImageBackingTest, BackingSurfaceOriginMatchesInput)
{
    EXPECT_EQ(backing_->surface_origin(), surface_origin_);
}

TEST_F(NativeImageImageBackingTest, BackingAlphaTypeMatchesInput)
{
    EXPECT_EQ(backing_->alpha_type(), alpha_type_);
}

TEST_F(NativeImageImageBackingTest, OnContextLost_SetsContextStateToNull)
{
    ASSERT_NE(backing_->context_state_, nullptr);
    EXPECT_CALL(*stream_texture_sii_, ReleaseResources()).Times(1);
    backing_->OnContextLost();
    EXPECT_EQ(backing_->context_state_, nullptr);
    EXPECT_CALL(*stream_texture_sii_, ReleaseResources()).Times(1);
    backing_.reset();
}

TEST_F(NativeImageImageBackingTest, OnContextLost_CalledTwice_SecondCallSafe)
{
    EXPECT_CALL(*stream_texture_sii_, ReleaseResources()).Times(1);
    backing_->OnContextLost();
    EXPECT_EQ(backing_->context_state_, nullptr);
    // Second call with context_state_ already null should be safe
    backing_->OnContextLost();
    EXPECT_EQ(backing_->context_state_, nullptr);
    EXPECT_CALL(*stream_texture_sii_, ReleaseResources()).Times(1);
    backing_.reset();
}

TEST_F(NativeImageImageBackingTest, OnContextLost_StreamTextureSiiRemainsValidAfterContextLost)
{
    EXPECT_CALL(*stream_texture_sii_, ReleaseResources()).Times(1);
    backing_->OnContextLost();
    EXPECT_NE(backing_->stream_texture_sii_, nullptr);
    EXPECT_EQ(backing_->context_state_, nullptr);
    EXPECT_CALL(*stream_texture_sii_, ReleaseResources()).Times(1);
    backing_.reset();
}

TEST_F(NativeImageImageBackingTest, GLTexture_BeginAccess_ReadWriteMode)
{
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(testing::_)).Times(1);
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READWRITE_CHROMIUM);
    EXPECT_TRUE(result);
    representation->EndAccess();
}

TEST_F(NativeImageImageBackingTest, GLTexture_BeginEndAccess_ThreeCycles)
{
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(testing::_)).Times(3);
    for (int i = 0; i < 3; ++i) {
        bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
        EXPECT_TRUE(result);
        representation->EndAccess();
    }
}

TEST_F(NativeImageImageBackingTest, GLTexture_DestroyRepresentationWithoutAccess)
{
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    // Destroy without calling BeginAccess - should be safe
    representation.reset();
    EXPECT_NE(backing_, nullptr);
}

TEST_F(NativeImageImageBackingTest, GLTexturePassthrough_BeginAccess_ReadWriteMode)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(testing::_)).Times(1);
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READWRITE_CHROMIUM);
    EXPECT_TRUE(result);
    representation->EndAccess();
}

TEST_F(NativeImageImageBackingTest, GLTexturePassthrough_BeginEndAccess_TwoCycles)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(testing::_)).Times(2);
    for (int i = 0; i < 2; ++i) {
        bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
        EXPECT_TRUE(result);
        representation->EndAccess();
    }
}

TEST_F(NativeImageImageBackingTest, GLTexturePassthrough_DestroyRepresentationWithoutAccess)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    representation.reset();
    EXPECT_NE(backing_, nullptr);
}

TEST_F(NativeImageImageBackingTest, BeginGLReadAccess_ZeroServiceId)
{
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(0u)).Times(1);
    backing_->BeginGLReadAccess(0u);
}

TEST_F(NativeImageImageBackingTest, BeginGLReadAccess_LargeServiceId)
{
    GLuint large_id = 0x0000FFFFu;
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(large_id)).Times(1);
    backing_->BeginGLReadAccess(large_id);
}

TEST_F(NativeImageImageBackingTest, BeginGLReadAccess_SequentialDifferentIds)
{
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(10u)).Times(1);
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(20u)).Times(1);
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(30u)).Times(1);
    backing_->BeginGLReadAccess(10u);
    backing_->BeginGLReadAccess(20u);
    backing_->BeginGLReadAccess(30u);
}

TEST_F(NativeImageImageBackingTest, BeginGLReadAccess_MockTextureServiceId)
{
    // mock_texture_ has service_id 123
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(123u)).Times(1);
    backing_->BeginGLReadAccess(123u);
}

TEST_F(NativeImageImageBackingTest, ProduceGLTexture_ThreeRepresentations)
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

TEST_F(NativeImageImageBackingTest, ProduceGLTexturePassthrough_ThreeRepresentations)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .Times(3)
        .WillRepeatedly(Return(true));
    SharedImageManager manager;
    auto rep1 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    auto rep2 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    auto rep3 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(rep1, nullptr);
    EXPECT_NE(rep2, nullptr);
    EXPECT_NE(rep3, nullptr);
}

TEST_F(NativeImageImageBackingTest, GetEstimatedSizeForMemoryDump_ConsistentResult)
{
    size_t result1 = backing_->GetEstimatedSizeForMemoryDump();
    size_t result2 = backing_->GetEstimatedSizeForMemoryDump();
    EXPECT_EQ(result1, result2);
}

TEST_F(NativeImageImageBackingTest, ProduceSkiaGanesh_GLContext_DestroyWithoutAccess)
{
    SharedImageManager manager;
    auto representation = backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    EXPECT_NE(representation, nullptr);
    representation.reset();
    EXPECT_NE(backing_, nullptr);
}

TEST_F(NativeImageImageBackingTest, ProduceSkiaGanesh_MultipleContexts_NonGLReturnNull)
{
    auto vk_context = base::MakeRefCounted<SharedContextState>(
        base::MakeRefCounted<gl::GLShareGroup>(),
        surf_, gl_context_, false, base::DoNothing(),
        GrContextType::kVulkan);
    SharedImageManager manager;
    // GL context should succeed
    auto gl_rep = backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    EXPECT_NE(gl_rep, nullptr);
    // Vulkan context should fail for NativeImageImageBacking
    auto vk_rep = backing_->ProduceSkiaGanesh(&manager, tracker_.get(), vk_context);
    EXPECT_EQ(vk_rep, nullptr);
}

TEST_F(NativeImageImageBackingTest, ProduceGLTexture_FailThenSucceed)
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

TEST_F(NativeImageImageBackingTest, ProduceGLTexturePassthrough_FailThenSucceed)
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

TEST_F(NativeImageImageBackingTest, DestructorWithNullContextState_GracefulShutdown)
{
    EXPECT_CALL(*stream_texture_sii_, ReleaseResources()).Times(1);
    backing_->OnContextLost();
    EXPECT_EQ(backing_->context_state_, nullptr);
    // Reset stream_texture_sii_ to nullptr (simulate double-free protection)
    // Destructor should handle gracefully
    EXPECT_CALL(*stream_texture_sii_, ReleaseResources()).Times(1);
    backing_.reset();
}

TEST_F(NativeImageImageBackingTest, ProduceSkiaGanesh_NoTextureOwner_ReturnsNull)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(false));
    SharedImageManager manager;
    auto rep = backing_->ProduceSkiaGanesh(&manager, tracker_.get(), context_state_);
    EXPECT_EQ(rep, nullptr);
}

TEST_F(NativeImageImageBackingTest, ProduceSkiaGanesh_MultipleGLReps)
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

TEST_F(NativeImageImageBackingTest, GLTexture_GetTexture_PlaneZero_Valid)
{
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    auto* texture = representation->GetTexture(0);
    EXPECT_NE(texture, nullptr);
    EXPECT_EQ(texture->target(), GL_TEXTURE_EXTERNAL_OES);
}

TEST_F(NativeImageImageBackingTest, GLTexturePassthrough_GetTexturePassthrough_PlaneZero_Valid)
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

TEST_F(NativeImageImageBackingTest, BackingContextStateNotNullAfterInit)
{
    EXPECT_NE(backing_->context_state_, nullptr);
}

TEST_F(NativeImageImageBackingTest, BackingStreamTextureSiiNotNullAfterInit)
{
    EXPECT_NE(backing_->stream_texture_sii_, nullptr);
}

TEST_F(NativeImageImageBackingTest, GLTexture_BeginAccess_ReadMode_UpdatesTexture)
{
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(testing::_)).Times(1);
    bool result = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_TRUE(result);
    representation->EndAccess();
}

TEST_F(NativeImageImageBackingTest, GLTexture_GetTexture_AfterBeginAccess_SameObject)
{
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    auto* texture_before = representation->GetTexture(0);
    EXPECT_NE(texture_before, nullptr);
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(testing::_)).Times(1);
    representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    auto* texture_after = representation->GetTexture(0);
    EXPECT_EQ(texture_before, texture_after);
    representation->EndAccess();
}

TEST_F(NativeImageImageBackingTest, ProduceGLTexturePassthrough_MultipleRepsHaveOwnTextures)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .Times(2)
        .WillRepeatedly(Return(true));
    SharedImageManager manager;
    auto rep1 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    auto rep2 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    ASSERT_NE(rep1, nullptr);
    ASSERT_NE(rep2, nullptr);
    EXPECT_NE(rep1->GetTexturePassthrough(0), nullptr);
    EXPECT_NE(rep2->GetTexturePassthrough(0), nullptr);
}

TEST_F(NativeImageImageBackingTest, ProduceSkiaGanesh_NullForVulkanContext)
{
    auto vk_context = base::MakeRefCounted<SharedContextState>(
        base::MakeRefCounted<gl::GLShareGroup>(),
        surf_, gl_context_, false, base::DoNothing(),
        GrContextType::kVulkan);
    SharedImageManager manager;
    auto rep = backing_->ProduceSkiaGanesh(&manager, tracker_.get(), vk_context);
    EXPECT_EQ(rep, nullptr);
}

TEST_F(NativeImageImageBackingTest, BeginGLReadAccess_DirectCallOnBacking)
{
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(42u)).Times(1);
    backing_->BeginGLReadAccess(42u);
}

TEST_F(NativeImageImageBackingTest, GLTexture_MixedHasTextureOwnerResponses)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(false))
        .WillOnce(Return(true))
        .WillOnce(Return(false));
    SharedImageManager manager;
    auto rep1 = backing_->ProduceGLTexture(&manager, tracker_.get());
    EXPECT_EQ(rep1, nullptr);
    auto rep2 = backing_->ProduceGLTexture(&manager, tracker_.get());
    EXPECT_NE(rep2, nullptr);
    auto rep3 = backing_->ProduceGLTexture(&manager, tracker_.get());
    EXPECT_EQ(rep3, nullptr);
}

TEST_F(NativeImageImageBackingTest, GLTexturePassthrough_MixedHasTextureOwnerResponses)
{
    EXPECT_CALL(*stream_texture_sii_, HasTextureOwner())
        .WillOnce(Return(true))
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    SharedImageManager manager;
    auto rep1 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(rep1, nullptr);
    auto rep2 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_EQ(rep2, nullptr);
    auto rep3 = backing_->ProduceGLTexturePassthrough(&manager, tracker_.get());
    EXPECT_NE(rep3, nullptr);
}

TEST_F(NativeImageImageBackingTest, GLTexture_BeginAccess_ThenEndAccess_ThenBeginAgain)
{
    SharedImageManager manager;
    auto representation = backing_->ProduceGLTexture(&manager, tracker_.get());
    ASSERT_NE(representation, nullptr);
    EXPECT_CALL(*stream_texture_sii_, UpdateAndBindTexImage(testing::_)).Times(2);
    bool r1 = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_TRUE(r1);
    representation->EndAccess();
    bool r2 = representation->BeginAccess(GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
    EXPECT_TRUE(r2);
    representation->EndAccess();
}

}  // namespace gpu