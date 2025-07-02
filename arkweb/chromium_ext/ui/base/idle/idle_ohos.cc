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

#include <multimodalinput/oh_input_manager.h>

#include "base/logging.h"
#include "base/time/time.h"
#include "ui/base/idle/idle.h"
#include "ui/base/idle/idle_internal.h"

#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace ui {

int CalculateIdleTime() {
  int64_t intervals = 0;
  Input_Result ret = OH_Input_GetIntervalSinceLastInput(&intervals);
  if (ret != INPUT_SUCCESS) {
    LOG(ERROR) << __FUNCTION__ << " OH_Input_GetIntervalSinceLastInput ret: "
               << static_cast<int64_t>(ret);
    return 0;
  }
  return static_cast<int>(intervals / base::Time::kMicrosecondsPerSecond);
}

bool CheckIdleStateIsLocked() {
  if (IdleStateForTesting().has_value()) {
    return IdleStateForTesting().value() == IDLE_STATE_LOCKED;
  }

  auto adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                     .CreateScreenlockManagerAdapter();
  if (!adapter) {
    LOG(ERROR) << __FUNCTION__ << " screenlock manager adapter is nullptr";
    return false;
  }
  return adapter->IsLocked();
}
}  // namespace ui
