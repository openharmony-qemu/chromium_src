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

#include <memory>
#include <string>
#include <string_view>

#include "base/threading/thread_checker.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/bindings/remote_set.h"
#include "arkweb/chromium_ext/services/device/public/mojom/res_sched_report.mojom.h"
#include "third_party/icu/source/common/unicode/uversion.h"

template <class T>
class scoped_refptr;

namespace base {
class SequencedTaskRunner;
}

namespace device {

namespace {
class ResSchedReportImpl;
}   // namespace


class ResSchedReport : public device::mojom::ResSchedReport {
 public:
  // Returns a new ResSchedReport object (likely a subclass) specific to the
  // platform. Inject |file_task_runner| to enable running blocking file
  // operations on it when necessary.
  static std::unique_ptr<ResSchedReport> Create(
      scoped_refptr<base::SequencedTaskRunner> file_task_runner);

  static std::unique_ptr<ResSchedReport> Create();

  ResSchedReport(const ResSchedReport&) = delete;
  ResSchedReport& operator=(const ResSchedReport&) = delete;
  ResSchedReport();
  ~ResSchedReport() override;

  void Bind(mojo::PendingReceiver<device::mojom::ResSchedReport> receiver);

 protected:

  uint8_t property_key;

 private:
  base::ThreadChecker thread_checker_;
  
  // Report audio data from mojom client to the adapter.
  void ReportData(uint8_t status, uint32_t pid, uint32_t platform_id) override;
  
  // Report getsture id data from mojom client to the adapter.
  void ReportGestureId(uint32_t id) override;
  
  // Report getsture ex data from mojom client to the adapter.
  void ReportGestureEx(uint32_t id, bool tag) override;

  void StartPerformanceBoost() override;

  mojo::ReceiverSet<device::mojom::ResSchedReport> receivers_;

  scoped_refptr<ResSchedReportImpl> impl_;
};

}  // namespace device
