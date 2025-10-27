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

#ifndef ARK_LOCATION_ADAPTER_H
#define ARK_LOCATION_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkLocationRequestConfig : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void SetScenario(int32_t scenario) = 0;

    /*--ark web()--*/
    virtual void SetFixNumber(int32_t number) = 0;

    /*--ark web()--*/
    virtual void SetMaxAccuracy(int32_t maxAccuary) = 0;

    /*--ark web()--*/
    virtual void SetDistanceInterval(int32_t disInterval) = 0;

    /*--ark web()--*/
    virtual void SetTimeInterval(int32_t timeInterval) = 0;

    /*--ark web()--*/
    virtual void SetPriority(int32_t priority) = 0;
};

/*--ark web(source=webview)--*/
class ArkLocationInfo : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual double GetLatitude() = 0;

    /*--ark web()--*/
    virtual double GetLongitude() = 0;

    /*--ark web()--*/
    virtual double GetAltitude() = 0;

    /*--ark web()--*/
    virtual float GetAccuracy() = 0;

    /*--ark web()--*/
    virtual float GetSpeed() = 0;

    /*--ark web()--*/
    virtual double GetDirection() = 0;

    /*--ark web()--*/
    virtual int64_t GetTimeStamp() = 0;

    /*--ark web()--*/
    virtual int64_t GetTimeSinceBoot() = 0;

    /*--ark web()--*/
    virtual ArkWebStringVector GetAdditions() = 0;
};

/*--ark web(source=webcore)--*/
class ArkLocationCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnLocationReport(const ArkWebRefPtr<ArkLocationInfo> location) = 0;

    /*--ark web()--*/
    virtual void OnLocatingStatusChange(const int status) = 0;

    /*--ark web()--*/
    virtual void OnErrorReport(const int errorCode) = 0;
};

/*--ark web(source=webview)--*/
class ArkLocationProxyAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t StartLocating(
        ArkWebRefPtr<ArkLocationRequestConfig> requestConfig, ArkWebRefPtr<ArkLocationCallbackAdapter> callback) = 0;

    /*--ark web()--*/
    virtual bool StopLocating(int32_t callbackId) = 0;

    /*--ark web()--*/
    virtual bool EnableAbility(bool isEnabled) = 0;

    /*--ark web()--*/
    virtual bool IsLocationEnabled() = 0;
};

/*--ark web(source=webview)--*/
class ArkLocationInstance : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    static ArkWebRefPtr<ArkLocationInstance> GetInstance();

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkLocationProxyAdapter> CreateLocationProxyAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkLocationRequestConfig> CreateLocationRequestConfig() = 0;
};

} // namespace OHOS::ArkWeb

#endif
