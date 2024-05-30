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

#include "components/subresource_filter/core/browser/user_subresource_filter_constants.h"

namespace subresource_filter {

const base::FilePath::CharType kUserTopLevelDirectoryName[] =
    FILE_PATH_LITERAL("User Subresource Filter");

const base::FilePath::CharType kUserIndexedRulesetBaseDirectoryName[] =
    FILE_PATH_LITERAL("User Indexed Rules");

const base::FilePath::CharType kUserUnindexedRulesetBaseDirectoryName[] =
    FILE_PATH_LITERAL("User Unindexed Rules");

const base::FilePath::CharType kUserRulesetDataFileName[] =
    FILE_PATH_LITERAL("Ruleset Data");

const base::FilePath::CharType kUserLicenseFileName[] =
    FILE_PATH_LITERAL("LICENSE");

const base::FilePath::CharType kUserSentinelFileName[] =
    FILE_PATH_LITERAL("Indexing in Progress");

const base::FilePath::CharType kUserUnindexedRulesetLicenseFileName[] =
    FILE_PATH_LITERAL("LICENSE");

const base::FilePath::CharType kUserUnindexedRulesetDataFileName[] =
    FILE_PATH_LITERAL("Filtering Rules");

const base::FilePath::CharType kUserDataBaseFileName[] =
    FILE_PATH_LITERAL("user_adblock.db");

const base::FilePath::CharType kUserEasylistFileName[] =
    FILE_PATH_LITERAL("user_easylist.txt");
}  // namespace subresource_filter
