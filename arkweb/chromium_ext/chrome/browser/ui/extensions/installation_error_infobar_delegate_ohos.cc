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

#include "arkweb/chromium_ext/chrome/browser/ui/extensions/installation_error_infobar_delegate_ohos.h"

#include "components/strings/grit/components_strings.h"
#include "ui/base/l10n/l10n_util.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "ohos_nweb_ex/core/extension/extensions/info_bar/nweb_extension_install_info_bar_dispatcher.h"
#endif // BUILDFLAG(IS_ARKWEB_EXT)

namespace ohos {

InstallationErrorInfoBarDelegate::InstallationErrorInfoBarDelegate(
    const extensions::CrxInstallError& error)
    : ConfirmInfoBarDelegate(), error_(error) {}

InstallationErrorInfoBarDelegate::~InstallationErrorInfoBarDelegate() = default;

// static
void InstallationErrorInfoBarDelegate::ShowInfoBar(
    const extensions::CrxInstallError& error) {
  std::unique_ptr<ConfirmInfoBarDelegate> info_bar(
      new InstallationErrorInfoBarDelegate(error));
#if BUILDFLAG(IS_ARKWEB_EXT)
  ExtensionInstallInfoBarDispatcher::ShowInfoBar(std::move(info_bar));
#endif // BUILDFLAG(IS_ARKWEB_EXT)
}

infobars::InfoBarDelegate::InfoBarIdentifier
InstallationErrorInfoBarDelegate::GetIdentifier() const {
  return INSTALLATION_ERROR_INFOBAR_DELEGATE;
}

std::u16string InstallationErrorInfoBarDelegate::GetMessageText() const {
  return error_.message();
}

int InstallationErrorInfoBarDelegate::GetButtons() const {
  return BUTTON_OK;
}

} // namespace ohos
