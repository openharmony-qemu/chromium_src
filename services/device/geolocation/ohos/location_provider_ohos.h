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
 
#ifndef SERVICES_DEVICE_GEOLOCATION_LOCATION_PROVIDER_OHOS_H_
#define SERVICES_DEVICE_GEOLOCATION_LOCATION_PROVIDER_OHOS_H_
 
#include <memory>
 
#include "base/task/single_thread_task_runner.h"
#include "base/threading/thread_checker.h"
#include "ohos/adapter/geolocation/location_adapter.h"
#include "ohos/adapter/geolocation/location_info.h"
#include "ohos/adapter/geolocation/location_proxy_adapter_impl.h"
#include "services/device/public/cpp/geolocation/location_provider.h"
#include "services/device/public/mojom/geoposition.mojom.h"
 
namespace device {
class LocationProviderCallback : public ohos::adapter::LocationCallbackAdapter {
 public:
  LocationProviderCallback() {}
  ~LocationProviderCallback() = default;
  enum LocationErrorCode {
    LOCATION_GET_SUCCESS = 0,
    LOCATION_GET_FAILED,
    LOCATION_UNKNOWN_ERROR,
  };
  using UpdateCallback =
      base::RepeatingCallback<void(mojom::GeopositionResultPtr)>;
  // ILocatorCallback implementation.
  void OnLocationReport(
      const std::unique_ptr<ohos::adapter::LocationInfo>& location) override;
  void OnErrorReport(const int errorCode) override;
  void OnNewLocationAvailable(
      const std::unique_ptr<ohos::adapter::LocationInfo>& location);
  void OnNewErrorAvailable(std::string message);
  void SetUpdateCallback(UpdateCallback callback) { callback_ = callback; }
 
 private:
  // Calls |callback_| with the new location.
  void NewGeopositionReport(mojom::GeopositionResultPtr result);
  UpdateCallback callback_;
};
 
class LocationProviderOhos : public LocationProvider {
 public:
  LocationProviderOhos();
  ~LocationProviderOhos() override;
 
  // LocationProvider implementation.
  void FillDiagnostics(mojom::GeolocationDiagnostics& diagnostics) override;
  void SetUpdateCallback(
      const LocationProviderUpdateCallback& callback) override;
  void StartProvider(bool high_accuracy) override;
  void StopProvider() override;
  const mojom::GeopositionResult* GetPosition() override;
  void OnPermissionGranted() override;
  void ProviderUpdateCallback(mojom::GeopositionResultPtr result);
 
 private:
  void RequestLocationUpdate(bool high_accuracy);
  void CreateLocationManagerIfNeeded();
  mojom::GeolocationDiagnostics::ProviderState state_ =
      mojom::GeolocationDiagnostics::ProviderState::kStopped;
  std::unique_ptr<ohos::adapter::LocationProxyAdapter> locator_;
  LocationProviderUpdateCallback callback_;
  bool is_running_ = false;
  std::shared_ptr<LocationProviderCallback> locator_callback_ = nullptr;
  mojom::GeopositionResultPtr last_result_;
};
 
}  // namespace device
 
#endif  // SERVICES_DEVICE_GEOLOCATION_LOCATION_PROVIDER_OHOS_H_
