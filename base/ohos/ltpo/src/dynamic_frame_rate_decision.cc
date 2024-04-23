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

#include "base/ohos/ltpo/include/dynamic_frame_rate_decision.h"

#include <chrono>
#include <cmath>

#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/task/thread_pool.h"
#include "base/trace_event/trace_event.h"
#include "ohos_adapter_helper.h"

namespace base {
namespace ohos {
using OHOS::NWeb::OhosAdapterHelper;
static const int32_t kDefaultPreferedFrameRate = 120;

DynamicFrameRateDecision::DynamicFrameRateDecision()
{}

DynamicFrameRateDecision::~DynamicFrameRateDecision()
{}

DynamicFrameRateDecision& DynamicFrameRateDecision::GetInstance()
{
    static base::NoDestructor<DynamicFrameRateDecision> instance;
    return *instance.get();
}

void DynamicFrameRateDecision::ReportSlidingFrameRate(int32_t frame_rate)
{
  // check, enable ltpo only for phone
  if (frame_rate == slidingFrameRate_) {
    return;
  }
  slidingFrameRate_ = frame_rate;
  UpdateFramePreferredRate();
}

void DynamicFrameRateDecision::ReportDirtyRectFrameRate(int32_t frame_rate)
{
}

void DynamicFrameRateDecision::ReportVideoFrameRate(int32_t frame_rate)
{
}

void DynamicFrameRateDecision::UpdateFramePreferredRate()
{
  // invoke frame rate linker
  curFrameRate_ = std::max(slidingFrameRate_, videoFrameRate_);
  if (curFrameRate_ <= 0) {
    curFrameRate_ = kDefaultPreferedFrameRate;
  }
  LOG(INFO) << "final prefered frame rate " << curFrameRate_;
  OhosAdapterHelper::GetInstance().GetVSyncAdapter().SetFramePreferredRate(
    curFrameRate_ > 0 ? curFrameRate_ : kDefaultPreferedFrameRate);
}
}  // namespace ohos
}  // namespace base
