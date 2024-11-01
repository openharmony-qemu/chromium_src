/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "content/browser/code_cache/oh_code_cache.h"

namespace oh_code_cache {
// ResponseCacheMetadata --------------------------------------------------

ResponseCacheMetadata::ResponseCacheMetadata(
    const std::string& url,
    const std::string& response_body,
    const std::map<std::string, std::string>& response_headers) {
  url_hash_ = std::to_string(disk_cache::simple_util::GetEntryHashKey(url));

  content_length_ = std::to_string(response_body.size());

  auto e_tag_it = response_headers.find(kETagKey);
  if (e_tag_it != response_headers.end()) {
    e_tag_ = e_tag_it->second;
  }

  auto last_modified_it = response_headers.find(kLastModifiedKey);
  if (last_modified_it != response_headers.end()) {
    last_modified_ = last_modified_it->second;
  }

  auto access_control_it = response_headers.find(kAccessControlAllowOriginKey);
  if (access_control_it != response_headers.end()) {
    access_control_allow_origin_ = access_control_it->second;
  }

  file_hash_ =
      std::to_string(disk_cache::simple_util::GetEntryHashKey(response_body));
}

std::string ResponseCacheMetadata::ToString() {
  return kURLKey + ":" + url_hash_ + "|" +
         kContentLengthKey + ":" + content_length_ + "|" +
         kETagKey + ":" + e_tag_ + "|" +
         kLastModifiedKey + ":" + last_modified_ + "|" +
         kAccessControlAllowOriginKey + ":" + access_control_allow_origin_ + "|" +
         kFileHashKey + ":" + file_hash_ + "\n";
}

// TaskRunner --------------------------------------------------

scoped_refptr<base::SingleThreadTaskRunner> TaskRunner::task_runner_ = nullptr;

// static
scoped_refptr<base::SingleThreadTaskRunner> TaskRunner::GetTaskRunner() {
  if (!task_runner_) {
    task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
        {base::MayBlock(), base::WithBaseSyncPrimitives(),
        base::TaskPriority::BEST_EFFORT},
        base::SingleThreadTaskRunnerThreadMode::DEDICATED);
  }

  return task_runner_;
}

// ResponseCache --------------------------------------------------

std::unique_ptr<base::FilePath> ResponseCache::cache_dir_path_ = std::make_unique<base::FilePath>();
std::map<std::string, std::shared_ptr<ResponseCacheMetadata>> ResponseCache::cache_metadata_map_ = {};

// static
void ResponseCache::InitCacheDirectory(base::FilePath path) {
  if (path.empty() || !base::PathExists(path)) {
    LOG(ERROR) << "Response Cache directory does not exist.";
    return;
  }

  cache_dir_path_ = std::make_unique<base::FilePath>(path);
}

// static
std::shared_ptr<ResponseCache> ResponseCache::CreateResponseCache(const std::string& url) {
  if (url.empty()) {
    LOG(ERROR) << "Do not create response cache of empty url.";
    return nullptr;
  }

  std::regex url_regex(R"(^http(s)?:\/\/.+\.js(\?.+)?$)", std::regex_constants::icase);
  if (!std::regex_match(url, url_regex)) {
    return nullptr;
  }

  if (!cache_dir_path_ || cache_dir_path_->empty()) {
    LOG(ERROR) << "Create Response Cache error: cache dir path has not initialized.";
    return nullptr;
  }
  
  auto response_cache = std::make_shared<ResponseCache>(url);
  response_cache->url_hash_ = std::to_string(disk_cache::simple_util::GetEntryHashKey(url));
  base::FilePath file_path(kFileTag + response_cache->url_hash_);
  response_cache->cache_file_path_ = cache_dir_path_->Append(file_path);
  response_cache->metadata_file_path_ = cache_dir_path_->Append(kCacheMetadataFileName);
  response_cache->metadata_out_ = std::make_shared<ResponseCacheMetadata>();

  return response_cache;
}

// static
void ResponseCache::ClearAllCache() {
  auto cache_dir_path = cache_dir_path_.get();

  if (!cache_dir_path || !base::PathExists(*cache_dir_path)) {
    LOG(ERROR) << "Cannot clear response cache. cache directory path has not initailized";
    return;
  }

  base::FileEnumerator enumerator(*cache_dir_path, false, base::FileEnumerator::FILES);
  for (auto name = enumerator.Next(); !name.empty(); name = enumerator.Next()) {
    base::FileEnumerator::FileInfo info = enumerator.GetInfo();
    auto file_name = name.BaseName().MaybeAsASCII();
    if (file_name == kCacheMetadataFileName ||
        file_name.compare(0, kFileTag.size(), kFileTag) == 0) {
      base::DeleteFile(name);
    }
  }

  cache_metadata_map_.clear();
}

ResponseCache::ResponseCache(const std::string& url) : url_(url) {}

NextOp ResponseCache::Write(const std::map<std::string, std::string> response_headers,
                            const std::string response_body) {
  metadata_in_ = std::make_shared<ResponseCacheMetadata>(url_, response_body, response_headers);
  response_body_in_ = response_body;

  if (!FindMetadata()) {
    return DoCreate();
  }

  if (NeedUpdate()) {
    return DoUpdate();
  }

  return NextOp::DO_NOTHING;
}

bool ResponseCache::CanUseCache() {
  if (!FindMetadata()) {
    return false;
  }

  if (!ReadContent()) {
    return false;
  }

  return true;
}

bool ResponseCache::CreateStream() {
  if (!base::PathExists(metadata_file_path_)) {
    return false;
  }

  metadata_file_stream_ = std::make_shared<std::fstream>(metadata_file_path_.LossyDisplayName());
  if (!metadata_file_stream_) {
    return false;
  }

  return true;
}

bool ResponseCache::FindMetadata() {
  TRACE_EVENT1("net","ResponseCache::FindMetadata", "url", url_);

  if (cache_metadata_map_.empty()) {
    if (!CreateStream()) {
      return false;
    }

    while(ReadMetadata()) {
      cache_metadata_map_.emplace(metadata_out_->url_hash_, metadata_out_);
    }

    CloseStream();
  }

  auto it = cache_metadata_map_.find(url_hash_);
  
  if (it == cache_metadata_map_.end()) {
    return false;
  }

  metadata_out_ = it->second;
  return true;
}

bool ResponseCache::ReadContent() {
  TRACE_EVENT1("net","ResponseCache::ReadContent", "url", url_);

  if (!base::PathExists(cache_file_path_)) {
    return false;
  }

  if (!base::ReadFileToString(cache_file_path_, &response_body_out_)) {
    return false;
  }

  return true;
}

bool ResponseCache::ReadMetadata() {
  TRACE_EVENT1("net","ResponseCache::ReadMetadata", "url", url_);

  if (!metadata_file_stream_) {
    return false;
  }

  std::string line;
  if (std::getline(*(metadata_file_stream_.get()), line)) {
    std::regex pattern(
      kURLKey + ":(\\w+)\\|" + kContentLengthKey + ":(\\w+)\\|" +
      kETagKey + ":(.*)\\|" + kLastModifiedKey + ":(.*)\\|" +
      kAccessControlAllowOriginKey + ":(.*)\\|" + kFileHashKey + ":(.*)");

    std::sregex_iterator it(line.begin(), line.end(), pattern);

    // Check the count of metadata.
    // Each resource must has 7 metadata(url、response、headers...)
    int item_count = 7;
    if (it != std::sregex_iterator() && it->size() == item_count) {
      metadata_out_->url_hash_ = (*it)[1].str();
      metadata_out_->content_length_ = (*it)[2].str();
      metadata_out_->e_tag_ = (*it)[3].str();
      metadata_out_->last_modified_ = (*it)[4].str();
      metadata_out_->access_control_allow_origin_ = (*it)[5].str();
      metadata_out_->file_hash_ = (*it)[6].str();
    }

    return true;
  }

  return false;
}

bool ResponseCache::NeedUpdate() {
  if (metadata_in_->content_length_ != metadata_out_->content_length_) {
    return true;
  }

  if (metadata_in_->e_tag_ != metadata_out_->e_tag_) {
    return true;
  }

  if (metadata_in_->last_modified_ != metadata_out_->last_modified_) {
    return true;
  }

  if (metadata_in_->access_control_allow_origin_ != metadata_out_->access_control_allow_origin_) {
    return true;
  }

  if (metadata_in_->file_hash_ != metadata_out_->file_hash_) {
    return true;
  }

  return false;
}

NextOp ResponseCache::DoCreate() {
  if (!DoWriteIntoFile(metadata_file_path_, metadata_in_->ToString())) {
    LOG(ERROR) << "Create new response cache failed. Reason: write metadata faild.";
    return NextOp::THROW_ERROR;
  }

  cache_metadata_map_.emplace(url_hash_, metadata_in_);

  if (!DoWriteIntoFile(cache_file_path_, response_body_in_)) {
    LOG(ERROR) << "Create new response cache failed. Reason: write content faild.";
    return NextOp::THROW_ERROR;
  }

  return NextOp::WRITE_CODE_CACHE;
}

NextOp ResponseCache::DoUpdate() {
  if (!DoUpdateMetadata()) {
    LOG(ERROR) << "Update response cache failed. Reason: update metadata faild.";
    return NextOp::THROW_ERROR;
  }

  if (!DeleteCacheFile()) {
    LOG(ERROR) << "Update response cache failed. Reason: delete old content faild.";
    return NextOp::THROW_ERROR;
  }

  if (!DoWriteIntoFile(cache_file_path_, response_body_in_)) {
    LOG(ERROR) << "Update response cache failed. Reason: write new content faild.";
    return NextOp::THROW_ERROR;
  }

  return NextOp::WRITE_CODE_CACHE;
}

bool ResponseCache::DoUpdateMetadata() {
  if (!CreateStream()) {
    return false;
  }

  base::FilePath temp_file_path = cache_dir_path_->Append(kTempFilePath);
  auto temp_file =
    std::make_unique<base::File>(temp_file_path, base::File::FLAG_CREATE | base::File::FLAG_WRITE);
  temp_file->Lock(base::File::LockMode::kExclusive);

  if (!temp_file->IsValid()) {
    CloseStream();
    return false;
  }

  bool result = false;
  while (ReadMetadata()) {
    auto wait_to_write = metadata_out_;
    if (metadata_out_->url_hash_ == url_hash_) {
      wait_to_write = metadata_in_;
      result = true;
    }

    if (!temp_file->WriteAtCurrentPosAndCheck(base::as_bytes(base::make_span(wait_to_write->ToString())))) {
      result = false;
      break;
    }

    if (result) {
      cache_metadata_map_[url_hash_] = metadata_in_;
    }
  }

  CloseStream();

  if (!result) {
    base::DeleteFile(temp_file_path);
    return false;
  }

  base::File::Error error;
  if (!base::ReplaceFile(temp_file_path, metadata_file_path_, &error)) {
    base::DeleteFile(temp_file_path);
    return false;
  }

  temp_file->Unlock();
  temp_file->Close();

  return true;
}

bool ResponseCache::DoWriteIntoFile(base::FilePath path, std::string data) {
  std::unique_ptr<base::File> file = nullptr;
  if (!base::PathExists(path)) {
    file = std::make_unique<base::File>(
      path, base::File::FLAG_CREATE | base::File::FLAG_WRITE);
  } else {
    file = std::make_unique<base::File>(
      path, base::File::FLAG_OPEN | base::File::FLAG_WRITE);
  }

  if (!file->IsValid()) {
    return false;
  }

  int64_t current_size = file->GetLength();
  file->Lock(base::File::LockMode::kExclusive);
  bool result = file->WriteAndCheck(current_size, base::as_bytes(base::make_span(data)));
  file->Unlock();
  file->Close();

  if (!result) {
    return false;
  }

  return true;
}

bool ResponseCache::DeleteCacheFile() {
  if (!base::PathExists(cache_file_path_)) {
    return true;
  }

  bool result = base::DeleteFile(cache_file_path_);
  return result;
}

void ResponseCache::CloseStream() {
  metadata_file_stream_->close();
  metadata_file_stream_ = nullptr;
}

// ResourceResponse --------------------------------------------------

ResourceResponse::ResourceResponse(std::shared_ptr<ResponseCache> cache) :
    response_cache_(cache) {}

bool ResourceResponse::OpenInputStream(int32_t request_id,
                                       const network::ResourceRequest& request,
                                       OpenCallback callback) {
  auto stream = std::make_unique<InputStream>(response_cache_->response_body_out_);
  std::move(callback).Run(std::move(stream));
  return true;
}

void ResourceResponse::GetResponseHeaders(int32_t request_id,
                                          int* status_code,
                                          std::string* reason_phrase,
                                          std::string* mime_type,
                                          std::string* charset,
                                          int64_t* content_length,
                                          HeaderMap* extra_headers) {
  TRACE_EVENT0("net","ResourceResponse::GetResponseHeaders");
  *status_code = 200;
  *reason_phrase = "OK";
  *mime_type = "text/javascript";
  *charset = "utf-8";
  *content_length = response_cache_->response_body_out_.size();

  auto response_time = static_cast<int>(base::Time::UnixEpoch().ToJsTime());
  extra_headers->insert(std::make_pair(kResponseDataID, std::to_string(response_time)));
  extra_headers->insert(std::make_pair(kContentLengthKey, response_cache_->metadata_out_->content_length_));
  extra_headers->insert(std::make_pair(kETagKey, response_cache_->metadata_out_->e_tag_));
  extra_headers->insert(std::make_pair(kLastModifiedKey, response_cache_->metadata_out_->last_modified_));
  extra_headers->insert(std::make_pair(kAccessControlAllowOriginKey, response_cache_->metadata_out_->access_control_allow_origin_));
}

// ResourceResponse --------------------------------------------------

InputStream::InputStream(const std::string& data) : data_(data) {}

bool InputStream::Skip(int64_t n, int64_t* bytes_skipped, SkipCallback callback) {
  std::move(callback).Run(0);
  return true;
}

bool InputStream::Read(net::IOBuffer* dest,
                       int length,
                       int* bytes_read,
                       ReadCallback callback) {
  TRACE_EVENT0("net","InputStream::Read");
  bool has_data = false;
  int transfer_size = 0;
  if (offset_ < data_.length()) {
    transfer_size = std::min(length, static_cast<int>(data_.length() - offset_));
    if (memcpy_s(dest->data(), static_cast<size_t>(length), data_.c_str() + offset_, transfer_size) != EOK) {
      LOG(WARNING) << "InputStream::Read memcpy failed";
      *bytes_read = net::ERR_FAILED;
      return false;
    }
    offset_ += transfer_size;
    *bytes_read = transfer_size;
    has_data = true;
  }
  std::move(callback).Run(transfer_size);
  return has_data;
}
}  // namespace oh_code_cache
