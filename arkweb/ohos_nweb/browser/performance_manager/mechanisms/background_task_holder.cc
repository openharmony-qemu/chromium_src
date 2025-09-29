/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "background_task_holder.h"
#include "background_task_adapter.h"
#include "ohos_adapter_helper.h"
#include "base/logging.h"
#include "base/check.h"

namespace performance_manager::mechanism {
using namespace OHOS::NWeb;

void BackgroundStateChangeCallback::RegisterBackgroundTaskPolicyCallback(
    const std::weak_ptr<ApplicationStateChangeCallback> &applicationStateChangeCallback)
{
    std::lock_guard<std::mutex> lock(applicationStateCallbackLock_);
    applicationStateCallback_.push_back(applicationStateChangeCallback);
    LOG(INFO) << "BackgroundStateChangeCallback::RegisterBackgroundTaskPolicyCallback is called.";
}

void BackgroundStateChangeCallback::NotifyApplicationForeground()
{
    std::lock_guard<std::mutex> lock(applicationStateCallbackLock_);
    for (auto callback : applicationStateCallback_) {
      auto callbackSptr = callback.lock();
      if (callbackSptr != nullptr) {
        callbackSptr->NotifyApplicationForeground();
      }
    }
    LOG(INFO) << "BackgroundStateChangeCallback::NotifyApplicationForeground is called.";
}

void BackgroundStateChangeCallback::NotifyApplicationBackground()
{
    std::lock_guard<std::mutex> lock(applicationStateCallbackLock_);
    for (auto callback : applicationStateCallback_) {
      auto callbackSptr = callback.lock();
      if (callbackSptr != nullptr) {
        callbackSptr->NotifyApplicationBackground();
      }
    }
    LOG(INFO) << "BackgroundStateChangeCallback::NotifyApplicationBackground is called.";
}

BackgroundTaskHolder::BackgroundTaskHolder()
{
    backgroundTaskAdapter_ = OhosAdapterHelper::GetInstance().CreateBackgroundTaskAdapter();
    if (!backgroundTaskAdapter_) {
      LOG(ERROR) << "BackgroundTaskHolder::backgroundTaskAdapter_ is nullptr.";
      return;
    }
    backgroundStateChangeCallbackAdapter_ = std::make_shared<BackgroundStateChangeCallback>();
    if (backgroundStateChangeCallbackAdapter_) {
      backgroundTaskAdapter_->RegisterBackgroundTaskPolicyCallback(backgroundStateChangeCallbackAdapter_);
      LOG(INFO) << "BackgroundTaskHolder::backgroundStateChangeCallbackAdapter_ is registered.";
    } else {
      LOG(ERROR) << "BackgroundTaskHolder::backgroundStateChangeCallbackAdapter_ is nullptr.";
    }
}

bool BackgroundTaskHolder::MaybeRequestBackgroundRunning(
    bool running,
    BackgroundModeAdapter bgMode) {
  return BackgroundTaskAdapter::RequestBackgroundRunning(running, bgMode);
}
}  // namespace performance_manager::mechanism
