// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/device/geolocation/ohos/location_provider_ohos.h"

#include <memory>

#include "base/functional/bind.h"
#include "base/memory/singleton.h"

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

void LocationProviderOhos::SetUpdateCallback(
    const LocationProviderUpdateCallback& callback) {
  LOG(DEBUG) << "LocationProviderOhos::SetUpdateCallback";
  callback_ = callback;

  if (!locator_callback_) {
    return;
  }

  locator_callback_->SetUpdateCallback(
      base::BindRepeating(&LocationProviderOhos::ProviderUpdateCallback,
                          weak_factory_.GetWeakPtr()));
}

void LocationProviderOhos::ProviderUpdateCallback(
    mojom::GeopositionResultPtr position) {
  if (!callback_.is_null() && is_running_) {
    callback_.Run(this, position.Clone());
  }
}

void LocationProviderOhos::StartProvider(bool high_accuracy) {
  LOG(INFO) << "LocationProviderOhos::StartProvider";
  StopProvider();
  state_ = high_accuracy
               ? mojom::GeolocationDiagnostics::ProviderState::kHighAccuracy
               : mojom::GeolocationDiagnostics::ProviderState::kLowAccuracy;
  RequestLocationUpdate(high_accuracy);
}

void LocationProviderOhos::StopProvider() {
  LOG(INFO) << "LocationProviderOhos::StopProvider";
  if (!is_running_) {
    return;
  }
  is_running_ = false;
  if (!locator_) {
    return;
  }
  locator_->StopLocating(callback_id_);
  state_ = mojom::GeolocationDiagnostics::ProviderState::kStopped;
}

const mojom::GeopositionResult* LocationProviderOhos::GetPosition() {
  LOG(INFO) << "LocationProviderOhos::GetPosition";
  return locator_callback_->GetPosition();
}

void LocationProviderOhos::OnPermissionGranted() {
  LOG(INFO) << "LocationProviderOhos::OnPermissionGranted";
  // Nothing to do here.
}

void LocationProviderCallback::OnNewLocationAvailable(
    const std::shared_ptr<OHOS::NWeb::LocationInfo> location) {
  LOG(INFO) << "LocationProviderCallback::OnNewLocationAvailable";
  if (!location) {
    return;
  }

  auto position = mojom::Geoposition::New();
  position->latitude = location->GetLatitude();
  position->longitude = location->GetLongitude();
  // location->GetTimeStamp() has no value now, so that temporarily passed
  // value 10.
  position->timestamp = base::Time::FromSecondsSinceUnixEpoch(10);
  position->altitude = location->GetAltitude();
  position->accuracy = location->GetAccuracy();
  position->heading = location->GetDirection();
  position->speed = location->GetSpeed();

  NewGeopositionReport(
      mojom::GeopositionResult::NewPosition(std::move(position)));
}

void LocationProviderCallback::OnNewErrorAvailable(std::string message) {
  LOG(DEBUG) << "LocationProviderCallback::OnNewErrorAvailable";
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

  std::shared_ptr<OHOS::NWeb::LocationRequestConfig> request_config =
      OHOS::NWeb::LocationInstance::GetInstance().CreateLocationRequestConfig();
  SetRequestConfig(request_config, high_accuracy);
  if (!locator_->IsLocationEnabled()) {
    LOG(ERROR) << "geolocation setting is not turned on";
    locator_callback_->OnErrorReport(
        LocationProviderCallback::LOCATION_GET_FAILED);
    return;
  }

  callback_id_ = locator_->StartLocating(request_config, locator_callback_);
  if (callback_id_ == -1) {
    LOG(ERROR) << "StartLocating failed. Can not get location";
    locator_callback_->OnErrorReport(
        LocationProviderCallback::LOCATION_GET_FAILED);
  }
}

void LocationProviderOhos::CreateLocationManagerIfNeeded() {
  if (locator_ != nullptr) {
    return;
  }
  locator_ =
      OHOS::NWeb::LocationInstance::GetInstance().CreateLocationProxyAdapter();
}

void LocationProviderOhos::FillDiagnostics(
    mojom::GeolocationDiagnostics& diagnostics) {
  // TODO
  diagnostics.provider_state = state_;
  return;
}

void LocationProviderOhos::SetRequestConfig(
    std::shared_ptr<OHOS::NWeb::LocationRequestConfig>& request_config,
    bool high_accuracy) {
  if (!request_config) {
    return;
  }

  request_config->SetPriority(
      OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_FAST_FIRST_FIX);
  request_config->SetScenario(
      OHOS::NWeb::LocationRequestConfig::Scenario::UNSET);
  request_config->SetTimeInterval(1);
  request_config->SetDistanceInterval(0);
  request_config->SetFixNumber(0);
}

void LocationProviderCallback::NewGeopositionReport(
    mojom::GeopositionResultPtr position) {
  last_position_ = std::move(position);
  if (!callback_.is_null()) {
    callback_.Run(last_position_.Clone());
  }
}

void LocationProviderCallback::OnLocationReport(
    const std::shared_ptr<OHOS::NWeb::LocationInfo> location) {
  OnNewLocationAvailable(location);
}

void LocationProviderCallback::OnLocatingStatusChange(const int status) {}

void LocationProviderCallback::OnErrorReport(const int errorCode) {
  if (errorCode == LOCATION_GET_FAILED) {
    OnNewErrorAvailable("Failed to get location!");
  } else {
    OnNewErrorAvailable("Unknown error during the locating occured!");
  }
}

// static
std::unique_ptr<LocationProvider> NewSystemLocationProvider() {
  LOG(ERROR) << "NewSystemLocationProvider provider come in";
  return base::WrapUnique(new LocationProviderOhos);
}

}  // namespace device
