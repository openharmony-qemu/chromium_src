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

#ifndef NWEB_RUNTIME_CONNECT_INFO_IMPL_H
#define NWEB_RUNTIME_CONNECT_INFO_IMPL_H

#include "nweb_runtime_connect_info.h"

namespace OHOS::NWeb {
class NWebRuntimeConnectInfoImpl : public NWebRuntimeConnectInfo {
 public:
  NWebRuntimeConnectInfoImpl(
      const std::string& bundleName,
      const std::string& extensionOrigin,
      const int messageReadPipe,
      const int messageWritePipe);
  ~NWebRuntimeConnectInfoImpl() = default;

  std::string GetBundleName() override;
  std::string GetExtensionOrigin() override;
  int GetMessageReadPipe() override;
  int GetMessageWritePipe() override;

 private:
  std::string bundleName_;
  std::string extensionOrigin_;
  int messageReadPipe_;
  int messageWritePipe_;
};
}  // namespace OHOS::NWeb
#endif  // NWEB_RUNTIME_CONNECT_INFO_IMPL_H