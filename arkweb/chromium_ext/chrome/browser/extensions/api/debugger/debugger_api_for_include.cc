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
#include "arkweb/chromium_ext/content/browser/web_contents/web_contents_impl_ext.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"

namespace extensions {

void ExtensionDevToolsClientHost::NotifyShowConfirmInfoBar() {
  LOG(INFO) << " func:" << __FUNCTION__;
  if (!agent_host_) {
    LOG(WARNING) << " func:" << __FUNCTION__;
    return;
  }
  if (WebContents* wc = agent_host_->GetWebContents()) {
    if (ExtensionDevToolsInfoBarDelegate* infoBarDelegate =
            ExtensionDevToolsInfoBarDelegate::GetDelegateById(extension_id())) {
      LOG(INFO) << " func:" << __FUNCTION__ << " send notification.";
      static_cast<content::WebContentsImplExt*>(wc)->OnShowConfirmInfoBar(
          "", extension_id(),
          base::UTF16ToUTF8(infoBarDelegate->GetMessageText()),
          infoBarDelegate->GetButtons(),
          base::UTF16ToUTF8(infoBarDelegate->GetButtonLabel(
              ExtensionDevToolsInfoBarDelegate::BUTTON_OK)),
          base::UTF16ToUTF8(infoBarDelegate->GetButtonLabel(
              ExtensionDevToolsInfoBarDelegate::BUTTON_CANCEL)));
    }
  }
}

void ExtensionDevToolsClientHost::NotifyHideConfirmInfoBar() {
  LOG(INFO) << " func:" << __FUNCTION__;
  if (!agent_host_) {
    LOG(WARNING) << " func:" << __FUNCTION__;
    return;
  }
  if (WebContents* wc = agent_host_->GetWebContents()) {
    if (ExtensionDevToolsInfoBarDelegate* infoBarDelegate =
            ExtensionDevToolsInfoBarDelegate::GetDelegateById(extension_id())) {
      LOG(INFO) << " func:" << __FUNCTION__ << " send notification.";
      static_cast<content::WebContentsImplExt*>(wc)->OnHideConfirmInfoBar(
          "", extension_id(),
          base::UTF16ToUTF8(infoBarDelegate->GetMessageText()),
          infoBarDelegate->GetButtons(),
          base::UTF16ToUTF8(infoBarDelegate->GetButtonLabel(
              ExtensionDevToolsInfoBarDelegate::BUTTON_OK)),
          base::UTF16ToUTF8(infoBarDelegate->GetButtonLabel(
              ExtensionDevToolsInfoBarDelegate::BUTTON_CANCEL)));
    }
  }
}

}  // namespace extensions
