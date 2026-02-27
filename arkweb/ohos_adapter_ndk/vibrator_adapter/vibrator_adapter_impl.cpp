/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache (c) 2025 Huawei Device Co., Ltd.
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

#include "vibrator_adapter_impl.h"
#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include <algorithm>
#include <sensors/vibrator.h>

namespace OHOS::NWeb {
constexpr int32_t VIBRATOR_SUCCESS = 0;
constexpr int32_t VIBRATOR_ERR_INVALID_PARAM = -1;
constexpr int32_t MINIMUM_VIBRATION_DURATION_MS = 1;
constexpr int32_t MAXIMUM_VIBRATION_DURATION_MS = 10000;

int32_t VibratorAdapterImpl::Vibrate(int32_t duration) {
  if (duration < 0) {
    WVLOG_E("Vibrate failed, duration is invalid: %{public}d", duration);
    return VIBRATOR_ERR_INVALID_PARAM;
  }

  int32_t durationMs = std::max(MINIMUM_VIBRATION_DURATION_MS,
    std::min(duration, MAXIMUM_VIBRATION_DURATION_MS));

  Vibrator_Attribute attribute;
  attribute.usage = VIBRATOR_USAGE_RING;

  int32_t ret = OH_Vibrator_PlayVibration(durationMs, attribute);
  if (ret != VIBRATOR_SUCCESS) {
    WVLOG_E("Vibrate failed, OH_Vibrator_PlayVibration returned: %{public}d", ret);
    return ret;
  }

  WVLOG_I("Vibrate succeeded, duration: %{public}d ms", durationMs);
  return VIBRATOR_SUCCESS;
}

int32_t VibratorAdapterImpl::Cancel() {
  int32_t ret = OH_Vibrator_Cancel();
  if (ret != VIBRATOR_SUCCESS) {
    WVLOG_E("Cancel failed, OH_Vibrator_Cancel returned: %{public}d", ret);
    return ret;
  }

  WVLOG_I("Cancel succeeded");
  return VIBRATOR_SUCCESS;
}

}  // namespace OHOS::NWeb
