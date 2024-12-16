/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef BASE_OHOS_SLIDING_OBSERVER_H_
#define BASE_OHOS_SLIDING_OBSERVER_H_

#include <stdint.h>
#include <vector>
#include "adapter_base.h"
#include "system_properties_adapter.h"

namespace base {
namespace ohos {
// dynamic frame rate
class SlidingObserver {
public:
  SlidingObserver() = default;
  ~SlidingObserver() = default;

  SlidingObserver(const SlidingObserver&) = delete;
  SlidingObserver& operator=(const SlidingObserver&) = delete;

  static SlidingObserver& GetInstance();

    void Init();
    void StartSliding();
    int32_t StopSliding();
    void StartFling();
    int32_t StopFling();
    int32_t OnScrollUpdate(float delta_x, float delta_y);
    int32_t OnFlingUpdate(float velocity_x, float velocity_y);
    void OnDisplayInfoChange();

private:
  float GetVelocity(float velocity_x, float velocity_y);
  int32_t GetPreferedFrameRate(float velocity,
    const std::vector<OHOS::NWeb::FrameRateSetting>& setting);
  int64_t GetCurrentTimestamp();

private:
  bool is_inited_ {false};
  bool is_sliding_ {false};
  bool is_off_screen_ {false};
  int32_t dpi_ {-1};
  std::vector<OHOS::NWeb::FrameRateSetting> on_screen_setting_ {};
  std::vector<OHOS::NWeb::FrameRateSetting> off_screen_setting_ {};

  float virtual_pixel_ratio_ {-1};
  int64_t current_timestamp_ {-1};
  int32_t sliding_frame_rate_ {0};
};
}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_SLIDING_OBSERVER_H_
