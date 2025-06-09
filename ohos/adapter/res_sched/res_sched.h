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

#ifndef OHOS_ADAPTER_RES_SCHED_RES_CHED_H_
#define OHOS_ADAPTER_RES_SCHED_RES_CHED_H_

#include <string>

#include "ohos/adapter/res_sched/manager/throttle_manager.h"

namespace ohos::adapter::res_sched {
/**
 * ThrottleManager used to throttle an operation with time span,
 * `AttemptEnter` only return `true` if current time is over prev call time plus
 * `throttleDelta`, use `AttemptEnter` in if guard to limit an operation from
 * repeatly trigger
 */
using ThrottleManager = manager::ThrottleManager;

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
using DebounceManager = manager::DebounceManager;

class ADAPTER_EXPORT_API ResSchedManager {
 public:
  /**
   * number of web slide normal reportData enum number
   */
  static constexpr const uint32_t kResTypeWebSlideNormal = 29u;
  static constexpr const TimeMicro64 kWebSlideThrottleTime = MillSeconds(100);
  /** WebSlideNormal param */
  enum WebSlideParam : int64_t {
    /** start boost */
    kBegin = 0,
    /** end boost */
    kEnd = 1
  };

  static ResSchedManager& GetInstance();
  ResSchedManager();

  /**
   * call web slide normal mode with debounce enabled
   * @param current_time the current time to enable throttle
   */
  bool TriggerWebSlideNormal(TimeMicro64 current_time);

  /**
   * force to trigger the web slide normal regardless of throttle
   */
  void ForceWebSlideNormal(WebSlideParam param);

 private:
  ThrottleManager web_slide_move_throttle_{kWebSlideThrottleTime};
  std::string bundle_name_;
};
}  // namespace ohos::adapter::res_sched

#endif  // OHOS_ADAPTER_RES_SCHED_RES_CHED_H_
