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

#include "nweb_extension_omnibox_cef_delegate.h"

#include "chrome/browser/extensions/api/omnibox/omnibox_api.h"
#include "chrome/browser/profiles/profile.h"
#include "nweb_extension_utils.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_omnibox_dispatcher.h"
#endif

namespace OHOS::NWeb {

NWebExtensionOmniboxCefDelegate&
NWebExtensionOmniboxCefDelegate::GetInstance() {
  static NWebExtensionOmniboxCefDelegate instance;
  return instance;
}

void NWebExtensionOmniboxCefDelegate::OnInputStarted(
    const std::string& extension_id) {
  auto profile = Profile::FromBrowserContext(GetBrowserContext());
  if (!profile) {
    return;
  }

  extensions::ExtensionOmniboxEventRouter::OnInputStarted(profile,
                                                          extension_id);
}

void NWebExtensionOmniboxCefDelegate::OnInputChanged(
    int32_t tab_id,
    const std::string& text,
    const std::string& extension_id) {
  auto browser_context = GetBrowserContext();
  if (!browser_context) {
    return;
  }

  extensions::ExtensionOmniboxEventRouter::OnInputChanged(text, extension_id,
                                                          browser_context);
}

void NWebExtensionOmniboxCefDelegate::OnInputEntered(
    int32_t tab_id,
    int32_t disposition,
    const std::string& text,
    const std::string& extension_id) {
  auto browser_context = GetBrowserContext();
  if (!browser_context) {
    return;
  }

  extensions::ExtensionOmniboxEventRouter::OnInputEntered(
      tab_id, text, extension_id,
      static_cast<WindowOpenDisposition>(disposition), browser_context);
}

void NWebExtensionOmniboxCefDelegate::OnInputCancelled(
    const std::string& extension_id) {
  auto profile = Profile::FromBrowserContext(GetBrowserContext());
  if (!profile) {
    return;
  }

  extensions::ExtensionOmniboxEventRouter::OnInputCancelled(profile,
                                                            extension_id);
}

void NWebExtensionOmniboxCefDelegate::OnDeleteSuggestion(
    const std::string& suggestion,
    const std::string& extension_id) {
  auto profile = Profile::FromBrowserContext(GetBrowserContext());
  if (!profile) {
    return;
  }

  extensions::ExtensionOmniboxEventRouter::OnDeleteSuggestion(
      profile, extension_id, suggestion);
}

void NWebExtensionOmniboxCefDelegate::OnInputChangedCallback(
    const std::vector<OmniboxSuggestResult>& results) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebExtensionOmniboxDispatcher::GetInstance().OnInputChangedCallback(results);
#endif
}

void NWebExtensionOmniboxCefDelegate::SetDefaultSuggestionCallback(
    const std::string& extension_id,
    const OmniboxSuggestResult& result) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebExtensionOmniboxDispatcher::GetInstance().SetDefaultSuggestionCallback(
      extension_id, result);
#endif
}

}  // namespace OHOS::NWeb
