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
#include <gmock/gmock.h>

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
#include "gtest/gtest.h"

using namespace OHOS::NWeb;

TEST(NwebFileWriterCleanerTEST, CleanInBackground) {
    std::vector<base::FilePath> pending_files;
    base::FilePath file;
    pending_files.push_back(file);
    NwebFileWriterCleaner::CleanInBackground(pending_files);
}


TEST(NwebFileWriterCleanerTEST, OnBackgroundTaskFinished) {
    NwebFileWriterCleaner::OnBackgroundTaskFinished();
}


TEST(NwebFileWriterCleanerTEST, GetDeletePendingFiles) {
    NwebFileWriterCleaner::GetDeletePendingFiles();
}

TEST(NwebFileWriterCleanerTEST, DeleteDownloadTempDir) {
    NwebFileWriterCleaner::DeleteDownloadTempDir();
    NwebFileWriterCleaner::DeleteDownloadTempDir();
}
