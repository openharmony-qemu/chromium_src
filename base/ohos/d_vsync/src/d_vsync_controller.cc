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

#include "base/ohos/d_vsync/include/d_vsync_controller.h"

#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/task/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"
#include "ohos_adapter_helper.h"

namespace base {
namespace ohos {
DVsyncController::DVsyncController()
{}

DVsyncController::~DVsyncController()
{}

DVsyncController& DVsyncController::GetInstance()
{
    static base::NoDestructor<DVsyncController> instance;
    return *instance.get();
}

void DVsyncController::Init()
{
    if (!current_task_runner_) {
        LOG(INFO) << "start init current_task_runner_";
        current_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
    }
}

void DVsyncController::SetIsFling(bool is_fling_enabled)
{
    if (!current_task_runner_) {
        LOG(WARNING) << "current_task_runner_ is nullptr, try to start init";
        Init();
    }
    LOG(DEBUG) << "DVsyncController::SetIsFling: " << is_fling_enabled;
    current_task_runner_->PostTask(FROM_HERE, base::BindOnce(
        &DVsyncController::SetIsFlingImpl,
        base::Unretained(this), is_fling_enabled));
}

void DVsyncController::SetIsFlingImpl(bool is_fling_enabled)
{
    LOG(DEBUG) << "DVsyncController::SetIsFlingImpl: " << is_fling_enabled;
    is_fling_ = is_fling_enabled;
}

bool DVsyncController::GetIsFling()
{
    if (!current_task_runner_) {
        LOG(WARNING) << "current_task_runner_ is nullptr, try to start init";
        Init();
    }
    LOG(DEBUG) << "DVsyncController::GetIsFling: " << is_fling_;
    return is_fling_;
}
}   // namespace ohos
}   // namespace base