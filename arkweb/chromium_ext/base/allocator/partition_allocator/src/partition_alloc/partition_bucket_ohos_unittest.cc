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

namespace partition_alloc::internal {
namespace {
const char* TYPENAME = nullptr;
}
class PartitionBucketTest : public ::testing::Test {
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

TEST_F(PartitionBucketTest, ComputeSystemPagesPerSlotSpanSmallSlots) {
  constexpr size_t kSmallSlotSize = 64;
  uint8_t pages = ComputeSystemPagesPerSlotSpan(kSmallSlotSize, false);
  EXPECT_GT(pages, 0);
  EXPECT_LE(pages, 255);
}

TEST_F(PartitionBucketTest, ComputeSystemPagesPerSlotSpanMediumSlots) {
  constexpr size_t kMediumSlotSize = 512;
  uint8_t pages = ComputeSystemPagesPerSlotSpan(kMediumSlotSize, false);
  EXPECT_GT(pages, 0);
  EXPECT_LE(pages, 255);
}

TEST_F(PartitionBucketTest, ComputeSystemPagesPerSlotSpanLargeSlots) {
  constexpr size_t kLargeSlotSize = 4096;
  uint8_t pages = ComputeSystemPagesPerSlotSpan(kLargeSlotSize, false);
  EXPECT_GT(pages, 0);
  EXPECT_LE(pages, 255);
}

TEST_F(PartitionBucketTest, ComputeSystemPagesPerSlotSpanPreferSmaller) {
  constexpr size_t kSlotSize = 256;
  uint8_t pages_normal = ComputeSystemPagesPerSlotSpan(kSlotSize, false);
  uint8_t pages_prefer_small = ComputeSystemPagesPerSlotSpan(kSlotSize, true);
  
  EXPECT_GT(pages_normal, 0);
  EXPECT_GT(pages_prefer_small, 0);
  EXPECT_LE(pages_prefer_small, pages_normal);
}

TEST_F(PartitionBucketTest, BucketInit) {
  PartitionBucket bucket;
  constexpr size_t kSlotSize = 128;
  constexpr bool kUseSmallSingleSlotSpans = false;
  
  bucket.Init(kSlotSize, kUseSmallSingleSlotSpans);
  
  EXPECT_EQ(bucket.slot_size, kSlotSize);
  EXPECT_GT(bucket.slot_size_reciprocal, 0);
  EXPECT_NE(bucket.active_slot_spans_head, nullptr);
  EXPECT_EQ(bucket.empty_slot_spans_head, nullptr);
  EXPECT_EQ(bucket.decommitted_slot_spans_head, nullptr);
  EXPECT_EQ(bucket.num_full_slot_spans, 0);
  EXPECT_GT(bucket.num_system_pages_per_slot_span, 0);
}

TEST_F(PartitionBucketTest, BucketInitWithSmallSingleSlotSpans) {
  PartitionBucket bucket;
  constexpr size_t kSlotSize = 64;
  constexpr bool kUseSmallSingleSlotSpans = true;
  
  bucket.Init(kSlotSize, kUseSmallSingleSlotSpans);
  
  EXPECT_EQ(bucket.slot_size, kSlotSize);
  EXPECT_GT(bucket.num_system_pages_per_slot_span, 0);
}

TEST_F(PartitionBucketTest, BucketInitCanStoreRawSize) {
  PartitionBucket bucket;
  constexpr size_t kSmallSlotSize = 32;
  constexpr bool kUseSmallSingleSlotSpans = true;
  
  bucket.Init(kSmallSlotSize, kUseSmallSingleSlotSpans);
  
  EXPECT_EQ(bucket.slot_size, kSmallSlotSize);
  EXPECT_GT(bucket.num_system_pages_per_slot_span, 0);
}

TEST_F(PartitionBucketTest, BucketIsValid) {
  PartitionBucket bucket;
  constexpr size_t kSlotSize = 256;
  
  bucket.Init(kSlotSize, false);
  EXPECT_TRUE(bucket.is_valid());
}

TEST_F(PartitionBucketTest, BucketIsNotDirectMapped) {
  PartitionBucket bucket;
  constexpr size_t kSlotSize = 128;
  
  bucket.Init(kSlotSize, false);
  EXPECT_FALSE(bucket.is_direct_mapped());
}

TEST_F(PartitionBucketTest, BucketGetBytesPerSpan) {
  PartitionBucket bucket;
  constexpr size_t kSlotSize = 512;
  
  bucket.Init(kSlotSize, false);
  
  size_t bytes_per_span = bucket.get_bytes_per_span();
  EXPECT_GT(bytes_per_span, 0);
  EXPECT_GE(bytes_per_span, kSlotSize);
}

TEST_F(PartitionBucketTest, BucketGetSlotsPerSpan) {
  PartitionBucket bucket;
  constexpr size_t kSlotSize = 256;
  
  bucket.Init(kSlotSize, false);
  
  size_t slots_per_span = bucket.get_slots_per_span();
  EXPECT_GT(slots_per_span, 0);
  EXPECT_LE(slots_per_span * kSlotSize, bucket.get_bytes_per_span());
}

TEST_F(PartitionBucketTest, BucketGetPagesPerSlotSpan) {
  PartitionBucket bucket;
  constexpr size_t kSlotSize = 1024;
  
  bucket.Init(kSlotSize, false);
  
  size_t pages_per_span = bucket.get_pages_per_slot_span();
  EXPECT_GT(pages_per_span, 0);
}

TEST_F(PartitionBucketTest, BucketGetSlotNumber) {
  PartitionBucket bucket;
  constexpr size_t kSlotSize = 128;
  
  bucket.Init(kSlotSize, false);
  
  size_t offset = kSlotSize * 3;
  size_t slot_number = bucket.GetSlotNumber(offset);
  EXPECT_EQ(slot_number, 3);
  
  offset = kSlotSize * 10;
  slot_number = bucket.GetSlotNumber(offset);
  EXPECT_EQ(slot_number, 10);
}

TEST_F(PartitionBucketTest, BucketCanStoreRawSize) {
  PartitionBucket bucket;
  constexpr size_t kSlotSize = 64;
  
  bucket.Init(kSlotSize, true);
  
  bool can_store = bucket.CanStoreRawSize();
  EXPECT_FALSE(can_store);
}

TEST_F(PartitionBucketTest, AllocationThroughBucket) {
  constexpr size_t kAllocSize = 100;
  constexpr int kNumAllocs = 50;
  
  std::vector<void*> allocations;
  allocations.reserve(kNumAllocs);
  
  for (int i = 0; i < kNumAllocs; ++i) {
    void* ptr = root_->Alloc(kAllocSize);
    ASSERT_NE(ptr, nullptr);
    memset(ptr, static_cast<int>(i & 0xFF), kAllocSize);
    allocations.push_back(ptr);
  }
  
  for (void* ptr : allocations) {
    root_->Free(ptr);
  }
}

TEST_F(PartitionBucketTest, AllocationDifferentSizes) {
  std::vector<size_t> sizes = {32, 64, 128, 256, 512, 1024};
  
  for (size_t size : sizes) {
    void* ptr = root_->Alloc(size);
    ASSERT_NE(ptr, nullptr);
    memset(ptr, 0xAA, size);
    root_->Free(ptr);
  }
}

TEST_F(PartitionBucketTest, AllocationFreeCycle) {
  constexpr size_t kAllocSize = 256;
  constexpr int kIterations = 100;
  
  for (int i = 0; i < kIterations; ++i) {
    void* ptr = root_->Alloc(kAllocSize);
    ASSERT_NE(ptr, nullptr);
    memset(ptr, 0xBB, kAllocSize);
    root_->Free(ptr);
  }
}

TEST_F(PartitionBucketTest, MultipleBucketsAllocation) {
  std::vector<size_t> sizes = {32, 64, 128, 256, 512};
  std::vector<std::vector<void*>> allocations_by_size;
  
  for (size_t size : sizes) {
    std::vector<void*> allocations;
    for (int i = 0; i < 20; ++i) {
      void* ptr = root_->Alloc(size);
      ASSERT_NE(ptr, nullptr);
      allocations.push_back(ptr);
    }
    allocations_by_size.push_back(allocations);
  }
  
  for (auto& allocations : allocations_by_size) {
    for (void* ptr : allocations) {
      root_->Free(ptr);
    }
  }
}

TEST_F(PartitionBucketTest, LargeAllocationThroughBucket) {
  constexpr size_t kLargeSize = 1024 * 1024;
  
  void* ptr = root_->Alloc(kLargeSize);
  ASSERT_NE(ptr, nullptr);
  
  memset(ptr, 0xCC, kLargeSize);
  root_->Free(ptr);
}

TEST_F(PartitionBucketTest, VerySmallAllocation) {
  constexpr size_t kTinySize = 8;
  
  void* ptr = root_->Alloc(kTinySize);
  ASSERT_NE(ptr, nullptr);
  
  memset(ptr, 0xDD, kTinySize);
  root_->Free(ptr);
}

TEST_F(PartitionBucketTest, ReuseFreedSlots) {
  constexpr size_t kAllocSize = 128;
  constexpr int kNumAllocs = 30;
  
  std::vector<void*> first_allocations;
  for (int i = 0; i < kNumAllocs; ++i) {
    void* ptr = root_->Alloc(kAllocSize);
    ASSERT_NE(ptr, nullptr);
    first_allocations.push_back(ptr);
  }
  
  for (void* ptr : first_allocations) {
    root_->Free(ptr);
  }
  
  std::vector<void*> second_allocations;
  for (int i = 0; i < kNumAllocs; ++i) {
    void* ptr = root_->Alloc(kAllocSize);
    ASSERT_NE(ptr, nullptr);
    second_allocations.push_back(ptr);
  }
  
  for (void* ptr : second_allocations) {
    root_->Free(ptr);
  }
}

TEST_F(PartitionBucketTest, MixedAllocationSizes) {
  std::vector<size_t> sizes = {32, 64, 128, 256, 512, 1024, 2048};
  std::vector<void*> allocations;
  
  for (int i = 0; i < 100; ++i) {
    size_t size = sizes[i % sizes.size()];
    void* ptr = root_->Alloc(size);
    ASSERT_NE(ptr, nullptr);
    allocations.push_back(ptr);
  }
  
  for (void* ptr : allocations) {
    root_->Free(ptr);
  }
}

TEST_F(PartitionBucketTest, AllocationStressTest) {
  constexpr int kIterations = 500;
  constexpr size_t kMaxSize = 2048;
  
  std::vector<void*> allocations;
  
  for (int i = 0; i < kIterations; ++i) {
    size_t size = (i % kMaxSize) + 1;
    void* ptr = root_->Alloc(size);
    ASSERT_NE(ptr, nullptr);
    
    if (i % 3 == 0) {
      allocations.push_back(ptr);
    } else {
      root_->Free(ptr);
    }
  }
  
  for (void* ptr : allocations) {
    root_->Free(ptr);
  }
}

TEST_F(PartitionBucketTest, BucketMemoryPressure) {
  constexpr size_t kAllocSize = 1024;
  constexpr int kNumAllocs = 100;
  
  std::vector<void*> allocations;
  for (int i = 0; i < kNumAllocs; ++i) {
    void* ptr = root_->Alloc(kAllocSize);
    ASSERT_NE(ptr, nullptr);
    allocations.push_back(ptr);
  }
  
  for (int i = 0; i < kNumAllocs / 2; ++i) {
    root_->Free(allocations[i]);
  }
  
  for (int i = kNumAllocs / 2; i < kNumAllocs; ++i) {
    void* ptr = root_->Alloc(kAllocSize);
    ASSERT_NE(ptr, nullptr);
    root_->Free(ptr);
  }
}

TEST_F(PartitionBucketTest, AlignedAllocationThroughBucket) {
  constexpr size_t kAllocSize = 512;
  
  void* ptr = root_->AlignedAlloc(kAlignment, kAllocSize);
  ASSERT_NE(ptr, nullptr);
  EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % kAlignment, 0);
  
  root_->Free(ptr);
}

TEST_F(PartitionBucketTest, ReallocThroughBucket) {
  constexpr size_t kInitialSize = 128;
  constexpr size_t kNewSize = 256;
  
  void* ptr = root_->Alloc(kInitialSize);
  ASSERT_NE(ptr, nullptr);
  
  memset(ptr, 0xEE, kInitialSize);
  
  void* new_ptr = root_->Realloc(ptr, kNewSize, TYPENAME);
  ASSERT_NE(new_ptr, nullptr);
  
  root_->Free(new_ptr);
}

TEST_F(PartitionBucketTest, ZeroAllocationThroughBucket) {
  void* ptr = root_->Alloc(0);
  ASSERT_NE(ptr, nullptr);
  root_->Free(ptr);
}

TEST_F(PartitionBucketTest, BucketAfterPurge) {
  constexpr size_t kAllocSize = 256;
  constexpr int kNumAllocs = 40;
  
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
  
  void* new_ptr = root_->Alloc(kAllocSize);
  ASSERT_NE(new_ptr, nullptr);
  root_->Free(new_ptr);
}

TEST_F(PartitionBucketTest, BucketMultiplePartitions) {
  PartitionOptions opts1;
  PartitionOptions opts2;
  auto allocator1 = std::make_unique<PartitionAllocator>(opts1);
  auto allocator2 = std::make_unique<PartitionAllocator>(opts2);
  
  PartitionRoot* root1 = allocator1->root();
  PartitionRoot* root2 = allocator2->root();
  
  constexpr size_t kAllocSize = 128;
  constexpr int kNumAllocs = 20;
  
  std::vector<void*> allocations1;
  std::vector<void*> allocations2;
  
  for (int i = 0; i < kNumAllocs; ++i) {
    void* ptr1 = root1->Alloc(kAllocSize);
    void* ptr2 = root2->Alloc(kAllocSize);
    
    ASSERT_NE(ptr1, nullptr);
    ASSERT_NE(ptr2, nullptr);
    ASSERT_NE(ptr1, ptr2);
    
    allocations1.push_back(ptr1);
    allocations2.push_back(ptr2);
  }
  
  for (void* ptr : allocations1) {
    root1->Free(ptr);
  }
  
  for (void* ptr : allocations2) {
    root2->Free(ptr);
  }
  
  allocator1->root()->ResetForTesting(true);
  allocator2->root()->ResetForTesting(true);
}

TEST_F(PartitionBucketTest, BucketSlotSizeBoundary) {
  std::vector<size_t> boundary_sizes = {
      1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
  
  for (size_t size : boundary_sizes) {
    void* ptr = root_->Alloc(size);
    ASSERT_NE(ptr, nullptr);
    memset(ptr, 0xFF, size);
    root_->Free(ptr);
  }
}

TEST_F(PartitionBucketTest, BucketAllocationPattern) {
  constexpr size_t kAllocSize = 512;
  constexpr int kNumCycles = 10;
  constexpr int kAllocsPerCycle = 15;
  
  for (int cycle = 0; cycle < kNumCycles; ++cycle) {
    std::vector<void*> allocations;
    
    for (int i = 0; i < kAllocsPerCycle; ++i) {
      void* ptr = root_->Alloc(kAllocSize);
      ASSERT_NE(ptr, nullptr);
      allocations.push_back(ptr);
    }
    
    for (void* ptr : allocations) {
      root_->Free(ptr);
    }
  }
}

TEST_F(PartitionBucketTest, BucketFragmentationTest) {
  std::vector<size_t> sizes = {64, 128, 256, 512};
  std::vector<std::vector<void*>> allocations_by_size;
  
  for (size_t size : sizes) {
    std::vector<void*> allocations;
    for (int i = 0; i < 30; ++i) {
      void* ptr = root_->Alloc(size);
      ASSERT_NE(ptr, nullptr);
      allocations.push_back(ptr);
    }
    allocations_by_size.push_back(allocations);
  }
  
  for (auto& allocations : allocations_by_size) {
    for (size_t i = 0; i < allocations.size(); i += 2) {
      root_->Free(allocations[i]);
    }
  }
}

TEST_F(PartitionBucketTest, BucketComputeSystemPagesPerSlotSpanVariousSizes) {
  std::vector<size_t> test_sizes = {
      8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
  
  for (size_t size : test_sizes) {
    uint8_t pages = ComputeSystemPagesPerSlotSpan(size, false);
    EXPECT_GT(pages, 0);
    EXPECT_LE(pages, 255);
  }
}

TEST_F(PartitionBucketTest, BucketSlotSizeReciprocal) {
  PartitionBucket bucket;
  constexpr size_t kSlotSize = 256;
  
  bucket.Init(kSlotSize, false);
  
  EXPECT_GT(bucket.slot_size_reciprocal, 0);
  
  size_t offset = kSlotSize * 5;
  size_t slot_number = bucket.GetSlotNumber(offset);
  EXPECT_EQ(slot_number, 5);
}

TEST_F(PartitionBucketTest, BucketGetUsableSize) {
  constexpr size_t kAllocSize = 200;
  
  void* ptr = root_->Alloc(kAllocSize);
  ASSERT_NE(ptr, nullptr);
  
  size_t usable_size = PartitionRoot::GetUsableSize(ptr);
  EXPECT_GE(usable_size, kAllocSize);
  
  root_->Free(ptr);
}

TEST_F(PartitionBucketTest, BucketDirectMapAllocation) {
  constexpr size_t kLargeSize = 2 * 1024 * 1024;
  
  void* ptr = root_->Alloc(kLargeSize);
  ASSERT_NE(ptr, nullptr);
  
  memset(ptr, 0xAA, kLargeSize);
  root_->Free(ptr);
}

}  // namespace partition_alloc::internal

#endif  // !defined(MEMORY_TOOL_REPLACES_ALLOCATOR)