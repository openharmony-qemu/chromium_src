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

#ifndef COMPONENTS_ARKWEB_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_FILE_IMPL_EXT_H_
#define COMPONENTS_ARKWEB_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_FILE_IMPL_EXT_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "arkweb/chromium_ext/components/download/public/common/arkweb_download_file_ext.h"
#include "base/cancelable_callback.h"
#include "base/files/file.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/sequence_checker.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/task_runner.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "build/build_config.h"
#include "components/download/public/common/base_file.h"
#include "components/download/public/common/download_file.h"
#include "components/download/public/common/download_item.h"
#include "components/download/public/common/download_file_impl.h"
#include "components/download/public/common/download_save_info.h"
#include "components/download/public/common/rate_estimator.h"
#include "components/enterprise/buildflags/buildflags.h"
#include "components/services/quarantine/public/mojom/quarantine.mojom.h"
#include "mojo/public/cpp/system/simple_watcher.h"

#if BUILDFLAG(ENTERPRISE_CONTENT_ANALYSIS)
#include "components/enterprise/obfuscation/core/download_obfuscator.h"  // nogncheck
#endif  // BUILDFLAG(ENTERPRISE_CONTENT_ANALYSIS)

#include "base/functional/callback_forward.h"

namespace download {

class DownloadFileImpl;
class DownloadDestinationObserver;

class COMPONENTS_DOWNLOAD_EXPORT ArkWebDownloadFileImplExt
    : public DownloadFileImpl,
      public ArkWebDownloadFileExt {
 public:
  // Takes ownership of the object pointed to by |save_info|.
  // |net_log| will be used for logging the download file's events.
  // May be constructed on any thread.  All methods besides the constructor
  // (including destruction) must occur in the same sequence.
  //
  // Note that the DownloadFileImpl automatically reads from the passed in
  // |stream|, and sends updates and status of those reads to the
  // DownloadDestinationObserver.
  ArkWebDownloadFileImplExt(
      std::unique_ptr<DownloadSaveInfo> save_info,
      const base::FilePath& default_downloads_directory,
      std::unique_ptr<InputStream> stream,
      uint32_t download_id,
      base::WeakPtr<DownloadDestinationObserver> observer);

  ArkWebDownloadFileImplExt(const ArkWebDownloadFileImplExt&) = delete;
  ArkWebDownloadFileImplExt& operator=(const ArkWebDownloadFileImplExt&) =
      delete;

  ~ArkWebDownloadFileImplExt() override;

  ArkWebDownloadFileImplExt* AsArkWebDownloadFileImplExt() override {
    return this;
  }

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  void ReadAndRunCallbackIfDataReady() override;

  void RegisterReadDownloadCallback(ReadDownloadDataCallback callback,
                                    int32_t size) override;

  bool ReadDownloadDataFromFile(int64_t offset,
                                char* data,
                                size_t size) override;

  uint32_t GetNoHoleDownloadDataSize() override;

  void CreateDownloadJobTimer(SourceStream* source_stream);

  void StopDownloadJobTimer();

  void ResetDownloadJobTimer();

  void ReadDownloadDataAndRunCallback(int32_t read_download_size) override;
#endif

 protected:
#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  void OnTimeout(SourceStream* source_stream,
                 DownloadInterruptReason reason,
                 InputStream::StreamState stream_state,
                 bool should_terminate);
#endif

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  void MaybeRunReadDownloadCallback();
#endif

  // The base file instance.
#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  std::unique_ptr<base::OneShotTimer> download_job_timer_;
#endif

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  uint32_t read_download_size_{0};
  ReadDownloadDataCallback read_download_callback_;
#endif
};

}  // namespace download

#endif  // COMPONENTS_ARKWEB_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_FILE_IMPL_EXT_H_
