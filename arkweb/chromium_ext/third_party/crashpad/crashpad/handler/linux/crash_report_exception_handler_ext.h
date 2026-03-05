// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CRASHPAD_HANDLER_LINUX_CRASH_REPORT_EXCEPTION_HANDLER_EXT_H
#define CRASHPAD_HANDLER_LINUX_CRASH_REPORT_EXCEPTION_HANDLER_EXT_H

#include "third_party/crashpad/crashpad/handler/linux/crash_report_exception_handler.h"
#include <map>
#include <string>

#include "base/memory/raw_ptr.h"
#include "client/crash_report_database.h"
#include "handler/crash_report_upload_thread.h"
#include "handler/linux/exception_handler_server.h"
#include "handler/user_stream_data_source.h"
#include "util/linux/exception_handler_protocol.h"
#include "util/linux/ptrace_connection.h"
#include "util/misc/address_types.h"
#include "util/misc/uuid.h"
#include "third_party/crashpad/crashpad/minidump/minidump_user_extension_stream_data_source.h"
#include "base/threading/thread_checker.h"

namespace crashpad {

class ProcessSnapshotLinux;
class ProcessSnapshotSanitized;

//! \brief An exception handler that writes crash reports for exceptions
//!     to a CrashReportDatabase.
class CrashReportExceptionHandlerExt : public ExceptionHandlerServer::Delegate {
 public:
  //! \brief Creates a new object that will store crash reports in \a database.
  //!
  //! \param[in] database The database to store crash reports in. Weak.
  //! \param[in] upload_thread The upload thread to notify when a new crash
  //!     report is written into \a database. Report upload is skipped if this
  //!     value is `nullptr`.
  //! \param[in] process_annotations A map of annotations to insert as
  //!     process-level annotations into each crash report that is written. Do
  //!     not confuse this with module-level annotations, which are under the
  //!     control of the crashing process, and are used to implement Chrome’s
  //!     “crash keys.” Process-level annotations are those that are beyond the
  //!     control of the crashing process, which must reliably be set even if
  //!     the process crashes before it’s able to establish its own annotations.
  //!     To interoperate with Breakpad servers, the recommended practice is to
  //!     specify values for the `"prod"` and `"ver"` keys as process
  //!     annotations.
  //! \param[in] attachments A vector of file paths that should be captured with
  //!     each report at the time of the crash.
  //! \param[in] write_minidump_to_database Whether the minidump shall be
  //!     written to database.
  //! \param[in] write_minidump_to_log Whether the minidump shall be written to
  //!     log.
  //! \param[in] user_stream_data_sources Data sources to be used to extend
  //!     crash reports. For each crash report that is written, the data sources
  //!     are called in turn. These data sources may contribute additional
  //!     minidump streams. `nullptr` if not required.
  CrashReportExceptionHandlerExt() = default;
  CrashReportExceptionHandlerExt(
      CrashReportDatabase* database,
      CrashReportUploadThread* upload_thread,
      const std::map<std::string, std::string>* process_annotations,
      const std::vector<base::FilePath>* attachments,
      bool write_minidump_to_database,
      bool write_minidump_to_log,
      const UserStreamDataSources* user_stream_data_sources);

  CrashReportExceptionHandlerExt(const CrashReportExceptionHandlerExt&) = delete;
  CrashReportExceptionHandlerExt& operator=(const CrashReportExceptionHandlerExt&) =
      delete;

  ~CrashReportExceptionHandlerExt() override;

    bool HandleOverwritten(
      PtraceConnection* connection,
      const ExceptionHandlerProtocol::ClientInformation& info,
      uid_t client_uid,
      VMAddress requesting_thread_stack_address,
      pid_t* requesting_thread_id,
      UUID* local_report_id = nullptr);
};


class OhosDfxDataSource final : public MinidumpUserExtensionStreamDataSource {
 public:
  //! \brief Creates a data source with \a stream_type.
  //!
  //! param[in] stream_type The type of the stream
  //! param[in] data The data of the stream.
  //! param[in] data_size The length of \a data.
  OhosDfxDataSource(uint32_t stream_type, const void* data, size_t data_size);
  OhosDfxDataSource(const OhosDfxDataSource&) = delete;
  OhosDfxDataSource& operator=(const OhosDfxDataSource&) = delete;

  size_t StreamDataSize() override;
  bool ReadStreamData(Delegate* delegate) override;

 private:
  std::vector<uint8_t> data_;
};

class OhosUserStreamDataSource : public UserStreamDataSource {
 public:
  OhosUserStreamDataSource(PtraceConnection* connection):
    connection_(connection) {}

  OhosUserStreamDataSource(const OhosUserStreamDataSource&) = delete;
  OhosUserStreamDataSource& operator=(const OhosUserStreamDataSource&) = delete;

  std::unique_ptr<MinidumpUserExtensionStreamDataSource> ProduceStreamData(
      ProcessSnapshot* process_snapshot) override;

 private:
  raw_ptr<PtraceConnection> connection_;
#if BUILDFLAG(ARKWEB_CRASHPAD)
  base::ThreadChecker thread_checker_;
#endif
};

}  // namespace crashpad

#endif  // CRASHPAD_HANDLER_LINUX_CRASH_REPORT_EXCEPTION_HANDLER_H_
