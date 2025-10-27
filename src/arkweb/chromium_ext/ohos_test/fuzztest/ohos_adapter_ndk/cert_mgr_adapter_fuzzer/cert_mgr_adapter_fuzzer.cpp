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

#include <fuzzer/FuzzedDataProvider.h>

#include "cert_mgr_adapter/cert_mgr_adapter_impl.h"

namespace OHOS::NWeb {
bool LLVMFuzzerTestSetup() {
  return true;
}

void FuzzApi(const uint8_t* data, size_t size) {
  FuzzedDataProvider fdp(data, size);
  CertManagerAdapterImpl certMgr;
  certMgr.GetCertMaxSize();
  certMgr.GetAppCertMaxSize();
  certMgr.GetSytemRootCertSum();
  certMgr.GetCertDataBySubject(nullptr, nullptr, 0);
  certMgr.GetSytemRootCertData(0, nullptr);
  certMgr.GetUserRootCertSum();
  certMgr.GetUserRootCertData(0, nullptr);
  certMgr.GetAppCert(nullptr, nullptr, nullptr);
  certMgr.Sign(nullptr, nullptr, 0, nullptr, 0);
  std::vector<std::string> certs;
  std::string hostname = fdp.ConsumeRandomLengthString();
  certMgr.GetTrustAnchorsForHostName(hostname, certs);
  std::vector<std::string> pins;
  certMgr.CertManagerAdapterImpl::GetPinSetForHostName(hostname, pins);
}
}  // namespace OHOS::NWeb

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if ((data == nullptr) || (size < sizeof(int32_t))) {
    return 0;
  }
  static bool init = OHOS::NWeb::LLVMFuzzerTestSetup();
  if (init) {
    OHOS::NWeb::FuzzApi(data, size);
  }
  return 0;
}
