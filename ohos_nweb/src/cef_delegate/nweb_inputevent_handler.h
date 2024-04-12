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

#ifndef NWEB_INPUTEVENT_HANDLER_H
#define NWEB_INPUTEVENT_HANDLER_H

#include <vector>

namespace OHOS::NWeb {
template <typename EeventType, typename ConditionType>
class NWebInputEventHandle {
 public:
  virtual ~NWebInputEventHandle() = default;
  void RegistInputEvent(EeventType event,
                        ConditionType condition,
                        bool defaultResult) {
    event_.push_back(event);
    condition_.push_back(condition);
    result_.push_back(defaultResult);
  }

  void SetInputEvent(EeventType event, ConditionType condition) {
    for (size_t i = 0; i < event_.size(); i++) {
      if (event == event_[i]) {
        result_[i] = (condition == condition_[i]);
      }
    }
  }

  bool GetInputEvent(EeventType event) {
    for (size_t i = 0; i < event_.size(); i++) {
      if (event == event_[i]) {
        return result_[i];
      }
    }
    return false;
  }

  void ClearInputEvent() {
    for (size_t i = 0; i < event_.size(); i++) {
        result_[i] = false;
    }
  }

 private:
  std::vector<EeventType> event_;
  std::vector<ConditionType> condition_;
  // result will be true when condition.
  std::vector<bool> result_;
};
}  // namespace OHOS::NWeb

#endif
