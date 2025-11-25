/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
 
#ifndef HIAPPEVENT_ADAPTER_H
#define HIAPPEVENT_ADAPTER_H

#include <string>

namespace OHOS::NWeb {

class HiAppeventAdapter {
public:
    struct DynamicFrameDropInfo {
        int64_t start_time;
        int64_t duration;
        int64_t max_app_frame_time;
        int32_t web_id;
    };

    virtual void ReportDynamicStastic(const std::string& domain,
        const std::string& eventName, int32_t eventType, const DynamicFrameDropInfo& dynamicFrameDropInfo) = 0;

protected:
    HiAppeventAdapter() = default;

    virtual ~HiAppeventAdapter() = default;
};
} // namespace OHOS::NWeb

#endif // HIAPPEVENT_ADAPTER_H