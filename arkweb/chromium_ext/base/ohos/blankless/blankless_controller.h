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

#ifndef BASE_OHOS_BLANK_OPT_CONTROLLER_H_
#define BASE_OHOS_BLANK_OPT_CONTROLLER_H_

#include <mutex>
#include <unordered_map>
#include <unordered_set>

namespace base {
namespace ohos {
struct BlanklessDumpInfo {
  uint64_t blankless_key = 0;
  int64_t pref_hash = 0;
  int32_t lcp_time = 0;
  uint32_t nweb_id = 0;
  bool dump_enabled = false;
};

class BlanklessController {
public:
  static constexpr uint64_t INVALID_BLANKLESS_KEY = UINT64_MAX;

  static uint64_t ConvertToBlanklessKey(const std::string& value);

  ~BlanklessController() = default;

  BlanklessController(const BlanklessController&) = delete;
  BlanklessController& operator=(const BlanklessController&) = delete;
  BlanklessController(const BlanklessController&&) = delete;
  BlanklessController& operator=(const BlanklessController&&) = delete;

  static BlanklessController& GetInstance();

  static bool CheckEnableForDeviceType();
  static bool CheckGlobalProperty();

  static bool SimpleCheck();

  uint64_t GetBlanklessLoadingKey(const std::string& url, int32_t nweb_id);

  void SetPrivacyStatus(int32_t nweb_id, bool is_private);
  bool GetPrivacyStatus(int32_t nweb_id);

  void SetCapacity(int32_t capacity);
  int32_t GetCapacity() const;

  void RecordBlanklessKey(int32_t nweb_id, uint64_t blankless_key);
  bool SetLoadingEnabled(int32_t nweb_id, uint64_t blankless_key, bool enabled);

  uint32_t AddEnabledUrlList(const std::vector<std::string>& url_list);
  void RemoveEnabledUrlList(const std::vector<std::string>& url_list);
  void ClearEnabledUrlList();
  bool CheckEnableForUrl(const std::string& url);

private:
  enum class LoadingStatus {
    LOADING_UNSET,
    LOADING_CAN_SET,
    LOADING_ENABLE,
    LOADING_DISABLE
  };

  struct NWebInfo {
    uint64_t blankless_key = INVALID_BLANKLESS_KEY;
    LoadingStatus status = LoadingStatus::LOADING_UNSET;
  };

  BlanklessController() = default;

  uint64_t GetKeyAndResetLoadingStatus(int32_t nweb_id);

  /* This Class is designed for testing and will be delete someday. */
  class BlankOptWhiteList {
  public:
    BlankOptWhiteList() = default;
    ~BlankOptWhiteList() = default;

    void LoadWhiteList();
    bool CheckWhiteList(const std::string& url);

  private:
    bool ExactMatch(const std::string& url);
    bool FuzzyMatch(const std::string& url);

    std::unordered_set<std::string> m_exact_match_set_;
    std::unordered_set<std::string> m_fuzzy_match_set_;
    bool m_is_loaded_ = false;
  };
  BlankOptWhiteList m_white_list_;

  std::mutex m_enabled_url_set_mtx_;
  std::unordered_set<std::string> m_enabled_url_set_;

  std::mutex m_privacy_mtx_;
  std::unordered_map<int32_t, bool> m_nweb_privacy_map_;

  std::mutex m_nweb_info_map_mtx_;
  std::unordered_map<int32_t, NWebInfo> m_nweb_info_map_;

  std::atomic<int32_t> m_capacity_ = 30; // default capacity is 30
};
}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_BLANK_OPT_CONTROLLER_H_