/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#include <sys/mman.h>
#include <memory>
#include "base/memory/platform_shared_memory_mapper.h"

namespace base {

constexpr uint64_t kBuf_Offset = 20;
constexpr size_t kBuf_Size = 20;

class PlatformSharedMemoryMapperTest : public ::testing::Test {
public:
  void SetUp() override {}
  void TearDown() override {}
  PlatformSharedMemoryMapper platform_shared_memory_mapper;
};

TEST_F(PlatformSharedMemoryMapperTest, Map_001) {
  subtle::PlatformSharedMemoryHandle myHandle = 0;
  absl::optional<span<uint8_t>> result =
      platform_shared_memory_mapper.Map(myHandle, true, kBuf_Offset, kBuf_Size);
  EXPECT_EQ(result, absl::nullopt);
}

TEST_F(PlatformSharedMemoryMapperTest, Map_002) {
  void* mapping_ptr = nullptr;
  size_t mapping_size = 4096;
  mapping_ptr = mmap(nullptr, mapping_size, PROT_READ | PROT_WRITE,
                     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  span<uint8_t> mapping(reinterpret_cast<uint8_t*>(mapping_ptr), mapping_size);
  for (auto& byte : mapping) {
    byte = 0;
  }
  span<uint8_t> invalid_mapping(
      reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(mapping_ptr) + 1),
      mapping_size);
  platform_shared_memory_mapper.Unmap(invalid_mapping);
  EXPECT_FALSE(munmap(mapping.data(), mapping.size()));
}

}  // namespace base
