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

#ifndef COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_USER_UNINDEXED_RULESET_STREAM_GENERATOR_H_
#define COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_USER_UNINDEXED_RULESET_STREAM_GENERATOR_H_

#include <stdint.h>

#include <memory>
#include <sstream>

namespace base {
class FilePath;
}

namespace google {
namespace protobuf {
namespace io {
class ZeroCopyInputStream;
}
}  // namespace protobuf
}  // namespace google

namespace subresource_filter {

class CopyingFileInputStream;
struct UserUnindexedRulesetInfo;

// Processes the on-disk representation of the unindexed ruleset data into a
// stream via which a client can read this data.
class UserUnindexedRulesetStreamGenerator {
 public:
  explicit UserUnindexedRulesetStreamGenerator(
      const UserUnindexedRulesetInfo& ruleset_info);
  ~UserUnindexedRulesetStreamGenerator();

  UserUnindexedRulesetStreamGenerator(
      const UserUnindexedRulesetStreamGenerator&) = delete;
  UserUnindexedRulesetStreamGenerator& operator=(
      const UserUnindexedRulesetStreamGenerator&) = delete;

  // Returns a ZeroCopyInputStream* via which the unindexed ruleset data can be
  // streamed. If the returned pointer is null, the stream is not valid.
  // NOTE: The returned pointer will be valid only for the lifetime of this
  // object.
  google::protobuf::io::ZeroCopyInputStream* ruleset_stream() {
    return ruleset_stream_.get();
  }

  // Returns the size of the unindexed ruleset data in bytes.
  // If the size is < 0, the stream is not valid.
  int64_t ruleset_size() const { return ruleset_size_; }

 private:
  // Generates |ruleset_stream_| from the file at |ruleset_path_|.
  void GenerateStreamFromFile(base::FilePath ruleset_path);

  // Generates |ruleset_stream_| from the contents of the string stored in the
  // resource bundle at |resource_id|.
  void GenerateStreamFromResourceId(int resource_id);

  int64_t ruleset_size_ = -1;

  // Used when the stream is generated from a file on disk.
  std::unique_ptr<CopyingFileInputStream> copying_stream_;

  // Used when the stream is generated from a resource ID.
  std::istringstream string_stream_;

  // The stream via which a client of this class can read the data of the
  // unindexed ruleset.
  std::unique_ptr<google::protobuf::io::ZeroCopyInputStream> ruleset_stream_;
};

}  // namespace subresource_filter

#endif  // COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_USER_UNINDEXED_RULESET_STREAM_GENERATOR_H_
