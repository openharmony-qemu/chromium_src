// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/services/network/prp_preload/src/disk_cache_backend_factory.h"

#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/task/single_thread_task_runner.h"

namespace {
const size_t DEFAULT_PRELOAD_DISK_CACHE_BYTES = 12 * 1024 * 1024;
const base::FilePath::CharType PRELOAD_CACHE_DIRNAME[] = FILE_PATH_LITERAL("PreloadCache");
const base::FilePath::CharType PRELOAD_CACHE_INDEX[] = FILE_PATH_LITERAL("index");
constexpr int32_t PRELOAD_DISK_CACHE_DELETE_TIMEOUT = 7; // 7 days
}  // namespace

namespace ohos_prp_preload {
void DiskCacheBackendFactory::CreateBackend() {
  if (!base::PathService::Get(base::DIR_CACHE, &cache_path_)) {
    LOG(WARNING) << "PRPPreload.DiskCacheBackendFactory::CreateBackend get cache path failed";
    return;
  }
  cache_path_ = cache_path_.Append(PRELOAD_CACHE_DIRNAME);
  base::File::Info file_info;
  if (base::GetFileInfo(cache_path_.Append(PRELOAD_CACHE_INDEX), &file_info)) {
    base::Time last_modified = file_info.last_modified;
    if (base::Time::Now() - last_modified > base::Days(PRELOAD_DISK_CACHE_DELETE_TIMEOUT)) {
      LOG(DEBUG) << "PRPPreload.DiskCacheBackendFactory::CreateBackend clear";
      base::DeletePathRecursively(cache_path_);
    }
  }
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
    if (!callback.is_null()) {
      std::move(callback).Run();
    }
  }
  backend_complete_callback_list_.clear();
}

void DiskCacheBackendFactory::RemoveCache(base::Time start_time, base::Time end_time)
{
  if (!backend_) {
    is_inited_.store(false);
    backend_complete_callback_list_.clear();
    return;
  }
  int rv;
  if (start_time.is_null() && end_time.is_max()) {
    rv = backend_->DoomAllEntries(base::BindOnce(
        &DiskCacheBackendFactory::ClearPreloadCacheDone, weak_factory_.GetWeakPtr()));
  } else {
    rv = backend_->DoomEntriesBetween(
        start_time, end_time,
        base::BindOnce(&DiskCacheBackendFactory::ClearPreloadCacheDone,
                       weak_factory_.GetWeakPtr()));
  }
  if (rv != net::ERR_IO_PENDING) {
    base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
        FROM_HERE, base::BindOnce(&DiskCacheBackendFactory::ClearPreloadCacheDone,
                                  weak_factory_.GetWeakPtr(), rv));
  }
}
 
void DiskCacheBackendFactory::ClearPreloadCacheDone(int rv)
{
  backend_complete_callback_list_.clear();
}
}  // namespace ohos_prp_preload