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

#ifndef MMI_ADAPTER_H
#define MMI_ADAPTER_H

#include <memory>
#include <string>

namespace OHOS::NWeb {

enum MMIAdapterKeyboardType : int32_t {
    NONE = 0,
    UNKNOWN_TYPE = 1,
    ALPHABETIC_KEYBOARD = 2,
    DIGITAL_KEYBOARD = 3,
    HANDWRITING_PEN = 4,
    REMOTE_CONTROL = 5,
};

class MMIDeviceInfoAdapter {
public:
    MMIDeviceInfoAdapter() = default;

    virtual ~MMIDeviceInfoAdapter() = default;

    virtual int32_t GetId() = 0;

    virtual int32_t GetType() = 0;

    virtual int32_t GetBus() = 0;

    virtual int32_t GetVersion() = 0;

    virtual int32_t GetProduct() = 0;

    virtual int32_t GetVendor() = 0;

    virtual std::string GetName() = 0;

    virtual std::string GetPhys() = 0;

    virtual std::string GetUniq() = 0;

    virtual void SetId(int32_t id) = 0;

    virtual void SetType(int32_t type) = 0;

    virtual void SetBus(int32_t bus) = 0;

    virtual void SetVersion(int32_t version) = 0;

    virtual void SetProduct(int32_t product) = 0;

    virtual void SetVendor(int32_t vendor) = 0;

    virtual void SetName(std::string name) = 0;

    virtual void SetPhys(std::string phys) = 0;

    virtual void SetUniq(std::string uniq) = 0;
};

class MMIListenerAdapter {
public:
    MMIListenerAdapter() = default;
    virtual ~MMIListenerAdapter() = default;
    virtual void OnDeviceAdded(int32_t deviceId, const std::string& type) = 0;
    virtual void OnDeviceRemoved(int32_t deviceId, const std::string& type) = 0;
};

class MMIInputListenerAdapter {
public:
    MMIInputListenerAdapter() = default;
    virtual ~MMIInputListenerAdapter() = default;
    virtual void OnInputEvent(int32_t keyCode, int32_t keyAction) = 0;
};

class MMIAdapter {
public:
    MMIAdapter() = default;

    virtual ~MMIAdapter() = default;

    virtual char* KeyCodeToString(int32_t keyCode) = 0;

    virtual int32_t RegisterMMIInputListener(std::shared_ptr<MMIInputListenerAdapter> eventCallback) = 0;

    virtual void UnregisterMMIInputListener(int32_t monitorId) = 0;

    virtual int32_t RegisterDevListener(std::string type, std::shared_ptr<MMIListenerAdapter> listener) = 0;

    virtual int32_t UnregisterDevListener(std::string type) = 0;

    virtual int32_t GetKeyboardType(int32_t deviceId, int32_t& type) = 0;

    virtual int32_t GetDeviceIds(std::vector<int32_t>& ids) = 0;

    virtual int32_t GetDeviceInfo(int32_t deviceId, std::shared_ptr<MMIDeviceInfoAdapter> info) = 0;

    virtual int32_t GetMaxTouchPoints() = 0;
};

} // namespace OHOS::NWeb

#endif // MMI_ADAPTER_H
