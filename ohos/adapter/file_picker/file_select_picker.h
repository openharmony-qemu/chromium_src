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

#ifndef OHOS_ADAPTER_FILE_PICKER_FILE_SELECT_PICKER_H_
#define OHOS_ADAPTER_FILE_PICKER_FILE_SELECT_PICKER_H_

#include <napi/native_api.h>

#include <functional>

#include "ohos/adapter/export.h"

namespace ohos::adapter::file_select_picker {

using FileSelectedCallback = std::function<void(const std::string&)>;

struct SelectFileDialogParams {
  bool multi_files;
  std::vector<std::vector<std::string>> extensions;
  std::vector<std::string> descriptions;
  bool include_all_files;
};

struct SaveAsDialogParams {
  std::string file_name;
  std::string dir_name;
  std::vector<std::vector<std::string>> extensions;
  std::vector<std::string> descriptions;
  bool include_all_files;
};

class ADAPTER_EXPORT_API FileSelectPicker {
 public:
  FileSelectPicker() = default;
  ~FileSelectPicker() = default;
  static FileSelectPicker& GetInstance();
  void ShowFilePickerDialog(const SelectFileDialogParams& params,
                            FileSelectedCallback);
  void ShowFilePickerDirDialog(bool file_access_persist, FileSelectedCallback);
  void ShowSaveAsDialog(const SaveAsDialogParams& params, FileSelectedCallback);

 private:
};

}  // namespace ohos::adapter::file_select_picker

#endif  // OHOS_ADAPTER_FILE_SELECT_PICKER_H_
