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

#include "base/ohos/locale_utils.h"

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
  if (systemLanguage.find("zh-Hant") != std::string::npos) {
    locale = (region == "TW") ? "zh-TW" : "zh-HK";
    return locale;
  }
  // Current system only support zh, bo, ug, en.
  if (systemLanguage.find("en") != std::string::npos) {
    locale = "en-US";
  } else if (systemLanguage.find("zh") != std::string::npos) {
    locale = "zh-CN";
  } else if (systemLanguage.find("bo") != std::string::npos) {
    locale = "bo-CN";
  } else if (systemLanguage.find("ug") != std::string::npos) {
    locale = "ug";
  }
  return locale;
}

}  // namespace ohos
}  // namespace base
