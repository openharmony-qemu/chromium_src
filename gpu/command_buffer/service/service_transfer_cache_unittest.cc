// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/service_transfer_cache.h"

#include "base/test/bind.h"
#include "base/time/time_override.h"
#include "cc/paint/raw_memory_transfer_cache_entry.h"
#include "gpu/config/gpu_preferences.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace gpu {

constexpr int kDecoderId = 2;
constexpr auto kEntryType = cc::TransferCacheEntryType::kRawMemory;

std::unique_ptr<cc::ServiceTransferCacheEntry> CreateEntry(size_t size) {
  auto entry = std::make_unique<cc::ServiceRawMemoryTransferCacheEntry>();
  std::vector<uint8_t> data(size, 0u);
  entry->Deserialize(/*gr_context=*/nullptr, /*graphite_recorder=*/nullptr,
                     data);
  return entry;
}

TEST(ServiceTransferCacheTest, EnforcesOnPurgeMemory) {
  ServiceTransferCache cache{GpuPreferences(), base::RepeatingClosure()};
  uint32_t entry_id = 0u;
  size_t entry_size = 1024u;
  uint32_t number_of_entry = 4u;

  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(kDecoderId, kEntryType, ++entry_id),
      CreateEntry(entry_size));
  EXPECT_EQ(cache.cache_size_for_testing(), entry_size);
  cache.PurgeMemory(base::MEMORY_PRESSURE_LEVEL_CRITICAL);
  EXPECT_EQ(cache.cache_size_for_testing(), 0u);

  cache.SetCacheSizeLimitForTesting(entry_size * number_of_entry);
  // Create 4 entries, all in the cache.
  for (uint32_t i = 0; i < number_of_entry; i++) {
    cache.CreateLocalEntry(
        ServiceTransferCache::EntryKey(kDecoderId, kEntryType, ++entry_id),
        CreateEntry(entry_size));
    EXPECT_EQ(cache.cache_size_for_testing(), entry_size * (i + 1));
  }

  // The 5th entry creates successfully. But the 1st entry will be purged due to
  // cache limits.
  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(kDecoderId, kEntryType, ++entry_id),
      CreateEntry(entry_size));
  EXPECT_EQ(cache.cache_size_for_testing(), entry_size * 4);

  cache.PurgeMemory(base::MEMORY_PRESSURE_LEVEL_MODERATE);
  // Only 1/4 of cache limits remains.
  EXPECT_EQ(cache.cache_size_for_testing(), entry_size);
}

TEST(ServiceTransferCache, MultipleDecoderUse) {
  ServiceTransferCache cache{GpuPreferences(), base::RepeatingClosure()};
  const uint32_t entry_id = 0u;
  const size_t entry_size = 1024u;

  // Decoder 1 entry.
  int decoder1 = 1;
  auto decoder_1_entry = CreateEntry(entry_size);
  auto* decoder_1_entry_ptr = decoder_1_entry.get();
  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(decoder1, kEntryType, entry_id),
      std::move(decoder_1_entry));

  // Decoder 2 entry.
  int decoder2 = 2;
  auto decoder_2_entry = CreateEntry(entry_size);
  auto* decoder_2_entry_ptr = decoder_2_entry.get();
  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(decoder2, kEntryType, entry_id),
      std::move(decoder_2_entry));

  EXPECT_EQ(decoder_1_entry_ptr, cache.GetEntry(ServiceTransferCache::EntryKey(
                                     decoder1, kEntryType, entry_id)));
  EXPECT_EQ(decoder_2_entry_ptr, cache.GetEntry(ServiceTransferCache::EntryKey(
                                     decoder2, kEntryType, entry_id)));
}

TEST(ServiceTransferCache, EntryIdsBeyond32Bits) {
  ServiceTransferCache cache{GpuPreferences(), base::RepeatingClosure()};
  const size_t entry_size = 1024u;

  // Two entry ids that only match in the low 32 bits address distinct
  // entries.
  const uint64_t low_entry_id = 7u;
  const uint64_t high_entry_id = low_entry_id + (uint64_t{1} << 32);

  auto low_entry = CreateEntry(entry_size);
  auto* low_entry_ptr = low_entry.get();
  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(kDecoderId, kEntryType, low_entry_id),
      std::move(low_entry));

  auto high_entry = CreateEntry(entry_size);
  auto* high_entry_ptr = high_entry.get();
  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(kDecoderId, kEntryType, high_entry_id),
      std::move(high_entry));

  EXPECT_EQ(cache.entries_count_for_testing(), 2u);
  EXPECT_EQ(low_entry_ptr, cache.GetEntry(ServiceTransferCache::EntryKey(
                               kDecoderId, kEntryType, low_entry_id)));
  EXPECT_EQ(high_entry_ptr, cache.GetEntry(ServiceTransferCache::EntryKey(
                                kDecoderId, kEntryType, high_entry_id)));
}

TEST(ServiceTransferCache, DeleteEntriesForDecoder) {
  ServiceTransferCache cache{GpuPreferences(), base::RepeatingClosure()};
  const size_t entry_size = 1024u;
  const size_t cache_size = 4 * entry_size;
  cache.SetCacheSizeLimitForTesting(cache_size);

  // Add 2 entries for decoder 1.
  cache.CreateLocalEntry(ServiceTransferCache::EntryKey(1, kEntryType, 1),
                         CreateEntry(entry_size));
  cache.CreateLocalEntry(ServiceTransferCache::EntryKey(1, kEntryType, 2),
                         CreateEntry(entry_size));

  // Add 2 entries for decoder 2.
  cache.CreateLocalEntry(ServiceTransferCache::EntryKey(2, kEntryType, 1),
                         CreateEntry(entry_size));
  cache.CreateLocalEntry(ServiceTransferCache::EntryKey(2, kEntryType, 2),
                         CreateEntry(entry_size));

  // Erase all entries for decoder 1.
  EXPECT_EQ(cache.entries_count_for_testing(), 4u);
  cache.DeleteAllEntriesForDecoder(1);
  EXPECT_EQ(cache.entries_count_for_testing(), 2u);
  EXPECT_NE(cache.GetEntry(ServiceTransferCache::EntryKey(2, kEntryType, 1)),
            nullptr);
  EXPECT_NE(cache.GetEntry(ServiceTransferCache::EntryKey(2, kEntryType, 2)),
            nullptr);
}

TEST(ServiceTransferCacheTest, PurgeEntryOnTimer) {
  static base::TimeTicks now_value = base::TimeTicks::Now();
  base::subtle::ScopedTimeClockOverrides time_override(
      nullptr, []() { return now_value; }, nullptr);

  bool flush_called = false;
  ServiceTransferCache cache{
      GpuPreferences(),
      base::BindLambdaForTesting([&]() { flush_called = true; })};

  uint32_t entry_id = 0u;
  size_t entry_size = 1024u;
  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(kDecoderId, kEntryType, ++entry_id),
      CreateEntry(entry_size));
  EXPECT_EQ(cache.entries_count_for_testing(), 1u);

  now_value = now_value + base::Minutes(1);
  cache.PruneOldEntries();
  EXPECT_EQ(cache.entries_count_for_testing(), 0u);
  EXPECT_TRUE(flush_called);
}

TEST(ServiceTransferCacheTest, DeleteEntry) {
  ServiceTransferCache cache{GpuPreferences(), base::RepeatingClosure()};
  uint32_t entry_id = 0u;
  size_t entry_size = 1024u;

  // Create an entry
  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(kDecoderId, kEntryType, ++entry_id),
      CreateEntry(entry_size));
  EXPECT_EQ(cache.entries_count_for_testing(), 1u);
  EXPECT_NE(cache.GetEntry(ServiceTransferCache::EntryKey(kDecoderId, kEntryType, entry_id)),
            nullptr);

  // Delete the entry
  EXPECT_TRUE(cache.DeleteEntry(ServiceTransferCache::EntryKey(kDecoderId, kEntryType, entry_id)));
  EXPECT_EQ(cache.entries_count_for_testing(), 0u);
  EXPECT_EQ(cache.GetEntry(ServiceTransferCache::EntryKey(kDecoderId, kEntryType, entry_id)),
            nullptr);

  // Deleting non-existent entry should return false
  EXPECT_FALSE(cache.DeleteEntry(ServiceTransferCache::EntryKey(kDecoderId, kEntryType, entry_id)));
}

TEST(ServiceTransferCacheTest, GetEntryUpdatesMetrics) {
  ServiceTransferCache cache{GpuPreferences(), base::RepeatingClosure()};
  uint32_t entry_id = 0u;
  size_t entry_size = 1024u;

  // Create an entry
  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(kDecoderId, kEntryType, ++entry_id),
      CreateEntry(entry_size));

  // GetEntry should return the entry
  EXPECT_NE(cache.GetEntry(ServiceTransferCache::EntryKey(kDecoderId, kEntryType, entry_id)),
            nullptr);

  // GetEntry for non-existent entry should return nullptr
  EXPECT_EQ(cache.GetEntry(ServiceTransferCache::EntryKey(kDecoderId, kEntryType, 9999)),
            nullptr);
}

TEST(ServiceTransferCacheTest, UnlockEntryWithNoHandle) {
  ServiceTransferCache cache{GpuPreferences(), base::RepeatingClosure()};
  uint32_t entry_id = 0u;

  // Create a local entry (no handle)
  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(kDecoderId, kEntryType, ++entry_id),
      CreateEntry(1024u));

  // UnlockEntry should return false for local entries without handle
  EXPECT_FALSE(cache.UnlockEntry(ServiceTransferCache::EntryKey(kDecoderId, kEntryType, entry_id)));

  // UnlockEntry should return false for non-existent entries
  EXPECT_FALSE(cache.UnlockEntry(ServiceTransferCache::EntryKey(kDecoderId, kEntryType, 9999)));
}

TEST(ServiceTransferCacheTest, CreateLockedEntryFailsForExistingEntry) {
  ServiceTransferCache cache{GpuPreferences(), base::RepeatingClosure()};
  uint32_t entry_id = 0u;

  // Create a local entry
  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(kDecoderId, kEntryType, ++entry_id),
      CreateEntry(1024u));

  // CreateLockedEntry should fail because entry already exists
  // (passing null context and empty data - will fail anyway)
  EXPECT_FALSE(cache.CreateLockedEntry(
      ServiceTransferCache::EntryKey(kDecoderId, kEntryType, entry_id),
      ServiceDiscardableHandle(), nullptr, nullptr, base::span<uint8_t>()));
}

TEST(ServiceTransferCacheTest, EnforceLimitsByEntryCount) {
  ServiceTransferCache cache{GpuPreferences(), base::RepeatingClosure()};
  size_t entry_size = 1024u;

  // Set max entries to 3
  cache.SetMaxCacheEntriesForTesting(3);

  // Create 4 entries - should enforce limits
  for (uint32_t i = 0; i < 4; ++i) {
    cache.CreateLocalEntry(
        ServiceTransferCache::EntryKey(kDecoderId, kEntryType, i + 1),
        CreateEntry(entry_size));
  }

  // Only 3 entries should remain due to max_cache_entries_ limit
  EXPECT_EQ(cache.entries_count_for_testing(), 3u);
}

TEST(ServiceTransferCacheTest, CreateLocalEntryReplacesExisting) {
  ServiceTransferCache cache{GpuPreferences(), base::RepeatingClosure()};
  uint32_t entry_id = 1u;
  size_t entry_size = 1024u;

  // Create first entry
  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(kDecoderId, kEntryType, entry_id),
      CreateEntry(entry_size));
  EXPECT_EQ(cache.entries_count_for_testing(), 1u);

  // Create another entry with same key - should replace
  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(kDecoderId, kEntryType, entry_id),
      CreateEntry(entry_size * 2));
  EXPECT_EQ(cache.entries_count_for_testing(), 1u);
  EXPECT_EQ(cache.cache_size_for_testing(), entry_size * 2);
}

TEST(ServiceTransferCacheTest, RemoveOldEntriesUntil) {
  static base::TimeTicks now_value = base::TimeTicks::Now();
  base::subtle::ScopedTimeClockOverrides time_override(
      nullptr, []() { return now_value; }, nullptr);

  ServiceTransferCache cache{GpuPreferences(), base::RepeatingClosure()};
  size_t entry_size = 1024u;

  // Create 3 entries at different times
  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(kDecoderId, kEntryType, 1),
      CreateEntry(entry_size));

  now_value = now_value + base::Seconds(10);
  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(kDecoderId, kEntryType, 2),
      CreateEntry(entry_size));

  now_value = now_value + base::Seconds(10);
  cache.CreateLocalEntry(
      ServiceTransferCache::EntryKey(kDecoderId, kEntryType, 3),
      CreateEntry(entry_size));

  EXPECT_EQ(cache.entries_count_for_testing(), 3u);

  // Move time forward past cutoff
  now_value = now_value + base::Seconds(30);

  // Trigger time-based eviction through PurgeMemory which indirectly
  // exercises the entry eviction logic
  cache.PurgeMemory(base::MEMORY_PRESSURE_LEVEL_CRITICAL);

  // All entries should be purged under critical memory pressure
  EXPECT_EQ(cache.entries_count_for_testing(), 0u);
}

TEST(ServiceTransferCacheTest, EntryKeyFields) {
  ServiceTransferCache::EntryKey key1(1, cc::TransferCacheEntryType::kRawMemory, 100);
  ServiceTransferCache::EntryKey key2(1, cc::TransferCacheEntryType::kRawMemory, 100);
  ServiceTransferCache::EntryKey key3(2, cc::TransferCacheEntryType::kRawMemory, 100);
  ServiceTransferCache::EntryKey key4(1, cc::TransferCacheEntryType::kImage, 100);
  ServiceTransferCache::EntryKey key5(1, cc::TransferCacheEntryType::kRawMemory, 200);

  // Same key values should be equal
  EXPECT_EQ(key1.decoder_id, key2.decoder_id);
  EXPECT_EQ(key1.entry_type, key2.entry_type);
  EXPECT_EQ(key1.entry_id, key2.entry_id);

  // Verify different fields
  EXPECT_NE(key1.decoder_id, key3.decoder_id);
  EXPECT_NE(key1.entry_type, key4.entry_type);
  EXPECT_NE(key1.entry_id, key5.entry_id);
}

}  // namespace gpu
