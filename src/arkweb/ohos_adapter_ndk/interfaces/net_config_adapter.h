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

#ifndef NET_CONFIG_ADAPTER_H
#define NET_CONFIG_ADAPTER_H

#include <functional>
#include <memory>
#include <vector>

namespace OHOS::NWeb {

class NetConfigAdapter {
 public:
  NetConfigAdapter() = default;
  virtual ~NetConfigAdapter() = default;

  virtual bool GetIsCleartextPermittedByHostName(
      const std::string& hostname) = 0;

  virtual bool GetIsCleartextCfgByComponent(const std::string& component) = 0;
};

}  // namespace OHOS::NWeb

#endif  // NET_CONFIG_ADAPTER_H