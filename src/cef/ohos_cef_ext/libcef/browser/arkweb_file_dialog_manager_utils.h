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

#ifndef ARKWEB_FILE_DIALOG_MANAGER_UTILS_H_
#define ARKWEB_FILE_DIALOG_MANAGER_UTILS_H_

#include "base/memory/raw_ptr.h"
#include "cef/include/internal/cef_string.h"
#include "cef/ohos_cef_ext/include/arkweb_dialog_handler_ext.h"
#include "third_party/blink/public/mojom/choosers/file_chooser.mojom.h"
#include "ui/shell_dialogs/select_file_dialog.h"
#include "ui/shell_dialogs/select_file_policy.h"
#include "ui/shell_dialogs/selected_file_info.h"

using blink::mojom::FileChooserParams;

class CefFileDialogManager;

class ArkwebFileDialogManagerUtils {
 public:
  friend class CefFileDialogManager;
  explicit ArkwebFileDialogManagerUtils(
      CefFileDialogManager* cef_file_dialog_manager);
  ~ArkwebFileDialogManagerUtils();

  ArkwebFileDialogManagerUtils(const ArkwebFileDialogManagerUtils&) = delete;
  ArkwebFileDialogManagerUtils& operator=(const ArkwebFileDialogManagerUtils&) =
      delete;

  bool CheckDialogValidity();

  std::vector<CefString> ConvertMimeTypesToFilters(
      const blink::mojom::FileChooserParams& params);

  void HandleDialogDestruction();

  void HandleSetFileChooserInActive();

 private:
  const raw_ptr<CefFileDialogManager> cef_file_dialog_manager_;
};

#endif  // ARKWEB_FILE_DIALOG_MANAGER_UTILS_H_
