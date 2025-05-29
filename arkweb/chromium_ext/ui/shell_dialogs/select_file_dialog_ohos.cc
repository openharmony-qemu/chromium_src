// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
#include "ui/shell_dialogs/selected_file_info.h"

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
  switch (type) {
    case SELECT_FOLDER:
    case SELECT_UPLOAD_FOLDER:
    case SELECT_EXISTING_FOLDER:
      break;
    case SELECT_OPEN_FILE:
      break;
    case SELECT_OPEN_MULTI_FILE:
      break;
    case SELECT_SAVEAS_FILE: {
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
