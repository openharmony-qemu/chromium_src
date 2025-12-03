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

#include "libcef/browser/subresource_filter/adblock_safe_browsing_database_manager_impl.h"

#include <set>
#include <string>
#include <vector>

#include "base/notreached.h"
#include "base/notimplemented.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"

namespace safe_browsing {

SafeBrowsingDatabaseManagerImpl::SafeBrowsingDatabaseManagerImpl(
    scoped_refptr<base::SequencedTaskRunner> ui_task_runner)
    : SafeBrowsingDatabaseManager(std::move(ui_task_runner)), enabled_(false) {}

void SafeBrowsingDatabaseManagerImpl::CancelCheck(Client* client) {
  NOTIMPLEMENTED();
}

bool SafeBrowsingDatabaseManagerImpl::CanCheckUrl(const GURL& url) const {
  NOTIMPLEMENTED();
  return (url != GURL("about:blank"));
}

bool SafeBrowsingDatabaseManagerImpl::CheckBrowseUrl(
    const GURL& url,
    const SBThreatTypeSet& threat_types,
    Client* client,
    CheckBrowseUrlType check_type) {
  NOTIMPLEMENTED();
  return true;
}

bool SafeBrowsingDatabaseManagerImpl::CheckDownloadUrl(
    const std::vector<GURL>& url_chain,
    Client* client) {
  NOTIMPLEMENTED();
  return true;
}

bool SafeBrowsingDatabaseManagerImpl::CheckExtensionIDs(
    const std::set<std::string>& extension_ids,
    Client* client) {
  NOTIMPLEMENTED();
  return true;
}

void SafeBrowsingDatabaseManagerImpl::CheckUrlForHighConfidenceAllowlist(
    const GURL& url,
    CheckUrlForHighConfidenceAllowlistCallback callback) {
  NOTIMPLEMENTED();
  std::move(callback).Run(
      /*url_on_high_confidence_allowlist=*/false,
      /*logging_details=*/std::nullopt);
}

bool SafeBrowsingDatabaseManagerImpl::CheckUrlForSubresourceFilter(
    const GURL& url,
    Client* client) {
  NOTIMPLEMENTED();
  return true;
}

AsyncMatch SafeBrowsingDatabaseManagerImpl::CheckCsdAllowlistUrl(
    const GURL& url,
    Client* client) {
  NOTIMPLEMENTED();
  return AsyncMatch::MATCH;
}

void SafeBrowsingDatabaseManagerImpl::MatchDownloadAllowlistUrl(
    const GURL& url,
    base::OnceCallback<void(bool)> callback) {
  NOTIMPLEMENTED();
  std::move(callback).Run(true);
}

safe_browsing::ThreatSource
SafeBrowsingDatabaseManagerImpl::GetBrowseUrlThreatSource(
    CheckBrowseUrlType check_type) const {
  NOTIMPLEMENTED();
  return safe_browsing::ThreatSource::UNKNOWN;
}

safe_browsing::ThreatSource
SafeBrowsingDatabaseManagerImpl::GetNonBrowseUrlThreatSource() const {
  NOTIMPLEMENTED();
  return safe_browsing::ThreatSource::UNKNOWN;
}

void SafeBrowsingDatabaseManagerImpl::StartOnUIThread(
    scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory,
    const V4ProtocolConfig& config) {
  SafeBrowsingDatabaseManager::StartOnUIThread(url_loader_factory, config);
  enabled_ = true;
}

void SafeBrowsingDatabaseManagerImpl::StopOnUIThread(bool shutdown) {
  enabled_ = false;
  SafeBrowsingDatabaseManager::StopOnUIThread(shutdown);
}

bool SafeBrowsingDatabaseManagerImpl::IsDatabaseReady() const {
  return enabled_;
}

}  // namespace safe_browsing
