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
  using Callback = std::function<void()>;
  enum class StatusCode {
    ALLOWED,                // Operation is permitted
    NOT_ALLOWED,            // Operation is not permitted
    DUMPED,                 // Static frame has been dumped
    CALL_MULTIPLED_TIMES,   // Function called multiple times
    INSERTED,               // Static frame has been inserted
    KEY_NOT_MATCH           // Key does not match the expected value
  };

  static constexpr uint64_t INVALID_BLANKLESS_KEY = UINT64_MAX;
  static constexpr double CALLBACK_SIMILARITY_THRESHOLD = 0.75;

  static uint64_t ConvertToBlanklessKey(const std::string& value);

  ~BlanklessController() = default;

  BlanklessController(const BlanklessController&) = delete;
  BlanklessController& operator=(const BlanklessController&) = delete;
  BlanklessController(const BlanklessController&&) = delete;
  BlanklessController& operator=(const BlanklessController&&) = delete;

  static BlanklessController& GetInstance();

  static bool CheckGlobalProperty();

  void RegisterFrameRemoveCallback(uint64_t blankless_key, Callback&& callback);
  void FireFrameRemoveCallback(uint64_t blankless_key);

  void RegisterFrameInsertCallback(uint64_t blankless_key, Callback&& callback, int32_t lcp_time);
  int32_t FireFrameInsertCallback(uint64_t blankless_key);
  void CancelFrameInsertCallback(uint64_t blankless_key);

  /**
   * Resets the status of a specified web instance.
   * 
   * @param nweb_id         Unique identifier for the web instance
   * @param is_main_frame   Indicates whether it is the main frame
   * @param is_redirect     Indicates whether it is a redirect request
   * @return Status code indicating the result of the operation
   */
  StatusCode ResetStatus(int32_t nweb_id, bool is_main_frame, bool is_redirect);

  /**
   * Removes the status of a specified web instance.
   * 
   * @param nweb_id        Unique identifier for the web instance
   */
  void RemoveStatus(int32_t nweb_id);

  /**
   * Records a key value for a specified web instance.
   * 
   * @param nweb_id        Unique identifier for the web instance
   * @param blankless_key  Key value without whitespace
   * @return Status code indicating the result of the operation
   */
  StatusCode RecordKey(int32_t nweb_id, uint64_t blankless_key);

  /**
   * Matches a key value against the recorded value for a specified web instance.
   * 
   * @param nweb_id        Unique identifier for the web instance
   * @param blankless_key  Key value without whitespace to match
   * @return Status code indicating the result of the match operation
   */
  StatusCode MatchKey(int32_t nweb_id, uint64_t blankless_key);

  bool CheckEnableForUrl(const std::string& url);

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

    // Set of all historical dumped blankless key values (unique entries)
    std::unordered_set<uint64_t> blankless_key_dumped_history;

    // Set of all historical inserted blankless key values (unique entries)
    std::unordered_set<uint64_t> blankless_key_inserted_history;
  };

  BlanklessController() = default;

  void ResetForTest();
  bool CheckStatusForTest(int32_t nweb_id, const StatusInfo& expected_status, bool expected_found = true);

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

  std::mutex m_nweb_status_map_mtx_;
  std::unordered_map<int32_t, StatusInfo> m_nweb_status_map_;

  std::mutex m_frame_remove_callback_map_mtx_;
  std::unordered_map<uint64_t, Callback> m_frame_remove_callback_map_;

  std::mutex m_frame_insert_callback_map_mtx_;
  std::unordered_map<uint64_t, std::pair<Callback, int32_t>> m_frame_insert_callback_map_;

  std::atomic<int32_t> m_capacity_ = 30; // default capacity is 30
};
}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_BLANK_OPT_CONTROLLER_H_