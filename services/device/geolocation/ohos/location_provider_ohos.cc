/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#include "services/device/geolocation/ohos/location_provider_ohos.h"
 
#include <iostream>
#include <memory>
 
#include "base/functional/bind.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/memory/singleton.h"
#include "ohos/adapter/geolocation/location_adapter.h"
#include "ohos/adapter/geolocation/location_info.h"
#include "ohos/adapter/geolocation/location_proxy_adapter_impl.h"
#include "third_party/jsoncpp/source/include/json/json.h"

namespace device {
class GeolocationManager;
// LocationProviderOhos
LocationProviderOhos::LocationProviderOhos() {
  locator_callback_ = std::make_shared<LocationProviderCallback>();
}
 
LocationProviderOhos::~LocationProviderOhos() {
  StopProvider();
  if (locator_ != nullptr) {
    locator_.reset();
  }
  if (locator_callback_ != nullptr) {
    locator_callback_ = nullptr;
  }
}
 
void LocationProviderOhos::FillDiagnostics(
    mojom::GeolocationDiagnostics& diagnostics) {
  diagnostics.provider_state = state_;
}
 
void LocationProviderOhos::SetUpdateCallback(
    const LocationProviderUpdateCallback& callback) {
  callback_ = callback;
  if (!locator_callback_) {
    return;
  }
 
  locator_callback_->SetUpdateCallback(base::BindRepeating(
      &LocationProviderOhos::ProviderUpdateCallback, base::Unretained(this)));
}
 
void LocationProviderOhos::ProviderUpdateCallback(
    mojom::GeopositionResultPtr result) {
  last_result_ = std::move(result);
  if (!callback_.is_null() && is_running_) {
    callback_.Run(this, last_result_.Clone());
  }
}
 
void LocationProviderOhos::StartProvider(bool high_accuracy) {
  state_ = high_accuracy
               ? mojom::GeolocationDiagnostics::ProviderState::kHighAccuracy
               : mojom::GeolocationDiagnostics::ProviderState::kLowAccuracy;
  StopProvider();
  RequestLocationUpdate(high_accuracy);
}
 
void LocationProviderOhos::StopProvider() {
  state_ = mojom::GeolocationDiagnostics::ProviderState::kStopped;
  if (!is_running_) {
    return;
  }
  is_running_ = false;
  if (!locator_) {
    return;
  }
  locator_->StopListening(locator_callback_);
}
 
const mojom::GeopositionResult* LocationProviderOhos::GetPosition() {
  return last_result_.get();
}
 
void LocationProviderOhos::OnPermissionGranted() {
  LOG(INFO) << "LocationProviderOhos::OnPermissionGranted ";
  // Nothing to do here.
}
 
void LocationProviderCallback::OnNewLocationAvailable(
    const std::unique_ptr<ohos::adapter::LocationInfo>& location) {
  if (!location) {
    return;
  }
  auto position = mojom::Geoposition::New();
  position->latitude = location->latitude;
  position->longitude = location->longitude;
  position->timestamp = base::Time::FromMillisecondsSinceUnixEpoch(location->timeStamp);
  position->altitude = location->altitude;
  position->accuracy = location->accuracy;
  position->speed = location->speed;
  NewGeopositionReport(
      mojom::GeopositionResult::NewPosition(std::move(position)));
}
 
void LocationProviderCallback::OnNewErrorAvailable(std::string message) {
  NewGeopositionReport(
      mojom::GeopositionResult::NewError(mojom::GeopositionError::New(
          mojom::GeopositionErrorCode::kPositionUnavailable, message, "")));
}
 
void LocationProviderOhos::RequestLocationUpdate(bool high_accuracy) {
  LOG(INFO) << "LocationProviderOhos::RequestLocationUpdate";
  is_running_ = true;
  CreateLocationManagerIfNeeded();
  if (!locator_) {
    LOG(ERROR) << "Locator is null. Can not get location";
    locator_callback_->OnErrorReport(
        LocationProviderCallback::LOCATION_GET_FAILED);
    return;
  }
 
  bool ret = locator_->StartListening(locator_callback_, high_accuracy);
  if (!ret) {
    LOG(ERROR) << "StartListening failed. Can not get location";
    locator_callback_->OnErrorReport(
        LocationProviderCallback::LOCATION_GET_FAILED);
  }
}
 
void LocationProviderOhos::CreateLocationManagerIfNeeded() {
  if (locator_ != nullptr) {
    return;
  }
  locator_ = std::make_unique<ohos::adapter::LocationProxyAdapter>();
}
 
void LocationProviderCallback::NewGeopositionReport(
    mojom::GeopositionResultPtr result) {
  if (!callback_.is_null()) {
    callback_.Run(std::move(result));
  }
}
 
void LocationProviderCallback::OnLocationReport(
    const std::unique_ptr<ohos::adapter::LocationInfo>& location) {
  OnNewLocationAvailable(location);
}
 
void LocationProviderCallback::OnErrorReport(const int errorCode) {
  if (errorCode == LOCATION_GET_FAILED) {
    OnNewErrorAvailable("Failed to get location!");
  } else {
    OnNewErrorAvailable("Unknown error during the locating occured!");
  }
}
 
// static
std::unique_ptr<LocationProvider> NewSystemLocationProvider() {
  return base::WrapUnique(new LocationProviderOhos);
}
 
}  // namespace device
