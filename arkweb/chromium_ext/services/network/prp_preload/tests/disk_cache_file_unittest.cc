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

#include <string>

#include "base/memory/scoped_refptr.h"
#include "base/test/task_environment.h"
#include "base/time/time.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "net/base/io_buffer.h"
#include "net/base/net_errors.h"

#define private public
#define protected public
#include "arkweb/chromium_ext/services/network/prp_preload/src/disk_cache_backend_factory.h"
#include "arkweb/chromium_ext/services/network/prp_preload/src/disk_cache_file.h"
#undef protected
#undef private

using namespace ohos_prp_preload;
using ohos_prp_preload::DiskCacheEntry;

namespace disk_cache {

class MockEntry : public Entry {
 public:
  MOCK_METHOD(void, Doom, (), (override));
  MOCK_METHOD(void, Close, (), (override));
  MOCK_METHOD(std::string, GetKey, (), (const, override));
  MOCK_METHOD(base::Time, GetLastUsed, (), (const, override));
  MOCK_METHOD(base::Time, GetLastModified, (), (const, override));
  MOCK_METHOD(int32_t, GetDataSize, (int index), (const, override));

  MOCK_METHOD(int,
              ReadData,
              (int index,
               int offset,
               IOBuffer* buf,
               int buf_len,
               CompletionOnceCallback callback),
              (override));

  MOCK_METHOD(int,
              WriteData,
              (int index,
               int offset,
               IOBuffer* buf,
               int buf_len,
               CompletionOnceCallback callback,
               bool truncate),
              (override));

  MOCK_METHOD(int,
              ReadSparseData,
              (int64_t offset,
               IOBuffer* buf,
               int buf_len,
               CompletionOnceCallback callback),
              (override));

  MOCK_METHOD(int,
              WriteSparseData,
              (int64_t offset,
               IOBuffer* buf,
               int buf_len,
               CompletionOnceCallback callback),
              (override));

  MOCK_METHOD(RangeResult,
              GetAvailableRange,
              (int64_t offset, int len, RangeResultCallback callback),
              (override));

  MOCK_METHOD(bool, CouldBeSparse, (), (const, override));
  MOCK_METHOD(void, CancelSparseIO, (), (override));

  MOCK_METHOD(net::Error,
              ReadyForSparseIO,
              (CompletionOnceCallback callback),
              (override));

  MOCK_METHOD(void, SetLastUsedTimeForTest, (base::Time time), (override));
};

}  // namespace disk_cache

class DiskCacheFileTest : public ::testing::Test {
 protected:
  base::test::SingleThreadTaskEnvironment task_environment_{
      base::test::TaskEnvironment::MainThreadType::DEFAULT,
      base::test::TaskEnvironment::TimeSource::MOCK_TIME};
};

class MockBackend : public disk_cache::Backend {
 public:
  MockBackend() : disk_cache::Backend(net::DISK_CACHE) {}
  ~MockBackend() override = default;
  MOCK_METHOD(int32_t, GetEntryCount, (), (const, override));

  MOCK_METHOD(disk_cache::EntryResult,
              OpenOrCreateEntry,
              (const std::string& key,
               net::RequestPriority priority,
               disk_cache::EntryResultCallback callback),
              (override));

  MOCK_METHOD(disk_cache::EntryResult,
              OpenEntry,
              (const std::string& key,
               net::RequestPriority priority,
               disk_cache::EntryResultCallback callback),
              (override));

  MOCK_METHOD(disk_cache::EntryResult,
              CreateEntry,
              (const std::string& key,
               net::RequestPriority priority,
               disk_cache::EntryResultCallback callback),
              (override));

  MOCK_METHOD(net::Error,
              DoomEntry,
              (const std::string& key,
               net::RequestPriority priority,
               CompletionOnceCallback callback),
              (override));

  MOCK_METHOD(net::Error,
              DoomAllEntries,
              (CompletionOnceCallback callback),
              (override));

  MOCK_METHOD(net::Error,
              DoomEntriesBetween,
              (base::Time initial_time,
               base::Time end_time,
               CompletionOnceCallback callback),
              (override));

  MOCK_METHOD(net::Error,
              DoomEntriesSince,
              (base::Time initial_time, CompletionOnceCallback callback),
              (override));

  MOCK_METHOD(int64_t,
              CalculateSizeOfAllEntries,
              (Int64CompletionOnceCallback callback),
              (override));

  MOCK_METHOD(int64_t,
              CalculateSizeOfEntriesBetween,
              (base::Time initial_time,
               base::Time end_time,
               Int64CompletionOnceCallback callback),
              (override));

  MOCK_METHOD(std::unique_ptr<Iterator>, CreateIterator, (), (override));

  MOCK_METHOD(void, GetStats, (base::StringPairs * stats), (override));

  MOCK_METHOD(void, OnExternalCacheHit, (const std::string& key), (override));

  MOCK_METHOD(uint8_t,
              GetEntryInMemoryData,
              (const std::string& key),
              (override));

  MOCK_METHOD(void,
              SetEntryInMemoryData,
              (const std::string& key, uint8_t data),
              (override));

  MOCK_METHOD(int64_t, MaxFileSize, (), (const, override));
};

TEST_F(DiskCacheFileTest, CacheTest) {
  auto discache_factory =
      base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  auto mockBackend = std::make_unique<MockBackend>();
  MockBackend* mockBackendPtr = mockBackend.get();
  discache_factory->backend_ = std::move(mockBackend);
  EXPECT_CALL(*mockBackendPtr,
              OpenOrCreateEntry(::testing::_, ::testing::_, ::testing::_))
      .WillOnce([](const std::string& key, net::RequestPriority priority,
                   disk_cache::EntryResultCallback&& callback) {
        std::move(callback).Run(
            disk_cache::EntryResult::MakeError(net::ERR_FAILED));
        return disk_cache::EntryResult::MakeError(net::ERR_IO_PENDING);
      });
  std::string url = "http://example.com/test";
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});

  auto cache_file = base::MakeRefCounted<ohos_prp_preload::DiskCacheFile>(
      discache_factory, url, entry_loaded_cb);
  DiskCacheEntry entry(cache_file.get(), "http://example.com/test",
                       "fake-content");
  entry.cache_ = cache_file.get();
  entry.Cache();
}

TEST_F(DiskCacheFileTest, DiskCacheEntryOpenCallbackTest) {
  auto discache_factory =
      base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});
  auto cache_file = base::MakeRefCounted<ohos_prp_preload::DiskCacheFile>(
      discache_factory, "url", entry_loaded_cb);
  DiskCacheEntry entry(cache_file.get(), "http://example.com/test",
                       "fake-content");
  auto rv = entry.OpenCallback(net::OK);
  EXPECT_EQ(rv, net::ERR_FAILED);
}

TEST_F(DiskCacheFileTest, DiskCacheEntryDestructorTest) {
  auto discache_factory =
      base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});
  auto cache_file = base::MakeRefCounted<ohos_prp_preload::DiskCacheFile>(
      discache_factory, "url", entry_loaded_cb);
  auto entry = std::make_unique<DiskCacheEntry>(
      cache_file.get(), "http://example.com/test", "fake-content");
  auto mockEntry = std::make_unique<disk_cache::MockEntry>();
  disk_cache::MockEntry* mockEntryPtr = mockEntry.get();
  EXPECT_CALL(*mockEntryPtr, Close()).Times(1);
  entry->entry_ = mockEntryPtr;
  entry.reset();
}

TEST_F(DiskCacheFileTest, OnOpCompleteWriteDataTest) {
  auto discache_factory =
      base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});
  auto cache_file = base::MakeRefCounted<ohos_prp_preload::DiskCacheFile>(
      discache_factory, "url", entry_loaded_cb);
  auto entry = std::make_unique<DiskCacheEntry>(
      cache_file.get(), "http://example.com/test", "fake-content");
  auto mockEntry = std::make_unique<disk_cache::MockEntry>();
  disk_cache::MockEntry* mockEntryPtr = mockEntry.get();
  EXPECT_CALL(*mockEntryPtr, Close()).Times(1);
  entry->entry_ = mockEntryPtr;
  ohos_prp_preload::DiskCacheEntry* cacheEntryPtr = entry.get();
  cache_file->entry_ = std::move(entry);
  cacheEntryPtr->op_type_ = ohos_prp_preload::DiskCacheEntry::WRITE_DATA;
  cacheEntryPtr->OnOpComplete(net::ERR_IO_PENDING);
  EXPECT_TRUE(cache_file->entry_ == nullptr);
}

TEST_F(DiskCacheFileTest, DiskCacheEntryWriteCallbackTest01) {
  auto discache_factory =
      base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});
  auto cache_file = base::MakeRefCounted<ohos_prp_preload::DiskCacheFile>(
      discache_factory, "url", entry_loaded_cb);
  auto entry = std::make_unique<DiskCacheEntry>(
      cache_file.get(), "http://example.com/test", "fake-content");
  auto rv = entry->WriteCallback(net::OK);
  EXPECT_EQ(entry->op_type_, ohos_prp_preload::DiskCacheEntry::WRITE_DATA);
  EXPECT_EQ(rv, net::ERR_FAILED);
}

TEST_F(DiskCacheFileTest, DiskCacheEntryWriteCallbackTest02) {
  auto entry = std::make_unique<DiskCacheEntry>(nullptr, "http://example.com/test", "fake-content");
  auto mockEntry = std::make_unique<disk_cache::MockEntry>();
  disk_cache::MockEntry* mockEntryPtr = mockEntry.get();
  EXPECT_CALL(*mockEntryPtr,
              WriteData(::testing::_, ::testing::_, ::testing::_, ::testing::_,
                        ::testing::_, ::testing::_))
      .Times(1)
      .WillOnce(::testing::Return(net::OK));
  EXPECT_CALL(*mockEntryPtr, Close()).Times(1);
  entry->entry_ = mockEntryPtr;
  int rv = entry->WriteCallback(net::OK);
  EXPECT_EQ(rv, net::OK);
  entry.reset();
}

TEST_F(DiskCacheFileTest, DiskCacheEntryOpenCallback01) {
  DiskCacheEntry entry(nullptr, "", "");
  entry.op_type_ = DiskCacheEntry::OPEN_ENTRY;
  entry.cache_ = nullptr;
  int rv = entry.OpenCallback(net::OK);
  EXPECT_EQ(entry.op_type_, ohos_prp_preload::DiskCacheEntry::CREATE_ENTRY);
  EXPECT_EQ(rv, net::OK);
  auto discache_factory =
      base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  auto mockBackend = std::make_unique<MockBackend>();
  MockBackend* mockBackendPtr = mockBackend.get();
  discache_factory->backend_ = std::move(mockBackend);
  EXPECT_CALL(*mockBackendPtr,
              OpenOrCreateEntry(::testing::_, ::testing::_, ::testing::_))
      .Times(::testing::AtLeast(1));
  std::string url = "http://example.com/test";
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});

  auto cache_file = base::MakeRefCounted<ohos_prp_preload::DiskCacheFile>(
      discache_factory, url, entry_loaded_cb);
  entry.cache_ = cache_file.get();
  rv = entry.OpenCallback(net::OK);
  EXPECT_EQ(rv, net::ERR_FAILED);
}

TEST_F(DiskCacheFileTest, DiskCacheFileStoreInfoAsyncTest02) {
  auto discache_factory =
      base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  std::string url = "http://example.com/test";
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});

  auto cache_file = base::MakeRefCounted<ohos_prp_preload::DiskCacheFile>(
      discache_factory, url, entry_loaded_cb);
  cache_file->DoLoadInfo();
  cache_file->StoreInfoAsync("http://example.com/test");
  EXPECT_TRUE(cache_file->helper_ == nullptr);
}

TEST_F(DiskCacheFileTest, LoadInfoAsyncTest01) {
  auto discache_factory =
      base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  std::string url = "http://example.com/test";
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});

  auto cache_file = base::MakeRefCounted<ohos_prp_preload::DiskCacheFile>(
      discache_factory,
      url,
      entry_loaded_cb);
  cache_file->LoadInfoAsync();
  EXPECT_EQ(cache_file->disk_cache_backend_factory_
                ->backend_complete_callback_list_.size(),
            1);
}

TEST_F(DiskCacheFileTest, LoadInfoAsyncTest02) {
  auto discache_factory =
      base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  discache_factory->is_inited_ = true;
  auto mockBackend = std::make_unique<MockBackend>();
  MockBackend* mockBackendPtr = mockBackend.get();
  discache_factory->backend_ = std::move(mockBackend);
  EXPECT_CALL(*mockBackendPtr,
              OpenEntry(::testing::_, ::testing::_, ::testing::_))
      .WillOnce([](const std::string& key, net::RequestPriority priority,
                   disk_cache::EntryResultCallback&& callback) {
        std::move(callback).Run(
            disk_cache::EntryResult::MakeError(net::ERR_FAILED));

        return disk_cache::EntryResult::MakeError(net::ERR_IO_PENDING);
      });
  std::string url = "http://example.com/test";
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});

  auto cache_file = base::MakeRefCounted<ohos_prp_preload::DiskCacheFile>(
      discache_factory, url, entry_loaded_cb);
  auto helper = std::make_unique<DiskCacheReadHelper>(
      cache_file.get(), "http://example.com/test", base::DoNothing());
  ohos_prp_preload::DiskCacheReadHelper* cacheHelperPtr = helper.get();
  cache_file->helper_ = std::move(helper);
  cache_file->LoadInfoAsync();
  cache_file->helper_ = nullptr;
  cache_file->LoadInfoAsync();
}

TEST_F(DiskCacheFileTest, DiskCacheReadHelperNullTest) {
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});
  DiskCacheReadHelper helper(nullptr, "", entry_loaded_cb);
  auto rv = helper.OpenCallback(net::ERR_FAILED);
  EXPECT_EQ(rv, net::ERR_FAILED);
}

TEST_F(DiskCacheFileTest, DiskCacheReadHelperDestructorTest) {
  auto mockEntry = std::make_unique<disk_cache::MockEntry>();
  disk_cache::MockEntry* mockEntryPtr = mockEntry.get();
  EXPECT_CALL(*mockEntryPtr, Close()).Times(1);
  auto helper = std::make_unique<DiskCacheReadHelper>(
      nullptr, "http://example.com/test", base::DoNothing());
  helper->entry_ = mockEntryPtr;
  helper->buf_ = new net::IOBufferWithSize();
  helper.reset();
}

TEST_F(DiskCacheFileTest, DiskCacheReadHelperOnOpCompleteReadDataTest) {
  auto discache_factory =
      base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});
  auto cache_file = base::MakeRefCounted<ohos_prp_preload::DiskCacheFile>(
      discache_factory, "url", entry_loaded_cb);
  auto helper = std::make_unique<DiskCacheReadHelper>(
      cache_file.get(), "http://example.com/test", base::DoNothing());
  auto mockEntry = std::make_unique<disk_cache::MockEntry>();
  disk_cache::MockEntry* mockEntryPtr = mockEntry.get();
  EXPECT_CALL(*mockEntryPtr, Close()).Times(1);
  helper->entry_ = mockEntryPtr;
  ohos_prp_preload::DiskCacheReadHelper* cacheHelperPtr = helper.get();
  cache_file->helper_ = std::move(helper);
  cacheHelperPtr->op_type_ = ohos_prp_preload::DiskCacheReadHelper::READ_DATA;
  cacheHelperPtr->buf_ = new net::IOBufferWithSize(net::OK);
  EXPECT_TRUE(cacheHelperPtr->entry_ != nullptr);
  EXPECT_TRUE(cacheHelperPtr->buf_ != nullptr);
  cacheHelperPtr->OnOpComplete(net::ERR_IO_PENDING);
  EXPECT_TRUE(cache_file->helper_ == nullptr);
  EXPECT_TRUE(cacheHelperPtr->entry_ == nullptr);
  EXPECT_TRUE(cacheHelperPtr->buf_ == nullptr);
}

TEST_F(DiskCacheFileTest, DiskCacheReadHelperReadCallbackTest) {
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});
  DiskCacheReadHelper helper(nullptr, "", entry_loaded_cb);
  auto rv = helper.ReadCallback(net::ERR_FAILED);
  EXPECT_EQ(rv, net::ERR_FAILED);
}

TEST_F(DiskCacheFileTest, IOCompleteTest) {
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});
  DiskCacheReadHelper helper(nullptr, "", entry_loaded_cb);
  auto rv = helper.IOComplete(net::ERR_FAILED);
  EXPECT_EQ(rv, net::ERR_FAILED);
}

TEST_F(DiskCacheFileTest, IOCompleteTest02) {
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});
  DiskCacheReadHelper helper(nullptr, "", entry_loaded_cb);
  helper.buf_ = new net::IOBufferWithSize(1);
  auto rv = helper.IOComplete(1);
  EXPECT_EQ(rv, 1);
}

TEST_F(DiskCacheFileTest, DiskCacheReadHelperLoadCacheTest) {
  auto discache_factory =
      base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  auto mockBackend = std::make_unique<MockBackend>();
  MockBackend* mockBackendPtr = mockBackend.get();
  discache_factory->backend_ = std::move(mockBackend);
  EXPECT_CALL(*mockBackendPtr,
              OpenEntry(::testing::_, ::testing::_, ::testing::_))
      .WillOnce([](const std::string& key, net::RequestPriority priority,
                   disk_cache::EntryResultCallback&& callback) {
        std::move(callback).Run(
            disk_cache::EntryResult::MakeError(net::ERR_FAILED));

        return disk_cache::EntryResult::MakeError(net::ERR_IO_PENDING);
      });
  std::string url = "http://example.com/test";
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});
  auto cache_file = base::MakeRefCounted<ohos_prp_preload::DiskCacheFile>(
      discache_factory, url, entry_loaded_cb);
  auto helper = std::make_unique<DiskCacheReadHelper>(cache_file.get(), url,
                                                      entry_loaded_cb);
  helper->cache_ = cache_file.get();
  helper->LoadCache();
}

TEST_F(DiskCacheFileTest, DiskCacheFileStoreInfoAsyncTest) {
  auto discache_factory =
      base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  discache_factory->is_inited_ = true;
  std::string url = "http://example.com/test";
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});
  auto cache_file = base::MakeRefCounted<ohos_prp_preload::DiskCacheFile>(
      discache_factory, url, entry_loaded_cb);
  cache_file->StoreInfoAsync("entry_content");
  EXPECT_EQ(cache_file->entry_, nullptr);
}

TEST_F(DiskCacheFileTest, DiskCacheFileRunStoreTaskTest) {
  auto entry_loaded_cb = base::BindRepeating([](const std::string& content) {});
  std::string url = "http://example.com/test";
  auto cache_file = base::MakeRefCounted<ohos_prp_preload::DiskCacheFile>(
      nullptr, url, entry_loaded_cb);
  cache_file->StoreInfoAsync("entry_content");
  cache_file->LoadInfoAsync();
  auto discache_factory =
      base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  auto mockBackend = std::make_unique<MockBackend>();
  discache_factory->backend_ = std::move(mockBackend);
  auto cache_file2 = base::MakeRefCounted<ohos_prp_preload::DiskCacheFile>(
      discache_factory, url, entry_loaded_cb);
  EXPECT_TRUE(cache_file2->Backend() != nullptr);
  cache_file2->EntryReadComplete();
  DiskCacheEntry entry(nullptr, "", "");
  cache_file2->EntryWriteComplete(&entry);
  bool called = false;
  cache_file2->SetDelayedStoreTask(
      base::BindOnce([](bool* called_ptr) { *called_ptr = true; }, &called));
  cache_file2->RunStoreTask();
  EXPECT_TRUE(called);
  cache_file2->RunStoreTask(true);
  called = false;
  cache_file2->RunStoreTask();
  EXPECT_FALSE(called);
}

TEST_F(DiskCacheFileTest, SetDelayedLoadTask_Run) {
  bool called = false;
  scoped_refptr<DiskCacheBackendFactory> backend_factory = nullptr;
  auto entry_loaded_cb = base::BindRepeating([](const std::string&) {});
  auto file = base::MakeRefCounted<DiskCacheFile>(backend_factory, "url",
                                                  entry_loaded_cb);
  file->SetDelayedLoadTask(
      base::BindOnce([](bool* called_ptr) { *called_ptr = true; }, &called));

  file->RunLoadTask(false);

  EXPECT_TRUE(called);
}

TEST_F(DiskCacheFileTest, SetDelayedLoadTask_Clear) {
  bool called = false;
  bool entry_cb_called = false;
  auto entry_loaded_cb =
      base::BindRepeating([](bool* flag, const std::string&) { *flag = true; },
                          base::Unretained(&entry_cb_called));
  scoped_refptr<DiskCacheBackendFactory> backend_factory = nullptr;
  auto file = base::MakeRefCounted<DiskCacheFile>(backend_factory, "url",
                                                  entry_loaded_cb);
  file->SetDelayedLoadTask(base::BindOnce([](bool* flag) { *flag = true; },
                                          base::Unretained(&called)));

  file->RunLoadTask(true);
  EXPECT_FALSE(called);
  EXPECT_TRUE(entry_cb_called);
}

TEST_F(DiskCacheFileTest, BackendCompleteTest) {
  bool called = false;
  scoped_refptr<DiskCacheBackendFactory> backend_factory = nullptr;
  auto entry_loaded_cb = base::BindRepeating([](const std::string&) {});
  auto file = base::MakeRefCounted<DiskCacheFile>(backend_factory, "url",
                                                  entry_loaded_cb);
  file->SetDelayedLoadTask(
      base::BindOnce([](bool* called_ptr) { *called_ptr = true; }, &called));
  file->BackendComplete();
  EXPECT_TRUE(called);
}