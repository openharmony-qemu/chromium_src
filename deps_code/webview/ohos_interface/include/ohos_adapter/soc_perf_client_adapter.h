/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef SOC_PERF_CLIENT_ADAPTER_H
#define SOC_PERF_CLIENT_ADAPTER_H

#include <cstdint>

namespace OHOS::NWeb {

class SocPerfClientAdapter {
public:
    enum : int32_t {
        SOC_PERF_WEB_GESTURE_ID = 10012,
        SOC_PERF_WEB_GESTURE_MOVE_ID = 10020,
        SOC_PERF_SLIDE_NORMAL_ID = 10025,
        SOC_PERF_LOAD_URL_ID = 10070,
        SOC_PERF_MOUSEWHEEL_ID = 10071,
    };

    enum : int64_t {
        SOC_PERF_START = 0,
        SOC_PERF_END = 1,
    };

    SocPerfClientAdapter() = default;

    virtual ~SocPerfClientAdapter() = default;

    virtual void ApplySocPerfConfigById(int32_t id) = 0;

    virtual void ApplySocPerfConfigByIdEx(int32_t id, bool onOffTag) = 0;
};

} // namespace OHOS::NWeb

#endif // SOC_PERF_CLIENT_ADAPTER_H
