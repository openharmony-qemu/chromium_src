// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/allocator/partition_allocator/internal_allocator.h"
#include "base/allocator/partition_allocator/partition_alloc_base/no_destructor.h"
namespace partition_alloc {
PA_COMPONENT_EXPORT(PARTITION_ALLOC)
PartitionRoot<internal::ThreadSafe>& InternalAllocatorRoot() {
  static partition_alloc::internal::base::NoDestructor<PartitionRoot<internal::ThreadSafe>> allocator([]() {
    // Disable features using the internal root to avoid reentrancy issue.
    constexpr PartitionOptions kConfig{
        PartitionOptions::AlignedAlloc::kDisallowed,
        PartitionOptions::ThreadCache::kDisabled,
        PartitionOptions::Quarantine::kDisallowed,
        PartitionOptions::Cookie::kAllowed,
        PartitionOptions::BackupRefPtr::kDisabled,
        PartitionOptions::BackupRefPtrZapping::kDisabled,
        PartitionOptions::UseConfigurablePool::kNo,
    };
    return kConfig;
  }());

  return *allocator;
}
}  // namespace partition_alloc::internal
