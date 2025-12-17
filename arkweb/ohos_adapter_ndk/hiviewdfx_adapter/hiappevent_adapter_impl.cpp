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
 
#include <string>
#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "hiappevent_adapter_impl.h"
#include "ohos_sdk/openharmony/native/sysroot/usr/include/hiappevent/hiappevent.h"



namespace OHOS::NWeb {

const char* START_TIME = "start_time";
const char* DURATION = "duration";
const char* MAX_APP_FRAME_TIME = "max_app_frame_time";
const char* WEB_ID = "web_id";

HiAppeventAdapterImpl& HiAppeventAdapterImpl::GetInstance() {
    static HiAppeventAdapterImpl instance;
    return instance;
}

EventType GetHiappeventType(int32_t eventType) {
    EventType hiappevent_type;
    switch (eventType) {
        case 1:
            hiappevent_type = EventType::FAULT;
            break;
        case 2:
            hiappevent_type = EventType::STATISTIC;
            break;
        case 3:
            hiappevent_type = EventType::SECURITY;
            break;
        default:
            hiappevent_type = EventType::BEHAVIOR;
    }
    return hiappevent_type;
}

ParamList ConvertFrameDropInfoToHiappeventParams(HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo) {
    ParamList params = OH_HiAppEvent_CreateParamList();
    OH_HiAppEvent_AddInt64Param(params, START_TIME, dynamicFrameDropInfo.start_time);
    OH_HiAppEvent_AddInt64Param(params, DURATION, dynamicFrameDropInfo.duration);
    OH_HiAppEvent_AddInt64Param(params, MAX_APP_FRAME_TIME, dynamicFrameDropInfo.max_app_frame_time);
    OH_HiAppEvent_AddInt32Param(params, WEB_ID, dynamicFrameDropInfo.web_id);
    return params;
}

void HiAppeventAdapterImpl::ReportDynamicStastic(const std::string& domain, const std::string& eventName,
        int32_t eventType, const HiAppeventAdapter::DynamicFrameDropInfo& dynamicFrameDropInfo) {
    EventType hiappevent_type = GetHiappeventType(eventType);
    ParamList hiappevent_params = ConvertFrameDropInfoToHiappeventParams(dynamicFrameDropInfo);
    int res = OH_HiAppEvent_Write(domain.c_str(), eventName.c_str(), hiappevent_type, hiappevent_params);
    if (res == 0) {
        WVLOG_D("HiAppevent_Write %{public}s Successfully", eventName.c_str());
    } else {
        WVLOG_E("HiAppevent_Write %{public}s failed", eventName.c_str());
    }
    OH_HiAppEvent_DestroyParamList(hiappevent_params);
}
} // namespace OHOS::NWeb