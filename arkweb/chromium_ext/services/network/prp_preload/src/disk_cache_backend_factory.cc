// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/services/network/prp_preload/src/disk_cache_backend_factory.h"

#include "base/logging.h"
#include "base/path_service.h"

namespace {
const size_t DEFAULT_PRELOAD_DISK_CACHE_BYTES = 12 * 1024 * 1024;
const base::FilePath::CharType PRELOAD_CACHE_DIRNAME[] = FILE_PATH_LITERAL("PreloadCache");
}  // namespace

namespace ohos_prp_preload {
void DiskCacheBackendFactory::CreateBackend() {
  if (!base::PathService::Get(base::DIR_CACHE, &cache_path_)) {
    LOG(WARNING) << "PRPPreload.DiskCacheBackendFactory::CreateBackend get cache path failed";
    return;
  }
  cache_path_ = cache_path_.Append(PRELOAD_CACHE_DIRNAME);
  disk_cache::BackendResult rv = disk_cache::CreateCacheBackend(
    net::DISK_CACHE, net::CACHE_BACKEND_SIMPLE, /*file_operations=*/nullptr,
    cache_path_, DEFAULT_PRELOAD_DISK_CACHE_BYTES,
    disk_cache::ResetHandling::kResetOnError, /*net_log=*/nullptr,
    base::BindOnce(&DiskCacheBackendFactory::CacheCreatedCallback, this));

  if (rv.net_error == net::OK) {
    backend_ = std::move(rv.backend);
    is_inited_.store(true);
  }
}

bool DiskCacheBackendFactory::WaitInitedTimeout() {
  if (is_inited_) {
    return true;
  }
  constexpr int32_t WAIT_INITED_TIME_OUT = 200; // 200ms
  std::unique_lock<std::mutex> lk(fac_mutex_);
  bool ret = cv_backend_ready_.wait_until(lk,
    std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_INITED_TIME_OUT),
    [&] { return is_inited_.load(); });
  return ret;
}

void DiskCacheBackendFactory::CacheCreatedCallback(disk_cache::BackendResult result) {
  if (result.net_error != net::OK) {
    LOG(ERROR) << "PRPPreload.DiskCacheBackendFactory::CacheCreatedCallback cache creation failed, error: "
      << result.net_error;
      return;
  }
  backend_ = std::move(result.backend);
  is_inited_.store(true);
  cv_backend_ready_.notify_all();
}
}  // namespace ohos_prp_preload