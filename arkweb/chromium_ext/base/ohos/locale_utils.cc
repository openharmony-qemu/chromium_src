/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/base/ohos/locale_utils.h"

#include <string>

#include "base/logging.h"
#include "ohos_adapter_helper.h"

namespace base {
namespace ohos {

std::string GetSystemLanguage() {
  auto resourceInstance =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().GetResourceAdapter();
  if (resourceInstance) {
    return resourceInstance->GetSystemLanguage();
  }
  return "zh-Hans";
}

bool IsTraditionalLanguage() {
  std::string systemLanguage = GetSystemLanguage();
  if (systemLanguage.find("zh-Hant") != std::string::npos) {
    return true;
  }
  return false;
}

std::string ComputeLanguageByRegion(const std::string& region) {
  std::string locale = "";
  std::string systemLanguage = GetSystemLanguage();
  // format: "language-region"
  // zh-Hant and region is "HK"/"MO" return "zh-HK", other return "zh-TW"
  // zh-Hans or other zh, return zh-CN
  // en only region is "GB" return "en-GB", other return "en-US"
  // bo only "bo-CN", ug only "ug"
  if (systemLanguage.find("zh-Hant") != std::string::npos) {
    locale = (region == "HK" || region == "MO") ? "zh-HK" : "zh-TW";
    return locale;
  }

  if (systemLanguage.find("en") != std::string::npos) {
    locale = region == "GB" ? "en-GB" : "en-US";
  } else if (systemLanguage.find("zh") != std::string::npos) {
    locale = "zh-CN";
  } else if (systemLanguage.find("bo") != std::string::npos) {
    locale = "bo-CN";
  } else if (systemLanguage.find("ug") != std::string::npos) {
    locale = "ug";
  } else {
    locale = systemLanguage;
    if (locale.find("-") == std::string::npos && !region.empty()) {
      locale += "-" + region;
    }
  }
  return locale;
}

}  // namespace ohos
}  // namespace base
