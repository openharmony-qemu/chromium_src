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

#include "arkweb/chromium_ext/chrome/browser/extensions/extension_multiple_uninstall_dialog_ohos.h"

#include "arkweb/ohos_nweb/src/cef_delegate/nweb_extension_prompt_cef_delegate.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/strings/strcat.h"
#include "chrome/grit/generated_resources.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/common/extension.h"
#include "ui/base/l10n/l10n_util.h"

namespace ohos {

namespace {

class PromptInfoHolder final {
 public:
  PromptInfoHolder(
      const std::vector<extensions::ExtensionId>& extension_ids,
      base::OnceClosure accept_callback,
      base::OnceClosure cancel_callback)
    : extension_ids_(extension_ids),
      accept_callback_(std::move(accept_callback)),
      cancel_callback_(std::move(cancel_callback)) {}

  ~PromptInfoHolder() {
    if (cancel_callback_) {
      std::move(cancel_callback_).Run();
    }
  }

  static void ShowExtensionPrompt(
      const std::vector<extensions::ExtensionId>& extension_ids,
      base::OnceClosure accept_callback,
      base::OnceClosure cancel_callback) {
    auto info_holder = std::make_unique<PromptInfoHolder>(
        extension_ids, std::move(accept_callback), std::move(cancel_callback));
    auto info_holder_weak_ptr = info_holder->weak_factory_.GetWeakPtr();
    auto showPromptFunc =
        base::BindRepeating(&PromptInfoHolder::OnGetPromptAction,
                            info_holder_weak_ptr);
    if (!OHOS::NWeb::NWebExtensionPromptCefDelegate::GetInstance()
            .ShowMultiExtensionUninstallPrompt(
                info_holder->extension_ids_, showPromptFunc)) {
      LOG(INFO) << "ShowExtensionPrompt failed";
      if (!info_holder_weak_ptr) {
        // info_holder is already destructed.
        std::ignore = info_holder.release();
      }
      return;
    }
    std::ignore = info_holder.release(); // managed by info holder itself.
  }

  void OnGetPromptAction(int action, const std::string& error) {
    LOG(INFO) << "OnGetPromptAction(" << action << "," << error  << ")";
    std::unique_ptr<PromptInfoHolder> auto_release(this);
    switch (static_cast<DialogAction>(action)) {
      case DialogAction::kAutoConfirm:
        if (accept_callback_) {
          std::move(accept_callback_).Run();
        }
        break;
      default:
        if (cancel_callback_) {
          std::move(cancel_callback_).Run();
        }
        break;
    }
    accept_callback_.Reset();
    cancel_callback_.Reset();
  }

 private:
  enum class DialogAction {
    kDefault,
    kAutoConfirm,
    kAutoReject,
    kProgrammatic,
  };
  const std::vector<extensions::ExtensionId> extension_ids_;
  base::OnceClosure accept_callback_;
  base::OnceClosure cancel_callback_;

  base::WeakPtrFactory<PromptInfoHolder> weak_factory_{this};
};

} // namespace

void ShowExtensionMultipleUninstallDialog(
    const std::vector<extensions::ExtensionId>& extension_ids,
    base::OnceClosure accept_callback,
    base::OnceClosure cancel_callback) {
  LOG(INFO) << "ShowExtensionMultipleUninstallDialog";
  LOG(INFO) << " extension_ids[";
  for (auto& id : extension_ids) {
    LOG(INFO) << "  " << id << ",";
  }
  LOG(INFO) << " ]";
  PromptInfoHolder::ShowExtensionPrompt(
      extension_ids, std::move(accept_callback), std::move(cancel_callback));
}

}  // namespace ohos

