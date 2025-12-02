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

#include "services/device/business_risk_intelligent_detection/business_risk_intelligent_detection_host_impl.h"

#include <dlfcn.h>

#include "mojo/public/cpp/bindings/self_owned_receiver.h"

namespace device {

typedef int32_t (*DetectSimulatedClickRiskEnhancedFFI)(
    const char *&result,
    const int8_t *nonceArr,
    size_t nonceLength,
    int32_t alg,
    int32_t version);

BusinessRiskIntelligentDetectionHostImpl::BusinessRiskIntelligentDetectionHostImpl() {}

BusinessRiskIntelligentDetectionHostImpl::~BusinessRiskIntelligentDetectionHostImpl() {
  if (detect_sim_click_risk_enhanced_handler_ != nullptr) {
    dlclose(detect_sim_click_risk_enhanced_handler_);
    detect_sim_click_risk_enhanced_handler_ = nullptr;
  }
}

void BusinessRiskIntelligentDetectionHostImpl::Create(
    mojo::PendingReceiver<device::mojom::BusinessRiskIntelligentDetection> receiver) {
  mojo::MakeSelfOwnedReceiver(std::make_unique<BusinessRiskIntelligentDetectionHostImpl>(),
                              std::move(receiver));
}

NO_SANITIZE("cfi-icall")
void BusinessRiskIntelligentDetectionHostImpl::DetectSimulatedClickRiskEnhanced(
    int32_t request_id,
    int32_t algorithm,
    const std::vector<int8_t>& nonce,
    int32_t version,
    DetectSimulatedClickRiskEnhancedCallback callback) {
  LOG(INFO) << "DetectSim: DetectSimulatedClickRiskEnhanced host enter.";

  if (!detect_sim_click_risk_enhanced_handler_) {
    detect_sim_click_risk_enhanced_handler_ = dlopen("/system/lib64/libbrid_client.z.so", RTLD_LAZY);

    if (!detect_sim_click_risk_enhanced_handler_) {
      const char* error = dlerror();
      LOG(WARNING) << "DetectSim: dlopen failed for :" << (error ? error : "Unknown error.");
      std::move(callback).Run(request_id, -1, "");
      return;
    }
  }

  DetectSimulatedClickRiskEnhancedFFI func = (DetectSimulatedClickRiskEnhancedFFI)
      dlsym(detect_sim_click_risk_enhanced_handler_, "DetectSimulatedClickRiskEnhancedFFI");
  if (!func) {
    const char* error = dlerror();
    LOG(WARNING) << "DetectSim: dlsym failed for :" << (error ? error : "Unknown error.");
    dlclose(detect_sim_click_risk_enhanced_handler_);
    detect_sim_click_risk_enhanced_handler_ = nullptr;
    std::move(callback).Run(request_id, -1, "");
    return;
  }

  const char *retStr = nullptr;
  int32_t ans = func(retStr, nonce.data(), nonce.size(), algorithm, version);

  LOG(INFO) << "DetectSim: result ans:" << ans;

  if (ans == 0 && retStr != nullptr) {
    std::move(callback).Run(request_id, ans, retStr);
    delete[] retStr;
  } else {
    std::move(callback).Run(request_id, ans, "");
  }
}

}  // namespace device
