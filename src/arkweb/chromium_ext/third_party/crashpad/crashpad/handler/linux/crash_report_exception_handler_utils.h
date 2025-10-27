// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CRASHPAD_HANDLER_LINUX_CRASH_REPORT_EXCEPTION_HANDLER_UTILS_H
#define CRASHPAD_HANDLER_LINUX_CRASH_REPORT_EXCEPTION_HANDLER_UTILS_H

#include "third_party/crashpad/crashpad/handler/linux/crash_report_exception_handler.h"
#include <map>
#include <string>

#include "client/crash_report_database.h"
#include "handler/crash_report_upload_thread.h"
#include "handler/linux/exception_handler_server.h"
#include "handler/user_stream_data_source.h"
#include "util/linux/exception_handler_protocol.h"
#include "util/linux/ptrace_connection.h"
#include "util/misc/address_types.h"
#include "util/misc/uuid.h"
#include "third_party/crashpad/crashpad/minidump/minidump_user_extension_stream_data_source.h"

namespace crashpad {

class ProcessSnapshotLinux;
class ProcessSnapshotSanitized;

//! \brief An exception handler that writes crash reports for exceptions
//!     to a CrashReportDatabase.
class CrashReportExceptionHandlerUtils {
public:

CrashReportExceptionHandlerUtils() = default;

~CrashReportExceptionHandlerUtils();

CrashReportExceptionHandlerUtils(const CrashReportExceptionHandlerUtils&) =
      delete;
CrashReportExceptionHandlerUtils& operator=(
      const CrashReportExceptionHandlerUtils&) = delete;

static void InitExtendedUserStream(
  PtraceConnection* connection,
  UserStreamDataSources* extendedUserStream);
};
}
#endif  // CRASHPAD_HANDLER_LINUX_CRASH_REPORT_EXCEPTION_HANDLER_H_