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

#include "base/containers/flat_map.h"
#define private public
#include "arkweb/chromium_ext/gpu/command_buffer/service/shared_image/ohos_native_buffer_image_backing_factory.h"
#undef private

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

#include "arkweb/chromium_ext/gpu/command_buffer/service/shared_image/ohos_native_buffer_image_backing_factory.cc"

using namespace gpu;
using testing::_;
using testing::Return;
using testing::NiceMock;

namespace gpu {

// 常量定义
static constexpr int SIZE = 100;

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

class OHOSNativeBufferImageBackingFactoryTest
    : public ::testing::Test {
public:
    OHOSNativeBufferImageBackingFactoryTest() : task_environment_() {}

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
        // backing_ = std::make_unique<NativeImageImageBacking>(mailbox_,
        //     size_, color_space_, surface_origin_, alpha_type_, debug_label_,
        //     stream_texture_sii_, context_state_);
    feature_info_ = base::MakeRefCounted<gles2::FeatureInfo>(
                            gpu_workarounds_, gpu_feature_info_);
    feature_info_->Initialize(ContextType::CONTEXT_TYPE_OPENGLES2,
                           true,
                           gles2::DisallowedFeatures());
    backing_factory_ = std::make_unique<OHOSNativeBufferImageBackingFactory>(
        feature_info_.get(), gpu_preferences_);
    LOG(ERROR) << "backing_factory_->max_gl_texture_size_" << backing_factory_->max_gl_texture_size_;
    }

    void TearDown() override {
        if (gl_context_ && surf_) {
            gl_context_->ReleaseCurrent(surf_.get());
        }
        // backing_.reset();
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
    // std::unique_ptr<NativeImageImageBacking> backing_;
    base::test::TaskEnvironment task_environment_{};
    scoped_refptr<gl::GLSurface> surf_;
    scoped_refptr<gl::GLContext> gl_context_;
    MockTextureBase mock_texture_{123};
    std::unique_ptr<OHOSNativeBufferImageBackingFactory> backing_factory_;
    GpuPreferences gpu_preferences_;
    GpuDriverBugWorkarounds gpu_workarounds_;
    gpu::GpuFeatureInfo gpu_feature_info_;
    scoped_refptr<gpu::gles2::FeatureInfo> feature_info_;
};

TEST_F(OHOSNativeBufferImageBackingFactoryTest, ConvertToNativeBufferFormat) {
  EXPECT_EQ(NATIVEBUFFER_PIXEL_FMT_RGBA_8888,
    ConvertToNativeBufferFormat(viz::SinglePlaneFormat::kRGBA_8888));
  EXPECT_EQ(NATIVEBUFFER_PIXEL_FMT_RGB_565,
    ConvertToNativeBufferFormat(viz::SinglePlaneFormat::kRGB_565));
  EXPECT_EQ(NATIVEBUFFER_PIXEL_FMT_BGR_565,
    ConvertToNativeBufferFormat(viz::SinglePlaneFormat::kBGR_565));
  EXPECT_EQ(NATIVEBUFFER_PIXEL_FMT_RGBA16_FLOAT,
    ConvertToNativeBufferFormat(viz::SinglePlaneFormat::kRGBA_F16));
  EXPECT_EQ(NATIVEBUFFER_PIXEL_FMT_RGBX_8888,
    ConvertToNativeBufferFormat(viz::SinglePlaneFormat::kRGBX_8888));
  EXPECT_EQ(NATIVEBUFFER_PIXEL_FMT_RGBA_1010102,
    ConvertToNativeBufferFormat(viz::SinglePlaneFormat::kRGBA_1010102));
  EXPECT_EQ(NATIVEBUFFER_PIXEL_FMT_RGBA_8888,
    ConvertToNativeBufferFormat(viz::SinglePlaneFormat::kALPHA_8));
}

TEST_F(OHOSNativeBufferImageBackingFactoryTest, FormatInfoForSupportedFormat) {
  EXPECT_EQ(true, gl::g_current_gl_driver->ext.b_GL_OES_EGL_image);
  gl::g_current_gl_driver->ext.b_GL_OES_EGL_image = false;
  backing_factory_->FormatInfoForSupportedFormat(viz::SinglePlaneFormat::kRGBA_8888,
    feature_info_->validators(), GLFormatCaps(feature_info_.get()));
  gl::g_current_gl_driver->ext.b_GL_OES_EGL_image = true;
  backing_factory_->FormatInfoForSupportedFormat(viz::SinglePlaneFormat::kRGBA_8888,
    feature_info_->validators(), GLFormatCaps(feature_info_.get()));
  backing_factory_->FormatInfoForSupportedFormat(viz::SinglePlaneFormat::kBGR_565,
    feature_info_->validators(), GLFormatCaps(feature_info_.get()));
  backing_factory_->FormatInfoForSupportedFormat(viz::SinglePlaneFormat::kRGBX_8888,
    feature_info_->validators(), GLFormatCaps(feature_info_.get()));
  backing_factory_->FormatInfoForSupportedFormat(viz::SinglePlaneFormat::kRGB_565,
    feature_info_->validators(), GLFormatCaps(feature_info_.get()));
  backing_factory_->FormatInfoForSupportedFormat(viz::SinglePlaneFormat::kRGBA_F16,
    feature_info_->validators(), GLFormatCaps(feature_info_.get()));
  auto info  = backing_factory_->FormatInfoForSupportedFormat(viz::SinglePlaneFormat::kRGBA_1010102,
    feature_info_->validators(), GLFormatCaps(feature_info_.get()));
  EXPECT_EQ(info.nb_format, 0);
}

TEST_F(OHOSNativeBufferImageBackingFactoryTest, ValidateUsage) {
  EXPECT_FALSE(backing_factory_->ValidateUsage(kSupportedUsage, size_,
    viz::SinglePlaneFormat::kALPHA_8));
  EXPECT_FALSE(backing_factory_->ValidateUsage(kSupportedUsage, gfx::Size(),
    viz::SinglePlaneFormat::kRGBA_8888));
  EXPECT_FALSE(backing_factory_->ValidateUsage(kSupportedUsage, gfx::Size(1, 0),
    viz::SinglePlaneFormat::kRGBA_8888));
  backing_factory_->max_gl_texture_size_ = SIZE;
  EXPECT_FALSE(backing_factory_->ValidateUsage(kSupportedUsage, gfx::Size(SIZE+1, SIZE),
    viz::SinglePlaneFormat::kRGBA_8888));
  EXPECT_FALSE(backing_factory_->ValidateUsage(kSupportedUsage, gfx::Size(SIZE, SIZE+1),
    viz::SinglePlaneFormat::kRGBA_8888));
  EXPECT_TRUE(backing_factory_->ValidateUsage(kSupportedUsage, gfx::Size(SIZE, SIZE),
    viz::SinglePlaneFormat::kRGBA_8888));
}

TEST_F(OHOSNativeBufferImageBackingFactoryTest, MakeBacking) {
  const uint8_t kData[] = "HelloWorld";
  base::span<const uint8_t> data(kData);
  EXPECT_EQ(nullptr, backing_factory_->MakeBacking(mailbox_,viz::SinglePlaneFormat::kALPHA_8,
            size_, color_space_, surface_origin_, alpha_type_, kSupportedUsage,
            debug_label_, false, data));
  backing_factory_->MakeBacking(mailbox_,viz::SinglePlaneFormat::kRGBA_8888,
            gfx::Size(std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::max()),
            color_space_, surface_origin_, alpha_type_, kSupportedUsage,
            debug_label_, false, data);
  backing_factory_->MakeBacking(mailbox_,viz::SinglePlaneFormat::kRGBA_8888,
            gfx::Size(SIZE, SIZE), color_space_, surface_origin_, alpha_type_, kSupportedUsage,
            debug_label_, false, data);
  backing_factory_->MakeBacking(mailbox_,viz::SinglePlaneFormat::kRGBA_8888,
            gfx::Size(SIZE, SIZE), color_space_, surface_origin_, alpha_type_, kSupportedUsage,
            debug_label_, false, base::span<const uint8_t>());
}

TEST_F(OHOSNativeBufferImageBackingFactoryTest, IsSupported) {
  const uint8_t kData[] = "HelloWorld";
  base::span<const uint8_t> data(kData);
  EXPECT_FALSE(backing_factory_->IsSupported(kSupportedUsage, viz::SinglePlaneFormat::kRGBA_8888,
    gfx::Size(), false, gfx::NATIVE_PIXMAP, GrContextType::kGL, data));
  EXPECT_FALSE(backing_factory_->IsSupported(kSupportedUsage, viz::SinglePlaneFormat::kRGBA_8888,
    gfx::Size(), false, gfx::IO_SURFACE_BUFFER, GrContextType::kGL, data));
  EXPECT_FALSE(backing_factory_->IsSupported(kSupportedUsage, viz::MultiPlaneFormat::kYV12,
    gfx::Size(), false, gfx::IO_SURFACE_BUFFER, GrContextType::kGL, data));
  EXPECT_TRUE(backing_factory_->IsSupported(kSupportedUsage, viz::SinglePlaneFormat::kRGBA_8888,
    gfx::Size(), false, gfx::EMPTY_BUFFER, GrContextType::kGL, data));
  EXPECT_FALSE(backing_factory_->IsSupported(kSupportedUsage, viz::SinglePlaneFormat::kRGBA_8888,
    gfx::Size(), false, gfx::IO_SURFACE_BUFFER, GrContextType::kGL, data));
  EXPECT_FALSE(backing_factory_->IsSupported(kSupportedUsage, viz::SinglePlaneFormat::kALPHA_8,
    gfx::Size(), false, gfx::OHOS_NATIVE_BUFFER, GrContextType::kGL, data));
  EXPECT_TRUE(backing_factory_->IsSupported(kSupportedUsage, viz::SinglePlaneFormat::kRGBA_8888,
    gfx::Size(), false, gfx::OHOS_NATIVE_BUFFER, GrContextType::kGL, data));

  EXPECT_FALSE(backing_factory_->IsSupported(kSupportedUsage, viz::SinglePlaneFormat::kALPHA_8,
    gfx::Size(), false, gfx::OHOS_NATIVE_BUFFER, GrContextType::kGL, data));
}
}  // namespace gpu
