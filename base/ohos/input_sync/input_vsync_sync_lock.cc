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

#include "base/ohos/input_sync/input_vsync_sync_lock.h"

#include <thread>
#include <chrono>
#include "base/no_destructor.h"
#include "base/logging.h"

namespace base  {
namespace ohos {

InputSyncLock::InputSyncLock() {}
InputSyncLock::~InputSyncLock() {}

InputSyncLock& InputSyncLock::GetInstance()
{
    static base::NoDestructor<InputSyncLock> instance;
    return *instance.get();
}

void InputSyncLock::SetNeedWaitForInput(bool need_wait_for_input)
{
    if (need_wait_for_input_.load() == need_wait_for_input) {
        return;
    }
    need_wait_for_input_.store(need_wait_for_input);
}

bool InputSyncLock::NeedWaitForInput()
{
    return need_wait_for_input_.load();
}

void InputSyncLock::SetHandledTouchEvent(bool handled_touch_event)
{
    handled_touch_event_.store(handled_touch_event);
}

bool InputSyncLock::HandledTouchEvent()
{
    return handled_touch_event_.load();
}
}  // namespace ohos
}  // namespace base