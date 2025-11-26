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

#include "arkweb/chromium_ext/third_party/crashpad/crashpad/handler/linux/crash_report_exception_handler_ext.h"
#include <memory>
#include <utility>

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "build/build_config.h"
#include "client/settings.h"
#include "handler/linux/capture_snapshot.h"
#include "minidump/minidump_file_writer.h"

#if BUILDFLAG(ARKWEB_CRASHPAD)
#include "minidump/minidump_user_extension_stream_data_source.h"
#include "securec.h"
#endif

#include "snapshot/linux/process_snapshot_linux.h"
#include "snapshot/sanitized/process_snapshot_sanitized.h"
#include "util/file/file_helper.h"
#include "util/file/file_reader.h"
#include "util/file/output_stream_file_writer.h"
#include "util/linux/direct_ptrace_connection.h"
#include "util/linux/ptrace_client.h"
#include "util/misc/implicit_cast.h"
#include "util/misc/metrics.h"
#include "util/misc/uuid.h"
#include "util/stream/base94_output_stream.h"
#include "util/stream/log_output_stream.h"
#include "util/stream/zlib_output_stream.h"

#if BUILDFLAG(IS_ANDROID)
#include <android/log.h>
#endif

#if BUILDFLAG(ARKWEB_CRASHPAD)
using namespace crashpad;
OhosDfxDataSource::OhosDfxDataSource(uint32_t stream_type,
                                     const void* data,
                                     size_t data_size)
    : MinidumpUserExtensionStreamDataSource(stream_type)
{
  data_.resize(data_size);
  if (data_size) {
    if (memcpy_s(data_.data(), data_size, data, data_size)) {
        LOG(ERROR) << "[OhosDfxDataSource] memcpy failed";
    }
  }
}

size_t OhosDfxDataSource::StreamDataSize()
{
  return data_.size();
}

bool OhosDfxDataSource::ReadStreamData(Delegate* delegate)
{
  if (delegate == nullptr) {
    return false;
  }
  return delegate->ExtensionStreamDataSourceRead(
      data_.size() ? data_.data() : nullptr, data_.size());
}

std::unique_ptr<MinidumpUserExtensionStreamDataSource>
OhosUserStreamDataSource::ProduceStreamData(ProcessSnapshot* process_snapshot)
{
  std::string contents;
  char path[32];
  sprintf_s(path, sizeof(path), "/proc/%d/maps", connection_->GetProcessID());
  if (!connection_->ReadFileContents(base::FilePath(path), &contents)) {
    contents = "read maps failed!!";
  }
  return std::make_unique<OhosDfxDataSource>(
      KMinidumpStreamTypeOhosDfxInfo, contents.c_str(), contents.size());
}
#endif
