// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_FILE_PICKER_FILE_SELECT_PICKER_H_
#define OHOS_ADAPTER_FILE_PICKER_FILE_SELECT_PICKER_H_

#include <napi/native_api.h>

#include <functional>

#include "ohos/adapter/export.h"

namespace ohos::adapter::file_select_picker {

class ADAPTER_EXPORT_API FileSelectPicker {
 public:
  using FileSelectedCallback = std::function<void(const std::string&)>;

  FileSelectPicker() = default;
  ~FileSelectPicker() = default;
  static FileSelectPicker& GetInstance();
  void ShowFilePickerDialog(
      bool multi_files,
      const std::vector<std::vector<std::string>>& extensions,
      FileSelectedCallback);
  void ShowFilePickerDirDialog(bool file_access_persist, FileSelectedCallback);
  void ShowSaveAsDialog(const std::string& file_name,
                        const std::string& dir_name,
                        const std::vector<std::vector<std::string>>& extensions,
                        FileSelectedCallback);

 private:
  FileSelectedCallback fileSelectCallback_;
};

}  // namespace ohos::adapter::file_select_picker

#endif  // OHOS_ADAPTER_FILE_SELECT_PICKER_H_
