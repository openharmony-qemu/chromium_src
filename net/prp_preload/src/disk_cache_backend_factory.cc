/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "disk_cache_backend_factory.h"

#include "base/logging.h"
#include "base/path_service.h"
#include "net/base/io_buffer.h"

namespace {
const size_t DEFAULT_PRELOAD_DISK_CACHE_BYTES = 12 * 1024 * 1024;
const base::FilePath::CharType PRELOAD_CACHE_DIRNAME[] = FILE_PATH_LITERAL("PreloadCache");
}  // namespace

namespace ohos_prp_preload {
void DiskCacheBackendFactory::CreateBackend(const base::FilePath& cache_path) {
  cache_path_ = cache_path;
  cache_path_ = cache_path_.Append(PRELOAD_CACHE_DIRNAME);
  LOG(INFO) << "PRPPreload.DiskCacheBackendFactory::CreateBackend cache path: " << cache_path_;
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
  std::mutex mutex;
  std::unique_lock<std::mutex> lk(mutex);
  bool ret = cv_backend_ready_.wait_until(lk,
      std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_INITED_TIME_OUT),
      [this] { return is_inited_.load(); });
  return ret;
}

void DiskCacheBackendFactory::CacheCreatedCallback(disk_cache::BackendResult result) {
  if (result.net_error != net::OK) {
    LOG(ERROR) << "PRPPreload.DiskCacheBackendFactory::CacheCreatedCallback cache creation failed, error: " <<
        result.net_error;
      return;
  }
  backend_ = std::move(result.backend);
  is_inited_.store(true);
  cv_backend_ready_.notify_all();
}
}  // namespace ohos_prp_preload
