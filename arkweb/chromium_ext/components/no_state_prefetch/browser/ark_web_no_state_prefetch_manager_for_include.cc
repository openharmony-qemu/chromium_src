/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
std::unique_ptr<NoStatePrefetchHandle>
NoStatePrefetchManager::StartOhPrefetchingFromOmnibox(
    const GURL& url,
    content::SessionStorageNamespace* session_storage_namespace,
    const gfx::Size& size,
    content::PreloadingAttempt* attempt,
    const std::string& extra_headers) {
  if (!url.is_empty() && url.is_valid()) {
    oh_prefetch_urls_.insert(url);
  }
  return StartPrefetchingWithPreconnectFallback(
      ORIGIN_OMNIBOX, url, content::Referrer(), std::nullopt, gfx::Rect(size),
      session_storage_namespace, attempt ? attempt->GetWeakPtr() : nullptr,
      extra_headers);
}

bool NoStatePrefetchManager::MayHitOmniboxUrl(
    const GURL& url,
    Origin origin,
    base::WeakPtr<content::PreloadingAttempt> attempt) {
  auto it = oh_prefetch_urls_.find(url);
  if (it == oh_prefetch_urls_.end()) {
    SkipNoStatePrefetchContentsAndMaybePreconnect(
        url, origin, FINAL_STATUS_TOO_MANY_PROCESSES);
    if (attempt) {
      attempt->SetFailureReason(
          ToPreloadingFailureReason(FINAL_STATUS_TOO_MANY_PROCESSES));
    }
    return false;
  }
  LOG(DEBUG) << "Prefetch url in single-process mode.";
  oh_prefetch_urls_.erase(it);
  return true;
}
#endif  // defined(ARKWEB_NO_STATE_PREFETCH)
