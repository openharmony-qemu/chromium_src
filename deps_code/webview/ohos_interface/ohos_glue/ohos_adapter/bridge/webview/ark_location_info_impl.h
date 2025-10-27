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

#ifndef ARK_LOCATION_INFO_IMPL_H
#define ARK_LOCATION_INFO_IMPL_H
#pragma once

#include "location_adapter.h"
#include "ohos_adapter/include/ark_location_adapter.h"

namespace OHOS::ArkWeb {

class ArkLocationInfoImpl : public ArkLocationInfo {
public:
    explicit ArkLocationInfoImpl(std::shared_ptr<OHOS::NWeb::LocationInfo>);

    double GetLatitude() override;

    double GetLongitude() override;

    double GetAltitude() override;

    float GetAccuracy() override;

    float GetSpeed() override;

    double GetDirection() override;

    int64_t GetTimeStamp() override;

    int64_t GetTimeSinceBoot() override;

    ArkWebStringVector GetAdditions() override;

private:
    std::shared_ptr<OHOS::NWeb::LocationInfo> real_;

    IMPLEMENT_REFCOUNTING(ArkLocationInfoImpl);
};

} // namespace OHOS::ArkWeb

#endif
