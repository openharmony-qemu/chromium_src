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

#ifndef LOCATION_CALLBACK_ADAPTER_IMPL_H
#define LOCATION_CALLBACK_ADAPTER_IMPL_H

#include "i_locator_callback.h"
#include "location_adapter.h"
#include "location.h"
#include "ipc_object_stub.h"
#include "message_option.h"
#include "message_parcel.h"
#include "iremote_stub.h"

namespace OHOS::NWeb {
class LocationCallbackImpl
    : public OHOS::IRemoteStub<OHOS::Location::ILocatorCallback> {
public:
    LocationCallbackImpl(std::shared_ptr<LocationCallbackAdapter> adapter);
    ~LocationCallbackImpl() = default;

    virtual int OnRemoteRequest(uint32_t code,
                              OHOS::MessageParcel& data,
                              OHOS::MessageParcel& reply,
                              OHOS::MessageOption& option) override;
    void OnLocationReport(
        const std::unique_ptr<OHOS::Location::Location>& location) override;
    void OnLocatingStatusChange(const int status) override;
    void OnErrorReport(const int errorCode) override;
private:
    std::shared_ptr<LocationCallbackAdapter> locationCallbackAdapter_;
};
}

#endif