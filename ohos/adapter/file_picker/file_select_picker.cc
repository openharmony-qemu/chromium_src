// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/file_picker/file_select_picker.h"

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::file_select_picker {

FileSelectPicker& FileSelectPicker::GetInstance() {
  static FileSelectPicker helper;
  return helper;
}

void FileSelectPicker::ShowFilePickerDialog(
    bool multi_files,
    const std::vector<std::vector<std::string>>& extensions,
    FileSelectPicker::FileSelectedCallback callback) {
  auto func =
      ohos::adapter::GetJSFunction("FilePickerAdapter.ShowDocumentViewPicker");
  if (func) {
    func->Invoke<void>(multi_files, extensions, callback);
  }
}

void FileSelectPicker::ShowFilePickerDirDialog(
    bool file_access_persist,
    FileSelectPicker::FileSelectedCallback callback) {
  auto func = ohos::adapter::GetJSFunction(
      "FilePickerAdapter.ShowDirDocumentViewPicker");
  if (func) {
    func->Invoke<void>(file_access_persist, callback);
  }
}

void FileSelectPicker::ShowSaveAsDialog(
    const std::string& file_name,
    const std::string& dir_name,
    const std::vector<std::vector<std::string>>& extensions,
    FileSelectPicker::FileSelectedCallback callback) {
  auto func = ohos::adapter::GetJSFunction(
      "FilePickerAdapter.ShowSaveAsDocumentViewPicker");
  if (func) {
    func->Invoke<void>(file_name, dir_name, extensions, callback);
  }
}

}  // namespace ohos::adapter::file_select_picker
