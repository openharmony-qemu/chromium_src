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

#include "cef/ohos_cef_ext/libcef/browser/arkweb_file_dialog_manager_utils.h"

#include "cef/libcef/browser/browser_host_base.h"
#include "cef/libcef/browser/file_dialog_manager.h"

using blink::mojom::FileChooserParams;

ArkwebFileDialogManagerUtils::ArkwebFileDialogManagerUtils(
    CefFileDialogManager* cef_file_dialog_manager)
    : cef_file_dialog_manager_(cef_file_dialog_manager) {
  DCHECK(cef_file_dialog_manager);
}

ArkwebFileDialogManagerUtils::~ArkwebFileDialogManagerUtils() = default;

bool ArkwebFileDialogManagerUtils::CheckDialogValidity() {
#if BUILDFLAG(ARKWEB_BUGFIX_CRASH)
  if (!cef_file_dialog_manager_->dialog_) {
    LOG(ERROR) << "app hasn't onFileSelectShow event, return";
    return false;
  }
#endif
  return true;
}

std::vector<CefString> ArkwebFileDialogManagerUtils::ConvertMimeTypesToFilters(
    const blink::mojom::FileChooserParams& params) {
  std::vector<CefString> mime_filters;
  for (const auto& mime_type : params.mime_types) {
    mime_filters.push_back(mime_type);
  }
  return mime_filters;
}

void ArkwebFileDialogManagerUtils::HandleDialogDestruction() {
  if (cef_file_dialog_manager_->dialog_ != nullptr) {
    // There should be no further listener callbacks after this call.
    cef_file_dialog_manager_->dialog_->ListenerDestroyed();
    cef_file_dialog_manager_->dialog_ = nullptr;
  }
}

void ArkwebFileDialogManagerUtils::HandleSetFileChooserInActive() {
  if (cef_file_dialog_manager_->browser_->GetWebContents()) {
    cef_file_dialog_manager_->browser_->GetWebContents()->SetFileChooserInActive();
  }
}
