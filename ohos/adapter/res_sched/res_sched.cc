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

#include "ohos/adapter/res_sched/res_sched.h"

#include <bundle/native_interface_bundle.h>
#include <unistd.h>

#include <atomic>
#include <string>
#include <unordered_map>

#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/res_sched/manager/res_api_wrapper.h"

namespace ohos::adapter::res_sched {
namespace {
const std::string kTriggerWebSlideNormal =
    "ResSchedManager::TriggerWebSlideNormal[ohos]";
const std::string kForceWebSlideNormal =
    "ResSchedManager::ForceWebSlideNormal[ohos]";

/**
 * use atomic reference to store process global bundle_name
 */
std::string GetBundleName() {
  OH_NativeBundle_ElementName element_name =
      OH_NativeBundle_GetMainElementName();
  return std::string(element_name.bundleName);
}
}  // namespace

ResSchedManager& ResSchedManager::GetInstance() {
  static ResSchedManager instance;
  return instance;
}

ResSchedManager::ResSchedManager() : bundle_name_(GetBundleName()) {}

bool ResSchedManager::TriggerWebSlideNormal(TimeMicro64 current_time) {
  TRACE_EVENT_0(kTriggerWebSlideNormal);
  bool result = web_slide_move_throttle_.AttemptEnter(current_time);
  if (result) {
    ForceWebSlideNormal(WebSlideParam::kBegin);
  }
  return result;
}

void ResSchedManager::ForceWebSlideNormal(WebSlideParam param) {
  TRACE_EVENT_1(kForceWebSlideNormal, "param", static_cast<int32_t>(param));
  pid_t pid = getpid();
  std::unordered_map<std::string, std::string> event_param = {
      {"pid", std::to_string(pid)}, {"bundleName", bundle_name_}};
  manager::ResApiWrapper::GetInstance().invokeApi(
      kResTypeWebSlideNormal, static_cast<int64_t>(param), event_param);
}
}  // namespace ohos::adapter::res_sched
