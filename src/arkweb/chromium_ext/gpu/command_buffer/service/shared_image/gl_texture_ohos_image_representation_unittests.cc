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
#include "gpu/command_buffer/service/shared_image/ohos_native_buffer_image_backing.h"
#include "gpu/command_buffer/service/shared_image/gl_texture_ohos_image_representation.h"
#undef private
#undef protected

#include "base/ohos/scoped_native_buffer_handle.h"
#include "base/functional/callback_helpers.h"
#include "base/memory/raw_ptr.h"
#include "base/posix/eintr_wrapper.h"
#include "base/test/scoped_feature_list.h"
#include "base/test/test_mock_time_task_runner.h"
#include "base/test/task_environment.h"
#include "gpu/command_buffer/service/ohos/ohos_image_backing.h"
#include "gpu/command_buffer/common/mailbox.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "gpu/command_buffer/service/shared_context_state.h"
#include "gpu/command_buffer/service/shared_image/shared_image_backing.h"
#include "gpu/command_buffer/service/shared_image/test_image_backing.h"
#include "gpu/command_buffer/service/texture_manager.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "third_party/dawn/include/dawn/dawn_proc.h"
#include "third_party/skia/include/gpu/ganesh/GrBackendSurface.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_surface.h"
#include "ui/gl/gl_utils.h"
#include "ui/gl/init/gl_factory.h"
#include "ui/gl/test/gl_surface_test_support.h"
#include "ui/gl/test/gl_test_support.h"

using namespace gpu;
using testing::_;
using testing::Return;

namespace gpu {
    
constexpr int SIZE = 256;

class MockGLTextureOhosImageRepresentation : public GLTextureOhosImageRepresentation {
public:
    MockGLTextureOhosImageRepresentation(
        SharedImageManager* manager,
        OhosNativeBufferImageBacking* backing,
        MemoryTypeTracker* tracker)
        : GLTextureOhosImageRepresentation(manager, backing, tracker,
                                           gl::ScopedEGLImage(), nullptr) {}
};

class MockOhosNativeBufferImageBacking : public OhosNativeBufferImageBacking {
public:
    MockOhosNativeBufferImageBacking(const Mailbox& mailbox,
                                     viz::SharedImageFormat format,
                                     const gfx::Size& size,
                                     const gfx::ColorSpace& color_space,
                                     GrSurfaceOrigin surface_origin,
                                     SkAlphaType alpha_type,
                                     SharedImageUsageSet usage,
                                     std::string debug_label,
                                     gpu::ScopedNativeBufferHandle handle,
                                     size_t estimated_size,
                                     bool is_thread_safe,
                                     base::ScopedFD initial_upload_fd,
                                     bool use_passthrough,
                                     const GLFormatCaps& gl_format_caps)
        : OhosNativeBufferImageBacking(mailbox, format, size, color_space,
                                       surface_origin, alpha_type, usage, debug_label,
                                       std::move(handle), estimated_size, is_thread_safe,
                                       std::move(initial_upload_fd), use_passthrough, gl_format_caps) {}

    MOCK_METHOD(bool, BeginRead, (const SharedImageRepresentation*, base::ScopedFD*), (override));
    MOCK_METHOD(bool, BeginWrite, (base::ScopedFD*), (override));
    using OhosNativeBufferImageBacking::ProduceGLTexture;

    std::unique_ptr<GLTextureImageRepresentation> ProduceGLTexture(
        SharedImageManager* manager,
        MemoryTypeTracker* tracker)
    {
        return std::make_unique<MockGLTextureOhosImageRepresentation>(
            manager, this, tracker);
    }
};


class GLTextureOhosImageRepresentationTest : public ::testing::Test {
public:
    GLTextureOhosImageRepresentationTest() : task_environment_() {}
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
        auto format = viz::SinglePlaneFormat::kRGBA_8888;
        gfx::Size size(SIZE, SIZE);
        auto color_space = gfx::ColorSpace::CreateSRGB();
        auto surface_origin = kTopLeft_GrSurfaceOrigin;
        auto alpha_type = kPremul_SkAlphaType;
        std::string debug_label = "test";
        size_t estimated_size = 0;
        bool is_thread_safe = true;
        gpu::ScopedNativeBufferHandle handle = ScopedNativeBufferHandle();
        bool use_passthrough = true;
        GLFormatCaps gl_format_caps;
        SharedImageUsageSet usage = {
            SHARED_IMAGE_USAGE_GLES2_READ,   SHARED_IMAGE_USAGE_GLES2_WRITE,
            SHARED_IMAGE_USAGE_RASTER_READ,  SHARED_IMAGE_USAGE_RASTER_WRITE,
            SHARED_IMAGE_USAGE_WEBGPU_WRITE, SHARED_IMAGE_USAGE_SCANOUT};
        auto backing = std::make_unique<MockOhosNativeBufferImageBacking>(
            mailbox_, format, size, color_space, surface_origin, alpha_type, usage, debug_label,
            std::move(handle), estimated_size, is_thread_safe, base::ScopedFD(), use_passthrough, gl_format_caps);
        mock_backing_ = backing.get();
        factory_ref_ = manager_.Register(std::move(backing), tracker_.get());
    }

    void TearDown() override {}

protected:
    gpu::Mailbox mailbox_;
    SharedImageManager manager_;
    std::unique_ptr<MemoryTypeTracker> tracker_;
    std::unique_ptr<SharedImageRepresentationFactoryRef> factory_ref_;
    raw_ptr<MockOhosNativeBufferImageBacking> mock_backing_;
    base::test::TaskEnvironment task_environment_{};
};

TEST_F(GLTextureOhosImageRepresentationTest, BeginAccessReadOnlySuccess)
{
    auto representation = mock_backing_->ProduceGLTexture(&manager_, tracker_.get());
    ASSERT_NE(representation, nullptr);

    // 设置BeginRead成功
    EXPECT_CALL(*mock_backing_, BeginRead(_, _))
        .WillOnce(Return(true));

    GLenum mode = GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM;
    bool result = representation->BeginAccess(mode);
    EXPECT_TRUE(result);
}

TEST_F(GLTextureOhosImageRepresentationTest, BeginAccessReadWriteSuccess)
{
    auto representation = mock_backing_->ProduceGLTexture(&manager_, tracker_.get());
    ASSERT_NE(representation, nullptr);

    // 设置BeginWrite成功
    EXPECT_CALL(*mock_backing_, BeginWrite(_))
        .WillOnce(Return(true));

    GLenum mode = GL_SHARED_IMAGE_ACCESS_MODE_READWRITE_CHROMIUM;
    bool result = representation->BeginAccess(mode);
    EXPECT_TRUE(result);
}

TEST_F(GLTextureOhosImageRepresentationTest, BeginAccessReadOnlyFails)
{
    auto representation = mock_backing_->ProduceGLTexture(&manager_, tracker_.get());
    ASSERT_NE(representation, nullptr);

    // 设置BeginRead失败
    EXPECT_CALL(*mock_backing_, BeginRead(_, _))
        .WillOnce(Return(false));

    GLenum mode = GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM;
    bool result = representation->BeginAccess(mode);
    EXPECT_FALSE(result);
}

TEST_F(GLTextureOhosImageRepresentationTest, BeginAccessReadWriteFails)
{
    auto representation = mock_backing_->ProduceGLTexture(&manager_, tracker_.get());
    ASSERT_NE(representation, nullptr);

    // 设置BeginWrite失败
    EXPECT_CALL(*mock_backing_, BeginWrite(_))
        .WillOnce(Return(false));

    GLenum mode = GL_SHARED_IMAGE_ACCESS_MODE_READWRITE_CHROMIUM;
    bool result = representation->BeginAccess(mode);
    EXPECT_FALSE(result);
}

TEST_F(GLTextureOhosImageRepresentationTest, BeginAccessInvalidMode)
{
    auto representation = mock_backing_->ProduceGLTexture(&manager_, tracker_.get());
    ASSERT_NE(representation, nullptr);

    // 无效访问模式
    GLenum mode = 0;
    bool result = representation->BeginAccess(mode);
    EXPECT_FALSE(result);
}

TEST_F(GLTextureOhosImageRepresentationTest, EndAccessReadMode)
{
    auto representation = mock_backing_->ProduceGLTexture(&manager_, tracker_.get());
    ASSERT_NE(representation, nullptr);

    EXPECT_CALL(*mock_backing_, BeginRead(_, _))
        .WillOnce(Return(true));

    representation->EndAccess();
    GLenum mode = GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM;
    representation->BeginAccess(mode);
    representation->EndAccess();
}

TEST_F(GLTextureOhosImageRepresentationTest, EndAccessWriteMode)
{
    auto representation = mock_backing_->ProduceGLTexture(&manager_, tracker_.get());
    ASSERT_NE(representation, nullptr);

    EXPECT_CALL(*mock_backing_, BeginWrite(_))
        .WillOnce(Return(true));

    GLenum mode = GL_SHARED_IMAGE_ACCESS_MODE_READWRITE_CHROMIUM;
    representation->BeginAccess(mode);
    representation->EndAccess();
}
}  // namespace gpu