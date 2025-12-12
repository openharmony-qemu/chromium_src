/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef SERVICES_DEVICE_BUSINESS_RISK_INTELLIGENT_DETECTION_HOST_IMPL_H_
#define SERVICES_DEVICE_BUSINESS_RISK_INTELLIGENT_DETECTION_HOST_IMPL_H_

#include "arkweb/chromium_ext/services/device/public/mojom/business_risk_intelligent_detection.mojom.h"

namespace device {

class BusinessRiskIntelligentDetectionHostImpl
    : public device::mojom::BusinessRiskIntelligentDetection {
 public:
  BusinessRiskIntelligentDetectionHostImpl();

  ~BusinessRiskIntelligentDetectionHostImpl();

  static void Create(
      mojo::PendingReceiver<device::mojom::BusinessRiskIntelligentDetection> receiver);

  void DetectSimulatedClickRiskEnhanced(
      int32_t request_id,
      int32_t algorithm,
      const std::vector<int8_t>& nonce,
      int32_t version,
      DetectSimulatedClickRiskEnhancedCallback callback);

 private:
  void* detect_sim_click_risk_enhanced_handler_ = nullptr;
};

}  // namespace device

#endif  // SERVICES_DEVICE_BUSINESS_RISK_INTELLIGENT_DETECTION_HOST_IMPL_H_
