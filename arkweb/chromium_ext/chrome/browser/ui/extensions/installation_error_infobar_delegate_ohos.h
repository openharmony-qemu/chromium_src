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

#ifndef CHROME_BROWSER_UI_EXTENSIONS_INSTALLATION_ERROR_INFOBAR_DELEGATE_OHOS_H_
#define CHROME_BROWSER_UI_EXTENSIONS_INSTALLATION_ERROR_INFOBAR_DELEGATE_OHOS_H_

#include "components/infobars/core/confirm_infobar_delegate.h"
#include "extensions/browser/install/crx_install_error.h"

namespace ohos {

// Helper class to put up an infobar when installation fails.
class InstallationErrorInfoBarDelegate : public ConfirmInfoBarDelegate {
 public:
  explicit InstallationErrorInfoBarDelegate(
      const extensions::CrxInstallError& error);
  InstallationErrorInfoBarDelegate(const InstallationErrorInfoBarDelegate&) =
      delete;
  InstallationErrorInfoBarDelegate& operator=(
      const InstallationErrorInfoBarDelegate&) = delete;

  ~InstallationErrorInfoBarDelegate() override;

  static void ShowInfoBar(const extensions::CrxInstallError& error);

  // Override ConfirmInfoBarDelegate.
  infobars::InfoBarDelegate::InfoBarIdentifier GetIdentifier() const override;
  std::u16string GetMessageText() const override;
  int GetButtons() const override;

  extensions::CrxInstallError error_;
};

} // namespace ohos

#endif  // CHROME_BROWSER_UI_EXTENSIONS_INSTALLATION_ERROR_INFOBAR_DELEGATE_OHOS_H_
