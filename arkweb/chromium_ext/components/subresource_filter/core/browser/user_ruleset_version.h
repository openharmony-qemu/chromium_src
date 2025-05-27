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

#ifndef COMPONENTS_SUBRESOURCE_FILTER_CORE_BROWSER_USER_RULESET_VERSION_H_
#define COMPONENTS_SUBRESOURCE_FILTER_CORE_BROWSER_USER_RULESET_VERSION_H_

#include <memory>
#include <string>

#include "base/files/file_path.h"

class PrefRegistrySimple;
class PrefService;

namespace base {
namespace trace_event {
class TracedValue;
}  // namespace trace_event
}  // namespace base

namespace subresource_filter {

extern const char kUserRulesetContentVersion[];
extern const char kUserRulesetFormatVersion[];
extern const char kUserRulesetChecksum[];
extern const char kUserEasylistPath[];
extern const char kUserEasylistReplace[];

// Encapsulates information about a version of unindexed subresource
// filtering rules on disk.
struct UserUnindexedRulesetInfo {
  UserUnindexedRulesetInfo();
  ~UserUnindexedRulesetInfo();
  UserUnindexedRulesetInfo(const UserUnindexedRulesetInfo&);
  UserUnindexedRulesetInfo& operator=(const UserUnindexedRulesetInfo&);

  // The version of the ruleset contents. Because the wire format of unindexed
  // rules is expected to be stable over time (at least backwards compatible),
  // the unindexed ruleset is uniquely identified by its content version.
  //
  // The version string must not be empty, but can be any string otherwise.
  // There is no ordering defined on versions.
  std::string content_version;

  // The (optional) path to the file containing the unindexed subresource
  // filtering rules. One (but not both) of |ruleset_path| and |resource_id|
  // should be set.
  base::FilePath ruleset_path;

  // The (optional) grit resource id containing the unindexed subresource
  // filtering rules, which if supplied is given to the ResourceBundle to
  // resolve to a string. One (but not both) of |ruleset_path| and |resource_id|
  // should be set.
  int resource_id = 0;

  // The (optional) path to a file containing the applicable license, which will
  // be copied next to the indexed ruleset. For convenience, the lack of license
  // can be indicated not only by setting |license_path| to empty, but also by
  // setting it to any non existent path.
  base::FilePath license_path;
};

// Encapsulates the combination of the binary format version of the indexed
// ruleset, and the version of the ruleset contents.
//
// In contrast to the unindexed ruleset, the binary format of the index data
// structures is expected to evolve over time, so the indexed ruleset is
// identified by a pair of versions: the content version of the rules that have
// been indexed; and the binary format version of the indexed data structures.
// It also contains a checksum of the data, to ensure it hasn't been corrupted.
struct UserIndexedRulesetVersion {
  explicit UserIndexedRulesetVersion(std::string_view filter_tag);
  UserIndexedRulesetVersion(const std::string& content_version,
                            int format_version,
                            std::string_view filter_tag);
  ~UserIndexedRulesetVersion();
  UserIndexedRulesetVersion& operator=(const UserIndexedRulesetVersion&);

  static void RegisterPrefs(PrefRegistrySimple* registry,
                            std::string_view filter_tag);
  static int CurrentFormatVersion();

  bool IsValid() const;
  bool IsCurrentFormatVersion() const;

  void SaveToPrefs(PrefService* local_state) const;
  void ReadFromPrefs(PrefService* local_state);

  void SaveUserEasylistInfoToPrefs(PrefService* local_state,
                                   std::string& path,
                                   bool replace) const;
  void ReadUserEasylistInfoFromPrefs(PrefService* local_state);

  std::unique_ptr<base::trace_event::TracedValue> ToTracedValue() const;

  std::string content_version;
  int format_version = 0;
  int checksum = 0;
  bool user_easylist_replace = false;
  std::string user_easylist_path;

  // Unique tag identifying the type of filter this IndexedRulesetVersion
  // is used for and thus what type of ruleset it corresponds to. Also used
  // as the prefix for pref names that store the current version.
  std::string filter_tag;
};

}  // namespace subresource_filter

#endif  // COMPONENTS_SUBRESOURCE_FILTER_CORE_BROWSER_USER_RULESET_VERSION_H_
