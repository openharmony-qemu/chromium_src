// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "disk_cache_file.h"

#include "base/logging.h"

namespace ohos_prp_preload {
DiskCacheEntry::DiskCacheEntry(DiskCacheFile* cache,
                               const std::string& url,
                               const std::string& entry_content) :
  cache_(cache), url_(url), entry_content_(entry_content), entry_(nullptr) {
  weak_ptr_ = weak_ptr_factory_.GetWeakPtr();
}
DiskCacheEntry::~DiskCacheEntry() {
  if (entry_) {
    entry_->Close();
    entry_ = nullptr;
  }
}
  
void DiskCacheEntry::Cache() {
  OnOpComplete(net::OK);
}

void DiskCacheEntry::OnOpComplete(int rv) {
  auto weak_ptr = std::move(weak_ptr_);
  do {
    switch (op_type_) {
      case OPEN_ENTRY:
        rv = OpenCallback(rv);
        break;
      case CREATE_ENTRY:
        rv = WriteCallback(rv);
        break;
      case WRITE_DATA:
        rv = IOComplete(rv);
        break;
    }
  } while (rv != net::ERR_IO_PENDING && weak_ptr);
  if (weak_ptr) {
    weak_ptr_ = std::move(weak_ptr);
  }
}

void DiskCacheEntry::OnEntryOpenComplete(disk_cache::EntryResult result) {
  int rv = result.net_error();
  entry_ = result.ReleaseEntry();
  OnOpComplete(rv);
}

int DiskCacheEntry::OpenCallback(int rv) {
  op_type_ = CREATE_ENTRY;

  auto callback = base::BindOnce(&DiskCacheEntry::OnEntryOpenComplete,
                                 weak_ptr_factory_.GetWeakPtr());

  disk_cache::EntryResult create_result =
      cache_->Backend()->OpenOrCreateEntry(url_, net::HIGHEST, std::move(callback));
  rv = create_result.net_error();

  if (rv != net::ERR_IO_PENDING) {
    entry_ = create_result.ReleaseEntry();
  }
  return rv;
}

int DiskCacheEntry::WriteCallback(int rv) {
  if (rv != net::OK) {
    cache_->EntryWriteComplete(this);
    return rv;
  }

  op_type_ = WRITE_DATA;
  auto io_buf = base::MakeRefCounted<net::StringIOBuffer>(entry_content_);
  return entry_->WriteData(1, 0, io_buf.get(), entry_content_.length(),
                           base::BindOnce(&DiskCacheEntry::OnOpComplete,
                                          weak_ptr_factory_.GetWeakPtr()), true);
}

int DiskCacheEntry::IOComplete(int rv) {
  cache_->EntryWriteComplete(this);
  return rv;
}


DiskCacheReadHelper::DiskCacheReadHelper(DiskCacheFile* cache,
                                         const std::string& url,
                                         const EntryLoadedCallback& callback) :
  cache_(cache), url_(url), entry_loaded_cb_(callback), buf_(nullptr), entry_(nullptr) {
  weak_ptr_ = weak_ptr_factory_.GetWeakPtr();
}
DiskCacheReadHelper::~DiskCacheReadHelper() {
  if (entry_) {
    entry_->Close();
    entry_ = nullptr;
  }
  if (buf_) {
    buf_ = nullptr;
  }
}

void DiskCacheReadHelper::LoadCache() {
  OnOpComplete(net::OK);
}

void DiskCacheReadHelper::OnOpComplete(int rv) {
  auto weak_ptr = std::move(weak_ptr_);
  do {
    switch (op_type_) {
      case OPEN_ENTRY:
        rv = OpenCallback(rv);
        break;
      case LOAD_ENTRY:
        rv = ReadCallback(rv);
        break;
      case READ_DATA:
        rv = IOComplete(rv);
        break;
    }
  } while (rv != net::ERR_IO_PENDING && weak_ptr);
  if (weak_ptr) {
    weak_ptr_ = std::move(weak_ptr);
  }
}

void DiskCacheReadHelper::OnEntryOpenComplete(disk_cache::EntryResult result) {
  int rv = result.net_error();
  entry_ = result.ReleaseEntry();
  OnOpComplete(rv);
}

int DiskCacheReadHelper::OpenCallback(int rv) {
  op_type_ = LOAD_ENTRY;

  auto callback = base::BindOnce(&DiskCacheReadHelper::OnEntryOpenComplete,
                                 weak_ptr_factory_.GetWeakPtr());

  disk_cache::EntryResult result =
      cache_->Backend()->OpenEntry(url_, net::HIGHEST, std::move(callback));
  rv = result.net_error();

  if (rv != net::ERR_IO_PENDING)
    entry_ = result.ReleaseEntry();  // may be nullptr
  return rv;
}

int DiskCacheReadHelper::ReadCallback(int rv) {
  if (rv != net::OK) {
    LOG(DEBUG) << "PRPPreload.DiskCacheReadHelper::ReadCallback load cache entry failed: " << rv;
    cache_->EntryReadComplete();
    return rv;
  }

  op_type_ = READ_DATA;
  buf_ = base::MakeRefCounted<net::IOBufferWithSize>(entry_->GetDataSize(1));
  return entry_->ReadData(1, 0, buf_.get(), buf_->size(),
                          base::BindOnce(&DiskCacheReadHelper::OnOpComplete,
                                         weak_ptr_factory_.GetWeakPtr()));
}

int DiskCacheReadHelper::IOComplete(int rv) {
  if (rv && buf_ != nullptr && rv == buf_->size() && !entry_loaded_cb_.is_null()) {
    entry_loaded_cb_.Run(std::string(buf_->data(), buf_->size()));
  }

  cache_->EntryReadComplete();
  return rv;
}

DiskCacheFile::DiskCacheFile(const scoped_refptr<DiskCacheBackendFactory>& disk_cache_backend_factory,
                             const std::string& url,
                             const EntryLoadedCallback& entry_loaded_cb) :
  disk_cache_backend_factory_(disk_cache_backend_factory), url_(url), entry_loaded_cb_(entry_loaded_cb) {}

void DiskCacheFile::StoreInfoAsync(const std::string& entry_content) {
  if (!disk_cache_backend_factory_->WaitInitedTimeout()) {
    LOG(ERROR) << "PRPPreload.DiskCacheFile::StoreInfoAsync backend not ready";
    return;
  }
  entry_ = std::make_unique<DiskCacheEntry>(this, url_, entry_content);
  entry_->Cache();
}

void DiskCacheFile::LoadInfoAsync() {
  if (!disk_cache_backend_factory_->WaitInitedTimeout()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheFile::LoadInfoAsync already load";
    return;
  }
  if (helper_ != nullptr) {
    return;
  }
  helper_ = std::make_unique<DiskCacheReadHelper>(this, url_, entry_loaded_cb_);
  helper_->LoadCache();
}

void DiskCacheFile::EntryReadComplete() {
    helper_.reset();
}

void DiskCacheFile::EntryWriteComplete(DiskCacheEntry* entry) {
  if (entry != entry_.get()) {
    LOG(DEBUG) << "PRPPreload.DiskCacheFile::EntryWriteComplete not current entry";
    return;
  }

  entry_.reset();
}

}  // namespace ohos_prp_preload