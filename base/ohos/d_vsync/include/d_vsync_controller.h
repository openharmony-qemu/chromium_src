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

#ifndef BASE_OHOS_D_VSYNC_CONTROLLER_H_
#define BASE_OHOS_D_VSYNC_CONTROLLER_H_

#include "base/task/single_thread_task_runner.h"

namespace base {
namespace ohos {
class DVsyncController {
public:
    DVsyncController();
    ~DVsyncController();

    DVsyncController(const DVsyncController&) = delete;
    DVsyncController& operator=(const DVsyncController&) = delete;

    static DVsyncController& GetInstance();
    void Init();
    void SetIsFling(bool is_fling_enabled);
    bool GetIsFling();
private:
    void SetIsFlingImpl(bool is_fling_enabled);

private:
    scoped_refptr<SingleThreadTaskRunner> current_task_runner_ {};
    bool is_fling_ { false };
};
}   // namespace ohos
}   // namespace base

#endif   // BASE_OHOS_D_VSYNC_CONTROLLER_H_