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

#ifndef PREDICTORS_DATABASE_H_
#define PREDICTORS_DATABASE_H_

#include <string>
#include <vector>

#include "base/no_destructor.h"
#include "base/values.h"
#include "components/pref_registry/pref_registry_syncable.h"
#include "url/gurl.h"
#include "url/origin.h"

namespace predictor {

extern const char kVisitedUrls[];

struct PreconnectUrlInfo {
  PreconnectUrlInfo();
  PreconnectUrlInfo(const PreconnectUrlInfo& other);
  ~PreconnectUrlInfo();
  std::string url;
  int num_sockets;
  bool is_preconnectable;
};

class VisitedUrlInfo {
 public:
  VisitedUrlInfo(const GURL& url);
  VisitedUrlInfo(const std::string& url, const std::string& origin, int count);
  ~VisitedUrlInfo() = default;
  bool operator==(const VisitedUrlInfo& info) const;

  static VisitedUrlInfo FromDict(const base::Value::Dict& dict);

  base::Value::Dict ToDict();

  std::string url() const { return url_; }
  std::string origin() const { return origin_; }
  int count() const { return count_; }
  void increment_count() { count_++; }

  bool is_valid();

 private:
  std::string url_;
  std::string origin_;
  int count_;
};

class PredictorDatabase {
 public:
  PredictorDatabase() = default;
  PredictorDatabase(const PredictorDatabase&) = delete;
  PredictorDatabase& operator=(const PredictorDatabase&) = delete;

  static void RegisterPrefs(user_prefs::PrefRegistrySyncable* registry);
  static PredictorDatabase* GetInstance();

  void RecordVisitedUrl(VisitedUrlInfo url_info);
  std::set<std::string> GetRecentVisitedUrl();

  static std::vector<PreconnectUrlInfo> preconnect_url_info_list;

 private:
  void UpdateFromPrefsAndClear();

  std::vector<VisitedUrlInfo> last_visited_urls_;
  std::vector<VisitedUrlInfo> last_visited_urls_sorted_;
  std::vector<VisitedUrlInfo> visited_urls_;
};

}  // namespace predictor

#endif  // PREDICTORS_DATABASE_H_
