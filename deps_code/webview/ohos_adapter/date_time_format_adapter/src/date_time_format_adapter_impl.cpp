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

#include "date_time_format_adapter_impl.h"

#include <cstddef>
#include <string>

#include "common_event_subscriber.h"
#include "matching_skills.h"
#include "nweb_log.h"
#include "time_service_client.h"

namespace OHOS::NWeb {
std::string WebTimezoneInfoImpl::GetTzId()
{
    return tzId_;
}

bool WebTimezoneInfoImpl::GetIsValid()
{
    return isValid_;
}

NWebTimeZoneEventSubscriber::NWebTimeZoneEventSubscriber(
    EventFwk::CommonEventSubscribeInfo& in, std::shared_ptr<TimezoneEventCallbackAdapter> cb)
    : EventFwk::CommonEventSubscriber(in), eventCallback_(cb)
{}

void NWebTimeZoneEventSubscriber::OnReceiveEvent(const EventFwk::CommonEventData& data)
{
    const std::string action = data.GetWant().GetAction();
    WVLOG_I("receive timezone action: %{public}s", action.c_str());
    if (action != EventFwk::CommonEventSupport::COMMON_EVENT_TIMEZONE_CHANGED) {
        return;
    }
    std::string ret = OHOS::MiscServices::TimeServiceClient::GetInstance()->GetTimeZone();
    std::shared_ptr<WebTimezoneInfoImpl> timezoneinfo = std::make_shared<WebTimezoneInfoImpl>(ret, true);
    if (eventCallback_ == nullptr) {
        return;
    }
    eventCallback_->TimezoneChanged(timezoneinfo);
}

void DateTimeFormatAdapterImpl::RegTimezoneEvent(std::shared_ptr<TimezoneEventCallbackAdapter> eventCallback)
{
    WVLOG_I("Reg Timezone Event.");
    cb_ = std::move(eventCallback);
}

bool DateTimeFormatAdapterImpl::StartListen()
{
    WVLOG_I("start time_zone listen.");
    EventFwk::MatchingSkills skill = EventFwk::MatchingSkills();
    skill.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_TIMEZONE_CHANGED);
    EventFwk::CommonEventSubscribeInfo info(skill);
    this->commonEventSubscriber_ = std::make_shared<NWebTimeZoneEventSubscriber>(info, this->cb_);
    bool ret = EventFwk::CommonEventManager::SubscribeCommonEvent(this->commonEventSubscriber_);
    if (ret == false) {
        WVLOG_E("start time_zone listen fail.");
    }
    return ret;
}

void DateTimeFormatAdapterImpl::StopListen()
{
    WVLOG_I("stop time_zone listen.");
    if (this->commonEventSubscriber_ != nullptr) {
        bool result = EventFwk::CommonEventManager::UnSubscribeCommonEvent(this->commonEventSubscriber_);
        if (result) {
            this->commonEventSubscriber_ = nullptr;
        } else {
            WVLOG_E("stop time_zone listen fail.");
        }
    }
}

std::string DateTimeFormatAdapterImpl::GetTimezone()
{
    std::string ret = OHOS::MiscServices::TimeServiceClient::GetInstance()->GetTimeZone();
    if (ret.empty()) {
        WVLOG_E("GetTimezone failed, return NULL.");
    }
    return ret;
}
} // namespace OHOS::NWeb
