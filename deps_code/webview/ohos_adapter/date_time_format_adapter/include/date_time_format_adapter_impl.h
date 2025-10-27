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

#ifndef DATE_TIME_FORMAT_ADAPTER_IMPL_H
#define DATE_TIME_FORMAT_ADAPTER_IMPL_H

#include <string>
#include <vector>
#include <map>

#include "date_time_format_adapter.h"

#include "common_event_manager.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "matching_skills.h"
#include "want.h"

namespace OHOS::NWeb {
class WebTimezoneInfoImpl final : public WebTimezoneInfo {
public:
    WebTimezoneInfoImpl(std::string tzId, bool isValid)
        : tzId_(tzId), isValid_(isValid) {}

    ~WebTimezoneInfoImpl() override = default;

    std::string GetTzId() override;

    bool GetIsValid();

private:
    std::string tzId_;

    bool isValid_;
};

class NWebTimeZoneEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    NWebTimeZoneEventSubscriber(EventFwk::CommonEventSubscribeInfo& in,
                                std::shared_ptr<TimezoneEventCallbackAdapter> cb);

    ~NWebTimeZoneEventSubscriber() override = default;

    void OnReceiveEvent(const EventFwk::CommonEventData& data) override;

private:
    std::shared_ptr<TimezoneEventCallbackAdapter> eventCallback_;
};

class DateTimeFormatAdapterImpl : public DateTimeFormatAdapter {
public:
    DateTimeFormatAdapterImpl() = default;

    ~DateTimeFormatAdapterImpl() override = default;

    void RegTimezoneEvent(const std::shared_ptr<TimezoneEventCallbackAdapter> eventCallback) override;

    bool StartListen() override;

    void StopListen() override;

    std::string GetTimezone() override;

private:
    std::shared_ptr<TimezoneEventCallbackAdapter> cb_ = nullptr;
    std::shared_ptr<EventFwk::CommonEventSubscriber> commonEventSubscriber_ = nullptr;
};
} // namespace OHOS::NWeb

#endif // DATE_TIME_FORMAT_ADAPTER_IMPL_H
