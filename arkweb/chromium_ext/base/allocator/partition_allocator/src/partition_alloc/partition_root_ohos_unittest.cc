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

#if !defined(MEMORY_TOOL_REPLACES_ALLOCATOR)

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

#include "base/allocator/partition_allocator/src/partition_alloc/partition_alloc.h"
#include "base/allocator/partition_allocator/src/partition_alloc/partition_alloc_config.h"
#include "base/allocator/partition_allocator/src/partition_alloc/partition_alloc_for_testing.h"
#include "base/allocator/partition_allocator/src/partition_alloc/partition_alloc_forward.h"
#include "base/allocator/partition_allocator/src/partition_alloc/partition_bucket.h"
#include "base/allocator/partition_allocator/src/partition_alloc/partition_root.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace partition_alloc {
namespace {
  const char* type_name = nullptr;
}
class PartitionRootTest : public ::testing::Test {
 protected:
  void SetUp() override {
    PartitionOptions opts;
    allocator_ = std::make_unique<PartitionAllocator>(opts);
    root_ = allocator_->root();
  }

  void TearDown() override {
    if (allocator_) {
      allocator_->root()->ResetForTesting(true);
    }
  }

  std::unique_ptr<PartitionAllocator> allocator_;
  PartitionRoot* root_;
};

TEST_F(PartitionRootTest, BasicAllocation) {
  ASSERT_NE(root_, nullptr);
  constexpr size_t kAllocSize = 100;
  void* ptr = root_->Alloc(kAllocSize);
  ASSERT_NE(ptr, nullptr);
  
  memset(ptr, 0xAA, kAllocSize);
  root_->Free(ptr);
}

TEST_F(PartitionRootTest, MultipleAllocations) {
  constexpr size_t kNumAllocs = 100;
  constexpr size_t kAllocSize = 64;
  
  std::vector<void*> allocations;
  allocations.reserve(kNumAllocs);
  
  for (size_t i = 0; i < kNumAllocs; ++i) {
    void* ptr = root_->Alloc(kAllocSize);
    ASSERT_NE(ptr, nullptr);
    memset(ptr, static_cast<int>(i & 0xFF), kAllocSize);
    allocations.push_back(ptr);
  }
  
  for (void* ptr : allocations) {
    root_->Free(ptr);
  }
}

TEST_F(PartitionRootTest, ZeroSizeAllocation) {
  void* ptr = root_->Alloc(0);
  ASSERT_NE(ptr, nullptr);
  root_->Free(ptr);
}

TEST_F(PartitionRootTest, LargeAllocation) {
  constexpr size_t kLargeSize = 1024 * 1024;
  void* ptr = root_->Alloc(kLargeSize);
  ASSERT_NE(ptr, nullptr);
  
  memset(ptr, 0xBB, kLargeSize);
  root_->Free(ptr);
}

TEST_F(PartitionRootTest, AlignedAllocation) {
  constexpr size_t kAlignment = 64;
  constexpr size_t kAllocSize = 256;
  
  void* ptr = root_->AlignedAlloc(kAlignment, kAllocSize);
  ASSERT_NE(ptr, nullptr);
  EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % kAlignment, 0);
  
  root_->Free(ptr);
}

TEST_F(PartitionRootTest, ReallocSameSize) {
  constexpr size_t kAllocSize = 128;
  void* ptr = root_->Alloc(kAllocSize);
  ASSERT_NE(ptr, nullptr);
  
  memset(ptr, 0xCC, kAllocSize);
  
  void* new_ptr = root_->Realloc(ptr, kAllocSize, type_name);
  ASSERT_EQ(new_ptr, ptr);
  
  root_->Free(new_ptr);
}

TEST_F(PartitionRootTest, ReallocGrow) {
  constexpr size_t kInitialSize = 64;
  constexpr size_t kNewSize = 256;
  
  void* ptr = root_->Alloc(kInitialSize);
  ASSERT_NE(ptr, nullptr);
  
  memset(ptr, 0xDD, kInitialSize);
  
  void* new_ptr = root_->Realloc(ptr, kNewSize, type_name);
  ASSERT_NE(new_ptr, nullptr);
  
  memset(static_cast<char*>(new_ptr) + kInitialSize, 0xEE, 
         kNewSize - kInitialSize);
  
  root_->Free(new_ptr);
}

TEST_F(PartitionRootTest, ReallocShrink) {
  constexpr size_t kInitialSize = 512;
  constexpr size_t kNewSize = 128;
  
  void* ptr = root_->Alloc(kInitialSize);
  ASSERT_NE(ptr, nullptr);
  
  memset(ptr, 0xFF, kInitialSize);
  
  void* new_ptr = root_->Realloc(ptr, kNewSize, type_name);
  ASSERT_NE(new_ptr, nullptr);
  
  root_->Free(new_ptr);
}

TEST_F(PartitionRootTest, ReallocNullPtr) {
  constexpr size_t kAllocSize = 100;
  void* ptr = root_->Realloc(nullptr, kAllocSize, type_name);
  ASSERT_NE(ptr, nullptr);
  root_->Free(ptr);
}

TEST_F(PartitionRootTest, GetUsableSize) {
  constexpr size_t kAllocSize = 100;
  void* ptr = root_->Alloc(kAllocSize);
  ASSERT_NE(ptr, nullptr);
  
  size_t usable_size = PartitionRoot::GetUsableSize(ptr);
  EXPECT_GE(usable_size, kAllocSize);
  
  root_->Free(ptr);
}

TEST_F(PartitionRootTest, PurgeMemoryDecommitEmpty) {
  constexpr size_t kAllocSize = 128;
  constexpr int kNumAllocs = 10;
  
  std::vector<void*> allocations;
  for (int i = 0; i < kNumAllocs; ++i) {
    void* ptr = root_->Alloc(kAllocSize);
    ASSERT_NE(ptr, nullptr);
    allocations.push_back(ptr);
  }
  
  for (void* ptr : allocations) {
    root_->Free(ptr);
  }
  
  root_->PurgeMemory(PurgeFlags::kDecommitEmptySlotSpans);
}

TEST_F(PartitionRootTest, PurgeMemoryDiscardUnused) {
  constexpr size_t kAllocSize = 1024;
  constexpr int kNumAllocs = 5;
  
  std::vector<void*> allocations;
  for (int i = 0; i < kNumAllocs; ++i) {
    void* ptr = root_->Alloc(kAllocSize);
    ASSERT_NE(ptr, nullptr);
    allocations.push_back(ptr);
  }
  
  for (void* ptr : allocations) {
    root_->Free(ptr);
  }
  
  root_->PurgeMemory(PurgeFlags::kDiscardUnusedSystemPages);
}

TEST_F(PartitionRootTest, PurgeMemoryAggressive) {
  constexpr size_t kAllocSize = 512;
  constexpr int kNumAllocs = 20;
  
  std::vector<void*> allocations;
  for (int i = 0; i < kNumAllocs; ++i) {
    void* ptr = root_->Alloc(kAllocSize);
    ASSERT_NE(ptr, nullptr);
    allocations.push_back(ptr);
  }
  
  for (void* ptr : allocations) {
    root_->Free(ptr);
  }
  
  root_->PurgeMemory(PurgeFlags::kAggressiveReclaim);
}

TEST_F(PartitionRootTest, DecommitEmptySlotSpansForTesting) {
  constexpr size_t kAllocSize = 256;
  constexpr int kNumAllocs = 15;
  
  std::vector<void*> allocations;
  for (int i = 0; i < kNumAllocs; ++i) {
    void* ptr = root_->Alloc(kAllocSize);
    ASSERT_NE(ptr, nullptr);
    allocations.push_back(ptr);
  }
  
  for (void* ptr : allocations) {
    root_->Free(ptr);
  }
  
  root_->DecommitEmptySlotSpansForTesting();
}

TEST_F(PartitionRootTest, ResetForTesting) {
  constexpr size_t kAllocSize = 128;
  void* ptr = root_->Alloc(kAllocSize);
  ASSERT_NE(ptr, nullptr);
  root_->Free(ptr);
  
  root_->ResetForTesting(true);
  
  ptr = root_->Alloc(kAllocSize);
  ASSERT_NE(ptr, nullptr);
  root_->Free(ptr);
}

TEST_F(PartitionRootTest, ResetBookkeepingForTesting) {
  constexpr size_t kAllocSize = 256;
  
  void* ptr1 = root_->Alloc(kAllocSize);
  ASSERT_NE(ptr1, nullptr);
  
  size_t max_allocated_before = root_->get_max_size_of_allocated_bytes();
  
  root_->Free(ptr1);
  root_->ResetBookkeepingForTesting();
  
  size_t max_allocated_after = root_->get_max_size_of_allocated_bytes();
  EXPECT_LE(max_allocated_after, max_allocated_before);
}

class PartitionRootStatsDumperTest : public PartitionStatsDumper {
 public:
  PartitionRootStatsDumperTest() = default;
  
  void PartitionsDumpBucketStats(const char* partition_name,
                                const PartitionBucketMemoryStats*) override {
    partition_names_.push_back(partition_name);
    bucket_stats_count_++;
  }
  
  void PartitionDumpTotals(const char* partition_name,
                          const PartitionMemoryStats*) override {
    total_stats_count_++;
    total_partition_name_ = partition_name;
  }
  
  int bucket_stats_count() const { return bucket_stats_count_; }
  int total_stats_count() const { return total_stats_count_; }
  const std::string& total_partition_name() const { 
    return total_partition_name_; 
  }
  
 private:
  int bucket_stats_count_ = 0;
  int total_stats_count_ = 0;
  std::string total_partition_name_;
  std::vector<std::string> partition_names_;
};

TEST_F(PartitionRootTest, DumpStatsLight) {
  constexpr size_t kAllocSize = 128;
  void* ptr = root_->Alloc(kAllocSize);
  ASSERT_NE(ptr, nullptr);
  
  auto dumper = std::make_unique<PartitionRootStatsDumperTest>();
  root_->DumpStats("test_partition", true, dumper.get());
  
  EXPECT_EQ(dumper->bucket_stats_count(), 0);
  EXPECT_EQ(dumper->total_stats_count(), 1);
  EXPECT_EQ(dumper->total_partition_name(), "test_partition");
  
  root_->Free(ptr);
}

TEST_F(PartitionRootTest, DumpStatsDetailed) {
  constexpr size_t kAllocSize = 256;
  constexpr int kNumAllocs = 10;
  
  std::vector<void*> allocations;
  for (int i = 0; i < kNumAllocs; ++i) {
    void* ptr = root_->Alloc(kAllocSize);
    ASSERT_NE(ptr, nullptr);
    allocations.push_back(ptr);
  }
  
  auto dumper = std::make_unique<PartitionRootStatsDumperTest>();
  root_->DumpStats("test_partition_detailed", false, dumper.get());
  
  EXPECT_GT(dumper->bucket_stats_count(), 0);
  EXPECT_EQ(dumper->total_stats_count(), 1);
  
  for (void* ptr : allocations) {
    root_->Free(ptr);
  }
}

TEST_F(PartitionRootTest, AllocationStressTest) {
  constexpr size_t kNumIterations = 1000;
  constexpr size_t kMaxAllocSize = 1024;
  
  std::vector<void*> allocations;
  
  for (size_t i = 0; i < kNumIterations; ++i) {
    size_t size = (i % kMaxAllocSize) + 1;
    void* ptr = root_->Alloc(size);
    ASSERT_NE(ptr, nullptr);
    
    if (i % 2 == 0) {
      allocations.push_back(ptr);
    } else {
      root_->Free(ptr);
    }
  }
  
  for (void* ptr : allocations) {
    root_->Free(ptr);
  }
}

TEST_F(PartitionRootTest, EnableLargeEmptySlotSpanRing) {
  root_->EnableLargeEmptySlotSpanRing();
  
  constexpr size_t kAllocSize = 128;
  constexpr int kNumAllocs = 50;
  
  std::vector<void*> allocations;
  for (int i = 0; i < kNumAllocs; ++i) {
    void* ptr = root_->Alloc(kAllocSize);
    ASSERT_NE(ptr, nullptr);
    allocations.push_back(ptr);
  }
  
  for (void* ptr : allocations) {
    root_->Free(ptr);
  }
}

TEST_F(PartitionRootTest, AdjustForForeground) {
  root_->AdjustForForeground();
  
  constexpr size_t kAllocSize = 256;
  void* ptr = root_->Alloc(kAllocSize);
  ASSERT_NE(ptr, nullptr);
  root_->Free(ptr);
}

TEST_F(PartitionRootTest, AdjustForBackground) {
  root_->AdjustForBackground();
  
  constexpr size_t kAllocSize = 256;
  void* ptr = root_->Alloc(kAllocSize);
  ASSERT_NE(ptr, nullptr);
  root_->Free(ptr);
}

TEST_F(PartitionRootTest, UncapEmptySlotSpanMemoryForTesting) {
  root_->UncapEmptySlotSpanMemoryForTesting();
  
  constexpr size_t kAllocSize = 128;
  constexpr int kNumAllocs = 20;
  
  std::vector<void*> allocations;
  for (int i = 0; i < kNumAllocs; ++i) {
    void* ptr = root_->Alloc(kAllocSize);
    ASSERT_NE(ptr, nullptr);
    allocations.push_back(ptr);
  }
  
  for (void* ptr : allocations) {
    root_->Free(ptr);
  }
}

TEST_F(PartitionRootTest, SetGlobalEmptySlotSpanRingIndexForTesting) {
  root_->SetGlobalEmptySlotSpanRingIndexForTesting(5);
  
  constexpr size_t kAllocSize = 64;
  void* ptr = root_->Alloc(kAllocSize);
  ASSERT_NE(ptr, nullptr);
  root_->Free(ptr);
}

TEST_F(PartitionRootTest, MultiplePartitions) {
  PartitionOptions opts1;
  PartitionOptions opts2;
  auto allocator1 = std::make_unique<PartitionAllocator>(opts1);
  auto allocator2 = std::make_unique<PartitionAllocator>(opts2);
  
  PartitionRoot* root1 = allocator1->root();
  PartitionRoot* root2 = allocator2->root();
  
  constexpr size_t kAllocSize = 128;
  
  void* ptr1 = root1->Alloc(kAllocSize);
  void* ptr2 = root2->Alloc(kAllocSize);
  
  ASSERT_NE(ptr1, nullptr);
  ASSERT_NE(ptr2, nullptr);
  ASSERT_NE(ptr1, ptr2);
  
  root1->Free(ptr1);
  root2->Free(ptr2);
  
  allocator1->root()->ResetForTesting(true);
  allocator2->root()->ResetForTesting(true);
}

}  // namespace partition_alloc

#endif  // !defined(MEMORY_TOOL_REPLACES_ALLOCATOR)