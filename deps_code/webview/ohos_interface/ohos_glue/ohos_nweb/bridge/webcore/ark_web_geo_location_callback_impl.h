/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_GEO_LOCATION_CALLBACK_IMPL_H_
#define ARK_WEB_GEO_LOCATION_CALLBACK_IMPL_H_
#pragma once

#include "include/nweb_geolocation_callback_interface.h"
#include "ohos_nweb/include/ark_web_geo_location_callback.h"

namespace OHOS::ArkWeb {

class ArkWebGeoLocationCallbackImpl : public ArkWebGeoLocationCallback {
    IMPLEMENT_REFCOUNTING(ArkWebGeoLocationCallbackImpl);

public:
    ArkWebGeoLocationCallbackImpl(
        std::shared_ptr<OHOS::NWeb::NWebGeolocationCallbackInterface> nweb_geo_location_callback);
    ~ArkWebGeoLocationCallbackImpl() = default;

    /**
     * @brief Report the geo location permission status from users.
     *
     * @param origin The origin that ask for the geo location permission.
     * @param allow The geo location permission status.
     * @param retain Whether to allow the geo location permission status to be
     *        saved to the system.
     * @param incognito true if web is in the incognito mode, flase otherwise.
     */
    void GeoLocationCallbackInvoke(const ArkWebString& origin, bool allow, bool retain, bool incognito) override;

private:
    std::shared_ptr<OHOS::NWeb::NWebGeolocationCallbackInterface> nweb_geo_location_callback_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_GEO_LOCATION_CALLBACK_IMPL_H_
