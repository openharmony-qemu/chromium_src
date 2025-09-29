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

#ifndef OHOS_EXTENSION_OMNIBOX_CEF_DELEGATE_H_
#define OHOS_EXTENSION_OMNIBOX_CEF_DELEGATE_H_

#include <optional>
#include <string>
#include <vector>

namespace OHOS::NWeb {

struct OmniboxSuggestResult {
  std::string content;
  std::string description;
  std::optional<bool> deletable;
};

class NWebExtensionOmniboxCefDelegate {
 public:
  ~NWebExtensionOmniboxCefDelegate() = default;

  static NWebExtensionOmniboxCefDelegate& GetInstance();

  void OnInputChanged(int32_t tab_id,
                      const std::string& text,
                      const std::string& extension_id);

  void OnInputEntered(int32_t tab_id,
                      int32_t disposition,
                      const std::string& text,
                      const std::string& extension_id);

  void OnInputChangedCallback(const std::vector<OmniboxSuggestResult>& results);

 private:
  NWebExtensionOmniboxCefDelegate() = default;
};

}  // namespace OHOS::NWeb

#endif  // OHOS_EXTENSION_OMNIBOX_CEF_DELEGATE_H_
