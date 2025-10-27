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

#if BUILDFLAG(ARKWEB_DEVTOOLS)
#include "cef/ohos_cef_ext/libcef/browser/devtools/arkweb/devtools_frontend.h"
#endif

#if BUILDFLAG(ARKWEB_DEVTOOLS)
void CefDevToolsProtocolManager::OnFrontEndDestroyed() {
  devtools_frontend_ = nullptr;
}

void CefDevToolsProtocolManager::ShowDevToolsWith(
    CefRefPtr<ArkWebBrowserHostExt> frontend_browser,
    CefRefPtr<CefDevToolsMessageHandlerDelegate> devtools_message_handler,
    const CefPoint& inspect_element_at) {
  CEF_REQUIRE_UIT();
  if (devtools_frontend_) {
    if (!inspect_element_at.IsEmpty()) {
      devtools_frontend_->InspectElementAt(inspect_element_at.x,
                                           inspect_element_at.y);
    }
    devtools_frontend_->Focus();
    return;
  }

  if (false) {
    NOTIMPLEMENTED();
  } else {
    auto* web_contents = inspected_browser_.get()->GetWebContents();
    devtools_frontend_ = CefDevToolsFrontend::ShowWith(
        frontend_browser, std::move(devtools_message_handler), web_contents,
        inspect_element_at,
        base::BindOnce(&CefDevToolsProtocolManager::OnFrontEndDestroyed,
                       weak_ptr_factory_.GetWeakPtr()));
  }
}
#endif  // BUILDFLAG(ARKWEB_DEVTOOLS)
