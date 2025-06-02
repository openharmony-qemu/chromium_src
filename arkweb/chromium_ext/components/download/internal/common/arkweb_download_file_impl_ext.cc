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
#pragma allow_unsafe_buffers
#endif

#include <algorithm>
#include <memory>
#include <string>
#include <utility>

#include "components/download/public/common/arkweb_download_file_impl_ext.h"
#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "base/trace_event/trace_event.h"
#include "base/values.h"
#include "build/build_config.h"
#include "components/download/internal/common/parallel_download_utils.h"
#include "components/download/public/common/download_create_info.h"
#include "components/download/public/common/download_destination_observer.h"
#include "components/download/public/common/download_features.h"
#include "components/download/public/common/download_file_impl.h"
#include "components/download/public/common/download_interrupt_reasons_utils.h"
#include "components/download/public/common/download_stats.h"
#include "crypto/secure_hash.h"
#include "crypto/sha2.h"
#include "mojo/public/c/system/types.h"
#include "net/base/io_buffer.h"

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
#include "components/download/public/common/download_utils.h"
#endif  // BUILDFLAG(ARKWEB_EXT_DOWNLOAD)

namespace download {

namespace {

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
static constexpr base::TimeDelta kTimeout = base::Seconds(60);
#endif
}  // namespace

ArkWebDownloadFileImplExt::ArkWebDownloadFileImplExt(
    std::unique_ptr<DownloadSaveInfo> save_info,
    const base::FilePath& default_download_directory,
    std::unique_ptr<InputStream> stream,
    uint32_t download_id,
    base::WeakPtr<DownloadDestinationObserver> observer)
    : DownloadFileImpl(std::move(save_info),
                       default_download_directory,
                       std::move(stream),
                       download_id,
                       observer) {}

ArkWebDownloadFileImplExt::~ArkWebDownloadFileImplExt() {}

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
bool ArkWebDownloadFileImplExt::ReadDownloadDataFromFile(int64_t offset,
                                                         char* data,
                                                         size_t size) {
  if (size == 0) {
    return false;
  }

  if (!file_.ReadDataFromFile(offset, data, size, file_.full_path())) {
    LOG(WARNING) << "DownloadFileImpl::ReadDownloadDataFromFile fail";
    return false;
  }

  return true;
}

void ArkWebDownloadFileImplExt::RunCallbackIfDataReady() {
  LOG(DEBUG)
      << "DownloadFileImpl::RunCallbackIfDataReady called, TotalBytesReceived: "
      << TotalBytesReceived();
  if (!read_download_callback_) {
    LOG(DEBUG) << "DownloadFileImpl::RunCallbackIfDataReady "
                  "read_download_callback_ null";
    return;
  }

  if (TotalBytesReceived() < read_download_size_) {
    return;
  }

  if (GetNoHoleDownloadDataSize() < read_download_size_) {
    LOG(DEBUG) << "DownloadFileImpl::RunCallbackIfDataReady PreDownloadSize:"
               << GetNoHoleDownloadDataSize();
    return;
  }

  LOG(DEBUG) << "DownloadFileImpl::RunCallbackIfDataReady "
                "MaybeRunReadDownloadCallback";
  MaybeRunReadDownloadCallback();
}

uint32_t ArkWebDownloadFileImplExt::GetNoHoleDownloadDataSize() {
  int64_t total_received_bytes = TotalBytesReceived();
  if (!IsSparseFile()) {
    LOG(DEBUG) << "DownloadFileImpl::GetNoHoleDownloadDataSize not sparse file";
    return std::min(total_received_bytes, (int64_t)read_download_size_);
  }

  int64_t received_no_hole_bytes = 0;

  for (const auto& received_slice : received_slices_) {
    received_no_hole_bytes =
        received_slice.offset + received_slice.received_bytes;
    if (!received_slice.finished) {
      LOG(DEBUG)
          << "DownloadFileImpl::GetNoHoleDownloadDataSize slice not finished";
      break;
    }
  }

  return std::min(received_no_hole_bytes, (int64_t)read_download_size_);
}

void ArkWebDownloadFileImplExt::RegisterReadDownloadCallback(
    base::OnceCallback<void()> callback,
    uint32_t size) {
  if (read_download_callback_) {
    LOG(DEBUG) << "DownloadFileImpl::RegisterReadDownloadCallback callback "
                  "already exsit";
    return;
  }

  LOG(DEBUG) << "DownloadFileImpl::RegisterReadDownloadCallback called";
  read_download_size_ = size;
  read_download_callback_ = std::move(callback);
}

void ArkWebDownloadFileImplExt::MaybeRunReadDownloadCallback() {
  if (read_download_callback_) {
    LOG(DEBUG) << "DownloadFileImpl::MaybeRunReadDownloadCallback called";
    std::move(read_download_callback_).Run();
  }
}
#endif

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
void ArkWebDownloadFileImplExt::OnTimeout(SourceStream* source_stream,
                                          DownloadInterruptReason reason,
                                          InputStream::StreamState stream_state,
                                          bool should_terminate) {
  LOG(INFO) << "download time out";
  if (download_job_timer_) {
    download_job_timer_.reset();
  }
  NotifyObserver(source_stream, reason, stream_state, should_terminate);
}

void ArkWebDownloadFileImplExt::CreateDownloadJobTimer(
    SourceStream* source_stream) {
  download_job_timer_ = std::make_unique<base::OneShotTimer>();
  download_job_timer_->Start(
      FROM_HERE, kTimeout,
      base::BindOnce(&ArkWebDownloadFileImplExt::OnTimeout,
                     base::Unretained(this), source_stream,
                     DOWNLOAD_INTERRUPT_REASON_NETWORK_DISCONNECTED,
                     InputStream::COMPLETE, true));
}

void ArkWebDownloadFileImplExt::StopDownloadJobTimer() {
  if (download_job_timer_ && download_job_timer_->IsRunning()) {
    download_job_timer_->Stop();
  }
}

void ArkWebDownloadFileImplExt::ResetDownloadJobTimer() {
  if (download_job_timer_ && download_job_timer_->IsRunning()) {
    download_job_timer_.reset();
  }
}
#endif

}  // namespace download
