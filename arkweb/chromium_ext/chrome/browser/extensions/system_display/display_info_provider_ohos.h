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

#ifndef CHROME_BROWSER_EXTENSIONS_SYSTEM_DISPLAY_DISPLAY_INFO_PROVIDER_OHOS_H_
#define CHROME_BROWSER_EXTENSIONS_SYSTEM_DISPLAY_DISPLAY_INFO_PROVIDER_OHOS_H_

#include "extensions/browser/api/system_display/display_info_provider.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/display_manager_adapter.h"

namespace extensions {

class DisplayInfoProviderOhos : public DisplayInfoProvider {
 public:
  DisplayInfoProviderOhos();

  DisplayInfoProviderOhos(const DisplayInfoProviderOhos&) = delete;
  DisplayInfoProviderOhos& operator=(const DisplayInfoProviderOhos&) = delete;

  // Overridden implementation from DisplayInfoProvider
  void UpdateDisplayUnitInfoForPlatform(
      const std::vector<display::Display>& displays,
      DisplayUnitInfoList& units) const override;

 private:
  std::unique_ptr<OHOS::NWeb::DisplayManagerAdapter> display_manager_adapter_;
};

}  // namespace extensions

#endif  // CHROME_BROWSER_EXTENSIONS_SYSTEM_DISPLAY_DISPLAY_INFO_PROVIDER_OHOS_H_
