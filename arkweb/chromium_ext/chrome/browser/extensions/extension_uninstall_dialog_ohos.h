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

#ifndef ARKWEB_CHROMIUM_EXT_CHROME_BROWSER_EXTENSIONS_EXTENSION_UNINSTALL_DIALOG_OHOS_H_
#define ARKWEB_CHROMIUM_EXT_CHROME_BROWSER_EXTENSIONS_EXTENSION_UNINSTALL_DIALOG_OHOS_H_

#include <optional>

#include "base/memory/weak_ptr.h"
#include "chrome/browser/extensions/extension_uninstall_dialog.h"

namespace ohos {

// OHOS implementation of the uninstall dialog.
class ExtensionUninstallDialogOhos
    : public extensions::ExtensionUninstallDialog {
 public:
  ExtensionUninstallDialogOhos(
      Profile* profile,
      gfx::NativeWindow parent,
      extensions::ExtensionUninstallDialog::Delegate* delegate);
  ExtensionUninstallDialogOhos(const ExtensionUninstallDialogOhos&) = delete;
  ExtensionUninstallDialogOhos& operator=(
      const ExtensionUninstallDialogOhos&) = delete;
  ~ExtensionUninstallDialogOhos() override;

  void DialogFinished(bool accepted);

 private:
  void Show() override;
  void Close() override;

  void DialogAccepted();
  void DialogClosing();

  bool waiting_result_ = false;
  bool is_showing_dialog_ = false;
  std::optional<bool> result_;

  base::WeakPtrFactory<ExtensionUninstallDialogOhos> weak_ptr_factory_{this};
};

}  // namespace ohos

#endif  // ARKWEB_CHROMIUM_EXT_CHROME_BROWSER_EXTENSIONS_EXTENSION_UNINSTALL_DIALOG_OHOS_H_
