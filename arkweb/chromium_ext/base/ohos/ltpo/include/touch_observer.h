/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#ifndef BASE_OHOS_TOUCH_OBSERVER_H_
#define BASE_OHOS_TOUCH_OBSERVER_H_

#include <cstdint>

namespace base {
namespace ohos {
class TouchObserver {
 public:
  TouchObserver();
  ~TouchObserver();

  TouchObserver(const TouchObserver&) = delete;
  TouchObserver& operator=(const TouchObserver&) = delete;

  static TouchObserver& GetInstance();

  void SetTouchUpTime(uint64_t touch_up_time);
  uint64_t GetLastTouchUpTime();

 private:
  uint64_t last_touch_up_time_{0};
};
}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_TOUCH_OBSERVER_H_
