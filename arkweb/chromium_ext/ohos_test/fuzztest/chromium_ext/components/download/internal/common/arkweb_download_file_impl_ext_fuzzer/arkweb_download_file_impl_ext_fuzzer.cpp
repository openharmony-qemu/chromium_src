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

#include <fuzzer/FuzzedDataProvider.h>

#include "base/logging.h"
#include "base/test/task_environment.h"
#include "chrome/browser/notifications/notification_platform_bridge_ohos.h"
#include "components/download/public/common/download_save_info.h"
#include "components/download/internal/common/parallel_download_utils.h"
#include "components/download/public/common/download_create_info.h"
#include "components/download/public/common/download_destination_observer.h"
#include "components/download/public/common/download_file_impl.h"
#include "components/download/public/common/download_task_runner.h"
#include "components/download/public/common/download_interrupt_reasons.h"
#include "components/download/public/common/download_item.h"
#include "components/download/public/common/mock_download_item.h"
#include "components/download/public/common/mock_input_stream.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "arkweb/chromium_ext/components/download/public/common/arkweb_download_file_impl_ext.h"

namespace download {

namespace {

class MockDownloadDestinationObserver : public DownloadDestinationObserver {
 public:
  void DestinationUpdate(
    int64_t bytes_so_far,
    int64_t bytes_per_sec,
    const std::vector<DownloadItem::ReceivedSlice>& received_slices) override {}

  void DestinationError(
    DownloadInterruptReason reason,
    int64_t bytes_so_far,
    std::unique_ptr<crypto::SecureHash> hash_state) override {}

  void DestinationCompleted(
    int64_t total_bytes,
    std::unique_ptr<crypto::SecureHash> hash_state) override {}
};

}  // namespace

void FuzzTest(FuzzedDataProvider* fdp) {
  auto save_info = std::make_unique<DownloadSaveInfo>();
  auto input_stream = std::make_unique<MockInputStream>();
  auto observer = std::make_unique<MockDownloadDestinationObserver>();
  base::WeakPtrFactory<DownloadDestinationObserver> observer_factory(
      observer.get());
  auto download_file = std::make_unique<ArkWebDownloadFileImplExt>(
      std::move(save_info), base::FilePath(),
      std::move(input_stream), DownloadItem::kInvalidId,
      observer_factory.GetWeakPtr());
  download_file->RunCallbackIfDataReady();
  download_file->RegisterReadDownloadCallback(base::OnceCallback<void()>(),
      fdp->ConsumeIntegral<int32_t>());
  size_t dataSize = fdp->ConsumeIntegralInRange<size_t>(1, 1024);
  char data[dataSize];
  download_file->ReadDownloadDataFromFile(fdp->ConsumeIntegral<int64_t>(), data, dataSize);
  download_file->GetNoHoleDownloadDataSize();
  download_file->CreateDownloadJobTimer(nullptr);
  download_file->StopDownloadJobTimer();
  download_file->ResetDownloadJobTimer();
}

} // namespace download

static base::test::TaskEnvironment task_environment;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if((data == nullptr) || (size < sizeof(int32_t))) {
    return 0;
  }
  FuzzedDataProvider fdp(data, size);
  download::FuzzTest(&fdp);
  return 0;
}
