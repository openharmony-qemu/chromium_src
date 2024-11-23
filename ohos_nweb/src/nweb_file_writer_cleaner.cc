/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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
#include "nweb_file_writer_cleaner.h"
#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>

#include "base/files/file_enumerator.h"
#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/process/process.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "base/time/time.h"
#include "build/build_config.h"

namespace {
static bool g_needsDelete = true;

std::vector<base::FilePath> GetTempraryFiles(base::FilePath directory) {
    std::vector<base::FilePath> files;
    // Temp
    base::FileEnumerator file_enum(
        directory, /*recursive=*/false, base::FileEnumerator::FILES,
        base::FormatTemporaryFileName(FILE_PATH_LITERAL("*")).value());
    for (base::FilePath path = file_enum.Next(); !path.empty();
        path = file_enum.Next()) {
            files.push_back(path);
    }
    // Temp/Download
    directory = directory.AppendASCII("Download");
    if (base::DirectoryExists(directory)) {
        base::FileEnumerator file_download_enum(
            directory, /*recursive=*/false, base::FileEnumerator::FILES,
            base::FormatTemporaryFileName(FILE_PATH_LITERAL("*")).value());
        for (base::FilePath path = file_download_enum.Next(); !path.empty();
            path = file_download_enum.Next()) {
            files.push_back(path);
        }
    }

    return files;
}
} // namespace

namespace OHOS::NWeb {
std::vector<base::FilePath> NwebFileWriterCleaner::pending_files_ = {};

// static
void NwebFileWriterCleaner::CleanInBackground(
    std::vector<base::FilePath> pending_files) {
    for (auto file: pending_files) {
        DeleteFile(file);
        LOG(INFO) << "CleanInBackground delete file: " << file;
    }
}

// static
void NwebFileWriterCleaner::OnBackgroundTaskFinished() {
    LOG(INFO) << "OnBackgroundTaskFinished.";
}

// static
void NwebFileWriterCleaner::GetDeletePendingFiles() {
    LOG(INFO) << "GetDeletePendingFiles.";
    // Get delete path
    base::FilePath temp_dir;
    if (!base::GetTempDir(&temp_dir)) {
        LOG(ERROR) << "Failed to find the temporary directory.";
        return;
    }

    NwebFileWriterCleaner::pending_files_ = GetTempraryFiles(temp_dir);
    LOG(INFO) << "GetDeletePendingFiles get pending_files end.";
}

// static
void NwebFileWriterCleaner::DeleteDownloadTempDir() {
    LOG(INFO) << "DeleteDownloadTempDir.";
    if (!g_needsDelete) {
        LOG(INFO) << "DeleteDownloadTempDir g_needsDelete is false.";
        return;
    }

    g_needsDelete = false;
    // Start asynchronous deletion
    base::ThreadPool::PostTaskAndReply(
        FROM_HERE,
        {base::TaskPriority::BEST_EFFORT, base::TaskShutdownBehavior::CONTINUE_ON_SHUTDOWN,
            base::MayBlock()},
        base::BindOnce(&NwebFileWriterCleaner::CleanInBackground,
                       std::move(NwebFileWriterCleaner::pending_files_)),
        // Unretained is safe here since the cleaner instance is never destroyed.
        base::BindOnce(&NwebFileWriterCleaner::OnBackgroundTaskFinished));
}

} // OHOS::NWeb