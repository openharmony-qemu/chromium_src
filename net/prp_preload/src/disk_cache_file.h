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

#ifndef NET_PRP_PRELOAD_SRC_DISK_CACHE_FILE_H
#define NET_PRP_PRELOAD_SRC_DISK_CACHE_FILE_H

#include <string>
#include "disk_cache_backend_factory.h"
#include "net/base/io_buffer.h"

namespace ohos_prp_preload {
class DiskCacheFile;
using EntryLoadedCallback = base::RepeatingCallback<void(const std::string& entry_content)>;

class DiskCacheEntry {
 public:
  DiskCacheEntry(DiskCacheFile* cache,
                 const std::string& url,
                 const std::string& entry_content);
  ~DiskCacheEntry();
  DiskCacheEntry(const DiskCacheEntry&) = delete;
  DiskCacheEntry& operator=(const DiskCacheEntry&) = delete;
  void Cache();
  void OnOpComplete(int rv);
  void OnEntryOpenComplete(disk_cache::EntryResult result);

 private:
  enum OpType {
    OPEN_ENTRY,
    WRITE_DATA,
    CREATE_ENTRY,
  };

  int OpenCallback(int rv);
  int WriteCallback(int rv);
  int IOComplete(int rv);

  raw_ptr<DiskCacheFile> cache_;
  OpType op_type_ = OPEN_ENTRY;
  const std::string& url_;
  std::string entry_content_;
  raw_ptr<disk_cache::Entry, DanglingUntriaged> entry_;
  base::WeakPtr<DiskCacheEntry> weak_ptr_;
  base::WeakPtrFactory<DiskCacheEntry> weak_ptr_factory_{this};
};

class DiskCacheReadHelper {
 public:
  DiskCacheReadHelper(DiskCacheFile* cache,
                      const std::string& url,
                      const EntryLoadedCallback& callback);
  DiskCacheReadHelper(const DiskCacheReadHelper&) = delete;
  DiskCacheReadHelper& operator=(const DiskCacheReadHelper&) = delete;
  ~DiskCacheReadHelper();

  void LoadCache();
  void OnOpComplete(int rv);
  void OnEntryOpenComplete(disk_cache::EntryResult result);

 private:
  enum OpType {
    OPEN_ENTRY,
    READ_DATA,
    LOAD_ENTRY,
  };

  int OpenCallback(int rv);
  int ReadCallback(int rv);
  int IOComplete(int rv);

  raw_ptr<DiskCacheFile> cache_;
  OpType op_type_ = OPEN_ENTRY;
  const std::string& url_;
  EntryLoadedCallback entry_loaded_cb_;
  scoped_refptr<net::IOBufferWithSize> buf_;
  raw_ptr<disk_cache::Entry, DanglingUntriaged> entry_;
  base::WeakPtr<DiskCacheReadHelper> weak_ptr_;
  base::WeakPtrFactory<DiskCacheReadHelper> weak_ptr_factory_{this};
};

class DiskCacheFile : public base::RefCounted<DiskCacheFile> {
 public:
  DiskCacheFile(const scoped_refptr<DiskCacheBackendFactory>& disk_cache_backend_factory,
                const std::string& url,
                const EntryLoadedCallback& entry_loaded_cb);
  ~DiskCacheFile() = default;
  DiskCacheFile(const DiskCacheFile&) = delete;
  DiskCacheFile& operator=(const DiskCacheFile&) = delete;

  void StoreInfoAsync(const std::string& entry_content);
  void LoadInfoAsync();
 private:
  friend class base::RefCounted<DiskCacheFile>;
  friend class DiskCacheEntry;
  friend class DiskCacheReadHelper;

  disk_cache::Backend* Backend() const { return disk_cache_backend_factory_->Backend(); }
  void DoStoreInfo(const std::string& entry_content);
  void DoLoadInfo();
  void EntryReadComplete();
  void EntryWriteComplete(DiskCacheEntry* entry);
  void SetDelayedStoreTask(base::OnceCallback<void()> delayed_store_task);
  void SetDelayedLoadTask(base::OnceCallback<void()> delayed_load_task);
  void BackendComplete();
  void RunStoreTask(bool clear = false);
  void RunLoadTask(bool clear = false);

  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory_;
  const std::string& url_;
  EntryLoadedCallback entry_loaded_cb_;
  std::unique_ptr<DiskCacheReadHelper> helper_;
  std::unique_ptr<DiskCacheEntry> entry_;
  base::OnceCallback<void()> delayed_store_task_;
  base::OnceCallback<void()> delayed_load_task_;
  base::WeakPtrFactory<DiskCacheFile> weak_factory_ { this };
};

}  // namespace ohos_prp_preload

#endif  // NET_PRP_PRELOAD_SRC_DISK_CACHE_FILE_H
