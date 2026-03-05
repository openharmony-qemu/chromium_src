// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/service/device/geolocation/ohos/location_provider_ohos.h"

#include <memory>

#include "base/functional/bind.h"
#include "base/memory/singleton.h"

namespace device {
constexpr static int EPOCH_OFFSET_DELAY_TIME = 10;
class GeolocationManager;
// LocationProviderOhos
LocationProviderOhos::LocationProviderOhos() {
  task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  DCHECK(task_runner_);
  locator_callback_ = std::make_shared<LocationProviderCallback>(task_runner_);
}

LocationProviderOhos::~LocationProviderOhos() {
  StopProviderTask();
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
  task_runner_->PostTask(
    FROM_HERE,
    base::BindOnce(&LocationProviderOhos::StartProviderTask, weak_factory_.GetWeakPtr(), high_accuracy));
}

void LocationProviderOhos::StopProvider() {
  LOG(INFO) << "LocationProviderOhos::StopProvider";
  task_runner_->PostTask(
    FROM_HERE,
    base::BindOnce(&LocationProviderOhos::StopProviderTask, weak_factory_.GetWeakPtr()));
}

void LocationProviderOhos::StartProviderTask(bool high_accuracy) {
  LOG(INFO) << "LocationProviderOhos::StartProviderTask";
  StopProviderTask();
  state_ = high_accuracy
               ? mojom::GeolocationDiagnostics::ProviderState::kHighAccuracy
               : mojom::GeolocationDiagnostics::ProviderState::kLowAccuracy;
  RequestLocationUpdate(high_accuracy);
}

void LocationProviderOhos::StopProviderTask() {
  LOG(INFO) << "LocationProviderOhos::StopProviderTask";
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
  position->timestamp = base::Time::FromSecondsSinceUnixEpoch(EPOCH_OFFSET_DELAY_TIME);
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
  LOG(INFO) << "LocationProviderCallback::OnLocationReport";
  task_runner_->PostTask(
    FROM_HERE,
    base::BindOnce(&LocationProviderCallback::OnNewLocationAvailable, weak_factory_.GetWeakPtr(), location));
}

void LocationProviderCallback::OnLocatingStatusChange(const int status) {}

void LocationProviderCallback::OnErrorReport(const int errorCode) {
  LOG(INFO) << "LocationProviderCallback::OnErrorReport";
  std::string message =
    errorCode == LOCATION_GET_FAILED ? "Failed to get location!" : "Unknown error during the locating occured!";
  task_runner_->PostTask(
    FROM_HERE,
    base::BindOnce(&LocationProviderCallback::OnNewErrorAvailable, weak_factory_.GetWeakPtr(), message));
}

// static
std::unique_ptr<LocationProvider> NewSystemLocationProvider() {
  LOG(ERROR) << "NewSystemLocationProvider provider come in";
  return base::WrapUnique(new LocationProviderOhos);
}

}  // namespace device
