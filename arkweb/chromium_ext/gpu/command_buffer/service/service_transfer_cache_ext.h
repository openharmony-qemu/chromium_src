// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_SERVICE_TRANSFER_CACHE_EXT_H_
#define GPU_COMMAND_BUFFER_SERVICE_SERVICE_TRANSFER_CACHE_EXT_H_

#include "gpu/config/gpu_preferences.h"
#include "gpu/command_buffer/service/service_transfer_cache.h"

namespace gpu {
class ServiceTransferCache;

class ServiceTransferCacheExt : public ServiceTransferCache {
public:
  ServiceTransferCacheExt(const GpuPreferences& preferences,
                          base::RepeatingClosure flush_callback);
                          
  raw_ptr<ServiceTransferCacheExt> AsServiceTransferCacheExt() override {
    return this;
  }

#if BUILDFLAG(ARKWEB_HEIF_SUPPORT)
  bool CreateLockedRGBAHardwareDecodedImageEntry(
      int decoder_id,
      uint32_t entry_id,
      ServiceDiscardableHandle handle,
      GrDirectContext* context,
      std::vector<sk_sp<SkImage>> plane_images,
      size_t buffer_byte_size);
#endif
};

}

#endif // GPU_COMMAND_BUFFER_SERVICE_SERVICE_TRANSFER_CACHE_EXT_H_