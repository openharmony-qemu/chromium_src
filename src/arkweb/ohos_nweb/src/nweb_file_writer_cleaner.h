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

#ifndef NWEB_FILE_WRITER_CLEANER_H
#define NWEB_FILE_WRITER_CLEANER_H

#include <atomic>
#include <vector>

#include "base/base_export.h"
#include "base/containers/flat_set.h"
#include "base/files/file_path.h"
#include "base/memory/scoped_refptr.h"
#include "base/no_destructor.h"
#include "base/numerics/clamped_math.h"
#include "base/sequence_checker.h"
#include "base/synchronization/lock.h"
#include "base/task/sequenced_task_runner.h"
#include "base/thread_annotations.h"

namespace OHOS::NWeb {

class NwebFileWriterCleaner {
 public:
  static void CleanInBackground(std::vector<base::FilePath> directories);
  static void OnBackgroundTaskFinished();
  static void DeleteDownloadTempDir();
  static void GetDeletePendingFiles();

  static std::vector<base::FilePath> pending_files_;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_FILE_WRITER_CLEANER_H
