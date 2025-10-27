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

#include "chrome/browser/extensions/system_display/display_info_provider_ohos.h"

#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "chrome/browser/extensions/system_display/display_info_provider.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace extensions {

DisplayInfoProviderOhos::DisplayInfoProviderOhos() {
  display_manager_adapter_ =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateDisplayMgrAdapter();
}

void DisplayInfoProviderOhos::UpdateDisplayUnitInfoForPlatform(
    const std::vector<display::Display>& displays,
    DisplayUnitInfoList& units) const {
  if (!display_manager_adapter_) {
    return;
  }
  auto ohos_displays = display_manager_adapter_->GetAllDisplays();
  if (ohos_displays.empty()) {
    return;
  }
  for (auto& ohos_display : ohos_displays) {
    if (!ohos_display) {
      continue;
    }
    auto display_id = static_cast<int64_t>(ohos_display->GetId());
    auto id = base::NumberToString(display_id);
    for (size_t unit_index = 0; unit_index < units.size(); unit_index++) {
      if (id == units[unit_index].id) {
        units[unit_index].name = ohos_display->GetName();
        auto state = ohos_display->GetDisplayState();
        if (state != OHOS::NWeb::DisplayState::UNKNOWN &&
            state != OHOS::NWeb::DisplayState::OFF) {
          units[unit_index].active_state =
              api::system_display::ActiveState::kActive;
        } else {
          units[unit_index].active_state =
              api::system_display::ActiveState::kInactive;
        }
        units[unit_index].is_enabled = ohos_display->GetAliveStatus();
        auto default_ratio = ohos_display->GetDefaultVirtualPixelRatio();
        if (default_ratio != 0) {
          units[unit_index].display_zoom_factor =
              ohos_display->GetVirtualPixelRatio() / default_ratio;
        }
        units[unit_index].dpi_x = ohos_display->GetDensityDpi();
        units[unit_index].dpi_y = ohos_display->GetDensityDpi();
        break;
      }
    }
  }
}

std::unique_ptr<DisplayInfoProvider> CreateChromeDisplayInfoProvider() {
  return std::make_unique<DisplayInfoProviderOhos>();
}

}  // namespace extensions
