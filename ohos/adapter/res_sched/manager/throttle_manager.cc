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

#include "ohos/adapter/res_sched/manager/throttle_manager.h"

namespace ohos::adapter::res_sched::manager {
namespace {
constexpr int kMaxSpinLimit = 20;

/**
 * update an atomic variable to a larget value
 * @param current the atomic variable
 * @param next a larget value
 * @return `true` if `next` is larger than stored value and update succeed
 */
bool TryChangeToLarger(std::atomic<TimeMicro64>& current, TimeMicro64 next) {
  for (int i = 0; i < kMaxSpinLimit; i++) {
    TimeMicro64 prev_value = current.load();
    if (prev_value > next) {
      return false;
    }
    if (current.compare_exchange_weak(prev_value, next)) {
      return true;
    }
  }
  return false;
}
}  // namespace
ThrottleManager::ThrottleManager(TimeMicro64 throttle_delta)
    : throttle_delta(throttle_delta) {}

bool ThrottleManager::AttemptEnter(TimeMicro64 current_time) {
  // if current is not larger, return
  TimeMicro64 prev = next_time_.load();
  if (current_time < prev) {
    return false;
  }
  TimeMicro64 new_next_time = current_time + throttle_delta;
  return TryChangeToLarger(next_time_, new_next_time);
}

DebounceManager::DebounceManager(TimeMicro64 debounce_delta)
    : debounce_delta_(debounce_delta) {}

[[nodiscard]] bool DebounceManager::AttemptRegisterOrExtend(
    TimeMicro64 current_time) {
  TimeMicro64 next_time = current_time + debounce_delta_;
  TryChangeToLarger(end_time_, next_time);
  return TryChangeToLarger(schedule_time_, next_time);
}

[[nodiscard]] DebounceManager::CallbackState
DebounceManager::CheckCallbackState(TimeMicro64 register_time) {
  if (schedule_time_.load() > register_time) {
    return DebounceManager::CallbackState::kIgnore;
  }
  TimeMicro64 end_value = end_time_.load();
  if (end_value > register_time) {
    if (TryChangeToLarger(schedule_time_, end_value)) {
      return DebounceManager::CallbackState::kNeedNewRegister;
    } else {
      return DebounceManager::CallbackState::kIgnore;
    }
  }
  return DebounceManager::CallbackState::kNeedExecute;
}

}  // namespace ohos::adapter::res_sched::manager
