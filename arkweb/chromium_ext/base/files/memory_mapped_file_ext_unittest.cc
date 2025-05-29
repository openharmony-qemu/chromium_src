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

#include "arkweb/build/features/features.h"
#if BUILDFLAG(ARKWEB_UNITTESTS)
#define private public
#include "base/files/memory_mapped_file.h"
#include "arkweb/chromium_ext/base/files/memory_mapped_file_ext.h"
#undef private
#else  // ARKWEB_UNITTESTS
#include "base/files/memory_mapped_file.h"
#include "arkweb/chromium_ext/base/files/memory_mapped_file_ext.h"
#endif  // ARKWEB_UNITTESTS

#include <cstddef>
#include <cstdint>
#include <optional>
#include <utility>

#include "base/containers/heap_array.h"
#include "base/containers/span.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/platform_test.h"

namespace base {

namespace {

#if BUILDFLAG(ARKWEB_UNITTESTS)

class MemoryMappedFileExtTest : public PlatformTest {
 protected:
  void SetUp() override { PlatformTest::SetUp(); }

  void TearDown() override { PlatformTest::TearDown(); }
};

class MockOhosFileMapper : public OHOS::NWeb::OhosFileMapper {
 public:
  int32_t GetFd() override { return 1; }
  int32_t GetOffset() override { return 1; }
  std::string GetFileName() override { return "name"; }
  bool IsCompressed() override { return true; }
  void* GetDataPtr() override { return nullptr; }
  size_t GetDataLen() override { return 1; }
  bool UnzipData(uint8_t** dest, size_t& len) override { return false; }
};

TEST_F(MemoryMappedFileExtTest, DeleteMemoryMappedFile_001)
{
  std::shared_ptr<MemoryMappedFile> memory_mapped_file_test =
      std::make_shared<MemoryMappedFile>();
  memory_mapped_file_test->mapper_file_ext_.data_ = nullptr;
  memory_mapped_file_test->mapper_file_ext_.customizeData_ = true;
  EXPECT_FALSE((memory_mapped_file_test->mapper_file_ext_.data_ != nullptr) &&
               (memory_mapped_file_test->mapper_file_ext_.mapper_ == nullptr));
  memory_mapped_file_test.reset();
}

TEST_F(MemoryMappedFileExtTest, DeleteMemoryMappedFile_002)
{
  std::shared_ptr<MemoryMappedFile> memory_mapped_file_test =
      std::make_shared<MemoryMappedFile>();
  uint8_t* raw_ptr_to_array = new uint8_t[2]{0x01, 0x02};
  memory_mapped_file_test->mapper_file_ext_.data_ = raw_ptr_to_array;
  memory_mapped_file_test->mapper_file_ext_.customizeData_ = true;
  memory_mapped_file_test->mapper_file_ext_.mapper_ = nullptr;
  EXPECT_TRUE((memory_mapped_file_test->mapper_file_ext_.data_ != nullptr) &&
              (memory_mapped_file_test->mapper_file_ext_.mapper_ == nullptr));
  memory_mapped_file_test.reset();
}

TEST_F(MemoryMappedFileExtTest, SetOhosFileMapper_001)
{
  MemoryMappedFile memory_mapped_file_test;
  MockOhosFileMapper mock_ohos_file_mapper;
  std::shared_ptr<OHOS::NWeb::OhosFileMapper> mapper =
      std::make_shared<MockOhosFileMapper>(mock_ohos_file_mapper);
  uint8_t test[2] = {0x01, 0x02};
  memory_mapped_file_test.bytes_ = span<uint8_t>{test};
  memory_mapped_file_test.mapper_file_ext_.customizeData_ = true;
  memory_mapped_file_test.mapper_file_ext_.mapper_ = nullptr;
  memory_mapped_file_test.SetOhosFileMapper(mapper);
  EXPECT_EQ(memory_mapped_file_test.bytes_.empty(), false);
}

TEST_F(MemoryMappedFileExtTest, SetOhosFileMapper_002)
{
  MemoryMappedFile memory_mapped_file_test;
  MockOhosFileMapper mock_ohos_file_mapper;
  std::shared_ptr<OHOS::NWeb::OhosFileMapper> mapper =
      std::make_shared<MockOhosFileMapper>(mock_ohos_file_mapper);
  uint8_t test[2] = {0x01, 0x02};
  memory_mapped_file_test.bytes_ = span<uint8_t>{test};
  memory_mapped_file_test.mapper_file_ext_.customizeData_ = false;
  memory_mapped_file_test.mapper_file_ext_.mapper_ = nullptr;
  memory_mapped_file_test.SetOhosFileMapper(mapper);
  EXPECT_EQ(memory_mapped_file_test.bytes_.empty(), true);
}

TEST_F(MemoryMappedFileExtTest, SetOhosFileMapper_003)
{
  MemoryMappedFile memory_mapped_file_test;
  MockOhosFileMapper mock_ohos_file_mapper;
  std::shared_ptr<OHOS::NWeb::OhosFileMapper> mapper =
      std::make_shared<MockOhosFileMapper>(mock_ohos_file_mapper);
  uint8_t test[2] = {0x01, 0x02};
  memory_mapped_file_test.bytes_ = span<uint8_t>{test};
  memory_mapped_file_test.mapper_file_ext_.customizeData_ = true;
  memory_mapped_file_test.mapper_file_ext_.mapper_ = mapper;
  memory_mapped_file_test.SetOhosFileMapper(mapper);
  EXPECT_EQ(memory_mapped_file_test.bytes_.empty(), false);
}
#endif  // ARKWEB_UNITTESTS

}  // namespace

}  // namespace base