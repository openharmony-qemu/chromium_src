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
  void StopSliding();
  void StartFling();
  void OnScrollUpdate(float delta_x, float delta_y);

  void SetVsyncPeriod(int64_t vsync_period);
private:
  float ConvertToVelocity(float delta_x, float delta_y);
  int32_t GetPreferedFrameRate(float velocity,
    const std::vector<OHOS::NWeb::FrameRateSetting>& setting);

private:
float vsync_period_ {-1};
  bool isInited_ {false};
  bool isSliding_ {false};
  bool isOffScreen_ {false};
  int32_t dpi_ {-1};
  std::vector<OHOS::NWeb::FrameRateSetting> onScreenSetting {};
  std::vector<OHOS::NWeb::FrameRateSetting> offScreenSetting {};

  float virtual_pixel_ratio_ {-1};
};
}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_SLIDING_OBSERVER_H_