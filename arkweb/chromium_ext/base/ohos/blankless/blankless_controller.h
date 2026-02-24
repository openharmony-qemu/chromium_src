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

#include <atomic>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "base/containers/lru_cache.h"
#include "build/buildflag.h"

namespace base {
namespace ohos {
struct BlanklessInfo {
  uint64_t blankless_key = UINT64_MAX;
  uint32_t nweb_id = 0;
  int32_t lcp_time = 0;
  uint64_t system_time = 0;
  int64_t pref_hash = 0;
  int64_t width = 0;
  int64_t height = 0;
};

struct BlanklessDumpInfo {
  BlanklessInfo info;
  bool dump_enabled = false;
};

class BlanklessController {
public:
  using Callback = std::function<void()>;
  enum class StatusCode {
    ALLOWED,                // Operation is permitted
    NOT_ALLOWED,            // Operation is not permitted
    DUMPED,                 // Static frame has been dumped
    INSERTED,               // Static frame has been inserted
    KEY_NOT_MATCH           // Key does not match the expected value
  };

  static constexpr uint64_t INVALID_BLANKLESS_KEY = UINT64_MAX;
  static constexpr double CALLBACK_SIMILARITY_THRESHOLD = 0.5;
  static constexpr double CALLBACK_SIMILARITY_THRESHOLD_MIDDLE = 0.3;
  static constexpr double CALLBACK_SIMILARITY_THRESHOLD_LOW = 0.1;
  static constexpr uint64_t INVALID_TIMESTAMP = UINT64_MAX;
  static constexpr int32_t A_STANDARD = 1600; // 1600 ms
  static constexpr int32_t MINIMUM_FRAME_LIFETIME = 40; // 40 ms
  static constexpr int32_t MAXIMUM_FRAME_LIFETIME = 2000; // 2000 ms
  static constexpr int32_t SNAPSHOT_SCALE_FACTOR = 2;

  static uint64_t ConvertToBlanklessKey(const std::string& value);

  ~BlanklessController() = default;

  BlanklessController(const BlanklessController&) = delete;
  BlanklessController& operator=(const BlanklessController&) = delete;
  BlanklessController(const BlanklessController&&) = delete;
  BlanklessController& operator=(const BlanklessController&&) = delete;

  static BlanklessController& GetInstance();

  static bool CheckGlobalProperty();

  void RegisterFrameRemoveCallback(uint32_t nweb_id, uint64_t blankless_key, Callback&& callback);
  void FireFrameRemoveCallback(uint32_t nweb_id, uint64_t blankless_key);

  void RegisterFrameInsertCallback(uint32_t nweb_id, uint64_t blankless_key, Callback&& callback, int32_t lcp_time);
  int32_t FireFrameInsertCallback(uint32_t nweb_id, uint64_t blankless_key);
  void CancelFrameInsertCallback(uint32_t nweb_id, uint64_t blankless_key);

  /**
   * Resets the status of a specified web instance.
   * 
   * @param nweb_id         Unique identifier for the web instance
   * @param is_main_frame   Indicates whether it is the main frame
   * @param is_redirect     Indicates whether it is a redirect request
   * @return Status code indicating the result of the operation
   */
  StatusCode ResetStatus(uint32_t nweb_id, bool is_main_frame, bool is_redirect);

  /**
   * Records a key value for a specified web instance.
   * 
   * @param nweb_id        Unique identifier for the web instance
   * @param blankless_key  Key value without whitespace
   * @return Status code indicating the result of the operation
   */
  StatusCode RecordKey(uint32_t nweb_id, uint64_t blankless_key);

  /**
   * Matches a key value against the recorded value for a specified web instance.
   * 
   * @param nweb_id        Unique identifier for the web instance
   * @param blankless_key  Key value without whitespace to match
   * @return Status code indicating the result of the match operation
   */
  StatusCode MatchKey(uint32_t nweb_id, uint64_t blankless_key);

  void RecordSystemTime(uint32_t nweb_id, uint64_t blankless_key, uint64_t system_time);
  uint64_t GetSystemTime(uint32_t nweb_id, uint64_t blankless_key);

  void RecordDumpTime(uint32_t nweb_id, uint64_t blankless_key, uint64_t dump_time);
  uint64_t GetDumpTime(uint32_t nweb_id, uint64_t blankless_key);

  void Clear(uint32_t nweb_id);

  bool CheckEnableForSysUrl(const std::string& url, uint64_t& blankless_key);

#if BUILDFLAG(ARKWEB_BLANK_PROP_CONFIG)
  bool CheckEnableForAppUrl(const std::string& url, uint64_t& blankless_key);
#endif // BUILDFLAG(ARKWEB_BLANK_PROP_CONFIG)

  void RecordWindowId(uint32_t nweb_id, uint32_t window_id);
  uint32_t GetWindowIdByNWebId(uint32_t nweb_id);

private:
  /**
   * Structure to track and manage the status information of a web instance.
   * Maintains the status code, key values, permission state, and a history of keys.
   */
  struct StatusInfo {
    // Current status code, initialized to allowed state
    StatusCode status_code = StatusCode::ALLOWED;

    // Current blankless key value, initialized to an invalid value
    uint64_t blankless_key = INVALID_BLANKLESS_KEY;

    // Flag indicating whether operations are allowed
    bool allowed = true;
  };

  BlanklessController() = default;

  void ResetForTest();
  bool CheckStatusForTest(int32_t nweb_id, const StatusInfo& expected_status, bool expected_found = true);

  /* This Class is designed for testing and will be delete someday. */
  class BlankOptWhiteList {
  public:
    BlankOptWhiteList() = default;
    ~BlankOptWhiteList() = default;

    bool CheckSysWhiteList(const std::string& url, uint64_t& blankless_key);

#if BUILDFLAG(ARKWEB_BLANK_PROP_CONFIG)
    bool CheckAppWhiteList(const std::string& url, uint64_t& blankless_key);
#endif // BUILDFLAG(ARKWEB_BLANK_PROP_CONFIG)

  private:
    void LoadSysWhiteList();
    bool ExactMatch(const std::string& url);
    bool FuzzyMatch(const std::string& url);

    std::unordered_set<std::string> m_exact_match_set_;
    std::unordered_set<std::string> m_fuzzy_match_set_;
    bool m_is_sys_loaded_ = false;

#if BUILDFLAG(ARKWEB_BLANK_PROP_CONFIG)
    void LoadAppWhiteList();
    void ParseAppWhiteList(std::vector<char>& buffer);
    bool QueryMatch(const std::string& url);
    std::string GetBaseUrl(const std::string& url);
    std::string GetQueryUrl(const std::string& url);

    std::unordered_map<std::string, std::unordered_set<std::string>> m_query_match_map_;
    bool m_is_app_loaded_ = false;
#endif // BUILDFLAG(ARKWEB_BLANK_PROP_CONFIG)
  };
  BlankOptWhiteList m_white_list_;

  std::mutex m_nweb_status_map_mtx_;
  std::unordered_map<int32_t, StatusInfo> m_nweb_status_map_;

  template<class InfoType>
  class NWebRelatedInfoMap {
  public:
    NWebRelatedInfoMap() = default;
    ~NWebRelatedInfoMap() = default;

    void Insert(uint32_t nweb_id, uint64_t blankless_key, const InfoType& info)
    {
      std::lock_guard<std::mutex> lck(m_mtx_);
      auto it = m_info_map_.find(nweb_id);
      if (it == m_info_map_.end()) {
        auto pair = m_info_map_.emplace(nweb_id, HashingLRUCache<uint64_t, InfoType>(LRU_CACHE_SIZE));
        it = pair.first;
      }
      it->second.Put(blankless_key, info);
    }

    std::optional<InfoType> Get(uint32_t nweb_id, uint64_t blankless_key, bool move)
    {
      std::optional<InfoType> info;
      std::lock_guard<std::mutex> lck(m_mtx_);
      auto it = m_info_map_.find(nweb_id);
      if (it == m_info_map_.end()) {
        return info;
      }
      auto& cache = it->second;
      auto found = cache.Peek(blankless_key);
      if (found == cache.end()) {
        return info;
      }
      info = found->second;
      if (move) {
        cache.Erase(found);
        if (cache.size() == 0) {
          m_info_map_.erase(it);
        }
      }
      return info;
    }

    void Erase(uint32_t nweb_id, uint64_t blankless_key)
    {
      std::lock_guard<std::mutex> lck(m_mtx_);
      auto it = m_info_map_.find(nweb_id);
      if (it == m_info_map_.end()) {
        return;
      }
      if (blankless_key == INVALID_BLANKLESS_KEY) {
        m_info_map_.erase(it);
        return;
      }
      auto& cache = it->second;
      auto found = cache.Peek(blankless_key);
      if (found == cache.end()) {
        return;
      }
      cache.Erase(found);
      if (cache.size() == 0) {
        m_info_map_.erase(it);
      }
    }

    void Clear()
    {
      std::lock_guard<std::mutex> lck(m_mtx_);
      m_info_map_.clear();
    }

  private:
    constexpr static int LRU_CACHE_SIZE = 3;
    std::mutex m_mtx_;
    std::unordered_map<uint32_t, HashingLRUCache<uint64_t, InfoType>> m_info_map_;
  };

  NWebRelatedInfoMap<Callback> m_frame_remove_callback_map_;

  NWebRelatedInfoMap<std::pair<Callback, int32_t>> m_frame_insert_callback_map_;

  NWebRelatedInfoMap<uint64_t> m_system_time_map_;
  // Record the time when the snapshot is written to the database.
  NWebRelatedInfoMap<uint64_t> m_dump_time_map_;

  std::unordered_map<uint32_t, uint32_t> m_window_id_map_;

  std::mutex m_window_id_map_mtx_;
};
}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_BLANK_OPT_CONTROLLER_H_