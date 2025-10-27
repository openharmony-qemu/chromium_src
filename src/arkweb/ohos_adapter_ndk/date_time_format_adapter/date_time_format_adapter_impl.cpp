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
#include <BasicServicesKit/oh_commonevent_support.h>
#include <BasicServicesKit/time_service.h>

#include "arkweb/ohos_nweb/src/nweb_hilog.h"

namespace OHOS::NWeb {

constexpr int MAX_TIMEZONE_LEN = 32;

//LCOV_EXCL_START
std::string WebTimezoneInfoImpl::GetTzId()
{
    return tzId_;
}

bool WebTimezoneInfoImpl::GetIsValid()
{
    return isValid_;
}

std::set<std::shared_ptr<TimezoneEventCallbackAdapter>> DateTimeFormatAdapterImpl::cbSet_;
CommonEvent_SubscribeInfo *DateTimeFormatAdapterImpl::commonEventSubscriberInfo_ = nullptr;
CommonEvent_Subscriber *DateTimeFormatAdapterImpl::commonEventSubscriber_ = nullptr;
std::mutex DateTimeFormatAdapterImpl::mutex_;

DateTimeFormatAdapterImpl::~DateTimeFormatAdapterImpl()
{
    StopListen();
}
//LCOV_EXCL_STOP

void DateTimeFormatAdapterImpl::RegTimezoneEvent(std::shared_ptr<TimezoneEventCallbackAdapter> eventCallback)
{
    if (!eventCallback) {
        WVLOG_E("WebTimeZoneEventCallback is nullptr.");
        return;
    }
    WVLOG_I("Reg Timezone Event.");
    cb_ = std::move(eventCallback);
}

void DateTimeFormatReceiveCallback(const CommonEvent_RcvData *data)
{
    if (data == nullptr ||
        strcmp(OH_CommonEvent_GetEventFromRcvData(data), COMMON_EVENT_TIMEZONE_CHANGED)) {
        WVLOG_E("not timezone event");
        return;
    }

    char timeZone[MAX_TIMEZONE_LEN];
    TimeService_ErrCode ret = OH_TimeService_GetTimeZone(timeZone, sizeof(timeZone));
    if (ret != TIMESERVICE_ERR_OK) {
        WVLOG_I("get timezone error: %{public}d", ret);
        return;
    }

    std::shared_ptr<WebTimezoneInfoImpl> timezoneinfo =
        std::make_shared<WebTimezoneInfoImpl>(std::string(timeZone), true);

    std::lock_guard<std::mutex> lock(DateTimeFormatAdapterImpl::mutex_);
    for (auto callback : DateTimeFormatAdapterImpl::cbSet_) {
        callback->TimezoneChanged(timezoneinfo);
    }
}

bool DateTimeFormatAdapterImpl::StartListen()
{
    WVLOG_I("start time_zone listen.");
    std::lock_guard<std::mutex> lock(mutex_);
    if (commonEventSubscriber_ == nullptr) {
        const char* events[] = {
            COMMON_EVENT_TIMEZONE_CHANGED,
        };

        int count = sizeof(events) / sizeof(events[0]);
        commonEventSubscriberInfo_ = OH_CommonEvent_CreateSubscribeInfo(events, count);
        if (commonEventSubscriberInfo_ == NULL) {
            WVLOG_E("Create SubscribeInfo fail.");
            return false;
        }

        commonEventSubscriber_ = OH_CommonEvent_CreateSubscriber(commonEventSubscriberInfo_,
            DateTimeFormatReceiveCallback);
        if (commonEventSubscriber_ == NULL) {
            OH_CommonEvent_DestroySubscribeInfo(commonEventSubscriberInfo_);
            commonEventSubscriberInfo_ = nullptr;
            WVLOG_E("Create Subscriber fail.");
            return false;
        }

        CommonEvent_ErrCode ret = OH_CommonEvent_Subscribe(commonEventSubscriber_);
        if (ret != COMMONEVENT_ERR_OK) {
            OH_CommonEvent_DestroySubscribeInfo(commonEventSubscriberInfo_);
            commonEventSubscriberInfo_ = nullptr;
            OH_CommonEvent_DestroySubscriber(commonEventSubscriber_);
            commonEventSubscriber_ = nullptr;
            WVLOG_E("Subscribe fail. ret: %{public}d", ret);
            return false;
        }
    }

    cbSet_.insert(cb_);
    return true;
}

void DateTimeFormatAdapterImpl::StopListen()
{
    WVLOG_I("stop time_zone listen.");
    std::lock_guard<std::mutex> lock(mutex_);
    if (cbSet_.find(cb_) == cbSet_.end()) {
        WVLOG_E("stop listen fail. Please RegTimezoneEvent first.");
        return;
    }

    cbSet_.erase(cb_);
    if (!cbSet_.empty()) {
        return;
    }

    if (commonEventSubscriber_ != nullptr) {
        CommonEvent_ErrCode ret = OH_CommonEvent_UnSubscribe(commonEventSubscriber_);
        if (ret != COMMONEVENT_ERR_OK) {
            WVLOG_E("UnSubscribe fail. ret: %{public}d", ret);
        }

        OH_CommonEvent_DestroySubscriber(commonEventSubscriber_);
        commonEventSubscriber_ = nullptr;
    }

    if (commonEventSubscriberInfo_ != nullptr) {
        OH_CommonEvent_DestroySubscribeInfo(commonEventSubscriberInfo_);
        commonEventSubscriberInfo_ = nullptr;
    }
}

std::string DateTimeFormatAdapterImpl::GetTimezone()
{
    char timeZone[MAX_TIMEZONE_LEN] = {0};
    TimeService_ErrCode ret = OH_TimeService_GetTimeZone(timeZone, sizeof(timeZone));
    if (ret != TIMESERVICE_ERR_OK) {
        WVLOG_I("get timezone error: %{public}d", ret);
    }

    return std::string(timeZone);
}
} // namespace OHOS::NWeb
