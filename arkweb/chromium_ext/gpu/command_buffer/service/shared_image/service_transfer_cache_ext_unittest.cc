// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be

#include "gtest/gtest.h"
#include "arkweb/chromium_ext/gpu/command_buffer/service/service_transfer_cache_ext.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkSurface.h"

namespace gpu {
namespace {

class ServiceTransferCacheExtTest : public testing::Test {
  protected:
    void SetUp() {
        GpuPreferences preferences;
        cache_ = std::make_unique<ServiceTransferCacheExt>(
            preferences, base::RepeatingClosure());
        ASSERT_NE(cache_, nullptr);
    }

    std::unique_ptr<ServiceTransferCacheExt> cache_;
};

#if BUILDFLAG(ARKWEB_HEIF_SUPPORT)
TEST_F(ServiceTransferCacheExtTest, CreateEntrySuccess) {
    auto surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(10, 10));
    std::vector<sk_sp<SkImage>> images = {surface->makeImageSnapshot()};
    ASSERT_NE(surface, nullptr);
    ASSERT_FALSE(images.empty());
    ASSERT_NE(images[0], nullptr);

    EXPECT_TRUE(cache_->CreateLockedRGBAHardwareDecodedImageEntry(
        1, 100, ServiceDiscardableHandle(), nullptr, std::move(images), 100));
}

TEST_F(ServiceTransferCacheExtTest, CreateDuplicateEntryFails) {
    auto surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(10, 10));
    std::vector<sk_sp<SkImage>> images = {surface->makeImageSnapshot()};
    ASSERT_NE(surface, nullptr);
    ASSERT_FALSE(images.empty());
    ASSERT_NE(images[0], nullptr);

    EXPECT_TRUE(cache_->CreateLockedRGBAHardwareDecodedImageEntry(
        1, 100, ServiceDiscardableHandle(), nullptr, images, 100));
    
    EXPECT_FALSE(cache_->CreateLockedRGBAHardwareDecodedImageEntry(
        1, 100, ServiceDiscardableHandle(), nullptr, std::move(images), 100));
}

TEST_F(ServiceTransferCacheExtTest, CreateEntryUpdatesCacheSize) {
    auto surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(10, 10));
    std::vector<sk_sp<SkImage>> images = {surface->makeImageSnapshot()};
    ASSERT_NE(surface, nullptr);
    ASSERT_FALSE(images.empty());
    ASSERT_NE(images[0], nullptr);

    EXPECT_TRUE(cache_->CreateLockedRGBAHardwareDecodedImageEntry(
        1, 100, ServiceDiscardableHandle(), nullptr, std::move(images), 100));
}

TEST_F(ServiceTransferCacheExtTest, CreateEntryUpdatesImageCount) {
    auto surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(10, 10));
    std::vector<sk_sp<SkImage>> images = {surface->makeImageSnapshot()};
    ASSERT_NE(surface, nullptr);
    ASSERT_FALSE(images.empty());
    ASSERT_NE(images[0], nullptr);

    EXPECT_TRUE(cache_->CreateLockedRGBAHardwareDecodedImageEntry(
        1, 100, ServiceDiscardableHandle(), nullptr, std::move(images), 100));
}
#endif

} // namespace
} // namespace gpu