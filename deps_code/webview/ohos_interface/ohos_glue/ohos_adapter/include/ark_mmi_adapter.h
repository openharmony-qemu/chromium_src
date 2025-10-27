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

#ifndef ARK_MMI_ADAPTER_H
#define ARK_MMI_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkMMIDeviceInfoAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetId() = 0;

    /*--ark web()--*/
    virtual int32_t GetType() = 0;

    /*--ark web()--*/
    virtual int32_t GetBus() = 0;

    /*--ark web()--*/
    virtual int32_t GetVersion() = 0;

    /*--ark web()--*/
    virtual int32_t GetProduct() = 0;

    /*--ark web()--*/
    virtual int32_t GetVendor() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetName() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetPhys() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetUniq() = 0;

    /*--ark web()--*/
    virtual void SetId(int32_t id) = 0;

    /*--ark web()--*/
    virtual void SetType(int32_t type) = 0;

    /*--ark web()--*/
    virtual void SetBus(int32_t bus) = 0;

    /*--ark web()--*/
    virtual void SetVersion(int32_t version) = 0;

    /*--ark web()--*/
    virtual void SetProduct(int32_t product) = 0;

    /*--ark web()--*/
    virtual void SetVendor(int32_t vendor) = 0;

    /*--ark web()--*/
    virtual void SetName(ArkWebString name) = 0;

    /*--ark web()--*/
    virtual void SetPhys(ArkWebString phys) = 0;

    /*--ark web()--*/
    virtual void SetUniq(ArkWebString uniq) = 0;
};

/*--ark web(source=webcore)--*/
class ArkMMIListenerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnDeviceAdded(int32_t deviceId, const ArkWebString& type) = 0;

    /*--ark web()--*/
    virtual void OnDeviceRemoved(int32_t deviceId, const ArkWebString& type) = 0;
};

/*--ark web(source=webcore)--*/
class ArkMMIInputListenerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnInputEvent(int32_t keyCode, int32_t keyAction) = 0;
};

/*--ark web(source=webview)--*/
class ArkMMIAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual char* KeyCodeToString(int32_t keyCode) = 0;

    /*--ark web()--*/
    virtual int32_t RegisterMMIInputListener(ArkWebRefPtr<ArkMMIInputListenerAdapter> eventCallback) = 0;

    /*--ark web()--*/
    virtual void UnregisterMMIInputListener(int32_t monitorId) = 0;

    /*--ark web()--*/
    virtual int32_t RegisterDevListener(ArkWebString type, ArkWebRefPtr<ArkMMIListenerAdapter> listener) = 0;

    /*--ark web()--*/
    virtual int32_t UnregisterDevListener(ArkWebString type) = 0;

    /*--ark web()--*/
    virtual int32_t GetKeyboardType(int32_t deviceId, int32_t& type) = 0;

    /*--ark web()--*/
    virtual int32_t GetDeviceIds(ArkWebInt32Vector& ids) = 0;

    /*--ark web()--*/
    virtual int32_t GetDeviceInfo(int32_t deviceId, ArkWebRefPtr<ArkMMIDeviceInfoAdapter> info) = 0;

    /*--ark web()--*/
    virtual int32_t GetMaxTouchPoints() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_MMI_ADAPTER_H
