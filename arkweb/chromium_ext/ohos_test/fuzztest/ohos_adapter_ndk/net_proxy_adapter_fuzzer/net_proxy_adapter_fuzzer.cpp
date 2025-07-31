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

#include "net_proxy_adapter.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "net_proxy_adapter_impl/net_proxy_adapter_impl.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using ::testing::NiceMock;

namespace OHOS::NWeb {
bool LLVMFuzzerTestSetup() {
  return true;
}

class NetProxyEventCallbackAdapterMock : public NetProxyEventCallbackAdapter {
 public:
  NetProxyEventCallbackAdapterMock() = default;
  ~NetProxyEventCallbackAdapterMock() override = default;

  MOCK_METHOD(void,
              Changed,
              (const std::string& host,
               const uint16_t& port,
               const std::string& pacUrl,
               const std::vector<std::string>& exclusionList),
              (override));
};

namespace Base64 {
  extern std::string Encode(const std::string& source);
  extern std::string Decode(const std::string& source);
}

void FuzzApi(const uint8_t* data, size_t size) {
  FuzzedDataProvider fdp(data, size);
  NetProxyAdapterImpl ins = NetProxyAdapterImpl::GetInstance();
  std::shared_ptr<NiceMock<NetProxyEventCallbackAdapterMock>> eventCallback =
      std::make_shared<NiceMock<NetProxyEventCallbackAdapterMock>>();
  ins.RegNetProxyEvent(eventCallback);
  ins.StartListen();
  ins.StopListen();
  std::string source = fdp.ConsumeRandomLengthString();
  Base64::Encode(source);
  Base64::Decode(source);
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
