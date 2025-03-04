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

#include "components/subresource_filter/content/shared/browser/user_unindexed_ruleset_stream_generator.h"

#include "base/check.h"
#include "base/check_op.h"
#include "base/files/file_path.h"
#include "components/subresource_filter/core/browser/user_ruleset_version.h"
#include "components/subresource_filter/core/common/copying_file_stream.h"
#include "third_party/protobuf/src/google/protobuf/io/zero_copy_stream_impl.h"
#include "third_party/protobuf/src/google/protobuf/io/zero_copy_stream_impl_lite.h"
#include "ui/base/resource/resource_bundle.h"

namespace subresource_filter {

UserUnindexedRulesetStreamGenerator::UserUnindexedRulesetStreamGenerator(
    const UserUnindexedRulesetInfo& ruleset_info) {
  bool has_ruleset_file = !ruleset_info.ruleset_path.empty();

  DCHECK(has_ruleset_file || ruleset_info.resource_id);
  DCHECK(!(has_ruleset_file && ruleset_info.resource_id));

  if (has_ruleset_file) {
    GenerateStreamFromFile(ruleset_info.ruleset_path);
  } else {
    GenerateStreamFromResourceId(ruleset_info.resource_id);
  }
}

UserUnindexedRulesetStreamGenerator::~UserUnindexedRulesetStreamGenerator() =
    default;

void UserUnindexedRulesetStreamGenerator::GenerateStreamFromFile(
    base::FilePath ruleset_path) {
  DCHECK(!ruleset_stream_);
  DCHECK(!copying_stream_);
  DCHECK_EQ(ruleset_size_, -1);

  base::File unindexed_ruleset_file(
      ruleset_path, base::File::FLAG_OPEN | base::File::FLAG_READ);

  if (!unindexed_ruleset_file.IsValid()) {
    return;
  }

  ruleset_size_ = unindexed_ruleset_file.GetLength();

  copying_stream_ = std::make_unique<CopyingFileInputStream>(
      std::move(unindexed_ruleset_file));
  ruleset_stream_ =
      std::make_unique<google::protobuf::io::CopyingInputStreamAdaptor>(
          copying_stream_.get(), 4096 /* buffer_size */);
}

void UserUnindexedRulesetStreamGenerator::GenerateStreamFromResourceId(
    int resource_id) {
  ui::ResourceBundle& bundle = ui::ResourceBundle::GetSharedInstance();
  std::string data_as_string = bundle.LoadDataResourceString(resource_id);
  ruleset_size_ = data_as_string.size();

  string_stream_.str(data_as_string);
  ruleset_stream_ = std::make_unique<google::protobuf::io::IstreamInputStream>(
      &string_stream_);
}

}  // namespace subresource_filter
