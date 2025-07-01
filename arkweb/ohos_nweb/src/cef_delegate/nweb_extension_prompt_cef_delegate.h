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

#ifndef OHOS_EXTENSION_PROMPT_CEF_DELEGATE_H_
#define OHOS_EXTENSION_PROMPT_CEF_DELEGATE_H_

#include "base/functional/callback.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_prompt_types.h"
#include "ui/gfx/image/image.h"

namespace OHOS::NWeb {

using ShowPromptCallbackFunc =
    base::RepeatingCallback<void(int, const std::string&)>;
using GetPromptDataCallbackFunc =
    base::RepeatingCallback<void(NWebExtensionPromptData*)>;

class NWebExtensionPromptCefDelegate {
 public:
  ~NWebExtensionPromptCefDelegate() = default;

  static NWebExtensionPromptCefDelegate& GetInstance();

  NWebExtensionPromptData* GetPromptData(int id);
  std::vector<std::string> GetIdList(int id);

  static void FreePromptData(void* addr);

  bool ShowExtensionPrompt(PromptType type,
                           const std::string& extensionId,
                           const gfx::Image& icon_image,
                           ShowPromptCallbackFunc showPromptFunc,
                           GetPromptDataCallbackFunc getPromptDataFunc);

  bool ShowMultiExtensionUninstallPrompt(
      const std::vector<std::string>& extension_ids,
      ShowPromptCallbackFunc show_prompt_func);

  void OnShowExtensionPrompt(int id, int action, const char* error);

 private:
  NWebExtensionPromptCefDelegate() = default;
};

}  // namespace OHOS::NWeb

#endif  // OHOS_EXTENSION_PROMPT_CEF_DELEGATE_H_
