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

#include "components/subresource_filter/core/browser/user_ruleset_version.h"

#include "base/logging.h"
#include "base/strings/strcat.h"
#include "base/trace_event/traced_value.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"
#include "components/subresource_filter/core/common/indexed_ruleset.h"

namespace subresource_filter {

// Names of the preferences storing the most recent ruleset version that
// was successfully stored to disk.
const char kUserRulesetContentVersion[] = ".user_ruleset_version.content";
const char kUserRulesetFormatVersion[] = ".user_ruleset_version.format";
const char kUserRulesetChecksum[] = ".user_ruleset_version.checksum";
const char kUserEasylistPath[] = ".user_easylist.path";
const char kUserEasylistReplace[] = ".user_easylist.replace";

std::string ContentVersionPrefName(std::string_view filter_tag) {
  return base::StrCat({filter_tag, kUserRulesetContentVersion});
}

std::string FormatVersionPrefName(std::string_view filter_tag) {
  return base::StrCat({filter_tag, kUserRulesetFormatVersion});
}

std::string ChecksumPrefName(std::string_view filter_tag) {
  return base::StrCat({filter_tag, kUserRulesetChecksum});
}

std::string EasylistPathPrefName(std::string_view filter_tag) {
  return base::StrCat({filter_tag, kUserEasylistPath});
}

std::string EasylistReplacePrefName(std::string_view filter_tag) {
  return base::StrCat({filter_tag, kUserEasylistReplace});
}

UserUnindexedRulesetInfo::UserUnindexedRulesetInfo() = default;
UserUnindexedRulesetInfo::~UserUnindexedRulesetInfo() = default;
UserUnindexedRulesetInfo::UserUnindexedRulesetInfo(
    const UserUnindexedRulesetInfo&) = default;
UserUnindexedRulesetInfo& UserUnindexedRulesetInfo::operator=(
    const UserUnindexedRulesetInfo&) = default;

UserIndexedRulesetVersion::UserIndexedRulesetVersion(
    std::string_view filter_tag)
    : filter_tag(std::string(filter_tag)) {}
UserIndexedRulesetVersion::UserIndexedRulesetVersion(
    const std::string& content_version,
    int format_version,
    std::string_view filter_tag)
    : content_version(content_version),
      format_version(format_version),
      filter_tag(std::string(filter_tag)) {}
UserIndexedRulesetVersion::~UserIndexedRulesetVersion() = default;
UserIndexedRulesetVersion& UserIndexedRulesetVersion::operator=(
    const UserIndexedRulesetVersion&) = default;

// static
void UserIndexedRulesetVersion::RegisterPrefs(PrefRegistrySimple* registry,
                                              std::string_view filter_tag) {
  registry->RegisterStringPref(ContentVersionPrefName(filter_tag),
                               std::string());
  registry->RegisterIntegerPref(FormatVersionPrefName(filter_tag), 0);
  registry->RegisterIntegerPref(ChecksumPrefName(filter_tag), 0);
  registry->RegisterStringPref(EasylistPathPrefName(filter_tag), std::string());
  registry->RegisterBooleanPref(EasylistReplacePrefName(filter_tag), false);
}

// static
int UserIndexedRulesetVersion::CurrentFormatVersion() {
  return RulesetIndexer::kIndexedFormatVersion;
}

void UserIndexedRulesetVersion::ReadFromPrefs(PrefService* local_state) {
  format_version = local_state->GetInteger(FormatVersionPrefName(filter_tag));
  content_version = local_state->GetString(ContentVersionPrefName(filter_tag));
  checksum = local_state->GetInteger(ChecksumPrefName(filter_tag));
}

void UserIndexedRulesetVersion::ReadUserEasylistInfoFromPrefs(
    PrefService* local_state) {
  if (!local_state) {
    LOG(ERROR) << "[Adblock] web core has not been initialized";
    return;
  }

  user_easylist_path = local_state->GetString(EasylistPathPrefName(filter_tag));
  user_easylist_replace =
      local_state->GetBoolean(EasylistReplacePrefName(filter_tag));
}

bool UserIndexedRulesetVersion::IsValid() const {
  return format_version != 0 && !content_version.empty();
}

bool UserIndexedRulesetVersion::IsCurrentFormatVersion() const {
  return format_version == CurrentFormatVersion();
}

void UserIndexedRulesetVersion::SaveToPrefs(PrefService* local_state) const {
  local_state->SetInteger(FormatVersionPrefName(filter_tag), format_version);
  local_state->SetString(ContentVersionPrefName(filter_tag), content_version);
  local_state->SetInteger(ChecksumPrefName(filter_tag), checksum);
}

void UserIndexedRulesetVersion::SaveUserEasylistInfoToPrefs(
    PrefService* local_state,
    std::string& path,
    bool replace) const {
  if (!local_state) {
    LOG(ERROR) << "[Adblock] web core has not been initialized";
    return;
  }

  local_state->SetString(EasylistPathPrefName(filter_tag), path);

  local_state->SetBoolean(EasylistReplacePrefName(filter_tag), replace);
}

std::unique_ptr<base::trace_event::TracedValue>
UserIndexedRulesetVersion::ToTracedValue() const {
  auto value = std::make_unique<base::trace_event::TracedValue>();
  value->SetString("content_version", content_version);
  value->SetInteger("format_version", format_version);
  return value;
}

}  // namespace subresource_filter
