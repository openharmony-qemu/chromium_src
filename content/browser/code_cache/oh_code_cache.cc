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

  LOG(DEBUG) << "New Response Cache Metadata. URL Hash: " << url_hash_.c_str()
      << ", Content-Length: " << content_length_.c_str()
      << ", E-Tag: " << e_tag_.c_str()
      << ", Last-Modified: " << last_modified_.c_str()
      << ", Access-Control-Allow-Origin: " << access_control_allow_origin_.c_str()
      << ", File Hash: " << file_hash_.c_str();
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
    LOG(DEBUG) << "Init Single Thread Task Runner For Precompile Javascript.";
    task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
        {base::MayBlock(), base::WithBaseSyncPrimitives(),
        base::TaskPriority::BEST_EFFORT},
        base::SingleThreadTaskRunnerThreadMode::DEDICATED);
  }

  LOG(DEBUG) << "Use exist Single Thread Task Runner For Precompile Javascript.";
  return task_runner_;
}

// ResponseCache --------------------------------------------------

std::unique_ptr<base::FilePath> ResponseCache::cache_dir_path_ = nullptr;

// static
void ResponseCache::InitCacheDirectory(base::FilePath path) {
  if (path.empty() || !base::PathExists(path)) {
    LOG(ERROR) << "Response Cache directory does not exist.";
    return;
  }

  cache_dir_path_ = std::make_unique<base::FilePath>(path);
  LOG(ERROR) << "Response Cache directory init succefully.";
}

// static
std::shared_ptr<ResponseCache> ResponseCache::CreateResponseCache(const std::string& url) {
  if (url.empty()) {
    LOG(DEBUG) << "Create Response Cache error: url is empty. url: " << url.c_str();
    return nullptr;
  }

  if (!cache_dir_path_ || cache_dir_path_->empty()) {
    LOG(DEBUG) << "Create Response Cache error: cache dir path has not initialized. url: " << url.c_str();
    return nullptr;
  }
  
  auto response_cache = std::make_shared<ResponseCache>(url);
  response_cache->url_hash_ = std::to_string(disk_cache::simple_util::GetEntryHashKey(url));
  base::FilePath file_path(kFileTag + response_cache->url_hash_);
  response_cache->cache_file_path_ = cache_dir_path_->Append(file_path);
  response_cache->metadata_file_path_ = cache_dir_path_->Append(kCacheMetadataFileName);
  response_cache->metadata_out_ = std::make_shared<ResponseCacheMetadata>();

  LOG(DEBUG) << "Create response cache successfully. url: " << url.c_str();
  return response_cache;
}

// static
void ResponseCache::ClearAllCache() {
  auto cache_dir_path = *(cache_dir_path_.get());
  base::FileEnumerator enumerator(cache_dir_path, false, base::FileEnumerator::FILES);
  for (auto name = enumerator.Next(); !name.empty(); name = enumerator.Next()) {
    base::FileEnumerator::FileInfo info = enumerator.GetInfo();
    auto file_name = name.BaseName().MaybeAsASCII();
    if (file_name == kCacheMetadataFileName ||
        file_name.compare(0, kFileTag.size(), kFileTag) == 0) {
      base::DeleteFile(name);
    }
  }

  LOG(ERROR) << "Delete All Response Cache.";
}

ResponseCache::ResponseCache(const std::string& url) : url_(url) {}

bool ResponseCache::Write(const std::map<std::string, std::string> response_headers,
                          const std::string response_body) {
  metadata_in_ = std::make_shared<ResponseCacheMetadata>(url_, response_body, response_headers);
  response_body_in_ = response_body;

  if (!FindMetadata()) {
    LOG(ERROR) << "Create response cache. url: " << url_.c_str();
    return DoCreate();
  }

  if (NeedUpdate()) {
    LOG(ERROR) << "Update response cache. url: " << url_.c_str();
    return DoUpdate();
  }

  LOG(ERROR) << "Response Cache is hot, no need to write again. url: " << url_.c_str();
  return true;
}

bool ResponseCache::CanUseCache() {
  if (!FindMetadata()) {
    LOG(ERROR) << "Cannot find response cache metadata. url: " << url_.c_str();
    return false;
  }

  if (!ReadContent()) {
    LOG(ERROR) << "Cannot read response cache content. url: " << url_.c_str();
    return false;
  }

  LOG(ERROR) << "Response cache can be used. url: " << url_.c_str();
  return true;
}

bool ResponseCache::CreateStream() {
  metadata_file_stream_ = std::make_shared<std::fstream>(metadata_file_path_.LossyDisplayName());
  if (!metadata_file_stream_) {
    LOG(ERROR) << "Create response cache metadata file stream failed. url: " << url_.c_str();
    return false;
  }

  return true;
}

bool ResponseCache::FindMetadata() {
  if (!CreateStream()) {
    LOG(ERROR) << "Create response cache metadata file stream failed. url: " << url_.c_str();
    return false;
  }
  
  bool find_metadata = false;
  while(ReadMetadata()) {
    if (metadata_out_->url_hash_ == url_hash_) {
      find_metadata = true;
      break;
    }
  }

  metadata_file_stream_->close();
  metadata_file_stream_ = nullptr;
  return find_metadata;
}

bool ResponseCache::ReadContent() {
  if (!base::PathExists(cache_file_path_)) {
    LOG(ERROR) << "Response cache content file path is not exist. url: " << url_.c_str();
    return false;
  }

  if (!base::ReadFileToString(cache_file_path_, &response_body_out_)) {
    LOG(ERROR) << "Read response cache content failed. url: " << url_.c_str();
    return false;
  }

  return true;
}

bool ResponseCache::ReadMetadata() {
  if (!metadata_file_stream_) {
    LOG(ERROR) << "Response cache metadata file stream has not initialized. url: " << url_.c_str();
    return false;
  }

  std::string line;
  if (std::getline(*(metadata_file_stream_.get()), line)) {
    std::regex pattern(
      kURLKey + ":(\\w+)\\|" + kContentLengthKey + ":(\\w+)\\|" +
      kETagKey + ":(.*)\\|" + kLastModifiedKey + ":(.*)\\|" +
      kAccessControlAllowOriginKey + ":(.*)\\|" + kFileHashKey + ":(.*)");

    std::sregex_iterator it(line.begin(), line.end(), pattern);

    if (it != std::sregex_iterator() && it->size() == 7) {
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
    LOG(ERROR) << "Response cache Content-Length changed. url: " << url_.c_str() <<
      ". old: " << metadata_out_->content_length_ << 
      ". new: " << metadata_in_->content_length_;
    return true;
  }

  if (metadata_in_->e_tag_ != metadata_out_->e_tag_) {
    LOG(ERROR) << "Response cache E-Tag changed. url: " << url_.c_str() <<
      ". old: " << metadata_out_->e_tag_ << 
      ". new: " << metadata_in_->e_tag_;
    return true;
  }

  if (metadata_in_->last_modified_ != metadata_out_->last_modified_) {
    LOG(ERROR) << "Response cache Last-Modified changed. url: " << url_.c_str() <<
      ". old: " << metadata_out_->last_modified_ << 
      ". new: " << metadata_in_->last_modified_;
    return true;
  }

  if (metadata_in_->access_control_allow_origin_ != metadata_out_->access_control_allow_origin_) {
    LOG(ERROR) << "Response cache Access-Control-Allow-Origin changed. url: " << url_.c_str() <<
      ". old: " << metadata_out_->access_control_allow_origin_ << 
      ". new: " << metadata_in_->access_control_allow_origin_;
    return true;
  }

  if (metadata_in_->file_hash_ != metadata_out_->file_hash_) {
    LOG(ERROR) << "Response cache File Hash changed. url: " << url_.c_str() <<
      ". old: " << metadata_out_->file_hash_ << 
      ". new: " << metadata_in_->file_hash_;
    return true;
  }

  return false;
}

bool ResponseCache::DoCreate() {
  if (!DoWriteIntoFile(metadata_file_path_, metadata_in_->ToString())) {
    LOG(ERROR) << "Create new response cache failed. Reason: write metadata faild. url: " << url_.c_str();
    return false;
  }

  if (!DoWriteIntoFile(cache_file_path_, response_body_in_)) {
    LOG(ERROR) << "Create new response cache failed. Reason: write content faild. url: " << url_.c_str();
    return false;
  }

  LOG(ERROR) << "Create new response cache successfully. url: " << url_.c_str();
  return true;
}

bool ResponseCache::DoUpdate() {
  if (!DoUpdateMetadata()) {
    LOG(ERROR) << "Update response cache failed. Reason: update metadata faild. url: " << url_.c_str();
    return false;
  }

  if (!DeleteCacheFile()) {
    LOG(ERROR) << "Update response cache failed. Reason: delete old content faild. url: " << url_.c_str();
    return false;
  }

  if (!DoWriteIntoFile(cache_file_path_, response_body_in_)) {
    LOG(ERROR) << "Update response cache failed. Reason: write new content faild. url: " << url_.c_str();
    return false;
  }

  LOG(ERROR) << "Update response cache successfully. url: " << url_.c_str();
  return true;
}

bool ResponseCache::DoUpdateMetadata() {
  if (!CreateStream()) {
    LOG(ERROR) << "Update metadata failed, file stream has not initialized. url: " << url_.c_str();
    return false;
  }

  base::FilePath temp_file_path = cache_dir_path_->Append(kTempFilePath);
  auto temp_file =
    std::make_unique<base::File>(temp_file_path, base::File::FLAG_CREATE | base::File::FLAG_WRITE);
  temp_file->Lock(base::File::LockMode::kExclusive);

  if (!temp_file->IsValid()) {
    LOG(ERROR) << "Update metadata failed, create temp file failed. url: " << url_.c_str();
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
      LOG(ERROR) << "Update metadata failed, write new metadata into temp file failed. url: " << url_.c_str();
      result = false;
      break;
    }
  }

  CloseStream();

  if (!result) {
    base::DeleteFile(temp_file_path);
    return false;
  }

  base::File::Error error;
  if (!base::ReplaceFile(temp_file_path, metadata_file_path_, &error)) {
    LOG(ERROR) << "Update metadata failed, replace old metadata file with temp file failed. url: " << url_.c_str();
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
    LOG(ERROR) << "Write into file failed. File is invalid. url: " << url_.c_str();
    return false;
  }

  int64_t current_size = file->GetLength();
  file->Lock(base::File::LockMode::kExclusive);
  bool result = file->WriteAndCheck(current_size, base::as_bytes(base::make_span(data)));
  file->Unlock();
  file->Close();

  if (!result) {
    LOG(ERROR) << "Write into file failed. Cannot write file. url: " << url_.c_str();
    return false;
  }

  return true;
}

bool ResponseCache::DeleteCacheFile() {
  if (!base::PathExists(cache_file_path_)) {
    LOG(ERROR) << "Response cache content file doesn't exist, no need to delete. url: " << url_.c_str();
    return true;
  }

  bool result = base::DeleteFile(cache_file_path_);
  LOG(ERROR) << "Delete response cache content file result: " << result << ". url: " << url_.c_str();
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
  bool has_data = false;
  int transfer_size = 0;
  if (offset_ < data_.length()) {
    transfer_size = std::min(length, static_cast<int>(data_.length() - offset_));
    memcpy(dest->data(), data_.c_str() + offset_, transfer_size);
    offset_ += transfer_size;
    *bytes_read = transfer_size;
    has_data = true;
  }
  std::move(callback).Run(transfer_size);
  return has_data;
}
}  // namespace oh_code_cache
