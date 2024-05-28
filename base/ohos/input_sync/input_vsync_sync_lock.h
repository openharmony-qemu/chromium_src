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

#ifndef BASE_OHOS_INPUT_VSYC_SYNC_LOCK_H_
#define BASE_OHOS_INPUT_VSYC_SYNC_LOCK_H_

#include "base/base_export.h"
#include <atomic>

namespace base {
namespace ohos {
class BASE_EXPORT InputSyncLock {
public:
    InputSyncLock();
    ~InputSyncLock();

    InputSyncLock(const InputSyncLock&) = delete;
    InputSyncLock& operator=(const InputSyncLock&) = delete;

    static InputSyncLock& GetInstance();
    void SetNeedWaitForInput(bool need_wait);
    bool NeedWaitForInput();
    void SetHandledTouchEvent(bool handled_touch_event);
    bool HandledTouchEvent();

private:
    std::atomic<bool> need_wait_for_input_ = false;
    std::atomic<bool> handled_touch_event_ = false;
};
}  // namespace ohos
}  // namespace  base


#endif // BASE_OHOS_INPUT_VSYC_SYNC_LOCK_H_