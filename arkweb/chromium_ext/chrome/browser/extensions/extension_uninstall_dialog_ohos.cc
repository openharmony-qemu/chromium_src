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

#include "arkweb/chromium_ext/chrome/browser/extensions/extension_uninstall_dialog_ohos.h"

#include "base/functional/callback.h"
#include "base/task/single_thread_task_runner.h"
#include "chrome/browser/extensions/extension_util.h"
#include "chrome/grit/generated_resources.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/image/image_skia_operations.h"
#include "ui/gfx/image/image_skia_rep.h"
#include "ui/strings/grit/ui_strings.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_prompt_cef_delegate.h"

namespace ohos {

namespace {

class PromptInfoHolder {
 public:
  PromptInfoHolder(
      const extensions::Extension* extension,
      const extensions::Extension* triggering_extension,
      base::OnceCallback<void(bool)> done_callback)
    : extension_(extension),
      triggering_extension_(triggering_extension),
      done_callback_(std::move(done_callback)) {}
  ~PromptInfoHolder() = default;

  static void ShowExtensionPrompt(
      const extensions::Extension* extension,
      const extensions::Extension* triggering_extension,
      gfx::Image icon,
      base::OnceCallback<void(bool)> done_callback) {
    auto info_holder = std::make_unique<PromptInfoHolder>(
        extension, triggering_extension, std::move(done_callback));
    auto showPromptFunc =
        base::BindRepeating(&PromptInfoHolder::OnGetPromptAction,
                            info_holder->weak_factory_.GetWeakPtr());
    auto getPromptDataFunc =
        base::BindRepeating(&PromptInfoHolder::GetPromptData,
                            info_holder->weak_factory_.GetWeakPtr());
    if (!OHOS::NWeb::NWebExtensionPromptCefDelegate::GetInstance()
            .ShowExtensionPrompt(PROMPT_INSTALLATION,
                                 extension->id(),
                                 icon,
                                 showPromptFunc,
                                 getPromptDataFunc)) {
      LOG(INFO) << "ShowExtensionPrompt failed";
      return;
    }
    std::ignore = info_holder.release(); // managed by info holder itself.
  }

  void GetPromptData(NWebExtensionPromptData* data) {
    if (!data) {
      return;
    }

    data->free_memory_func =
        OHOS::NWeb::NWebExtensionPromptCefDelegate::FreePromptData;

    std::string title = l10n_util::GetStringFUTF8(
        IDS_EXTENSION_PROMPT_UNINSTALL_TITLE,
        extensions::util::GetFixupExtensionNameForUIDisplay(
            extension_->name()));
    std::string abortButtonLabel = l10n_util::GetStringUTF8(IDS_APP_CANCEL);
    std::string acceptButtonLabel = l10n_util::GetStringUTF8(
        IDS_EXTENSION_PROMPT_UNINSTALL_BUTTON);

    std::string permissionsHeading;
    if (triggering_extension_) {
      permissionsHeading = l10n_util::GetStringFUTF8(
          IDS_EXTENSION_PROMPT_UNINSTALL_TRIGGERED_BY_EXTENSION,
          extensions::util::GetFixupExtensionNameForUIDisplay(
              triggering_extension_->name()));
    }

    data->title = strdup(title.c_str());
    data->abortButtonLabel = strdup(abortButtonLabel.c_str());
    data->acceptButtonLabel = strdup(acceptButtonLabel.c_str());
    data->permissionsHeading = strdup(permissionsHeading.c_str());

  }

  void OnGetPromptAction(int action, const std::string& error) {
    std::unique_ptr<PromptInfoHolder> auto_release(this);
    if (!done_callback_) {
      return;
    }
    switch (static_cast<DialogAction>(action)) {
      case DialogAction::kAutoConfirm:
        std::move(done_callback_).Run(true);
        break;
      default:
        std::move(done_callback_).Run(false);
        break;
    }
  }

 private:
  enum class DialogAction {
    kDefault,
    kAutoConfirm,
    kAutoReject,
    kProgrammatic,
  };
  raw_ptr<const extensions::Extension> extension_;
  raw_ptr<const extensions::Extension> triggering_extension_;
  base::OnceCallback<void(bool)> done_callback_;

  base::WeakPtrFactory<PromptInfoHolder> weak_factory_{this};
};

gfx::ImageSkia CreateSuitableImage(gfx::ImageSkia image) {
  std::vector<gfx::ImageSkiaRep> reps = image.image_reps();
  int min_distance = std::numeric_limits<int>::max();
  const gfx::ImageSkiaRep* best_match_rep = nullptr;
  for (const auto& rep : reps) {
    int distance = rep.pixel_width() - extension_misc::EXTENSION_ICON_SMALL;
    distance = std::abs(distance);
    if (distance < min_distance) {
      min_distance = distance;
      best_match_rep = &rep;
    }
  }
  if (best_match_rep) {
    return gfx::ImageSkiaOperations::CreateResizedImage(
        gfx::ImageSkia(*best_match_rep),
        skia::ImageOperations::ResizeMethod::RESIZE_GOOD,
        gfx::Size(extension_misc::EXTENSION_ICON_SMALL,
            extension_misc::EXTENSION_ICON_SMALL));
  }
  return gfx::ImageSkia();
}

} // namespace

ExtensionUninstallDialogOhos::ExtensionUninstallDialogOhos(
    Profile* profile,
    gfx::NativeWindow parent,
    extensions::ExtensionUninstallDialog::Delegate* delegate)
    : extensions::ExtensionUninstallDialog(profile, parent, delegate) {}

ExtensionUninstallDialogOhos::~ExtensionUninstallDialogOhos() {
  if (waiting_result_) {
    DialogClosing();
  }
}

void ExtensionUninstallDialogOhos::Show() {
  LOG(INFO) << "enter ExtensionUninstallDialogOhos::Show";
  waiting_result_ = true;

  auto done_callback = base::BindOnce(
      &ExtensionUninstallDialogOhos::DialogFinished,
      weak_ptr_factory_.GetWeakPtr());
  base::AutoReset<bool> show_scope(&is_showing_dialog_, true);
  PromptInfoHolder::ShowExtensionPrompt(
      extension(), triggering_extension(),
      gfx::Image(CreateSuitableImage(icon())), std::move(done_callback));

  LOG(INFO) << "exit ExtensionUninstallDialogOhos::Show";
}

void ExtensionUninstallDialogOhos::Close() {
}

void ExtensionUninstallDialogOhos::DialogFinished(bool accepted) {
  LOG(INFO) << "ExtensionUninstallDialogOhos::DialogFinished(" << accepted
            << "), is_showing_dialog_[" << is_showing_dialog_ << "]";
  if (is_showing_dialog_) {
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
        FROM_HERE,
        base::BindOnce(&ExtensionUninstallDialogOhos::DialogFinished,
            weak_ptr_factory_.GetWeakPtr(), accepted));
    return;
  }
  waiting_result_ = false;
  if (accepted) {
    DialogAccepted();
  } else {
    DialogClosing();
  }
}

void ExtensionUninstallDialogOhos::DialogAccepted() {
  const bool checkbox_is_checked = false;
  OnDialogClosed(checkbox_is_checked
                     ? CLOSE_ACTION_UNINSTALL_AND_CHECKBOX_CHECKED
                     : CLOSE_ACTION_UNINSTALL);
}

void ExtensionUninstallDialogOhos::DialogClosing() {
  OnDialogClosed(CLOSE_ACTION_CANCELED);
}

}  // namespace ohos

