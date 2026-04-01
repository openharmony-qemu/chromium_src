// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// File method ordering: Methods in this file are in the same order as
// in download_item_impl.h, with the following exception: The public
// interface Start is placed in chronological order with the other
// (private) routines that together define a DownloadItem's state
// transitions as the download progresses.  See "Download progression
// cascade" later in this file.

// A regular DownloadItem (created for a download in this session of
// the browser) normally goes through the following states:
//      * Created (when download starts)
//      * Destination filename determined
//      * Entered into the history database.
//      * Made visible in the download shelf.
//      * All the data is saved.  Note that the actual data download occurs
//        in parallel with the above steps, but until those steps are
//        complete, the state of the data save will be ignored.
//      * Download file is renamed to its final name, and possibly
//        auto-opened.

#include "components/download/public/common/download_item_impl.h"
#include "arkweb/chromium_ext/components/download/internal/common/arkweb_download_item_impl_ext.h"

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif
#include "base/files/file_util.h"
#include "base/format_macros.h"
#include "base/functional/bind.h"
#include "base/json/string_escape.h"
#include "base/logging.h"
#include "base/metrics/histogram_functions.h"
#include "base/metrics/histogram_macros.h"
#include "base/observer_list.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/sequenced_task_runner.h"
#include "base/trace_event/memory_usage_estimator.h"
#include "base/trace_event/trace_event.h"
#include "base/uuid.h"
#include "build/build_config.h"
#include "components/download/internal/common/download_job_impl.h"
#include "components/download/internal/common/parallel_download_utils.h"
#include "components/download/public/common/download_danger_type.h"
#include "components/download/public/common/download_features.h"
#include "components/download/public/common/download_file.h"
#include "components/download/public/common/download_interrupt_reasons.h"
#include "components/download/public/common/download_item_impl_delegate.h"
#include "components/download/public/common/download_item_rename_handler.h"
#include "components/download/public/common/download_job_factory.h"
#include "components/download/public/common/download_stats.h"
#include "components/download/public/common/download_task_runner.h"
#include "components/download/public/common/download_ukm_helper.h"
#include "components/download/public/common/download_url_parameters.h"
#include "components/download/public/common/download_utils.h"
#include "net/base/network_change_notifier.h"
#include "net/http/http_response_headers.h"
#include "net/http/http_status_code.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "net/url_request/referrer_policy.h"
#include "services/metrics/public/cpp/ukm_source_id.h"

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#endif

namespace download {

namespace {
void ReadDownloadDataAndRunCallbackImpl(DownloadFile* download_file) {
  DCHECK(GetDownloadTaskRunner()->RunsTasksInCurrentSequence());
  ArkWebDownloadFileExt* download_file_ext =
      download_file->AsArkWebDownloadFileImplExt();
 
  download_file_ext->ReadDownloadDataAndRunCallback(
      download_file_ext->GetNoHoleDownloadDataSize());
}
 
void ReadDataFromDownloadFile(
    DownloadFile* download_file,
    int32_t read_size,
    bool read_now,
    ArkWebDownloadFileExt::ReadDownloadDataCallback callback) {
  DCHECK(GetDownloadTaskRunner()->RunsTasksInCurrentSequence());
  ArkWebDownloadFileExt* download_file_ext =
      download_file->AsArkWebDownloadFileImplExt();
  download_file_ext->RegisterReadDownloadCallback(std::move(callback),
                                                  read_size);
  if (read_now) {
    ReadDownloadDataAndRunCallbackImpl(download_file);
  } else {
    download_file_ext->ReadAndRunCallbackIfDataReady();
  }
}
}  // namespace

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
const std::string& ArkWebDownloadItemImplExt::GetRequestMethod() const {
  return request_method_;
}
#endif

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
ArkWebDownloadItemImplExt::~ArkWebDownloadItemImplExt() {
  LOG(DEBUG) << "ArkWebDownloadItemImplExt::~ArkWebDownloadItemImplExt";
  RunCallbackIfExistsCallback();
}

bool ArkWebDownloadItemImplExt::IsAllowedAutoResume() {
  switch (last_reason_) {
    case DOWNLOAD_INTERRUPT_REASON_SERVER_NO_RANGE:
    case DOWNLOAD_INTERRUPT_REASON_FILE_HASH_MISMATCH:
    case DOWNLOAD_INTERRUPT_REASON_FILE_TOO_SHORT:
    case DOWNLOAD_INTERRUPT_REASON_NETWORK_SERVER_DOWN:
    case DOWNLOAD_INTERRUPT_REASON_SERVER_UNREACHABLE:
    case DOWNLOAD_INTERRUPT_REASON_CRASH:
    case DOWNLOAD_INTERRUPT_REASON_SERVER_FAILED:
    case DOWNLOAD_INTERRUPT_REASON_FILE_NO_SPACE:
    case DOWNLOAD_INTERRUPT_REASON_FILE_ACCESS_DENIED:
    case DOWNLOAD_INTERRUPT_REASON_FILE_NAME_TOO_LONG:
    case DOWNLOAD_INTERRUPT_REASON_FILE_TOO_LARGE:
      return false;
    default:
      return true;
  }
}

void ArkWebDownloadItemImplExt::IsNeedAutoResume(
    DownloadInterruptReason reason, ResumeMode& resume_mode, bool& need_auto_resume) {
  if (CheckIsNeedAutoResume(reason)) {
    LOG(INFO) << "ArkWebDownloadItemImplExt::CheckIsNeedAutoResume last_reason_: "
              << last_reason_
              << ", auto_resume_count_: " << auto_resume_count_
              << ", state_: " << DebugDownloadStateString(state_)
              << ", guid: " << GetGuid() << ", isPause: " << IsPaused()
              << ", is cancel: " << CallIsCancellation(reason);
    resume_mode = ResumeMode::IMMEDIATE_CONTINUE;
    need_auto_resume = true;
  }
  LOG(INFO) << "DownloadItemImpl::InterruptWithPartialState "
            << ", guid: " << GetGuid()
            << ", need_auto_resume: " << need_auto_resume
            << ", path: ***";
}

bool ArkWebDownloadItemImplExt::CheckIsNeedAutoResume(DownloadInterruptReason reason) {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebExDownload) &&
      (state_ == TARGET_RESOLVED_INTERNAL || state_ == IN_PROGRESS_INTERNAL) &&
      !IsPaused() && !CallIsCancellation(reason) && IsAllowedAutoResume() &&
      auto_resume_count_ <= kMaxAutoResumeAttempts) {
    return true;
  }
  return false;
}

void ArkWebDownloadItemImplExt::AutoResume() {
  DVLOG(20) << __func__ << "() " << DebugString(true);
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);

  auto_resume_count_++;
  ResumeInterruptedDownload(ResumptionRequestSource::AUTOMATIC);
}
#endif  // BUILDFLAG(ARKWEB_EXT_DOWNLOAD)

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
bool ArkWebDownloadItemImplExt::IsBeforeInProgress() const {
  switch (state_) {
    case INITIAL_INTERNAL:
    case TARGET_PENDING_INTERNAL:
    case TARGET_RESOLVED_INTERNAL:
    case INTERRUPTED_TARGET_PENDING_INTERNAL:
      return true;
    default:
      return false;
  }
}

bool ArkWebDownloadItemImplExt::IsDownloadInProgress() const {
  if (state_ == COMPLETE_INTERNAL || state_ == COMPLETING_INTERNAL ||
      state_ == INTERRUPTED_INTERNAL ||
      state_ == INTERRUPTED_TARGET_PENDING_INTERNAL ||
      state_ == CANCELLED_INTERNAL) {
    return false;
  }
 
  return true;
}
 
void ArkWebDownloadItemImplExt::ReadDownloadData(
    const std::string& guid,
    const int32_t read_size,
    base::OnceCallback<void(const std::vector<uint8_t>&)> callback) {
  LOG(INFO) << "DownloadItemImpl::ReadDownloadData " << state_;
  if (read_download_callback_from_ui_) {
    LOG(INFO) << "ReadDownloadData exsit callback and return";
    return;
  }
  read_download_callback_from_ui_ = std::move(callback);
  if (!download_file_) {
    LOG(INFO) << "ReadDownloadData download_file null";
    ReadDataFromDownloadFileDone(std::vector<uint8_t>());
    return;
  }
 
  if (GetDownloadTaskRunner()) {
    bool read_now = !IsDownloadInProgress() || PercentComplete() == 100;
    // Safe because we control download file lifetime.
    GetDownloadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(
            &ReadDataFromDownloadFile, base::Unretained(download_file_.get()),
            read_size, read_now,
            base::BindOnce(
                &ArkWebDownloadItemImplExt::ReadDataFromDownloadFileDone,
                weak_ptr_factory_.GetWeakPtr())));
  }
}

void ArkWebDownloadItemImplExt::ReadDataFromDownloadFileDone(
    const std::vector<uint8_t>& data) {
  if (!read_download_callback_from_ui_) {
    return;
  }

  std::move(read_download_callback_from_ui_).Run(std::move(data));
}

void ArkWebDownloadItemImplExt::RunCallbackIfStateMatch() {
  if (!read_download_callback_from_ui_ || IsDownloadInProgress() ||
      !download_file_) {
    return;
  }
 
  LOG(INFO) << "DownloadItemImpl::RunCallbackIfStateMatch";
 
  if (GetDownloadTaskRunner()) {
    // Safe because we control download file lifetime.
    GetDownloadTaskRunner()->PostTask(
        FROM_HERE, base::BindOnce(&ReadDownloadDataAndRunCallbackImpl,
                                  base::Unretained(download_file_.get())));
  }
}

void ArkWebDownloadItemImplExt::RunCallbackIfExistsCallback() {
  if (!read_download_callback_from_ui_) {
    return;
  }
  if (!download_file_) {
    ReadDataFromDownloadFileDone(std::vector<uint8_t>());
    return;
  }
  if (GetDownloadTaskRunner()) {
    // Safe because we control download file lifetime.
    GetDownloadTaskRunner()->PostTask(
        FROM_HERE, base::BindOnce(&ReadDownloadDataAndRunCallbackImpl,
                                  base::Unretained(download_file_.get())));
  }
}

void ArkWebDownloadItemImplExt::Cancel(bool user_cancel) {
  DownloadItemImpl::Cancel(user_cancel);
  ReadDataFromDownloadFileDone(std::vector<uint8_t>());
}

void ArkWebDownloadItemImplExt::UpdateFullPath(
    const base::FilePath& new_path) {
  destination_info_.current_path = new_path;
}

void ArkWebDownloadItemImplExt::UpdateFullPathOverWriteState(
    bool overwrite_state) {
  is_full_path_overwrite_ = overwrite_state;
}

bool ArkWebDownloadItemImplExt::IsFullPathOverWrite() const {
  return is_full_path_overwrite_;
} 

void ArkWebDownloadItemImplExt::SetByExtensionId(
    const std::string& extension_id) {
  extension_id_ = extension_id;
}

void ArkWebDownloadItemImplExt::SetByExtensionName(
    const std::string& extension_name) {
  extension_name_ = extension_name;
}

void ArkWebDownloadItemImplExt::SetConflictAction(int conflict_action) {
  conflict_action_ = conflict_action;
}

std::string ArkWebDownloadItemImplExt::GetByExtensionId() const {
  return extension_id_;
}

std::string ArkWebDownloadItemImplExt::GetByExtensionName() const {
  return extension_name_;
}

int ArkWebDownloadItemImplExt::GetConflictAction() const {
  return conflict_action_;
}

#endif  //  ARKWEB_EXT_DOWNLOAD

}  // namespace download
