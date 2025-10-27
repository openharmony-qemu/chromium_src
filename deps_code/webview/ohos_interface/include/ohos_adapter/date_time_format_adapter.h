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

#ifndef DATE_TIME_FORMAT_ADAPTER_H
#define DATE_TIME_FORMAT_ADAPTER_H

#include <memory>
#include <string>

namespace OHOS::NWeb {

class WebTimezoneInfo {
public:
    virtual ~WebTimezoneInfo() = default;

    virtual std::string GetTzId() = 0;
};

class TimezoneEventCallbackAdapter {
public:
    virtual ~TimezoneEventCallbackAdapter() = default;

    virtual void TimezoneChanged(std::shared_ptr<WebTimezoneInfo> info) = 0;
};

class DateTimeFormatAdapter {
public:
    DateTimeFormatAdapter() = default;

    virtual ~DateTimeFormatAdapter() = default;

    virtual void RegTimezoneEvent(std::shared_ptr<TimezoneEventCallbackAdapter> eventCallback) = 0;

    virtual bool StartListen() = 0;

    virtual void StopListen() = 0;

    virtual std::string GetTimezone() = 0;
};

} // namespace OHOS::NWeb

#endif // DATE_TIME_FORMAT_ADAPTER_H
