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

#include <map>
#include <regex>
#include <fstream>
#include <string>

#include "base/time/time.h"
#include "base/task/thread_pool.h"
#include "base/files/file_util.h"
#include "base/files/file_enumerator.h"
#include "net/disk_cache/simple/simple_util.h"
#include "cef/libcef/browser/net_service/stream_reader_url_loader.h"
#include "base/trace_event/trace_event.h"
#include "third_party/bounds_checking_function/include/securec.h"

namespace oh_code_cache {

using HeaderMap = std::multimap<std::string, std::string>;

const std::string kFileTag = "RC";

const std::string kURLKey = "URL";
const std::string kContentLengthKey = "Content-Length";
const std::string kETagKey = "E-Tag";
const std::string kLastModifiedKey = "Last-Modified";
const std::string kAccessControlAllowOriginKey = "Access-Control-Allow-Origin";
const std::string kFileHashKey = "File-Hash";

const base::FilePath::CharType kResponseCachePath[] =
    FILE_PATH_LITERAL("Cache_Data");
const base::FilePath::CharType kCacheMetadataFileName[] =
    FILE_PATH_LITERAL("metadata");
const base::FilePath::CharType kTempFilePath[] =
    FILE_PATH_LITERAL("temp");

const char kResponseDataID[] = "ResponseDataID";

enum class CacheError {
    NO_ERROR = 0,
    INTERNAL_ERROR = -1
};

enum class NextOp {
  WRITE_CODE_CACHE,
  THROW_ERROR,
  DO_NOTHING
};

class CacheOptions {
 public:
  explicit CacheOptions(const std::map<std::string, std::string> response_headers)
      : response_headers_(response_headers) {}

  std::map<std::string, std::string> response_headers_;
};

class TaskRunner {
 public:
  static scoped_refptr<base::SingleThreadTaskRunner> GetTaskRunner();

 private:
  static scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
};

class ResponseCacheMetadata {
 public:
  ResponseCacheMetadata() = default;
  ResponseCacheMetadata(const ResponseCacheMetadata& other) = default;
  ResponseCacheMetadata(const std::string& url,
                        const std::string& response_body,
                        const std::map<std::string, std::string>& response_headers);

  ~ResponseCacheMetadata() = default;

  std::string ToString();

  std::string url_hash_;
  std::string content_length_;
  std::string e_tag_;
  std::string last_modified_;
  std::string access_control_allow_origin_;
  std::string file_hash_;
};

class ResponseCache {
 public:
  ResponseCache(const std::string& url);
  ~ResponseCache() = default;

  static void InitCacheDirectory(base::FilePath path);
  static std::shared_ptr<ResponseCache> CreateResponseCache(const std::string& url);
  static void ClearAllCache();

  NextOp Write(const std::map<std::string, std::string> response_headers,
             const std::string response_body);
  bool CanUseCache();

  static std::unique_ptr<base::FilePath> cache_dir_path_;
  static std::map<std::string, std::shared_ptr<ResponseCacheMetadata>> cache_metadata_map_;

  std::string url_;
  std::string url_hash_;
  base::FilePath metadata_file_path_;
  base::FilePath cache_file_path_;
  std::string response_body_in_;
  std::string response_body_out_;
  std::shared_ptr<ResponseCacheMetadata> metadata_in_;
  std::shared_ptr<ResponseCacheMetadata> metadata_out_;

 private:
  bool CreateStream();
  bool FindMetadata();
  bool ReadMetadata();
  bool ReadContent();
  bool NeedUpdate();
  NextOp DoCreate();
  NextOp DoUpdate();
  bool DoUpdateMetadata();
  bool DoWriteIntoFile(base::FilePath path, std::string data);
  bool DeleteCacheFile();
  void CloseStream();

  std::shared_ptr<std::fstream> metadata_file_stream_;
};

class ResourceResponse : public net_service::ResourceResponse {
 public:
  ResourceResponse(std::shared_ptr<ResponseCache> cache);

  bool OpenInputStream(int32_t request_id,
                       const network::ResourceRequest& request,
                       OpenCallback callback);

  void GetResponseHeaders(int32_t request_id,
                          int* status_code,
                          std::string* reason_phrase,
                          std::string* mime_type,
                          std::string* charset,
                          int64_t* content_length,
                          HeaderMap* extra_headers);

#if BUILDFLAG(IS_OHOS)
  const std::string& GetResponseData() {static const std::string data; return data;}
  size_t GetResponseDataBuffer(char* data, size_t dest_size) {return 0;}
  size_t GetResponseDataBufferSize() {return 0;}
#endif

  std::shared_ptr<ResponseCache> response_cache_;
};

class InputStream : public net_service::InputStream {
 public:
  InputStream() = default;
  InputStream(const std::string& data);

  bool Skip(int64_t n, int64_t* byte_skipped, SkipCallback callback);

  bool Read(net::IOBuffer* dest,
            int length,
            int* bytes_read,
            ReadCallback callback);

 private:
  std::string data_;
  size_t offset_ = 0;
};
}  // namespace oh_code_cache
