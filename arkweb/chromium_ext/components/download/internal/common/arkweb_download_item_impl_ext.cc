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

#include "arkweb/ohos_nweb_ex/build/features/features.h"
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

#if BUILDFLAG(IS_ANDROID)
#include "components/download/internal/common/android/download_collection_bridge.h"
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_MAC)
#include "base/mac/mac_util.h"
#endif  // BUILDFLAG(IS_MAC)

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#endif

namespace download {

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
const std::string& ArkWebDownloadItemImplExt::GetRequestMethod() const {
  return request_method_;
}
#endif

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
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
            << ", path: " << GetFullPath();
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

void ArkWebDownloadItemImplExt::ReadDownloadData(
    const std::string& guid,
    const int32_t read_size,
    base::OnceCallback<void(const std::vector<uint8_t>&)> callback) {
  if (GetDownloadTaskRunner()) {
    GetDownloadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(&ArkWebDownloadItemImplExt::ReadDownloadDataInternal,
        weak_ptr_factory_.GetWeakPtr(), guid, read_size, std::move(callback)));
  }
}

void ArkWebDownloadItemImplExt::ReadDownloadDataInternal(
    const std::string& guid,
    const int32_t read_size,
    base::OnceCallback<void(const std::vector<uint8_t>&)> callback) {
  if (!read_download_callback_from_ui_) {
    LOG(INFO) << "ArkWebDownloadItemImplExt::ReadDownloadData set callback";
    read_download_callback_from_ui_ = std::move(callback);
  }
  read_download_size_ = read_size;

  RegisterReadDownloadCallback(base::BindOnce(&ArkWebDownloadItemImplExt::ReadDownloadDataAndRunCallback,
                                              weak_ptr_factory_.GetWeakPtr(), read_size),
                               read_size);
  LOG(INFO) << "ArkWebDownloadItemImplExt::ReadDownloadData set callback state_=" << state_;

  if (PercentComplete() == 100) {
    LOG(INFO) << "ArkWebDownloadItemImplExt::ReadDownloadData complete already";
    RunCallbackIfExistsCallback();
    return;
  }

  if (state_ == COMPLETE_INTERNAL ||
      state_ == COMPLETING_INTERNAL ||
      state_ == INTERRUPTED_INTERNAL ||
      state_ == INTERRUPTED_TARGET_PENDING_INTERNAL ||
      state_ == CANCELLED_INTERNAL) {
    LOG(INFO) << "ArkWebDownloadItemImplExt::ReadDownloadData state match";
    RunCallbackIfExistsCallback();
  } else {
    RunCallbackIfDataReady();
  }
}

void ArkWebDownloadItemImplExt::ReadDownloadDataAndRunCallback(uint32_t size) {
  if (GetDownloadTaskRunner()) {
    GetDownloadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(&ArkWebDownloadItemImplExt::ReadDownloadDataAndRunCallbackInternal,
        weak_ptr_factory_.GetWeakPtr(), size));
  }
}

void ArkWebDownloadItemImplExt::ReadDownloadDataAndRunCallbackInternal(uint32_t size) {
  if (!read_download_callback_from_ui_) {
    LOG(DEBUG) << "ArkWebDownloadItemImplExt::ReadDownloadDataAndRunCallbackInternal called, size: "
               << size;
    return;
  }

  if (!download_file_.get() ||
      !download_file_.get()->AsArkWebDownloadFileImplExt()) {
    LOG(INFO) << "ArkWebDownloadItemImplExt::ReadDownloadDataAndRunCallbackInternal called, size: "
              << size;
    std::move(read_download_callback_from_ui_).Run(std::vector<uint8_t>());
    return;
  }

  std::vector<uint8_t> data(size);
  if (!download_file_.get()
           ->AsArkWebDownloadFileImplExt()
           ->ReadDownloadDataFromFile(0, (char*)(data.data()), size)) {
    LOG(INFO) << "ArkWebDownloadItemImplExt::ReadDownloadDataAndRunCallbackInternal called, size: "
              << size;
    std::move(read_download_callback_from_ui_).Run(std::vector<uint8_t>());
    return;
  }

  LOG(INFO) << "ArkWebDownloadItemImplExt::ReadDownloadDataAndRunCallbackInternal called, size: "
            << size;
  std::move(read_download_callback_from_ui_).Run(std::move(data));
}

void ArkWebDownloadItemImplExt::RunCallbackIfDataReady() {
  if (download_file_.get() &&
      download_file_.get()->AsArkWebDownloadFileImplExt()) {
    LOG(INFO) << "ArkWebDownloadItemImplExt::RunCallbackIfDataReady called";
    download_file_.get()
        ->AsArkWebDownloadFileImplExt()
        ->RunCallbackIfDataReady();
  }
}

void ArkWebDownloadItemImplExt::RunCallbackIfStateMatch() {
  if (GetDownloadTaskRunner()) {
    GetDownloadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(&ArkWebDownloadItemImplExt::RunCallbackIfStateMatchInternal,
        weak_ptr_factory_.GetWeakPtr()));
  }
}

void ArkWebDownloadItemImplExt::RunCallbackIfStateMatchInternal() {
  LOG(INFO) << "ArkWebDownloadItemImplExt::RunCallbackIfStateMatchInternal called";
  if (state_ == COMPLETE_INTERNAL ||
      state_ == COMPLETING_INTERNAL ||
      state_ == INTERRUPTED_INTERNAL ||
      state_ == INTERRUPTED_TARGET_PENDING_INTERNAL ||
      state_ == CANCELLED_INTERNAL) {
    uint32_t size = 0;

    if (download_file_.get() &&
        download_file_.get()->AsArkWebDownloadFileImplExt()) {
      size = download_file_.get()
                 ->AsArkWebDownloadFileImplExt()
                 ->GetNoHoleDownloadDataSize();
      LOG(INFO) << "ArkWebDownloadItemImplExt::RunCallbackIfStateMatchInternal size: " << size;
    }

    ReadDownloadDataAndRunCallback(size);
  }
}

void ArkWebDownloadItemImplExt::RunCallbackIfExistsCallback() {
  if (GetDownloadTaskRunner()) {
    GetDownloadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(&ArkWebDownloadItemImplExt::RunCallbackIfExistsCallbackInternal,
        weak_ptr_factory_.GetWeakPtr()));
  }
}

void ArkWebDownloadItemImplExt::RunCallbackIfExistsCallbackInternal() {
  uint32_t size = 0;
  if (download_file_.get() &&
      download_file_.get()->AsArkWebDownloadFileImplExt()) {
    size = download_file_.get()
               ->AsArkWebDownloadFileImplExt()
               ->GetNoHoleDownloadDataSize();
    LOG(INFO) << "ArkWebDownloadItemImplExt::RunCallbackIfExistsCallback size: " << size;
  }

  ReadDownloadDataAndRunCallback(size);
}

void ArkWebDownloadItemImplExt::RegisterReadDownloadCallback(
      base::OnceCallback<void()> callback,
      uint32_t size) {
  if (!download_file_ || !download_file_.get()->AsArkWebDownloadFileImplExt()) {
    LOG(INFO) << "ArkWebDownloadItemImplExt::RegisterReadDownloadCallback download_file_ null";
    RunCallbackIfExistsCallback();
    return;
  }

  download_file_.get()
      ->AsArkWebDownloadFileImplExt()
      ->RegisterReadDownloadCallback(base::BindOnce(std::move(callback)), size);
}
#endif  //  ARKWEB_EXT_DOWNLOAD

}  // namespace download
