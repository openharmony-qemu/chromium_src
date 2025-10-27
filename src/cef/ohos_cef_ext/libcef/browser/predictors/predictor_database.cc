/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <algorithm>
#include <string>

#include "base/logging.h"
#include "base/values.h"
#include "components/pref_registry/pref_registry_syncable.h"
#include "components/prefs/pref_service.h"
#include "components/prefs/scoped_user_pref_update.h"
#include "components/user_prefs/user_prefs.h"
#include "content/public/browser/browser_context.h"
#include "libcef/browser/browser_context.h"
#include "predictor_database.h"

namespace {

const char kUrl[] = "url";
const char kOrigin[] = "origin";
const char kCount[] = "count";
const size_t kMaxPreconnect = 3;
const size_t kMaxVisitedUrlsLength = 20;

bool Compare(const predictor::VisitedUrlInfo& left,
             const predictor::VisitedUrlInfo& right) {
  return left.count() > right.count();
}

}  // namespace

namespace predictor {

const char kVisitedUrls[] = "predictors.visited_urls";

VisitedUrlInfo::VisitedUrlInfo(const GURL& url)
    : url_(url.spec()),
      origin_(url::Origin::Create(url).GetURL().spec()),
      count_(1) {}

VisitedUrlInfo::VisitedUrlInfo(const std::string& url,
                               const std::string& origin,
                               int count)
    : url_(url), origin_(origin), count_(count) {}

base::Value::Dict VisitedUrlInfo::ToDict() {
  base::Value::Dict dict;
  dict.Set(kUrl, url());
  dict.Set(kCount, count());
  dict.Set(kOrigin, origin());
  return dict;
}

bool VisitedUrlInfo::is_valid() {
  return !url_.empty() && !origin_.empty();
}

bool VisitedUrlInfo::operator==(const VisitedUrlInfo& info) const {
  return url() == info.url();
}

// static
VisitedUrlInfo VisitedUrlInfo::FromDict(const base::Value::Dict& dict) {
  const std::string* url = dict.FindString(kUrl);
  const std::string* origin = dict.FindString(kOrigin);
  int count = dict.FindInt(kCount).value_or(0);

  return VisitedUrlInfo(*url, *origin, count);
}

predictor::PreconnectUrlInfo::PreconnectUrlInfo() {}

predictor::PreconnectUrlInfo::~PreconnectUrlInfo() {}

predictor::PreconnectUrlInfo::PreconnectUrlInfo(
    const PreconnectUrlInfo& other) = default;

std::vector<PreconnectUrlInfo> PredictorDatabase::preconnect_url_info_list =
    std::vector<PreconnectUrlInfo>();

// static
PredictorDatabase* PredictorDatabase::GetInstance() {
  static PredictorDatabase instance;
  return &instance;
}

// static
void PredictorDatabase::RegisterPrefs(
    user_prefs::PrefRegistrySyncable* registry) {
  registry->RegisterListPref(kVisitedUrls);
}

void PredictorDatabase::RecordVisitedUrl(VisitedUrlInfo url_info) {
  auto cef_browser_context = CefBrowserContext::GetAll()[0];
  if (!cef_browser_context) {
    return;
  }

  content::BrowserContext* browser_context =
      cef_browser_context->AsBrowserContext();
  PrefService* pref_service = user_prefs::UserPrefs::Get(browser_context);
  if (!pref_service) {
    return;
  }

  // Update last visited urls in prefs.
  bool is_update = false;
  for (auto& info : visited_urls_) {
    if (info == url_info) {
      info.increment_count();
      is_update = true;
    }
  }

  if (!is_update && (visited_urls_.size() + 1) > kMaxVisitedUrlsLength) {
    return;
  }

  {
    ScopedListPrefUpdate update(pref_service, kVisitedUrls);
    if (is_update) {
      pref_service->ClearPref(kVisitedUrls);
      for (auto& info : visited_urls_) {
        update->Append(info.ToDict());
      }
    } else {
      visited_urls_.push_back(url_info);
      update->Append(url_info.ToDict());
    }

    pref_service->CommitPendingWrite();
  }
}

void PredictorDatabase::UpdateFromPrefsAndClear() {
  // Get last visited url list.
  if (last_visited_urls_.size() == 0 && last_visited_urls_sorted_.size() == 0) {
    std::vector<CefBrowserContext*> browser_context_all =
        CefBrowserContext::GetAll();
    if (browser_context_all.size() == 0) {
      return;
    }
    CefBrowserContext* context = browser_context_all[0];
    content::BrowserContext* browser_context = context->AsBrowserContext();
    if (!browser_context) {
      return;
    }
    PrefService* pref_service = user_prefs::UserPrefs::Get(browser_context);
    if (!pref_service) {
      return;
    }
    const base::Value::List& last_visited_urls =
        pref_service->GetList(kVisitedUrls);
    for (const auto& info : last_visited_urls) {
      last_visited_urls_.push_back(VisitedUrlInfo::FromDict(info.GetDict()));
      last_visited_urls_sorted_.push_back(
          VisitedUrlInfo::FromDict(info.GetDict()));
    }

    // Sort the list according to count.
    std::sort(last_visited_urls_sorted_.begin(),
              last_visited_urls_sorted_.end(), Compare);
    pref_service->ClearPref(kVisitedUrls);
  }
}

std::set<std::string> PredictorDatabase::GetRecentVisitedUrl() {
  std::set<std::string> recent_visited_urls;

  UpdateFromPrefsAndClear();
  // Get the most recently accessed url based on the number of occurrences of
  // origin.
  size_t count = 1;
  std::map<std::string, int> recent_visited_origin_map;
  for (auto& url_info : last_visited_urls_sorted_) {
    if (!recent_visited_origin_map.count(url_info.url())) {
      if (count++ > kMaxPreconnect) {
        break;
      }
      recent_visited_origin_map[url_info.url()] = url_info.count();
      recent_visited_urls.insert(url_info.url());
    } else {
      recent_visited_origin_map[url_info.url()] += url_info.count();
    }
  }

  // Get the first few URLs visited first.
  if (last_visited_urls_.size() > 0) {
    size_t url_size = std::min(kMaxPreconnect, last_visited_urls_.size());
    for (int i = int(url_size - 1); i >= 0; i--) {
      recent_visited_urls.insert(last_visited_urls_[i].url());
    }
  }

  return recent_visited_urls;
}

}  // namespace predictor
