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

#ifndef COMPONENTS_SUBRESOURCE_FILTER_CORE_BROWSER_USER_SUBRESOURCE_FILTER_CONSTANTS_H_
#define COMPONENTS_SUBRESOURCE_FILTER_CORE_BROWSER_USER_SUBRESOURCE_FILTER_CONSTANTS_H_

#include "base/files/file_path.h"

namespace subresource_filter {

// The name of the top-level directory under the user data directory that
// contains all files and subdirectories related to the subresource filter.
extern const base::FilePath::CharType kUserTopLevelDirectoryName[];

// Paths under |kTopLevelDirectoryName|
// ------------------------------------

// The name of the subdirectory under the top-level directory that stores
// versions of indexed rulesets. Files that belong to an IndexedRulesetVersion
// are stored under /format_version/content_version/.
extern const base::FilePath::CharType kUserIndexedRulesetBaseDirectoryName[];

// The name of the subdirectory under the top-level directory that stores
// versions of unindexed rulesets downloaded through the component updater.
extern const base::FilePath::CharType kUserUnindexedRulesetBaseDirectoryName[];

// Paths under IndexedRulesetVersion::GetSubdirectoryPathForVersion
// ----------------------------------------------------------------

// The name of the file that actually stores the ruleset contents.
extern const base::FilePath::CharType kUserRulesetDataFileName[];

// The name of the applicable license file, if any, stored next to the ruleset.
extern const base::FilePath::CharType kUserLicenseFileName[];

// The name of the sentinel file that is temporarily stored to indicate that the
// ruleset is being indexed.
extern const base::FilePath::CharType kUserSentinelFileName[];

// Paths under kUnindexedRulesetBaseDirectoryName
// ----------------------------------------------

// The name of the license file associated with the unindex ruleset.
extern const base::FilePath::CharType kUserUnindexedRulesetLicenseFileName[];

// The name of the file that stores the unindexed filtering rules.
extern const base::FilePath::CharType kUserUnindexedRulesetDataFileName[];

// The name of the file that stores meta data
extern const base::FilePath::CharType kUserDataBaseFileName[];

}  // namespace subresource_filter

#endif  // COMPONENTS_SUBRESOURCE_FILTER_CORE_BROWSER_USER_SUBRESOURCE_FILTER_CONSTANTS_H_
