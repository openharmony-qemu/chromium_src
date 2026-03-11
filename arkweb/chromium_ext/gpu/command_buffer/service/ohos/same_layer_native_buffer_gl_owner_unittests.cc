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

#include "base/containers/circular_deque.h"
#define private public
#define protected public
#include "gpu/command_buffer/service/ohos/same_layer_native_buffer_gl_owner.h"
#undef private
#undef protected

#include <memory>
#include <vector>
#include <thread>

#include "base/check_op.h"
#include "base/debug/alias.h"
#include "base/debug/dump_without_crashing.h"
#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/memory/ptr_util.h"
#include "base/memory/scoped_refptr.h"
#include "base/metrics/histogram_macros.h"
#include "base/notreached.h"
#include "base/posix/eintr_wrapper.h"
#include "base/synchronization/waitable_event.h"
#include "base/task/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"
#include "gmock/gmock.h"
#include "gpu/command_buffer/service/abstract_texture_ohos.h"
#include "gpu/config/gpu_finch_features.h"
#include "gpu/ipc/service/gpu_channel.h"
#include "gpu/ipc/service/gpu_channel_manager.h"
#include "gpu/ipc/service/gpu_channel_test_common.h"
#include "media/base/scoped_async_trace.h"
#include "scoped_native_buffer_fence_sync.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/scoped_binders.h"
#include "ui/gl/scoped_make_current.h"

using namespace gpu;
namespace gpu {
constexpr int WIDTH = 100;
constexpr int HEIGHT = 200;
constexpr int IMAGE_BUFFER_SIZE = 3;
constexpr int IMAGE_COUNT = 2;
namespace {
std::unique_ptr<AbstractTextureOHOS> CreateTexture(
    SharedContextState* context_state)
{
    DCHECK(context_state);
    gles2::FeatureInfo* feature_info = context_state->feature_info();
    if (feature_info && feature_info->is_passthrough_cmd_decoder()) {
        return AbstractTextureOHOS::CreateForPassthrough(gfx::Size());
    }

    return AbstractTextureOHOS::CreateForValidating(gfx::Size());
}
}  // namespace

class SameLayerNativeBufferGLOwnerTest : public GpuChannelTestCommon {
public:
    SameLayerNativeBufferGLOwnerTest()
        : GpuChannelTestCommon(true /* use_stub_bindings */) {}
    ~SameLayerNativeBufferGLOwnerTest() override = default;
    
    void SetUp() override {
        int32_t kClientId = 1;
        bool is_gpu_host = false;
        GpuChannel* channel = CreateChannel(kClientId, is_gpu_host);
        ContextResult result;
        auto context_state =
            channel->gpu_channel_manager()->GetSharedContextState(&result);
        auto texture = CreateTexture(context_state.get());
        scoped_refptr<RefCountedLock> drdc_lock;
        buffer_gl_owner_ = base::MakeRefCounted<gpu::SameLayerNativeBufferGLOwner>(
            std::move(texture), std::move(context_state), drdc_lock);
    }

    void TearDown() override {
        {
            base::AutoLock lock(buffer_gl_owner_->lock_);
            for (auto& entry : buffer_gl_owner_->image_refs_) {
                delete entry.first;
            }
            buffer_gl_owner_->image_refs_.clear();
        }
        buffer_gl_owner_ = nullptr;
        GpuChannelTestCommon::TearDown();
    }

    scoped_refptr<gpu::SameLayerNativeBufferGLOwner> buffer_gl_owner_;
};

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseResourcesWhenLoaderIsNull)
{
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->loader_ = nullptr;
    }
    buffer_gl_owner_->ReleaseResources();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_EQ(buffer_gl_owner_->loader_, nullptr);
        EXPECT_TRUE(buffer_gl_owner_->image_refs_.empty());
        EXPECT_FALSE(buffer_gl_owner_->current_image_ref_.has_value());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseResourcesWhenLoaderNotNullAndEmptyImageRefs)
{
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_NE(buffer_gl_owner_->loader_, nullptr);
        buffer_gl_owner_->image_refs_.clear();
        buffer_gl_owner_->current_image_ref_.reset();
    }
    buffer_gl_owner_->ReleaseResources();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_EQ(buffer_gl_owner_->loader_, nullptr);
        EXPECT_TRUE(buffer_gl_owner_->image_refs_.empty());
        EXPECT_FALSE(buffer_gl_owner_->current_image_ref_.has_value());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseResourcesWithImageRefsAndCurrentImage)
{
    OhosWindowBuffer* image1 = new OhosWindowBuffer();
    OhosWindowBuffer* image2 = new OhosWindowBuffer();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->image_refs_[image1] = SameLayerNativeBufferGLOwner::ImageRef();
        buffer_gl_owner_->image_refs_[image1].count = 1;
        buffer_gl_owner_->image_refs_[image2] = SameLayerNativeBufferGLOwner::ImageRef();
        buffer_gl_owner_->image_refs_[image2].count = IMAGE_COUNT;
        base::ScopedFD test_fd(HANDLE_EINTR(dup(1)));
        buffer_gl_owner_->current_image_ref_.emplace(
            buffer_gl_owner_.get(), image1, std::move(test_fd));
    }
    buffer_gl_owner_->ReleaseResources();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_EQ(buffer_gl_owner_->loader_, nullptr);
        EXPECT_TRUE(buffer_gl_owner_->image_refs_.empty());
        EXPECT_FALSE(buffer_gl_owner_->current_image_ref_.has_value());
    }
    delete image1;
    delete image2;
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseResourcesAfterContextLoss)
{
    OhosWindowBuffer* image = new OhosWindowBuffer();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->image_refs_[image] = SameLayerNativeBufferGLOwner::ImageRef();
        buffer_gl_owner_->image_refs_[image].count = 1;
    }
    buffer_gl_owner_->ReleaseResources();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_EQ(buffer_gl_owner_->loader_, nullptr);
        EXPECT_TRUE(buffer_gl_owner_->image_refs_.empty());
        EXPECT_FALSE(buffer_gl_owner_->current_image_ref_.has_value());
    }
    delete image;
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseResourcesWithPendingBufferAvailableCallback)
{
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        for (int i = 0; i < IMAGE_BUFFER_SIZE; ++i) {
            OhosWindowBuffer* image = new OhosWindowBuffer();
            buffer_gl_owner_->image_refs_[image].count = 1;
        }
        bool callback_called = false;
        buffer_gl_owner_->buffer_available_cb_ = base::BindOnce(
            [](bool* flag) { *flag = true; }, &callback_called);
    }
    buffer_gl_owner_->ReleaseResources();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_EQ(buffer_gl_owner_->loader_, nullptr);
        EXPECT_TRUE(buffer_gl_owner_->image_refs_.empty());
        EXPECT_FALSE(buffer_gl_owner_->current_image_ref_.has_value());
        EXPECT_FALSE(buffer_gl_owner_->buffer_available_cb_.is_null());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseResourcesWithFenceFDs)
{
    OhosWindowBuffer* image = new OhosWindowBuffer();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        auto& image_ref = buffer_gl_owner_->image_refs_[image];
        image_ref.count = 1;
        image_ref.release_fence_fd = base::ScopedFD(HANDLE_EINTR(dup(1)));
    }
    buffer_gl_owner_->ReleaseResources();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_EQ(buffer_gl_owner_->loader_, nullptr);
        EXPECT_TRUE(buffer_gl_owner_->image_refs_.empty());
    }
    delete image;
}

TEST_F(SameLayerNativeBufferGLOwnerTest, UpdateNativeImageWhenTextureInvalid)
{
    buffer_gl_owner_->texture_ = nullptr;
    bool had_image_before = false;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        had_image_before = buffer_gl_owner_->current_image_ref_.has_value();
    }
    buffer_gl_owner_->UpdateNativeImage();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_EQ(buffer_gl_owner_->current_image_ref_.has_value(), had_image_before);
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, UpdateNativeImageWhenLoaderNull)
{
    bool had_image_before = false;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        had_image_before = buffer_gl_owner_->current_image_ref_.has_value();
    }
    buffer_gl_owner_->UpdateNativeImage();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_EQ(buffer_gl_owner_->current_image_ref_.has_value(), had_image_before);
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, UpdateNativeImage)
{
    bool had_image_before = false;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        had_image_before = buffer_gl_owner_->current_image_ref_.has_value();
    }
    buffer_gl_owner_->UpdateNativeImage();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        if (buffer_gl_owner_->loader_) {
            EXPECT_FALSE(buffer_gl_owner_->current_image_ref_.has_value());
        }
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, UpdateNativeImageFailure)
{
    bool had_image_before = false;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        had_image_before = buffer_gl_owner_->current_image_ref_.has_value();
    }
    buffer_gl_owner_->UpdateNativeImage();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_EQ(buffer_gl_owner_->current_image_ref_.has_value(), had_image_before);
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, UpdateNativeImageWithFence)
{
    buffer_gl_owner_->UpdateNativeImage();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        if (buffer_gl_owner_->current_image_ref_.has_value()) {
            buffer_gl_owner_->current_image_ref_->GetReadyFence();
        }
    }
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_FALSE(buffer_gl_owner_->current_image_ref_.has_value());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, UpdateNativeImageReplacesPreviousImage)
{
    buffer_gl_owner_->UpdateNativeImage();
    void* first_image = nullptr;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        if (buffer_gl_owner_->current_image_ref_.has_value()) {
            first_image = buffer_gl_owner_->current_image_ref_->image();
        }
    }
    buffer_gl_owner_->UpdateNativeImage();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        if (buffer_gl_owner_->current_image_ref_.has_value()) {
            void* second_image = buffer_gl_owner_->current_image_ref_->image();
            if (first_image != nullptr) {
                EXPECT_NE(first_image, second_image);
            }
        }
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, UpdateNativeImageWithAcquireFailure)
{
    bool had_image_before = false;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        had_image_before = buffer_gl_owner_->current_image_ref_.has_value();
    }
    buffer_gl_owner_->UpdateNativeImage();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_EQ(buffer_gl_owner_->current_image_ref_.has_value(), had_image_before);
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, GetNativeBufferNoCurrentImage)
{
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->current_image_ref_.reset();
    }
    auto buffer = buffer_gl_owner_->GetNativeBuffer();
    EXPECT_EQ(buffer, nullptr);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ScopedNativeBufferImplFunctionality)
{
    buffer_gl_owner_->UpdateNativeImage();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        if (!buffer_gl_owner_->current_image_ref_.has_value()) {
            return;
        }
    }
    auto buffer = buffer_gl_owner_->GetNativeBuffer();
    ASSERT_NE(buffer, nullptr);
    base::ScopedFD read_fd(HANDLE_EINTR(dup(IMAGE_COUNT)));
    buffer->SetReadFence(std::move(read_fd));
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        auto it = buffer_gl_owner_->image_refs_.find(
            buffer_gl_owner_->current_image_ref_->image());
        ASSERT_NE(it, buffer_gl_owner_->image_refs_.end());
        EXPECT_GE(it->second.count, 1);
    }
    buffer.reset();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        auto it = buffer_gl_owner_->image_refs_.find(
            buffer_gl_owner_->current_image_ref_->image());
        if (it != buffer_gl_owner_->image_refs_.end()) {
            EXPECT_GE(it->second.count, 1);
        }
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, GetNativeBufferAfterReleaseResources)
{
    buffer_gl_owner_->UpdateNativeImage();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_FALSE(buffer_gl_owner_->current_image_ref_.has_value());
    }
    buffer_gl_owner_->ReleaseResources();
    auto buffer = buffer_gl_owner_->GetNativeBuffer();
    EXPECT_EQ(buffer, nullptr);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseRefOnImageLocked_LoaderNull)
{
    OhosWindowBuffer* image = new OhosWindowBuffer();
    base::ScopedFD fence_fd;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->loader_ = nullptr;
        buffer_gl_owner_->image_refs_[image] = SameLayerNativeBufferGLOwner::ImageRef();
        buffer_gl_owner_->image_refs_[image].count = 1;
        buffer_gl_owner_->ReleaseRefOnImageLocked(image, std::move(fence_fd));
        EXPECT_NE(buffer_gl_owner_->image_refs_.find(image), buffer_gl_owner_->image_refs_.end());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseRefOnImageLocked_CountGreaterThanOne)
{
    OhosWindowBuffer* image = new OhosWindowBuffer();
    base::ScopedFD fence_fd(HANDLE_EINTR(dup(1)));
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        auto& image_ref = buffer_gl_owner_->image_refs_[image];
        image_ref.count = IMAGE_COUNT;
        image_ref.release_fence_fd = base::ScopedFD(HANDLE_EINTR(dup(IMAGE_COUNT)));
        buffer_gl_owner_->ReleaseRefOnImageLocked(image, std::move(fence_fd));
        EXPECT_EQ(buffer_gl_owner_->image_refs_[image].count, 1);
        EXPECT_FALSE(buffer_gl_owner_->image_refs_[image].release_fence_fd.is_valid());
    }
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->image_refs_.erase(image);
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseRefOnImageLocked_CountZeroWithoutFence)
{
    OhosWindowBuffer* image = new OhosWindowBuffer();
    base::ScopedFD fence_fd;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        auto& image_ref = buffer_gl_owner_->image_refs_[image];
        image_ref.count = 1;
        buffer_gl_owner_->ReleaseRefOnImageLocked(image, std::move(fence_fd));
        EXPECT_EQ(buffer_gl_owner_->image_refs_.find(image), buffer_gl_owner_->image_refs_.end());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseRefOnImageLocked_CountZeroWithFence)
{
    OhosWindowBuffer* image = new OhosWindowBuffer();
    base::ScopedFD fence_fd(HANDLE_EINTR(dup(1)));
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        auto& image_ref = buffer_gl_owner_->image_refs_[image];
        image_ref.count = 1;
        buffer_gl_owner_->ReleaseRefOnImageLocked(image, std::move(fence_fd));
        EXPECT_EQ(buffer_gl_owner_->image_refs_.find(image), buffer_gl_owner_->image_refs_.end());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseRefOnImageLocked_InvokeBufferAvailableCallback)
{
    buffer_gl_owner_->max_images_ = IMAGE_COUNT;
    OhosWindowBuffer* image = new OhosWindowBuffer();
    base::ScopedFD fence_fd;
    bool callback_called = false;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        auto& image_ref = buffer_gl_owner_->image_refs_[image];
        image_ref.count = 1;
        buffer_gl_owner_->buffer_available_cb_ = base::BindOnce(
            [](bool* flag) { *flag = true; }, &callback_called);
        buffer_gl_owner_->ReleaseRefOnImageLocked(image, std::move(fence_fd));
        EXPECT_EQ(buffer_gl_owner_->image_refs_.find(image), buffer_gl_owner_->image_refs_.end());
    }
    EXPECT_TRUE(callback_called);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseRefOnImageLocked_NotInvokeBufferAvailableCallbackWhenMaxImagesOne)
{
    buffer_gl_owner_->max_images_ = 1;
    OhosWindowBuffer* image = new OhosWindowBuffer();
    base::ScopedFD fence_fd;
    bool callback_called = false;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        auto& image_ref = buffer_gl_owner_->image_refs_[image];
        image_ref.count = 1;
        buffer_gl_owner_->buffer_available_cb_ = base::BindOnce(
            [](bool* flag) { *flag = true; }, &callback_called);
        buffer_gl_owner_->ReleaseRefOnImageLocked(image, std::move(fence_fd));
        EXPECT_EQ(buffer_gl_owner_->image_refs_.find(image), buffer_gl_owner_->image_refs_.end());
    }
    EXPECT_TRUE(callback_called);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseRefOnImageLocked_MergeFences)
{
    OhosWindowBuffer* image = new OhosWindowBuffer();
    base::ScopedFD fence_fd1(HANDLE_EINTR(dup(1)));
    base::ScopedFD fence_fd2(HANDLE_EINTR(dup(IMAGE_COUNT)));
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        auto& image_ref = buffer_gl_owner_->image_refs_[image];
        image_ref.count = IMAGE_COUNT;
        image_ref.release_fence_fd = std::move(fence_fd1);
        buffer_gl_owner_->ReleaseRefOnImageLocked(image, std::move(fence_fd2));
        EXPECT_EQ(image_ref.count, 1);
        EXPECT_FALSE(image_ref.release_fence_fd.is_valid());
        buffer_gl_owner_->ReleaseRefOnImageLocked(image, base::ScopedFD());
        EXPECT_EQ(buffer_gl_owner_->image_refs_.find(image), buffer_gl_owner_->image_refs_.end());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, RunWhenBufferIsAvailable_BufferFull)
{
    buffer_gl_owner_->max_images_ = IMAGE_BUFFER_SIZE;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->image_refs_.clear();
        for (int i = 0; i < IMAGE_BUFFER_SIZE; ++i) {
            OhosWindowBuffer* image = new OhosWindowBuffer();
            buffer_gl_owner_->image_refs_[image] = SameLayerNativeBufferGLOwner::ImageRef();
            buffer_gl_owner_->image_refs_[image].count = 1;
        }
    }
    base::OnceClosure callback = base::OnceClosure();
    buffer_gl_owner_->RunWhenBufferIsAvailable(std::move(callback));
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_TRUE(buffer_gl_owner_->buffer_available_cb_.is_null());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, RunWhenBufferIsAvailable_ConcurrentRequests)
{
    buffer_gl_owner_->max_images_ = IMAGE_BUFFER_SIZE;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->image_refs_.clear();
        for (int i = 0; i < IMAGE_BUFFER_SIZE; ++i) {
            OhosWindowBuffer* image = new OhosWindowBuffer();
            buffer_gl_owner_->image_refs_[image] = SameLayerNativeBufferGLOwner::ImageRef();
            buffer_gl_owner_->image_refs_[image].count = 1;
        }
    }
    base::OnceClosure callback = base::OnceClosure();
    buffer_gl_owner_->RunWhenBufferIsAvailable(std::move(callback));
    SUCCEED();
}

TEST_F(SameLayerNativeBufferGLOwnerTest, RunWhenBufferIsAvailable_CallbackReleasesBuffer)
{
    buffer_gl_owner_->max_images_ = IMAGE_BUFFER_SIZE;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->image_refs_.clear();
        for (int i = 0; i < IMAGE_BUFFER_SIZE; ++i) {
            OhosWindowBuffer* image = new OhosWindowBuffer();
            buffer_gl_owner_->image_refs_[image] = SameLayerNativeBufferGLOwner::ImageRef();
            buffer_gl_owner_->image_refs_[image].count = 1;
        }
    }
    base::OnceClosure callback = base::OnceClosure();
    buffer_gl_owner_->RunWhenBufferIsAvailable(std::move(callback));
}

TEST_F(SameLayerNativeBufferGLOwnerTest, GetCodedSizeAndVisibleRect_001)
{
    gfx::Size rotated_visible_size = gfx::Size(WIDTH, HEIGHT);
    gfx::Size coded_size;
    gfx::Rect visible_rect;
    bool result = buffer_gl_owner_->GetCodedSizeAndVisibleRect(
        rotated_visible_size, &coded_size, &visible_rect);
    EXPECT_FALSE(result);
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->loader_ = nullptr;
    }
    result = buffer_gl_owner_->GetCodedSizeAndVisibleRect(
        rotated_visible_size, &coded_size, &visible_rect);
    EXPECT_FALSE(result);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, GetCodedSizeAndVisibleRect_002)
{
    gfx::Size rotated_visible_size = gfx::Size(WIDTH, HEIGHT);
    gfx::Size coded_size;
    gfx::Rect visible_rect;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->current_image_ref_.reset();
    }
    bool result = buffer_gl_owner_->GetCodedSizeAndVisibleRect(
        rotated_visible_size, &coded_size, &visible_rect);
    EXPECT_FALSE(result);
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->loader_ = nullptr;
    }
    result = buffer_gl_owner_->GetCodedSizeAndVisibleRect(
        rotated_visible_size, &coded_size, &visible_rect);
    EXPECT_FALSE(result);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, AquireOhosNativeWindow)
{
    void* window = buffer_gl_owner_->AquireOhosNativeWindow();
    EXPECT_NE(window, nullptr);
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->loader_ = nullptr;
    }
    window = buffer_gl_owner_->AquireOhosNativeWindow();
    EXPECT_EQ(window, nullptr);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, GetSurfaceId)
{
    uint64_t surface_id = 1;
    buffer_gl_owner_->GetSurfaceId(&surface_id);
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->loader_ = nullptr;
    }
    buffer_gl_owner_->GetSurfaceId(&surface_id);
    SUCCEED();
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseNativeImage)
{
    buffer_gl_owner_->ReleaseNativeImage();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->loader_ = nullptr;
    }
    buffer_gl_owner_->ReleaseNativeImage();
    SUCCEED();
}

TEST_F(SameLayerNativeBufferGLOwnerTest, GetReadyFence)
{
    buffer_gl_owner_->UpdateNativeImage();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->current_image_ref_->GetReadyFence();
    }
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_FALSE(buffer_gl_owner_->current_image_ref_.has_value());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, RunWhenBufferIsAvailable_MaxImagesOne_ImmediateCallback)
{
    buffer_gl_owner_->max_images_ = 1;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->image_refs_.clear();
    }
    bool callback_called = false;
    base::OnceClosure callback = base::BindOnce(
        [](bool* flag) { *flag = true; }, &callback_called);
    buffer_gl_owner_->RunWhenBufferIsAvailable(std::move(callback));
    EXPECT_TRUE(callback_called);
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_TRUE(buffer_gl_owner_->buffer_available_cb_.is_null());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, RunWhenBufferIsAvailable_BufferAvailable_ImmediateCallback)
{
    buffer_gl_owner_->max_images_ = IMAGE_BUFFER_SIZE;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->image_refs_.clear();
        // Add fewer images than max_images_ so buffer is available
        OhosWindowBuffer* image = new OhosWindowBuffer();
        buffer_gl_owner_->image_refs_[image] = SameLayerNativeBufferGLOwner::ImageRef();
        buffer_gl_owner_->image_refs_[image].count = 1;
    }
    bool callback_called = false;
    base::OnceClosure callback = base::BindOnce(
        [](bool* flag) { *flag = true; }, &callback_called);
    buffer_gl_owner_->RunWhenBufferIsAvailable(std::move(callback));
    EXPECT_TRUE(callback_called);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, RunWhenBufferIsAvailable_BufferFull_DeferCallback)
{
    buffer_gl_owner_->max_images_ = IMAGE_BUFFER_SIZE;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->image_refs_.clear();
        for (int i = 0; i < IMAGE_BUFFER_SIZE; ++i) {
            OhosWindowBuffer* image = new OhosWindowBuffer();
            buffer_gl_owner_->image_refs_[image] = SameLayerNativeBufferGLOwner::ImageRef();
            buffer_gl_owner_->image_refs_[image].count = 1;
        }
    }
    bool callback_called = false;
    base::OnceClosure callback = base::BindOnce(
        [](bool* flag) { *flag = true; }, &callback_called);
    buffer_gl_owner_->RunWhenBufferIsAvailable(std::move(callback));
    EXPECT_FALSE(callback_called);
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_FALSE(buffer_gl_owner_->buffer_available_cb_.is_null());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, RunWhenBufferIsAvailable_EmptyImageRefs_ImmediateCallback)
{
    buffer_gl_owner_->max_images_ = IMAGE_BUFFER_SIZE;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->image_refs_.clear();
    }
    bool callback_called = false;
    base::OnceClosure callback = base::BindOnce(
        [](bool* flag) { *flag = true; }, &callback_called);
    buffer_gl_owner_->RunWhenBufferIsAvailable(std::move(callback));
    EXPECT_TRUE(callback_called);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, SetFrameAvailableCallback)
{
    bool callback_called = false;
    base::RepeatingClosure frame_cb = base::BindRepeating(
        [](bool* flag) { *flag = true; }, &callback_called);
    buffer_gl_owner_->SetFrameAvailableCallback(frame_cb);
    EXPECT_TRUE(buffer_gl_owner_->is_frame_available_callback_set_);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, EnsureNativeImageBound)
{
    GLuint texture_id = buffer_gl_owner_->GetTextureId();
    buffer_gl_owner_->EnsureNativeImageBound(texture_id);
    // After call, texture_id should remain unchanged
    EXPECT_EQ(buffer_gl_owner_->GetTextureId(), texture_id);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseRefOnImageLocked_WithReleaseFenceValid)
{
    OhosWindowBuffer* image = new OhosWindowBuffer();
    base::ScopedFD fence_fd;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        auto& image_ref = buffer_gl_owner_->image_refs_[image];
        image_ref.count = 1;
        image_ref.release_fence_fd = base::ScopedFD(HANDLE_EINTR(dup(1)));
        buffer_gl_owner_->ReleaseRefOnImageLocked(image, std::move(fence_fd));
        EXPECT_EQ(buffer_gl_owner_->image_refs_.find(image), buffer_gl_owner_->image_refs_.end());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseRefOnImageLocked_CountGreaterThanOneNoExistingFence)
{
    OhosWindowBuffer* image = new OhosWindowBuffer();
    base::ScopedFD fence_fd;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        auto& image_ref = buffer_gl_owner_->image_refs_[image];
        image_ref.count = IMAGE_COUNT;
        // No existing release_fence_fd
        buffer_gl_owner_->ReleaseRefOnImageLocked(image, std::move(fence_fd));
        EXPECT_EQ(buffer_gl_owner_->image_refs_[image].count, 1);
    }
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->image_refs_.erase(image);
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, GetTextureBase_ReturnsNonNull)
{
    TextureBase* base_texture = buffer_gl_owner_->GetTextureBase();
    EXPECT_NE(base_texture, nullptr);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, GetTextureId_ReturnsPositiveValue)
{
    GLuint texture_id = buffer_gl_owner_->GetTextureId();
    EXPECT_GT(texture_id, 0u);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, GetTextureId_ConsistentAcrossCalls)
{
    GLuint id1 = buffer_gl_owner_->GetTextureId();
    GLuint id2 = buffer_gl_owner_->GetTextureId();
    EXPECT_EQ(id1, id2);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, RunWhenBufferIsAvailable_EmptyCallback_BufferFull)
{
    buffer_gl_owner_->max_images_ = IMAGE_BUFFER_SIZE;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->image_refs_.clear();
        for (int i = 0; i < IMAGE_BUFFER_SIZE; ++i) {
            OhosWindowBuffer* image = new OhosWindowBuffer();
            buffer_gl_owner_->image_refs_[image] = SameLayerNativeBufferGLOwner::ImageRef();
            buffer_gl_owner_->image_refs_[image].count = 1;
        }
    }
    // Null callback with full buffer - buffer_available_cb_ should remain null
    base::OnceClosure null_callback;
    buffer_gl_owner_->RunWhenBufferIsAvailable(std::move(null_callback));
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_TRUE(buffer_gl_owner_->buffer_available_cb_.is_null());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, RunWhenBufferIsAvailable_TwoSlotsAvailable)
{
    buffer_gl_owner_->max_images_ = IMAGE_BUFFER_SIZE;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->image_refs_.clear();
        // Only 1 image, max is 3, so 2 slots available
        OhosWindowBuffer* image = new OhosWindowBuffer();
        buffer_gl_owner_->image_refs_[image] = SameLayerNativeBufferGLOwner::ImageRef();
        buffer_gl_owner_->image_refs_[image].count = 1;
    }
    bool callback_called = false;
    base::OnceClosure callback = base::BindOnce(
        [](bool* flag) { *flag = true; }, &callback_called);
    buffer_gl_owner_->RunWhenBufferIsAvailable(std::move(callback));
    EXPECT_TRUE(callback_called);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, SetFrameAvailableCallback_SetsFlag)
{
    EXPECT_FALSE(buffer_gl_owner_->is_frame_available_callback_set_);
    base::RepeatingClosure callback = base::BindRepeating([]() {});
    buffer_gl_owner_->SetFrameAvailableCallback(callback);
    EXPECT_TRUE(buffer_gl_owner_->is_frame_available_callback_set_);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, SetFrameAvailableCallback_CanSetMultipleTimes)
{
    base::RepeatingClosure cb1 = base::BindRepeating([]() {});
    base::RepeatingClosure cb2 = base::BindRepeating([]() {});
    buffer_gl_owner_->SetFrameAvailableCallback(cb1);
    EXPECT_TRUE(buffer_gl_owner_->is_frame_available_callback_set_);
    buffer_gl_owner_->SetFrameAvailableCallback(cb2);
    EXPECT_TRUE(buffer_gl_owner_->is_frame_available_callback_set_);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, EnsureNativeImageBound_WithValidTextureId)
{
    GLuint texture_id = buffer_gl_owner_->GetTextureId();
    ASSERT_GT(texture_id, 0u);
    buffer_gl_owner_->EnsureNativeImageBound(texture_id);
    // texture_id must remain the same after the binding operation
    EXPECT_EQ(buffer_gl_owner_->GetTextureId(), texture_id);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseRefOnImageLocked_CountDecremented)
{
    OhosWindowBuffer* image = new OhosWindowBuffer();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        auto& image_ref = buffer_gl_owner_->image_refs_[image];
        image_ref.count = 3;
        buffer_gl_owner_->ReleaseRefOnImageLocked(image, base::ScopedFD());
        EXPECT_EQ(buffer_gl_owner_->image_refs_[image].count, 2);
        buffer_gl_owner_->ReleaseRefOnImageLocked(image, base::ScopedFD());
        EXPECT_EQ(buffer_gl_owner_->image_refs_[image].count, 1);
        buffer_gl_owner_->ReleaseRefOnImageLocked(image, base::ScopedFD());
        EXPECT_EQ(buffer_gl_owner_->image_refs_.find(image),
                  buffer_gl_owner_->image_refs_.end());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseRefOnImageLocked_CallbackFiredWhenBelowMax)
{
    buffer_gl_owner_->max_images_ = IMAGE_BUFFER_SIZE;
    OhosWindowBuffer* images[IMAGE_BUFFER_SIZE];
    for (int i = 0; i < IMAGE_BUFFER_SIZE; ++i) {
        images[i] = new OhosWindowBuffer();
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->image_refs_[images[i]].count = 1;
    }
    bool callback_called = false;
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->buffer_available_cb_ = base::BindOnce(
            [](bool* flag) { *flag = true; }, &callback_called);
        // Release one image -> size drops below max_images_
        buffer_gl_owner_->ReleaseRefOnImageLocked(images[0], base::ScopedFD());
    }
    EXPECT_TRUE(callback_called);
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        for (int i = 1; i < IMAGE_BUFFER_SIZE; ++i) {
            buffer_gl_owner_->image_refs_.erase(images[i]);
            delete images[i];
        }
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseResources_ClearsPendingCallback)
{
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        OhosWindowBuffer* image = new OhosWindowBuffer();
        buffer_gl_owner_->image_refs_[image].count = 1;
        buffer_gl_owner_->buffer_available_cb_ = base::BindOnce([]() {});
    }
    buffer_gl_owner_->ReleaseResources();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_EQ(buffer_gl_owner_->loader_, nullptr);
        EXPECT_TRUE(buffer_gl_owner_->image_refs_.empty());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, AquireOhosNativeWindow_WithLoader_ReturnsNonNull)
{
    void* window = buffer_gl_owner_->AquireOhosNativeWindow();
    EXPECT_NE(window, nullptr);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, AquireOhosNativeWindow_WithoutLoader_ReturnsNull)
{
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->loader_ = nullptr;
    }
    void* window = buffer_gl_owner_->AquireOhosNativeWindow();
    EXPECT_EQ(window, nullptr);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, GetSurfaceId_WithLoader_NoFault)
{
    uint64_t surface_id = 0;
    buffer_gl_owner_->GetSurfaceId(&surface_id);
    // With active loader, loader_ should not be cleared by GetSurfaceId
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_NE(buffer_gl_owner_->loader_, nullptr);
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, GetSurfaceId_WithoutLoader_NoFault)
{
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->loader_ = nullptr;
    }
    uint64_t surface_id = 0;
    buffer_gl_owner_->GetSurfaceId(&surface_id);
    // loader_ must remain null after the call
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_EQ(buffer_gl_owner_->loader_, nullptr);
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseNativeImage_WithLoader_NoFault)
{
    buffer_gl_owner_->ReleaseNativeImage();
    // After ReleaseNativeImage, image_refs_ should remain empty (nothing was added)
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_TRUE(buffer_gl_owner_->image_refs_.empty());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ReleaseNativeImage_WithoutLoader_NoFault)
{
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->loader_ = nullptr;
    }
    buffer_gl_owner_->ReleaseNativeImage();
    // loader_ must remain null and image_refs_ empty after the call
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_EQ(buffer_gl_owner_->loader_, nullptr);
        EXPECT_TRUE(buffer_gl_owner_->image_refs_.empty());
    }
}

TEST_F(SameLayerNativeBufferGLOwnerTest, GetNativeBuffer_AfterUpdateNativeImage_Null)
{
    buffer_gl_owner_->UpdateNativeImage();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_FALSE(buffer_gl_owner_->current_image_ref_.has_value());
    }
    auto buffer = buffer_gl_owner_->GetNativeBuffer();
    EXPECT_EQ(buffer, nullptr);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, MaxImages_DefaultValue)
{
    // max_images_ should have a reasonable default
    EXPECT_GE(buffer_gl_owner_->max_images_, 1);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, ImageRefs_EmptyAfterInit)
{
    base::AutoLock lock(buffer_gl_owner_->lock_);
    EXPECT_TRUE(buffer_gl_owner_->image_refs_.empty());
}

TEST_F(SameLayerNativeBufferGLOwnerTest, CurrentImageRef_AbsentAfterInit)
{
    base::AutoLock lock(buffer_gl_owner_->lock_);
    EXPECT_FALSE(buffer_gl_owner_->current_image_ref_.has_value());
}

TEST_F(SameLayerNativeBufferGLOwnerTest, MaxImages_CanBeChanged)
{
    buffer_gl_owner_->max_images_ = 5;
    EXPECT_EQ(buffer_gl_owner_->max_images_, 5);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, RunWhenBufferIsAvailable_CallbackFiredAfterRelease)
{
    buffer_gl_owner_->max_images_ = 1;
    OhosWindowBuffer* image = new OhosWindowBuffer();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->image_refs_[image].count = 1;
    }
    bool callback_called = false;
    base::OnceClosure callback = base::BindOnce(
        [](bool* flag) { *flag = true; }, &callback_called);
    // Buffer is at max capacity
    buffer_gl_owner_->RunWhenBufferIsAvailable(std::move(callback));
    EXPECT_FALSE(callback_called);
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_FALSE(buffer_gl_owner_->buffer_available_cb_.is_null());
    }
    // Release the image - callback should fire
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        buffer_gl_owner_->ReleaseRefOnImageLocked(image, base::ScopedFD());
    }
    EXPECT_TRUE(callback_called);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, TextureId_MatchesCreatedTexture)
{
    GLuint texture_id = buffer_gl_owner_->GetTextureId();
    EXPECT_GT(texture_id, 0u);
    // GetTextureBase returns the same underlying texture
    TextureBase* base = buffer_gl_owner_->GetTextureBase();
    EXPECT_NE(base, nullptr);
}

TEST_F(SameLayerNativeBufferGLOwnerTest, UpdateNativeImage_WithTextureNull_DoesNotCrash)
{
    buffer_gl_owner_->texture_ = nullptr;
    buffer_gl_owner_->UpdateNativeImage();
    {
        base::AutoLock lock(buffer_gl_owner_->lock_);
        EXPECT_FALSE(buffer_gl_owner_->current_image_ref_.has_value());
    }
}

} // namespace gpu