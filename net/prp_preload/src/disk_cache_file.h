// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
  base::OnceCallback<void()> delayed_load_task;
  base::WeakPtrFactory<DiskCacheFile> weak_factory_ { this };
};

}  // namespace ohos_prp_preload

#endif  // NET_PRP_PRELOAD_SRC_DISK_CACHE_FILE_H