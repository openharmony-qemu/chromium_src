// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <inttypes.h>

#include <utility>

#include "base/auto_reset.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/metrics/histogram_macros.h"
#include "base/strings/stringprintf.h"
#include "base/system/sys_info.h"
#include "base/task/single_thread_task_runner.h"
#include "base/trace_event/memory_dump_manager.h"
#include "cc/paint/image_transfer_cache_entry.h"
#include "gpu/command_buffer/service/service_discardable_manager.h"
#include "gpu/config/gpu_finch_features.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/gpu/ganesh/GrBackendSurface.h"
#include "third_party/skia/include/gpu/ganesh/SkImageGanesh.h"
#include "third_party/skia/include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "third_party/skia/include/gpu/ganesh/gl/GrGLTypes.h"
#include "ui/gl/trace_util.h"
#include "arkweb/chromium_ext/gpu/command_buffer/service/service_transfer_cache_ext.h"

namespace gpu {

ServiceTransferCacheExt::ServiceTransferCacheExt(
    const GpuPreferences& preferences,
    base::RepeatingClosure flush_callback)
    : ServiceTransferCache(preferences, std::move(flush_callback)) {

}

#if BUILDFLAG(ARKWEB_HEIF_SUPPORT)
bool ServiceTransferCacheExt::CreateLockedRGBAHardwareDecodedImageEntry(
    int decoder_id,
    uint32_t entry_id,
    ServiceDiscardableHandle handle,
    GrDirectContext* context,
    std::vector<sk_sp<SkImage>> plane_images,
    size_t buffer_byte_size) {
  EntryKey key(decoder_id, cc::TransferCacheEntryType::kImage, entry_id);
  auto found = entries_.Peek(key);
  if (found != entries_.end()) {
    return false;
  }

  // Create the service-side image transfer cache entry.
  auto entry = std::make_unique<cc::ServiceImageTransferCacheEntry>();
  if (!entry->BuildFromRGBAHardwareDecodedImage(
          context, std::move(plane_images), buffer_byte_size)) {
    return false;
  }

  // Insert it in the transfer cache.
  total_size_ += entry->CachedSize();
  if (key.entry_type == cc::TransferCacheEntryType::kImage) {
    total_image_count_++;
    total_image_size_ += entry->CachedSize();
  }
  entries_.Put(key, CacheEntryInternal(handle, std::move(entry)));
  EnforceLimits();
  return true;
}
#endif

}