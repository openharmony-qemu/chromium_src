// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_DEVICE_GEOLOCATION_LOCATION_PROVIDER_OHOS_H_
#define SERVICES_DEVICE_GEOLOCATION_LOCATION_PROVIDER_OHOS_H_

#include <location_adapter.h>

#include <memory>

#include "base/memory/weak_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/task/single_thread_task_runner.h"
#include "base/threading/thread_checker.h"
#include "services/device/public/cpp/geolocation/location_provider.h"
#include "services/device/public/mojom/geolocation_internals.mojom.h"
#include "services/device/public/mojom/geoposition.mojom.h"

namespace device {
class LocationProviderCallback : public OHOS::NWeb::LocationCallbackAdapter {
 public:
  LocationProviderCallback(const scoped_refptr<base::SingleThreadTaskRunner>& task_runner) {
    task_runner_ = task_runner;
  }
  ~LocationProviderCallback() = default;

  enum LocationErrorCode {
    LOCATION_GET_SUCCESS = 0,
    LOCATION_GET_FAILED,
    LOCATION_UNKNOWN_ERROR,
  };

  typedef base::RepeatingCallback<void(mojom::GeopositionResultPtr position)>
      UpdateCallback;

  // ILocatorCallback implementation.
  void OnLocationReport(
      const std::shared_ptr<OHOS::NWeb::LocationInfo> location) override;
  void OnLocatingStatusChange(const int status) override;
  void OnErrorReport(const int errorCode) override;

  void OnNewLocationAvailable(
      const std::shared_ptr<OHOS::NWeb::LocationInfo> location);
  void OnNewErrorAvailable(std::string message);
  void SetUpdateCallback(const UpdateCallback& callback) {
    callback_ = callback;
  }
  const mojom::GeopositionResult* GetPosition() { return last_position_.get(); }

 private:
  // Calls |callback_| with the new location.
  void NewGeopositionReport(mojom::GeopositionResultPtr position);

  mojom::GeopositionResultPtr last_position_;
  UpdateCallback callback_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::WeakPtrFactory<LocationProviderCallback> weak_factory_{this};
};

// Location provider for OpenHarmony using the platform provider over JNI.
class LocationProviderOhos : public LocationProvider {
 public:
  LocationProviderOhos();
  ~LocationProviderOhos() override;

  // LocationProvider implementation.
  void SetUpdateCallback(
      const LocationProviderUpdateCallback& callback) override;
  void StartProvider(bool high_accuracy) override;
  void StopProvider() override;
  const mojom::GeopositionResult* GetPosition() override;
  void OnPermissionGranted() override;
  void FillDiagnostics(mojom::GeolocationDiagnostics& diagnostics) override;

  void ProviderUpdateCallback(mojom::GeopositionResultPtr position);

 private:
  void StartProviderTask(bool high_accuracy);
  void StopProviderTask();
  void RequestLocationUpdate(bool high_accuracy);
  void CreateLocationManagerIfNeeded();
  void SetRequestConfig(
      std::shared_ptr<OHOS::NWeb::LocationRequestConfig>& requestConfig,
      bool high_accuracy);

  std::shared_ptr<OHOS::NWeb::LocationProxyAdapter> locator_;

  LocationProviderUpdateCallback callback_;
  mojom::GeolocationDiagnostics::ProviderState state_ =
      mojom::GeolocationDiagnostics::ProviderState::kStopped;
  bool is_running_ = false;
  std::shared_ptr<LocationProviderCallback> locator_callback_ = nullptr;
  int32_t callback_id_ = -1;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::WeakPtrFactory<LocationProviderOhos> weak_factory_{this};
};

}  // namespace device

#endif  // SERVICES_DEVICE_GEOLOCATION_LOCATION_PROVIDER_OHOS_H_
