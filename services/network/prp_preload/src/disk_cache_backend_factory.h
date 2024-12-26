// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_PRP_PRELOAD_SRC_DISK_CACHE_BACKEND_FACTORY_H
#define SERVICES_NETWORK_PRP_PRELOAD_SRC_DISK_CACHE_BACKEND_FACTORY_H

#include "base/files/file_path.h"
#include "net/base/io_buffer.h"
#include "net/disk_cache/disk_cache.h"

namespace ohos_prp_preload {
class DiskCacheBackendFactory : public base::RefCounted<DiskCacheBackendFactory> {
 public:
  DiskCacheBackendFactory() = default;
  ~DiskCacheBackendFactory() = default;
  DiskCacheBackendFactory(const DiskCacheBackendFactory&) = delete;
  DiskCacheBackendFactory& operator=(const DiskCacheBackendFactory&) = delete;
  void CreateBackend();
  bool WaitInitedTimeout();
  disk_cache::Backend* Backend() const { return backend_.get(); }
 private:
  void CacheCreatedCallback(disk_cache::BackendResult result);

  base::FilePath cache_path_;
  std::atomic<bool> is_inited_ { false };
  std::unique_ptr<disk_cache::Backend> backend_;
  std::condition_variable cv_backend_ready_;
  std::mutex fac_mutex_;
};

}  // namespace ohos_prp_preload

#endif  // SERVICES_NETWORK_PRP_PRELOAD_SRC_DISK_CACHE_BACKEND_FACTORY_H