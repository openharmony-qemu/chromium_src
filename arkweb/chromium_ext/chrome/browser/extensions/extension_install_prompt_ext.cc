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

#include "arkweb/chromium_ext/chrome/browser/extensions/extension_install_prompt_ext.h"

#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/extensions/extension_install_prompt_show_params.h"
#include "chrome/grit/generated_resources.h"
#include "extensions/common/extension.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_prompt_cef_delegate.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/image/image_skia_rep.h"

namespace ohos {

namespace {

class PromptInfoHolder {
 public:
  PromptInfoHolder(
      std::unique_ptr<ExtensionInstallPrompt::Prompt> prompt,
      ExtensionInstallPrompt::DoneCallback done_callback)
    : prompt_(std::move(prompt)),
      done_callback_(std::move(done_callback)) {}

  ~PromptInfoHolder() {
    if (!done_callback_) {
      return;
    }
    ExtensionInstallPrompt::DoneCallbackPayload payload(
        ExtensionInstallPrompt::Result::USER_CANCELED);
    std::move(done_callback_).Run(payload);
  }

  static void ShowExtensionPrompt(
      std::unique_ptr<ExtensionInstallPrompt::Prompt> prompt,
      ExtensionInstallPrompt::DoneCallback done_callback) {
    auto info_holder = std::make_unique<PromptInfoHolder>(
        std::move(prompt), std::move(done_callback));
    auto info_holder_weak_ptr = info_holder->weak_factory_.GetWeakPtr();
    auto showPromptFunc =
        base::BindRepeating(&PromptInfoHolder::OnGetPromptAction,
                            info_holder_weak_ptr);
    auto getPromptDataFunc =
        base::BindRepeating(&PromptInfoHolder::GetPromptData,
                            info_holder_weak_ptr);
    if (!OHOS::NWeb::NWebExtensionPromptCefDelegate::GetInstance()
            .ShowExtensionPrompt(PROMPT_INSTALLATION,
                                 info_holder->prompt_->extension()->id(),
                                 info_holder->prompt_->icon(),
                                 showPromptFunc,
                                 getPromptDataFunc)) {
      LOG(INFO) << "ShowExtensionPrompt failed";
      if (!info_holder_weak_ptr) {
        // info_holder is already destructed.
        std::ignore = info_holder.release();
      }
      return;
    }
    std::ignore = info_holder.release(); // managed by info holder itself.
  }

  void GetPromptData(NWebExtensionPromptData* data) {
    if (!data || !prompt_) {
      return;
    }

    data->free_memory_func =
        OHOS::NWeb::NWebExtensionPromptCefDelegate::FreePromptData;

    std::string title = base::UTF16ToUTF8(prompt_->GetDialogTitle());
    std::string abortButtonLabel =
        base::UTF16ToUTF8(prompt_->GetAbortButtonLabel());
    std::string acceptButtonLabel =
        base::UTF16ToUTF8(prompt_->GetAcceptButtonLabel());
    std::string permissionsHeading =
      base::UTF16ToUTF8(prompt_->GetPermissionsHeading());
    data->title = strdup(title.c_str());
    data->is_from_webstore = prompt_->is_from_webstore();
    data->abortButtonLabel = strdup(abortButtonLabel.c_str());
    data->acceptButtonLabel = strdup(acceptButtonLabel.c_str());
    data->permissionsHeading = nullptr;

    data->permissionCount = prompt_->GetPermissionCount();
    if (0 < data->permissionCount) {
      data->permissionsHeading = strdup(permissionsHeading.c_str());

      data->permissions = (NWebExtensionPermission*)calloc(
          data->permissionCount, sizeof(NWebExtensionPermission));
      if (!data->permissions) {
        return;
      }

      const std::string show_details_label =
          l10n_util::GetStringUTF8(IDS_EXTENSIONS_SHOW_DETAILS);
      const std::string hide_details_label =
          l10n_util::GetStringUTF8(IDS_EXTENSIONS_HIDE_DETAILS);
      for (uint32_t i = 0; i < data->permissionCount; i++) {
        std::string permission =
            base::UTF16ToUTF8(prompt_->GetPermission(i));
        std::string detail =
            base::UTF16ToUTF8(prompt_->GetPermissionsDetails(i));
        data->permissions[i].detail = strdup(detail.c_str());
        data->permissions[i].permission = strdup(permission.c_str());
        data->permissions[i].showDetailLabel = strdup(show_details_label.c_str());
        data->permissions[i].hideDetailLabel = strdup(hide_details_label.c_str());
      }
    }
  }

  void OnGetPromptAction(int action, const std::string& error) {
    std::unique_ptr<PromptInfoHolder> auto_release(this);
    if (!done_callback_) {
      return;
    }
    if (static_cast<DialogAction>(action) == DialogAction::kAutoConfirm) {
        ExtensionInstallPrompt::DoneCallbackPayload payload(
            ExtensionInstallPrompt::Result::ACCEPTED);
        std::move(done_callback_).Run(payload);
    } else {
        ExtensionInstallPrompt::DoneCallbackPayload payload(
            ExtensionInstallPrompt::Result::USER_CANCELED);
        std::move(done_callback_).Run(payload);
    }
  }

 private:
  enum class DialogAction {
    kDefault,
    kAutoConfirm,
    kAutoReject,
    kProgrammatic,
  };

  std::unique_ptr<ExtensionInstallPrompt::Prompt> prompt_;
  ExtensionInstallPrompt::DoneCallback done_callback_;

  base::WeakPtrFactory<PromptInfoHolder> weak_factory_{this};
};

} // namespace

void ShowExtensionInstallDialogImpl(
    std::unique_ptr<ExtensionInstallPromptShowParams> show_params,
    ExtensionInstallPrompt::DoneCallback done_callback,
    std::unique_ptr<ExtensionInstallPrompt::Prompt> prompt) {
  LOG(INFO) << "ShowExtensionInstallDialogImpl";
  PromptInfoHolder::ShowExtensionPrompt(
      std::move(prompt), std::move(done_callback));
}

} // namespace ohos
