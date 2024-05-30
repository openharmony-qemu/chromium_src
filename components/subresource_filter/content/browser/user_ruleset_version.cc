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

#include "components/subresource_filter/content/browser/user_ruleset_version.h"

#include "base/trace_event/traced_value.h"
#include "base/logging.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"
#include "components/subresource_filter/core/common/indexed_ruleset.h"

namespace subresource_filter {

// Names of the preferences storing the most recent ruleset version that
// was successfully stored to disk.
const char kSubresourceFilterUserRulesetContentVersion[] =
    "subresource_filter.user_ruleset_version.content";
const char kSubresourceFilterUserRulesetFormatVersion[] =
    "subresource_filter.user_ruleset_version.format";
const char kSubresourceFilterUserRulesetChecksum[] =
    "subresource_filter.user_ruleset_version.checksum";
const char kSubresourceFilterUserEasylistPath[] =
    "subresource_filter.user_easylist.path";
const char kSubresourceFilterUserEasylistReplace[] =
    "subresource_filter.user_easylist.replace";

UserUnindexedRulesetInfo::UserUnindexedRulesetInfo() = default;
UserUnindexedRulesetInfo::~UserUnindexedRulesetInfo() = default;
UserUnindexedRulesetInfo::UserUnindexedRulesetInfo(
    const UserUnindexedRulesetInfo&) = default;
UserUnindexedRulesetInfo& UserUnindexedRulesetInfo::operator=(
    const UserUnindexedRulesetInfo&) = default;

UserIndexedRulesetVersion::UserIndexedRulesetVersion() = default;
UserIndexedRulesetVersion::UserIndexedRulesetVersion(
    const std::string& content_version,
    int format_version)
    : content_version(content_version), format_version(format_version) {}
UserIndexedRulesetVersion::~UserIndexedRulesetVersion() = default;
UserIndexedRulesetVersion& UserIndexedRulesetVersion::operator=(
    const UserIndexedRulesetVersion&) = default;

// static
void UserIndexedRulesetVersion::RegisterPrefs(PrefRegistrySimple* registry) {
  registry->RegisterStringPref(kSubresourceFilterUserRulesetContentVersion,
                               std::string());
  registry->RegisterIntegerPref(kSubresourceFilterUserRulesetFormatVersion, 0);
  registry->RegisterIntegerPref(kSubresourceFilterUserRulesetChecksum, 0);
  registry->RegisterStringPref(kSubresourceFilterUserEasylistPath,
                               std::string());
  registry->RegisterBooleanPref(kSubresourceFilterUserEasylistReplace, false);
}

// static
int UserIndexedRulesetVersion::CurrentFormatVersion() {
  return RulesetIndexer::kIndexedFormatVersion;
}

void UserIndexedRulesetVersion::ReadFromPrefs(PrefService* local_state) {
  format_version =
      local_state->GetInteger(kSubresourceFilterUserRulesetFormatVersion);
  content_version =
      local_state->GetString(kSubresourceFilterUserRulesetContentVersion);
  checksum = local_state->GetInteger(kSubresourceFilterUserRulesetChecksum);
}

void UserIndexedRulesetVersion::ReadUserEasylistInfoFromPrefs(
    PrefService* local_state) {
  if (!local_state) {
    LOG(ERROR) << "[Adblock] web core has not been initialized";
    return;
  }

  user_easylist_path =
      local_state->GetString(kSubresourceFilterUserEasylistPath);
  user_easylist_replace =
      local_state->GetBoolean(kSubresourceFilterUserEasylistReplace);
}

bool UserIndexedRulesetVersion::IsValid() const {
  return format_version != 0 && !content_version.empty();
}

bool UserIndexedRulesetVersion::IsCurrentFormatVersion() const {
  return format_version == CurrentFormatVersion();
}

void UserIndexedRulesetVersion::SaveToPrefs(PrefService* local_state) const {
  local_state->SetInteger(kSubresourceFilterUserRulesetFormatVersion,
                          format_version);
  local_state->SetString(kSubresourceFilterUserRulesetContentVersion,
                         content_version);
  local_state->SetInteger(kSubresourceFilterUserRulesetChecksum, checksum);
}

void UserIndexedRulesetVersion::SaveUserEasylistInfoToPrefs(
    PrefService* local_state,
    std::string& path,
    bool replace) const {
  if (!local_state) {
    LOG(ERROR) << "[Adblock] web core has not been initialized";
    return;
  }

  local_state->SetString(kSubresourceFilterUserEasylistPath, path);

  local_state->SetBoolean(kSubresourceFilterUserEasylistReplace, replace);
}

std::unique_ptr<base::trace_event::TracedValue>
UserIndexedRulesetVersion::ToTracedValue() const {
  auto value = std::make_unique<base::trace_event::TracedValue>();
  value->SetString("content_version", content_version);
  value->SetInteger("format_version", format_version);
  return value;
}

}  // namespace subresource_filter
