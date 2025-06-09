/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ohos/adapter/file_picker/file_select_picker.h"

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::file_select_picker {

FileSelectPicker& FileSelectPicker::GetInstance() {
  static FileSelectPicker helper;
  return helper;
}

void FileSelectPicker::ShowFilePickerDialog(
    const SelectFileDialogParams& params,
    FileSelectedCallback callback) {
  auto func =
      ohos::adapter::GetJSFunction("FilePickerAdapter.ShowDocumentViewPicker");
  if (func) {
    func->Invoke<void>(params, callback);
  }
}

void FileSelectPicker::ShowFilePickerDirDialog(bool file_access_persist,
                                               FileSelectedCallback callback) {
  auto func = ohos::adapter::GetJSFunction(
      "FilePickerAdapter.ShowDirDocumentViewPicker");
  if (func) {
    func->Invoke<void>(file_access_persist, callback);
  }
}

void FileSelectPicker::ShowSaveAsDialog(const SaveAsDialogParams& params,
                                        FileSelectedCallback callback) {
  auto func = ohos::adapter::GetJSFunction(
      "FilePickerAdapter.ShowSaveAsDocumentViewPicker");
  if (func) {
    func->Invoke<void>(params, callback);
  }
}

JSBIND_CLASS(SelectFileDialogParams) {
  JSBIND_CONSTRUCTOR<>();
  JSBIND_PROPERTY(multi_files);
  JSBIND_PROPERTY(extensions);
  JSBIND_PROPERTY(descriptions);
  JSBIND_PROPERTY(include_all_files);
}

JSBIND_CLASS(SaveAsDialogParams) {
  JSBIND_CONSTRUCTOR<>();
  JSBIND_PROPERTY(file_name);
  JSBIND_PROPERTY(dir_name);
  JSBIND_PROPERTY(extensions);
  JSBIND_PROPERTY(descriptions);
  JSBIND_PROPERTY(include_all_files);
}

}  // namespace ohos::adapter::file_select_picker
