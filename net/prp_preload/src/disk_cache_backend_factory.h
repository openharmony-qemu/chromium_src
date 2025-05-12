// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_PRP_PRELOAD_SRC_DISK_CACHE_BACKEND_FACTORY_H
#define NET_PRP_PRELOAD_SRC_DISK_CACHE_BACKEND_FACTORY_H

#include <list>
#include <atomic>
#include <condition_variable>
#include "base/files/file_path.h"
#include "net/disk_cache/disk_cache.h"

using BackendCompleteCallback = base::OnceCallback<void()>;

namespace ohos_prp_preload {
class DiskCacheBackendFactory : public base::RefCountedThreadSafe<DiskCacheBackendFactory> {
 public:
  DiskCacheBackendFactory() = default;
  ~DiskCacheBackendFactory() = default;
  DiskCacheBackendFactory(const DiskCacheBackendFactory&) = delete;
  DiskCacheBackendFactory& operator=(const DiskCacheBackendFactory&) = delete;
  void CreateBackend();
  bool CheckBackendAsync(BackendCompleteCallback callback);
  disk_cache::Backend* Backend() const { return backend_.get(); }
 private:
  void CacheCreatedCallback(disk_cache::BackendResult result);

  base::FilePath cache_path_;
  std::atomic_bool is_inited_ { false };
  std::unique_ptr<disk_cache::Backend> backend_;
  std::list<BackendCompleteCallback> backend_complete_callback_list_;
  base::WeakPtrFactory<DiskCacheBackendFactory> weak_factory_ { this };
};

}  // namespace ohos_prp_preload

#endif  // NET_PRP_PRELOAD_SRC_DISK_CACHE_BACKEND_FACTORY_H