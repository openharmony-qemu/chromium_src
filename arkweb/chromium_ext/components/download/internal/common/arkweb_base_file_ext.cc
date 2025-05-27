/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifdef UNSAFE_BUFFERS_BUILD
// TODO(crbug.com/40285824): Remove this and convert code to safer constructs.
#pragma allow_unsafe_buffers
#endif

#include <memory>
#include <utility>

#include "base/containers/heap_array.h"
#include "base/files/file.h"
#include "base/files/file_util.h"
#include "base/format_macros.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/metrics/histogram_functions.h"
#include "base/numerics/safe_conversions.h"
#include "base/pickle.h"
#include "base/strings/stringprintf.h"
#include "base/threading/thread_restrictions.h"
#include "base/trace_event/trace_event.h"
#include "build/build_config.h"
#include "components/download/public/common/base_file.h"
#include "components/download/public/common/download_interrupt_reasons_utils.h"
#include "components/download/public/common/download_item.h"
#include "components/download/public/common/download_stats.h"
#include "components/services/quarantine/quarantine.h"
#include "crypto/secure_hash.h"

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
#include "third_party/bounds_checking_function/include/securec.h"
#endif

namespace download {

ArkWebBaseFileExt::ArkWebBaseFileExt(uint32_t download_id)
    : BaseFile(download_id) {}

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
bool ArkWebBaseFileExt::ReadDataFromFile(int64_t offset,
                                         char* data,
                                         size_t data_len,
                                         const base::FilePath& file_path) {
  LOG(DEBUG) << "BaseFile::ReadDataFromFile file_path: " << file_path;

  bool re_open_flag = false;
  if (!file_.IsValid()) {
    LOG(INFO) << "BaseFile::ReadDataFromFile file not valid";
    InitializeFile(&file_, file_path);
    if (!file_.IsValid()) {
      LOG(INFO) << "BaseFile::ReadDataFromFile file initialize fail";
      return false;
    } else {
      re_open_flag = true;
    }
  }

  // Only validate the first chunk of the file. So |offset| cannot be
  // larger than bytes received.
  if (offset > bytes_so_far_) {
    LOG(ERROR) << "BaseFile::ReadDataFromFile offset:" << offset
               << ", bytes_so_far_:" << bytes_so_far_;
    if (re_open_flag) {
      file_.Close();
    }
    return false;
  }

  if (data_len <= 0) {
    if (re_open_flag) {
      file_.Close();
    }
    return true;
  }

  file_.Flush();

  std::unique_ptr<char[]> buffer(new char[data_len]);
  int bytes_read = file_.Read(offset, buffer.get(), data_len);

  if (bytes_read < 0 || static_cast<size_t>(bytes_read) < data_len) {
    LOG(ERROR) << "BaseFile::ReadDataFromFile bytes_read: " << bytes_read
               << ", data_len: " << data_len;
    if (re_open_flag) {
      file_.Close();
    }
    return false;
  }

  if (memcpy_s(data, data_len, buffer.get(), data_len) != EOK) {
    LOG(INFO) << "BaseFile::ReadDataFromFile memcpy fail";
    if (re_open_flag) {
      file_.Close();
    }
    return false;
  }

  if (re_open_flag) {
    file_.Close();
  }
  return true;
}

base::FilePath ArkWebBaseFileExt::GetSaveDirectory(
    const base::FilePath& default_directory) {
  base::FilePath save_directory;
  if (default_directory.empty()) {
    base::GetTempDir(&save_directory);
    save_directory = save_directory.AppendASCII("Download");
    if (!base::DirectoryExists(save_directory)) {
      base::CreateDirectory(save_directory);
    }
  } else {
    save_directory = default_directory;
  }
  return save_directory;
}
#endif

}  // namespace download
