// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "disk_cache_backend_factory.h"

#include "base/logging.h"
#include "base/path_service.h"
#include "net/base/io_buffer.h"
#include "base/task/single_thread_task_runner.h"

namespace {
const size_t DEFAULT_PRELOAD_DISK_CACHE_BYTES = 12 * 1024 * 1024;
const base::FilePath::CharType PRELOAD_CACHE_DIRNAME[] = FILE_PATH_LITERAL("PreloadCache");
}  // namespace

namespace ohos_prp_preload {
void DiskCacheBackendFactory::CreateBackend() {
  if (!base::PathService::Get(base::DIR_CACHE, &cache_path_)) {
    LOG(ERROR) << "PRPPreload.DiskCacheBackendFactory::CreateBackend get cache path failed";
    return;
  }
  cache_path_ = cache_path_.Append(PRELOAD_CACHE_DIRNAME);
  LOG(DEBUG) << "PRPPreload.DiskCacheBackendFactory::CreateBackend cache path success";
  disk_cache::BackendResult rv = disk_cache::CreateCacheBackend(
    net::DISK_CACHE, net::CACHE_BACKEND_SIMPLE, /*file_operations=*/nullptr,
    cache_path_, DEFAULT_PRELOAD_DISK_CACHE_BYTES,
    disk_cache::ResetHandling::kResetOnError, /*net_log=*/nullptr,
    base::BindOnce(&DiskCacheBackendFactory::CacheCreatedCallback, weak_factory_.GetWeakPtr()));

  if (rv.net_error == net::OK) {
    backend_ = std::move(rv.backend);
    is_inited_.store(true);
  }
}

bool DiskCacheBackendFactory::CheckBackendAsync(BackendCompleteCallback callback) {
  if (is_inited_) {
    return true;
  }
  backend_complete_callback_list_.push_back(std::move(callback));
  return false;
}

void DiskCacheBackendFactory::CacheCreatedCallback(disk_cache::BackendResult result) {
  if (result.net_error != net::OK) {
    LOG(ERROR) << "PRPPreload.DiskCacheBackendFactory::CacheCreatedCallback cache creation failed, error: "
      << result.net_error;
      return;
  }
  backend_ = std::move(result.backend);
  is_inited_.store(true);
  for (auto& callback : backend_complete_callback_list_) {
    std::move(callback).Run();
  }
  backend_complete_callback_list_.clear();
}
}  // namespace ohos_prp_preload