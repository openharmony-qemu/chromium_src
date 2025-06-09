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

#include "ui/shell_dialogs/select_file_dialog_ohos.h"

#include <json/json.h>
#include <vector>

#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/notreached.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "base/threading/thread_restrictions.h"
#include "base/values.h"
#include "content/public/browser/browser_thread.h"
#include "ohos/adapter/file_picker/file_select_picker.h"
#include "ui/shell_dialogs/selected_file_info.h"

using namespace ohos::adapter::file_select_picker;

namespace ui {

SelectFileDialogOHOS::SelectFileDialogOHOS(
    Listener* listener,
    std::unique_ptr<ui::SelectFilePolicy> policy)
    : SelectFileDialog(listener, std::move(policy)) {}

SelectFileDialogOHOS::~SelectFileDialogOHOS() = default;

void SelectFileDialogOHOS::CompleteOpen(std::vector<ui::SelectedFileInfo> paths,
                                        bool multi_files) {
  UnparentOnMainThread();
  if (listener_) {
    if (multi_files) {
      listener_->MultiFilesSelected(paths);
    } else {
      listener_->FileSelected(paths.front(), 1);
    }
  }
}

void SelectFileDialogOHOS::CancelOpen() {
  UnparentOnMainThread();
  if (listener_) {
    listener_->FileSelectionCanceled();
  }
}

bool SelectFileDialogOHOS::IsRunning(gfx::NativeWindow parent_window) const {
  return false;
}

void SelectFileDialogOHOS::ListenerDestroyed() {
  listener_ = nullptr;
}

bool SelectFileDialogOHOS::CallDirectoryExistsOnUIThread(
    const base::FilePath& path) {
  base::ScopedAllowBlocking scoped_allow_blocking;
  return base::DirectoryExists(path);
}

void SelectFileDialogOHOS::SelectFileImpl(
    Type type,
    const std::u16string& title,
    const base::FilePath& default_path,
    const FileTypeInfo* file_types,
    int file_type_index,
    const base::FilePath::StringType& default_extension,
    gfx::NativeWindow owning_window,
    const GURL* caller) {
  bool multi_files = (type == Type::SELECT_OPEN_MULTI_FILE) ? true : false;
  auto callback = [this, multi_files](const std::string& path) {
    std::vector<ui::SelectedFileInfo> paths = ConvertJSONToStringArray(path);
    auto task = paths.size() ?
                  base::BindOnce(&SelectFileDialogOHOS::CompleteOpen, this, paths, multi_files) :
                  base::BindOnce(&SelectFileDialogOHOS::CancelOpen, this);
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE, std::move(task));
  };
  std::vector<std::string> descriptions;
  for (const std::u16string& u16_description :
       file_types->extension_description_overrides) {
    std::string description = base::UTF16ToUTF8(u16_description);
    descriptions.push_back(description);
  }
  switch (type) {
    case SELECT_FOLDER:
    case SELECT_UPLOAD_FOLDER:
    case SELECT_EXISTING_FOLDER:
      FileSelectPicker::GetInstance().ShowFilePickerDirDialog(
          file_types->file_access_persist, callback);
      break;
    case SELECT_OPEN_FILE:
    case SELECT_OPEN_MULTI_FILE: {
      SelectFileDialogParams select_params;
      select_params.multi_files = (type == SELECT_OPEN_MULTI_FILE);
      select_params.extensions = std::move(file_types->extensions);
      select_params.descriptions = std::move(descriptions);
      select_params.include_all_files = file_types->include_all_files;
      FileSelectPicker::GetInstance().ShowFilePickerDialog(select_params, callback);
      break;
    }
    case SELECT_SAVEAS_FILE: {
      SaveAsDialogParams save_as_params;
      save_as_params.file_name = default_path.BaseName().AsUTF8Unsafe();
      save_as_params.dir_name = default_path.DirName().AsUTF8Unsafe();
      save_as_params.extensions = std::move(file_types->extensions);
      save_as_params.descriptions = std::move(descriptions);
      save_as_params.include_all_files = file_types->include_all_files;
      FileSelectPicker::GetInstance().ShowSaveAsDialog(save_as_params, callback);
      break;
    }
    case SELECT_NONE:
      NOTREACHED();
      break;
    default:
      break;
  }
}

void SelectFileDialogOHOS::UnparentOnMainThread() {}

std::vector<ui::SelectedFileInfo> SelectFileDialogOHOS::ConvertJSONToStringArray(
    const std::string& json) {
  Json::Reader reader;
  Json::Value root;
  bool parsing_successful = reader.parse(json, root);
  if (!parsing_successful || !root.isArray()) {
    return {};
  }
  std::vector<ui::SelectedFileInfo> selected_files;
  for (const auto& value : root) {
    if (value.isString()) {
      std::string path = value.asString();
      int pos = path.find(prefix);
      if (pos == 0) {
        path = path.substr(strlen(prefix));
      }
      ui::SelectedFileInfo info = ui::SelectedFileInfo(base::FilePath(path));
      selected_files.emplace_back(info);    }
  }
  return selected_files;
}

bool SelectFileDialogOHOS::HasMultipleFileTypeChoicesImpl() {
  return false;
}
}  // namespace ui
