// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_SHELL_DIALOGS_SELECT_FILE_DIALOG_OHOS_H_
#define UI_SHELL_DIALOGS_SELECT_FILE_DIALOG_OHOS_H_

#include <stddef.h>

#include <memory>
#include <set>

#include "base/memory/raw_ptr.h"
#include "base/task/sequenced_task_runner.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/aura/window.h"
#include "ui/shell_dialogs/select_file_dialog.h"
#include "ui/shell_dialogs/select_file_policy.h"
#include "ui/shell_dialogs/shell_dialogs_export.h"

namespace ui {

// Shared implementation SelectFileDialog used on OHOS
class SHELL_DIALOGS_EXPORT SelectFileDialogOHOS : public SelectFileDialog {
 public:
  SelectFileDialogOHOS(const SelectFileDialogOHOS&) = delete;
  SelectFileDialogOHOS& operator=(const SelectFileDialogOHOS&) = delete;

  SelectFileDialogOHOS(Listener* listener,
                       std::unique_ptr<ui::SelectFilePolicy> policy);

  ~SelectFileDialogOHOS() override;

  // BaseShellDialog implementation.
  void ListenerDestroyed() override;

  void CompleteOpen(std::vector<ui::SelectedFileInfo> paths,
                    bool multi_files = false);
  void CancelOpen();

 protected:
  // BaseShellDialog implementation:
  bool IsRunning(gfx::NativeWindow parent_window) const override;

  // SelectFileDialog implementation.
  // |params| is user data we pass back via the Listener interface.
  void SelectFileImpl(Type type,
                      const std::u16string& title,
                      const base::FilePath& default_path,
                      const FileTypeInfo* file_types,
                      int file_type_index,
                      const base::FilePath::StringType& default_extension,
                      gfx::NativeWindow owning_window,
                      const GURL* caller) override;

  bool HasMultipleFileTypeChoicesImpl() override;

  // Wrapper for base::DirectoryExists() that allow access on the UI
  // thread. Use this only in the file dialog functions, where it's ok
  // because the file dialog has to do many stats anyway. One more won't
  // hurt too badly and it's likely already cached.
  bool CallDirectoryExistsOnUIThread(const base::FilePath& path);

  const FileTypeInfo& FileTypes() const { return file_types_; }
  void SetFileTypes(const FileTypeInfo& file_types) {
    file_types_ = file_types;
  }

  size_t FileTypeIndex() const { return file_type_index_; }
  void SetFileTypeIndex(size_t file_type_index) {
    file_type_index_ = file_type_index;
  }

  Type GetType() const { return type_; }
  void SetType(Type type) { type_ = type; }

 private:
  std::vector<ui::SelectedFileInfo> ConvertJSONToStringArray(
      const std::string& json);

  // Removes the DialogInfo parent. Must be called on the UI task runner.
  void UnparentOnMainThread();

  const char* prefix = "file:/";

  // The file filters.
  FileTypeInfo file_types_;

  // The index of the default selected file filter.
  // Note: This starts from 1, not 0.
  size_t file_type_index_ = 0;

  // The type of dialog we are showing the user.
  Type type_ = SELECT_NONE;
};

}  // namespace ui

#endif  // UI_SHELL_DIALOGS_SELECT_FILE_DIALOG_OHOS_H_
