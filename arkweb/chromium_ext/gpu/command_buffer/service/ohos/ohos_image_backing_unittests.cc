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
#include "gpu/command_buffer/service/ohos/ohos_image_backing.h"
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
#include "gpu/config/gpu_finch_features.h"

using namespace gpu;
using testing::_;
using testing::Return;
namespace gpu {
constexpr int SIZE = 256;

class MockSharedImageRepresentation : public SharedImageRepresentation {
public:
    MockSharedImageRepresentation(SharedImageManager* manager,
                                    SharedImageBacking* backing,
                                    MemoryTypeTracker* tracker)
        : SharedImageRepresentation(manager, backing, tracker) {}
    ~MockSharedImageRepresentation() override = default;
};

class MockOhosImageBacking : public OhosImageBacking {
public:
    MockOhosImageBacking(const Mailbox& mailbox,
                         viz::SharedImageFormat format,
                         const gfx::Size& size,
                         const gfx::ColorSpace& color_space,
                         GrSurfaceOrigin surface_origin,
                         SkAlphaType alpha_type,
                         SharedImageUsageSet usage,
                         std::string debug_label,
                         size_t estimated_size,
                         bool is_thread_safe,
                         base::ScopedFD initial_upload_fd)
        : OhosImageBacking(mailbox, format, size, color_space,
                           surface_origin, alpha_type, usage, debug_label,
                           estimated_size, is_thread_safe, std::move(initial_upload_fd)) {}
    
    SharedImageBackingType GetType() const override {
        return SharedImageBackingType::kTest;
    }

    void SetOverlayAccessing(bool accessing) {
        AutoLock auto_lock(this);
        is_overlay_accessing_ = accessing;
    }

    bool IsUsingVulkan() const {
        return features::IsUsingVulkan();
    }

    void SetWriteSyncFd(base::ScopedFD fd) {
        AutoLock auto_lock(this);
        write_sync_fd_ = std::move(fd);
    }

    bool HasWriteSyncFd() const {
        AutoLock auto_lock(this);
        return write_sync_fd_.is_valid();
    }

    void SetReadSyncFd(base::ScopedFD fd) {
        AutoLock auto_lock(this);
        read_sync_fd_ = std::move(fd);
    }

    bool HasReadSyncFd() const {
        AutoLock auto_lock(this);
        return read_sync_fd_.is_valid();
    }
};

class OhosImageBackingTest : public ::testing::Test {
public:
    OhosImageBackingTest() : task_environment_() {}
    void SetUp() override {
        gl::init::InitializeGLNoExtensionsOneOff(
            true, gl::GpuPreference::kDefault);
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
        SharedImageUsageSet usage = {
            SHARED_IMAGE_USAGE_GLES2_READ,   SHARED_IMAGE_USAGE_GLES2_WRITE,
            SHARED_IMAGE_USAGE_RASTER_READ,  SHARED_IMAGE_USAGE_RASTER_WRITE,
            SHARED_IMAGE_USAGE_WEBGPU_WRITE, SHARED_IMAGE_USAGE_SCANOUT};
        
        backing_ = std::make_unique<MockOhosImageBacking>(
            mailbox_, format, size, color_space, surface_origin, alpha_type, usage, debug_label,
            estimated_size, is_thread_safe, base::ScopedFD());
    }

    void TearDown() override {
        backing_.reset();
    }

protected:
    gpu::Mailbox mailbox_;
    std::unique_ptr<MemoryTypeTracker> tracker_;
    std::unique_ptr<MockOhosImageBacking> backing_;
    base::test::TaskEnvironment task_environment_{};
};

class OhosImageBackingVulkanTest : public OhosImageBackingTest {
public:
    OhosImageBackingVulkanTest() = default;

    void SetUp() override {
        scoped_feature_list_.InitAndEnableFeature(features::kVulkan);
        OhosImageBackingTest::SetUp();
    }

private:
    base::test::ScopedFeatureList scoped_feature_list_;
};

class OhosImageBackingNoVulkanTest : public OhosImageBackingTest {
public:
    OhosImageBackingNoVulkanTest() = default;

    void SetUp() override {
        scoped_feature_list_.InitAndDisableFeature(features::kVulkan);
        OhosImageBackingTest::SetUp();
    }

private:
    base::test::ScopedFeatureList scoped_feature_list_;
};

TEST_F(OhosImageBackingTest, BasicBeginWrite) {
    base::ScopedFD fd_to_wait_on;
    bool result = backing_->BeginWrite(&fd_to_wait_on);
    EXPECT_TRUE(result);
}

TEST_F(OhosImageBackingTest, BasicEndWrite) {
    base::ScopedFD fd_to_wait_on;
    backing_->BeginWrite(&fd_to_wait_on);
    int fds[2];
    ASSERT_EQ(0, pipe(fds));
    base::ScopedFD end_write_fd(fds[0]);
    close(fds[1]);
    backing_->EndWrite(std::move(end_write_fd));
}

TEST_F(OhosImageBackingTest, BasicBeginRead) {
    MockSharedImageRepresentation representation(nullptr, backing_.get(), tracker_.get());
    base::ScopedFD fd_to_wait_on;
    bool result = backing_->BeginRead(&representation, &fd_to_wait_on);
    EXPECT_TRUE(result);
}

TEST_F(OhosImageBackingTest, BasicEndRead) {
    MockSharedImageRepresentation representation(nullptr, backing_.get(), tracker_.get());
    base::ScopedFD fd_to_wait_on;
    backing_->BeginRead(&representation, &fd_to_wait_on);
    int fds[2];
    ASSERT_EQ(0, pipe(fds));
    base::ScopedFD end_read_fd(fds[0]);
    close(fds[1]);
    backing_->EndRead(&representation, std::move(end_read_fd));
}

TEST_F(OhosImageBackingTest, WriteBlocksRead) {
    base::ScopedFD fd_to_wait_on;
    backing_->BeginWrite(&fd_to_wait_on);
    MockSharedImageRepresentation representation(nullptr, backing_.get(), tracker_.get());
    bool result = backing_->BeginRead(&representation, &fd_to_wait_on);
    EXPECT_FALSE(result);
}

TEST_F(OhosImageBackingTest, ReadBlocksWrite) {
    MockSharedImageRepresentation representation(nullptr, backing_.get(), tracker_.get());
    base::ScopedFD fd_to_wait_on;
    backing_->BeginRead(&representation, &fd_to_wait_on);
    bool result = backing_->BeginWrite(&fd_to_wait_on);
    EXPECT_FALSE(result);
}

TEST_F(OhosImageBackingTest, MultipleReads) {
    MockSharedImageRepresentation representation1(nullptr, backing_.get(), tracker_.get());
    MockSharedImageRepresentation representation2(nullptr, backing_.get(), tracker_.get());
    base::ScopedFD fd_to_wait_on;
    bool result1 = backing_->BeginRead(&representation1, &fd_to_wait_on);
    bool result2 = backing_->BeginRead(&representation2, &fd_to_wait_on);
    EXPECT_TRUE(result1);
    EXPECT_TRUE(result2);
    int fds[2];
    ASSERT_EQ(0, pipe(fds));
    base::ScopedFD end_read_fd(fds[0]);
    close(fds[1]);
    backing_->EndRead(&representation1, std::move(end_read_fd));
    ASSERT_EQ(0, pipe(fds));
    base::ScopedFD end_read_fd2(fds[0]);
    close(fds[1]);
    backing_->EndRead(&representation2, std::move(end_read_fd2));
}

TEST_F(OhosImageBackingTest, BeginWrite_FailWhenOverlayAccessing) {
    backing_->SetOverlayAccessing(true);
    base::ScopedFD fd_to_wait_on;
    bool result = backing_->BeginWrite(&fd_to_wait_on);
    EXPECT_FALSE(result);
}

TEST_F(OhosImageBackingVulkanTest, BeginWrite_VulkanReturnsEmptyFd) {
    base::ScopedFD fd_to_wait_on;
    bool result = backing_->BeginWrite(&fd_to_wait_on);
    EXPECT_TRUE(result);
    EXPECT_FALSE(fd_to_wait_on.is_valid());
}

TEST_F(OhosImageBackingNoVulkanTest, BeginWrite_NonVulkanReturnsMergedFd) {
    int fds1[2], fds2[2];
    ASSERT_EQ(0, pipe(fds1));
    ASSERT_EQ(0, pipe(fds2));
    base::ScopedFD read_fd(fds1[0]);
    base::ScopedFD write_fd(fds2[0]);
    close(fds1[1]);
    close(fds2[1]);
    backing_->SetReadSyncFd(std::move(read_fd));
    backing_->SetWriteSyncFd(std::move(write_fd));
    base::ScopedFD fd_to_wait_on;
    bool result = backing_->BeginWrite(&fd_to_wait_on);
    EXPECT_TRUE(result);
}

TEST_F(OhosImageBackingVulkanTest, BeginRead_VulkanWaitsAndReturnsEmptyFd) {
    int fds[2];
    ASSERT_EQ(0, pipe(fds));
    base::ScopedFD write_fd(fds[0]);
    close(fds[1]);
    backing_->SetWriteSyncFd(std::move(write_fd));
    MockSharedImageRepresentation representation(nullptr, backing_.get(), tracker_.get());
    base::ScopedFD fd_to_wait_on;
    bool result = backing_->BeginRead(&representation, &fd_to_wait_on);
    EXPECT_TRUE(result);
}

TEST_F(OhosImageBackingNoVulkanTest, BeginRead_NonVulkanReturnsValidWriteFd) {
    int fds[2];
    ASSERT_EQ(0, pipe(fds));
    base::ScopedFD write_fd(fds[0]);
    close(fds[1]);
    backing_->SetWriteSyncFd(std::move(write_fd));
    MockSharedImageRepresentation representation(nullptr, backing_.get(), tracker_.get());
    base::ScopedFD fd_to_wait_on;
    bool result = backing_->BeginRead(&representation, &fd_to_wait_on);
    EXPECT_TRUE(result);
    EXPECT_TRUE(fd_to_wait_on.is_valid());
}

TEST_F(OhosImageBackingNoVulkanTest, BeginRead_NonVulkanReturnsEmptyFdWhenWriteFdInvalid) {
    backing_->SetWriteSyncFd(base::ScopedFD());
    MockSharedImageRepresentation representation(nullptr, backing_.get(), tracker_.get());
    base::ScopedFD fd_to_wait_on;
    bool result = backing_->BeginRead(&representation, &fd_to_wait_on);
    EXPECT_TRUE(result);
    EXPECT_FALSE(fd_to_wait_on.is_valid());
}

TEST_F(OhosImageBackingTest, EndRead_MergesReadFences) {
    MockSharedImageRepresentation representation(nullptr, backing_.get(), tracker_.get());
    base::ScopedFD fd_to_wait_on;
    backing_->BeginRead(&representation, &fd_to_wait_on);
    int fds1[2];
    ASSERT_EQ(0, pipe(fds1));
    base::ScopedFD end_read_fd1(fds1[0]);
    close(fds1[1]);
    backing_->EndRead(&representation, std::move(end_read_fd1));
    int fds2[2];
    ASSERT_EQ(0, pipe(fds2));
    base::ScopedFD end_read_fd2(fds2[0]);
    close(fds2[1]);
    backing_->EndRead(&representation, std::move(end_read_fd2));
    base::ScopedFD result = backing_->TakeReadFence();
    EXPECT_TRUE(result.is_valid());
}

TEST_F(OhosImageBackingTest, BeginWrite_FailWhenActiveReaders) {
    MockSharedImageRepresentation representation(nullptr, backing_.get(), tracker_.get());
    base::ScopedFD fd_to_wait_on;
    backing_->BeginRead(&representation, &fd_to_wait_on);
    base::ScopedFD write_fd;
    bool result = backing_->BeginWrite(&write_fd);
    EXPECT_FALSE(result);
    int fds[2];
    ASSERT_EQ(0, pipe(fds));
    base::ScopedFD end_read_fd(fds[0]);
    close(fds[1]);
    backing_->EndRead(&representation, std::move(end_read_fd));
}

TEST_F(OhosImageBackingTest, BeginRead_FailWhenActiveWriter) {
    base::ScopedFD write_fd;
    backing_->BeginWrite(&write_fd);
    MockSharedImageRepresentation representation(nullptr, backing_.get(), tracker_.get());
    base::ScopedFD read_fd;
    bool result = backing_->BeginRead(&representation, &read_fd);
    EXPECT_FALSE(result);
    int fds[2];
    ASSERT_EQ(0, pipe(fds));
    base::ScopedFD end_write_fd(fds[0]);
    close(fds[1]);
    backing_->EndWrite(std::move(end_write_fd));
}

TEST_F(OhosImageBackingTest, BeginRead_FailWhenSameReaderCallsTwice) {
    MockSharedImageRepresentation representation(nullptr, backing_.get(), tracker_.get());
    base::ScopedFD fd_to_wait_on;
    bool result1 = backing_->BeginRead(&representation, &fd_to_wait_on);
    EXPECT_TRUE(result1);
    bool result2 = backing_->BeginRead(&representation, &fd_to_wait_on);
    EXPECT_FALSE(result2);
    int fds[2];
    ASSERT_EQ(0, pipe(fds));
    base::ScopedFD end_read_fd(fds[0]);
    close(fds[1]);
    backing_->EndRead(&representation, std::move(end_read_fd));
}

TEST_F(OhosImageBackingTest, EndWrite_FailWhenNoActiveWriter) {
    int fds[2];
    ASSERT_EQ(0, pipe(fds));
    base::ScopedFD end_write_fd(fds[0]);
    close(fds[1]);
    backing_->EndWrite(std::move(end_write_fd));
}

TEST_F(OhosImageBackingTest, EndRead_FailWhenNoActiveReader) {
    MockSharedImageRepresentation representation(nullptr, backing_.get(), tracker_.get());
    int fds[2];
    ASSERT_EQ(0, pipe(fds));
    base::ScopedFD end_read_fd(fds[0]);
    close(fds[1]);
    backing_->EndRead(&representation, std::move(end_read_fd));
}

}  // namespace gpu