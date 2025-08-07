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

#ifndef CHROME_BROWSER_EXTENSIONS_API_DECLARATIVE_CONTENT_DECLARATIVE_CONTENT_IS_BOOKMARKED_CONDITION_TRACKER_H_
#error \
    "must be include form CHROME_BROWSER_EXTENSIONS_API_DECLARATIVE_CONTENT_DECLARATIVE_CONTENT_IS_BOOKMARKED_CONDITION_TRACKER_H_"
#endif

namespace extensions {

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
void DeclarativeContentIsBookmarkedConditionTracker::OnBookmarksCreated(
    const GURL& url) {
  if (!extensive_bookmark_changes_in_progress_) {
    for (const auto& web_contents_tracker_pair : per_web_contents_tracker_) {
      web_contents_tracker_pair.second->BookmarkAddedForUrl(url);
    }
  }
}

void DeclarativeContentIsBookmarkedConditionTracker::OnBookmarksRemoved(
    const std::set<GURL>& urls) {
  if (!extensive_bookmark_changes_in_progress_) {
    for (const auto& web_contents_tracker_pair : per_web_contents_tracker_) {
      web_contents_tracker_pair.second->BookmarkRemovedForUrls(urls);
    }
  }
}

void DeclarativeContentIsBookmarkedConditionTracker::OnBookmarksImportBegin() {
  ExtensiveBookmarkChangesBeginning();
}

void DeclarativeContentIsBookmarkedConditionTracker::OnBookmarksImportEnd() {
  ExtensiveBookmarkChangesEnded();
}
#endif
}  // namespace extensions
