/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OHOS_ADAPTER_RES_SCHED_MANAGER_OVERLAY_MANAGER_H_
#define OHOS_ADAPTER_RES_SCHED_MANAGER_OVERLAY_MANAGER_H_

#include <atomic>
#include <cstdint>
#include <type_traits>

#include "ohos/adapter/export.h"

namespace ohos::adapter::res_sched {
/**
 * time are in micro seconds and in int64_t format,
 * can get by `base::Time::Now().ToDeltaSinceWindowsEpoch().InMicroseconds()`
 */
using TimeMicro64 = int64_t;

constexpr const int64_t kMicroToMills = 1000;

/**
 * get TimeMicro64 from millseconds for convension
 */
consteval TimeMicro64 MillSeconds(int64_t millSeconds) {
  return millSeconds * kMicroToMills;
}

namespace manager {

/**
 * ThrottleManager used to throttle an operation with time span,
 * `AttemptEnter` only return `true` if current time is over prev call time plus
 * `throttleDelta`, use `AttemptEnter` in if guard to limit an operation from
 * repeatly trigger
 */
class ADAPTER_EXPORT_API ThrottleManager {
 public:
  explicit ThrottleManager(TimeMicro64 throttle_delta);
  bool AttemptEnter(TimeMicro64 current_time);

  const TimeMicro64 throttle_delta;

 private:
  std::atomic<TimeMicro64> next_time_{0};
};

/**
 * used to implement debounce logic, request side need call
 * `AttemptRegisterOrExtend` with current time, if return `true` can register
 * delay callback with memoized time `register_time`, the time that callback
 * should invoked, and delay callback need to call `CheckCallbackState` with
 * `register_time`, and check return value to perform action that:
 * - `CallbackState::kIgnore`: just ignore because the debounce time is extend.
 * - `CallbackState::kNeedNewRegister`: register a new delayed callback to
 * `schedule_time`.
 * - `CallbackState::kNeedExecute`: execute actual delayed task
 */
class ADAPTER_EXPORT_API DebounceManager {
 public:
  enum class CallbackState : int {
    kIgnore = 0,
    kNeedNewRegister = 1,
    kNeedExecute = 2
  };
  /**
   * construct with debounce time in micro seconds
   * @param debounce_delta debounce time in micro seconds
   */
  explicit DebounceManager(TimeMicro64 debounce_delta);
  /**
   * call by the request side for arbitrary time, if return true, need register
   * callback, the end_time may extended.
   * @param current_time the current time
   */
  [[nodiscard]] bool AttemptRegisterOrExtend(TimeMicro64 current_time);
  /**
   * call by the callback side to check if end_time is extended
   * @param the time of the register callback invoked, can be memorized when
   * register the callback or get when callback invoked
   * @return need to perform
   * action by return value:
   * - `CallbackState::kIgnore`: just ignore because the debounce time is
   * extend.
   * - `CallbackState::kNeedNewRegister`: register a new delayed callback to
   * `schedule_time`.
   * - `CallbackState::kNeedExecute`: execute actual delayed task
   */
  [[nodiscard]] CallbackState CheckCallbackState(TimeMicro64 register_time);
  /** debouce delta time since current time */
  TimeMicro64 debounce_delta() const { return debounce_delta_; }
  /** end time the actual task need to invoked */
  TimeMicro64 end_time() const { return end_time_.load(); }
  /** the callback scheduled time to be invoked */
  TimeMicro64 schedule_time() const { return schedule_time_.load(); }

 private:
  TimeMicro64 debounce_delta_{0};
  std::atomic<TimeMicro64> end_time_{0};
  std::atomic<TimeMicro64> schedule_time_{0};
};
}  // namespace manager
}  // namespace ohos::adapter::res_sched

#endif  // OHOS_ADAPTER_RES_SCHED_MANAGER_OVERLAY_MANAGER_H_
