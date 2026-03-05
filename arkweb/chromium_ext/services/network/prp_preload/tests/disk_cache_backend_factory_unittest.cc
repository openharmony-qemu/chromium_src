/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "testing/gtest/include/gtest/gtest.h"

#include "arkweb/chromium_ext/services/network/prp_preload/src/disk_cache_backend_factory.h"
#include "base/test/task_environment.h"
#include "base/files/file_util.h"
#include "base/path_service.h"
#include "base/time/time.h"

namespace ohos_prp_preload {

class DiskCacheBackendFactoryTest : public testing::Test {
public:
  static void SetUpTestCase();
  static void TearDownTestCase();
  void SetUp() override;
  void TearDown() override;
protected:
  std::unique_ptr<base::test::TaskEnvironment> task_environment_;
};

void DiskCacheBackendFactoryTest::SetUpTestCase() {}

void DiskCacheBackendFactoryTest::TearDownTestCase() {}

void DiskCacheBackendFactoryTest::SetUp() {
  task_environment_ = std::make_unique<base::test::TaskEnvironment>(
    base::test::TaskEnvironment::MainThreadType::DEFAULT);
}

void DiskCacheBackendFactoryTest::TearDown() {
  task_environment_.reset();
}

TEST_F(DiskCacheBackendFactoryTest, CreateBackendTest) {
  DiskCacheBackendFactory factory;
  factory.CreateBackend();
  EXPECT_FALSE(factory.is_inited_.load());
}

TEST_F(DiskCacheBackendFactoryTest, CheckBackendAsyncTest) {
  DiskCacheBackendFactory factory;
  factory.is_inited_.store(true);
  bool result = factory.CheckBackendAsync(base::BindOnce([](){}));
  EXPECT_TRUE(result);
  
  factory.is_inited_.store(false);
  result = factory.CheckBackendAsync(base::BindOnce([](){}));
  EXPECT_FALSE(result);
  EXPECT_EQ(factory.backend_complete_callback_list_.size(), 1);
}

TEST_F(DiskCacheBackendFactoryTest, CacheCreatedCallbackTest) {
  DiskCacheBackendFactory factory;
  disk_cache::BackendResult result;
  result.net_error = net::OK;
  result.backend = nullptr;
  factory.CacheCreatedCallback(std::move(result));
  EXPECT_TRUE(factory.is_inited_.load());
  
  disk_cache::BackendResult result2;
  result2.net_error = net::ERR_FAILED;
  result2.backend = nullptr;
  factory.CacheCreatedCallback(std::move(result2));
}

TEST_F(DiskCacheBackendFactoryTest, CacheCreatedCallbackWithCallbackListTest) {
  DiskCacheBackendFactory factory;
  factory.backend_complete_callback_list_.push_back(base::BindOnce([](){}));
  factory.backend_complete_callback_list_.push_back(base::BindOnce([](){}));
  
  disk_cache::BackendResult result;
  result.net_error = net::OK;
  result.backend = nullptr;
  factory.CacheCreatedCallback(std::move(result));
  EXPECT_TRUE(factory.is_inited_.load());
  EXPECT_EQ(factory.backend_complete_callback_list_.size(), 0);
}

TEST_F(DiskCacheBackendFactoryTest, RemoveCacheTest) {
  DiskCacheBackendFactory factory;
  factory.RemoveCache(base::Time::Now(), base::Time::Max());
  EXPECT_FALSE(factory.is_inited_.load());
  EXPECT_EQ(factory.backend_complete_callback_list_.size(), 0);
}

TEST_F(DiskCacheBackendFactoryTest, RemoveCacheWithBackendTest) {
  DiskCacheBackendFactory factory;
  factory.is_inited_.store(true);
  factory.backend_complete_callback_list_.push_back(base::BindOnce([](){}));
  factory.RemoveCache(base::Time::Now(), base::Time::Max());
  EXPECT_FALSE(factory.is_inited_.load());
  EXPECT_EQ(factory.backend_complete_callback_list_.size(), 0);
}

TEST_F(DiskCacheBackendFactoryTest, RemoveCacheWithTimeRangeTest) {
  DiskCacheBackendFactory factory;
  factory.is_inited_.store(true);
  factory.RemoveCache(base::Time::Now() - base::Days(10), base::Time::Now());
  EXPECT_FALSE(factory.is_inited_.load());
}

TEST_F(DiskCacheBackendFactoryTest, ClearPreloadCacheDoneTest) {
  DiskCacheBackendFactory factory;
  factory.backend_complete_callback_list_.push_back(base::BindOnce([](){}));
  factory.backend_complete_callback_list_.push_back(base::BindOnce([](){}));
  factory.ClearPreloadCacheDone(net::OK);
  EXPECT_EQ(factory.backend_complete_callback_list_.size(), 0);
}

} // namespace ohos_prp_preload