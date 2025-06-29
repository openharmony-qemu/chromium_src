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

#include "arkweb/chromium_ext/third_party/crashpad/crashpad/handler/linux/crash_report_exception_handler_utils.h"
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
using namespace crashpad;

void CrashReportExceptionHandlerUtils::InitExtendedUserStream(
  PtraceConnection* connection,
  UserStreamDataSources* extendedUserStream)
{
#if BUILDFLAG(ARKWEB_CRASHPAD)
  if (extendedUserStream == nullptr) {
    return;
  }
  extendedUserStream->push_back(
    std::make_unique<OhosUserStreamDataSource>(connection));
#endif
}