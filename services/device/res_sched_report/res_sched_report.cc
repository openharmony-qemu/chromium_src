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

#include "services/device/res_sched_report/res_sched_report.h"

#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/trace_event/trace_event.h"
#include "third_party/icu/source/common/unicode/unistr.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

using namespace OHOS::NWeb;

namespace device {

// There are only two status of the audio, start and stop, use 1 to indicate stop, then
// the other is start.
constexpr uint8_t AUDIO_STATUS_STOP = 1;

ResSchedReport::~ResSchedReport() {
  DCHECK(thread_checker_.CalledOnValidThread());
}

void ResSchedReport::Bind(
  mojo::PendingReceiver<device::mojom::ResSchedReport> receiver) {
  DCHECK(thread_checker_.CalledOnValidThread());
  receivers_.Add(this, std::move(receiver));
}

std::unique_ptr<ResSchedReport> ResSchedReport::Create() {
  return std::make_unique<ResSchedReport>();
}

namespace {
class ResSchedReportImpl
    : public base::RefCountedThreadSafe<ResSchedReportImpl> {
 public:
  static scoped_refptr<ResSchedReportImpl> Create(ResSchedReport* owner) {
    auto impl = base::WrapRefCounted(new ResSchedReportImpl(owner));
      return impl;
    }

    ResSchedReportImpl(const ResSchedReportImpl&) = delete;
    ResSchedReportImpl& operator=(const ResSchedReportImpl&) = delete;
    void ReportData(uint8_t status, uint32_t pid, uint32_t platform_id);
    void ReportGestureId(uint32_t id);
    void ReportGestureEx(uint32_t id, bool tag);
    void StartPerformanceBoost();

    explicit ResSchedReportImpl(ResSchedReport* owner) {
        task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
    }

 private:
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
};
}

void ResSchedReportImpl::ReportData(uint8_t status, uint32_t pid, uint32_t platform_id) {
  LOG(DEBUG) << "ResSchedReportImpl start to report audio data from mojom for status:" << status;
  if (status == AUDIO_STATUS_STOP) {
    OHOS::NWeb::ResSchedClientAdapter::ReportAudioData(
        OHOS::NWeb::ResSchedStatusAdapter::AUDIO_STATUS_STOP,
        pid, platform_id);
  } else {
    OHOS::NWeb::ResSchedClientAdapter::ReportAudioData(
        OHOS::NWeb::ResSchedStatusAdapter::AUDIO_STATUS_START,
        pid, platform_id);
  }
}

void ResSchedReportImpl::ReportGestureId(uint32_t id) {
  LOG(DEBUG) << "ResSchedReportImpl start to report gesture id from mojom";
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
    .CreateSocPerfClientAdapter()
    ->ApplySocPerfConfigById(id);
}

void ResSchedReportImpl::ReportGestureEx(uint32_t id, bool tag) {
  LOG(DEBUG) << "ResSchedReportImpl start to report gesture ex from mojom";
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
    .CreateSocPerfClientAdapter()
    ->ApplySocPerfConfigByIdEx(id, tag);
}

void ResSchedReportImpl::StartPerformanceBoost() {
  LOG(DEBUG) << "ResSchedReportImpl start to StartPerformanceBoost from mojom";
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
    .CreateFlowbufferAdapter()
    ->StartPerformanceBoost();
}

ResSchedReport::ResSchedReport() : impl_() {
  impl_ = ResSchedReportImpl::Create(this);
}

void ResSchedReport::ReportData(uint8_t status, uint32_t pid, uint32_t platform_id) {
  if (impl_.get()) {
    impl_->ReportData(status, pid, platform_id);
  }
}

void ResSchedReport::ReportGestureId(uint32_t id) {
  if (impl_.get()) {
    impl_->ReportGestureId(id);
  }
}

void ResSchedReport::ReportGestureEx(uint32_t id, bool tag) {
  if (impl_.get()) {
    impl_->ReportGestureEx(id, tag);
  }
}

void ResSchedReport::StartPerformanceBoost() {
  if (impl_.get()) {
    impl_->StartPerformanceBoost();
  }
}
}  // namespace device
