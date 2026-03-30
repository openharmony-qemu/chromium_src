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

#include "third_party/blink/renderer/core/render_mojom/render_mojom_client.h"

#include "base/gtest_prod_util.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/sequence_checker.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/thread_pool.h"

#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "mojo/public/cpp/bindings/self_owned_receiver.h"
#include "arkweb/chromium_ext/services/device/public/mojom/res_sched_report.mojom-blink.h"
#include "arkweb/chromium_ext/services/device/public/mojom/sysprop_render_observer.mojom-blink.h"
#include "third_party/blink/public/common/thread_safe_browser_interface_broker_proxy.h"
#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/public/platform/task_type.h"
#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/renderer/platform/scheduler/public/main_thread.h"
#include "third_party/blink/renderer/platform/scheduler/public/post_cross_thread_task.h"
#include "third_party/blink/renderer/platform/wtf/allocator/allocator.h"
#include "third_party/blink/renderer/platform/wtf/cross_thread_copier_mojo.h"
#include "third_party/blink/renderer/platform/wtf/cross_thread_copier_std.h"
#include "third_party/blink/renderer/platform/wtf/cross_thread_functional.h"
#include "third_party/blink/renderer/platform/wtf/functional.h"
#include "third_party/blink/renderer/platform/wtf/wtf.h"

namespace blink {

// 0 and 1 indicates the properties key, 0 for OHOS::NWeb::PropertiesKey::PROP_DEBUG_TRACE, now it is not used,
// 1 for OHOS::NWeb::PropertiesKey::PROP_RENDER_DUMP.
constexpr uint8_t DEBUG_TRACE_KEY = 0;
constexpr uint8_t DEBUG_RENDER_DUMP = 1;

class CORE_EXPORT SysPropRenderObserverClientImpl final
    : public device::mojom::blink::SysPropRenderObserverClient {
 public:
  ~SysPropRenderObserverClientImpl() override;

  // This will be called by core_initializer, at that time this instance will be binded
  // to mojom interface for the service.
  static void Init();
  
  // When someone wants to listen the system properties, this method will keep the callback, when the mojom client receives
  // updates, it will tell them about the update.
  void AttachSysPropObserver(uint8_t key, OHOS::NWeb::SystemPropertiesObserver* callback);
  
  // When someone wants to cancel to listen the system properties, this method will remove the callback, when the mojom client receives
  // updates, it will not tell them anymore.
  void DetachSysPropObserver(uint8_t key, OHOS::NWeb::SystemPropertiesObserver* callback);
  
  // When the mojom interface at the service receive properties update about PROP_DEBUG_TRACE, it will call
  // this method, and then tell someone who has listened this property.
  void TracePropertiesUpdate(const String& value) override;
  
  // When the mojom interface at the service receive properties update about PROP_RENDER_DUMP, it will call
  // this method, and then tell someone who has listened this property.
  void DumpPropertiesUpdate(const String& value) override;

  void PropertiesMap(uint8_t key);

  static SysPropRenderObserverClientImpl& instance();

 private:
  SysPropRenderObserverClientImpl();
  
  static void ClearSysPropRenderOverride();

  mojo::Receiver<device::mojom::blink::SysPropRenderObserverClient> receiver_{this};

  // this is used to keep the callbacks who wants to listen system properties.
  std::vector<OHOS::NWeb::SystemPropertiesObserver*> debug_callbacks_;
  std::vector<OHOS::NWeb::SystemPropertiesObserver*> dump_callbacks_;

  // Protect the callbacks
  std::unordered_map<uint8_t, std::shared_mutex> sysPropMutex_;
};

SysPropRenderObserverClientRep::SysPropRenderObserverClientRep() {
}

SysPropRenderObserverClientRep::~SysPropRenderObserverClientRep() {
}

void SysPropRenderObserverClientRep::Init() {
  SysPropRenderObserverClientImpl::Init();
}

void SysPropRenderObserverClientRep::AttachSysPropObserver(uint8_t key, OHOS::NWeb::SystemPropertiesObserver* callback) {
  SysPropRenderObserverClientImpl::instance().AttachSysPropObserver(key, callback);
}

void SysPropRenderObserverClientRep::DetachSysPropObserver(uint8_t key, OHOS::NWeb::SystemPropertiesObserver* callback) {
  SysPropRenderObserverClientImpl::instance().DetachSysPropObserver(key, callback);
}

SysPropRenderObserverClientImpl::SysPropRenderObserverClientImpl() {
  sysPropMutex_[DEBUG_TRACE_KEY];
  sysPropMutex_[DEBUG_RENDER_DUMP];
}

SysPropRenderObserverClientImpl::~SysPropRenderObserverClientImpl() = default;

void SysPropRenderObserverClientImpl::Init() {
  mojo::Remote<device::mojom::blink::SysPropRenderObserver> observer;
  Platform::Current()->GetBrowserInterfaceBroker()->GetInterface(
      observer.BindNewPipeAndPassReceiver());
  observer->Attach(instance().receiver_.BindNewPipeAndPassRemote());
}

SysPropRenderObserverClientImpl& SysPropRenderObserverClientImpl::instance() {
  static SysPropRenderObserverClientImpl instance;
  return instance;
}

void SysPropRenderObserverClientImpl::AttachSysPropObserver(uint8_t key, OHOS::NWeb::SystemPropertiesObserver* callback) {
  std::unique_lock lock(sysPropMutex_[key]);
  LOG(DEBUG) << "SysPropRenderObserverClientImpl receives AttachSysPropObserver request for key:" << key;
  if (key == DEBUG_TRACE_KEY) {
    instance().debug_callbacks_.push_back(callback);
  } else {
    instance().dump_callbacks_.push_back(callback);
  }
}

void SysPropRenderObserverClientImpl::DetachSysPropObserver(uint8_t key, OHOS::NWeb::SystemPropertiesObserver* callback) {
  std::unique_lock lock(sysPropMutex_[key]);
  LOG(DEBUG) << "SysPropRenderObserverClientImpl receives DetachSysPropObserver request for key:" << key;
  if (key == DEBUG_TRACE_KEY) {
    auto it = std::find(instance().debug_callbacks_.begin(), instance().debug_callbacks_.end(), callback);
    if (it != instance().debug_callbacks_.end()) {
      instance().debug_callbacks_.erase(it);
    }
  } else {
    auto it = std::find(instance().dump_callbacks_.begin(), instance().dump_callbacks_.end(), callback);
    if (it != instance().dump_callbacks_.end()) {
      instance().dump_callbacks_.erase(it);
    }
  }
}

void SysPropRenderObserverClientImpl::TracePropertiesUpdate(const String& value) {
  std::unique_lock lock(sysPropMutex_[DEBUG_TRACE_KEY]);
  std::string str_value = value.Utf8().data();
  LOG(DEBUG) << "SysPropRenderObserverClientImpl receives TracePropertiesUpdate from mojom.";
  for (auto& callback : instance().debug_callbacks_) {
    if (callback != nullptr) {
      callback->PropertiesUpdate(str_value.c_str());
    }
  }
}

void SysPropRenderObserverClientImpl::DumpPropertiesUpdate(const String& value) {
  std::unique_lock lock(sysPropMutex_[DEBUG_RENDER_DUMP]);
  std::string str_value = value.Utf8().data();
  LOG(DEBUG) << "SysPropRenderObserverClientImpl receives DumpPropertiesUpdate from mojom.";
  for (auto& callback : instance().dump_callbacks_) {
    if (callback != nullptr) {
      callback->PropertiesUpdate(str_value.c_str());
    }
  }
}

ResSchedReportClient::ResSchedReportClient(uint32_t id) {
  resSchedMutex_[id];
}

void SendDataMainThread(uint8_t status, uint32_t pid, uint32_t platform_id) {
  mojo::Remote<device::mojom::blink::ResSchedReport> observer;
  if (Platform::Current() && Platform::Current()->GetBrowserInterfaceBroker()) {
    Platform::Current()->GetBrowserInterfaceBroker()->GetInterface(
        observer.BindNewPipeAndPassReceiver());
    observer->ReportData(status, pid, platform_id);
  }
}

void ResSchedReportClient::SendAudioData(uint8_t status, uint32_t pid, uint32_t platform_id) {
  std::unique_lock lock(resSchedMutex_[pid]);
  LOG(DEBUG) << "ResSchedReportClient start to send data to mojom for report audio data.";
  // Because there has some problems when this method is called at some subthreads, then here post
  // the task to the main thread when it is not main thread.
  if (!IsMainThread()) {
    if (Thread::MainThread()) {
      PostCrossThreadTask(
          *Thread::MainThread()->GetTaskRunner(MainThreadTaskRunnerRestricted()),
          FROM_HERE,
          CrossThreadBindOnce(&SendDataMainThread, status, pid, platform_id));
      return;
    }
  } else {
    mojo::Remote<device::mojom::blink::ResSchedReport> observer;
    if (Platform::Current() && Platform::Current()->GetBrowserInterfaceBroker()) {
      Platform::Current()->GetBrowserInterfaceBroker()->GetInterface(
          observer.BindNewPipeAndPassReceiver());
      observer->ReportData(status, pid, platform_id);
    }
  }
}

void ResSchedReportClient::ApplySocByIdEx(uint32_t id, bool tag) {
  std::unique_lock lock(resSchedMutex_[id]);
  LOG(DEBUG) << "ResSchedReportClient start to send data to mojom for ApplySocByIdEx.";
  mojo::Remote<device::mojom::blink::ResSchedReport> observer;
  if (Platform::Current() && Platform::Current()->GetBrowserInterfaceBroker()) {
    Platform::Current()->GetBrowserInterfaceBroker()->GetInterface(
        observer.BindNewPipeAndPassReceiver());
    observer->ReportGestureEx(id, tag);
  }
}

void ResSchedReportClient::ApplySocById(uint32_t id) {
  std::unique_lock lock(resSchedMutex_[id]);
  LOG(DEBUG) << "ResSchedReportClient start to send data to mojom for ApplySocById.";
  mojo::Remote<device::mojom::blink::ResSchedReport> observer;
  // In some particular circumstances Platform::Current() will be empty, this is not contralable.
  if (Platform::Current() && Platform::Current()->GetBrowserInterfaceBroker()) {
    Platform::Current()->GetBrowserInterfaceBroker()->GetInterface(
        observer.BindNewPipeAndPassReceiver());
    observer->ReportGestureId(id);
  }
}

void ResSchedReportClient::StartPerformanceBoost() {
  LOG(DEBUG) << "ResSchedReportClient start to send data to mojom for StartPerformanceBoost.";
  mojo::Remote<device::mojom::blink::ResSchedReport> observer;
  // In some particular circumstances Platform::Current() will be empty, this is not contralable.
  if (Platform::Current() && Platform::Current()->GetBrowserInterfaceBroker()) {
    Platform::Current()->GetBrowserInterfaceBroker()->GetInterface(
        observer.BindNewPipeAndPassReceiver());
    observer->StartPerformanceBoost();
  }
}

#if BUILDFLAG(ARKWEB_OHOS_MEM_USAGE_REPORT)
void ResSchedReportClient::ReportMemoryUsage(const String& msg) {
  mojo::Remote<device::mojom::blink::ResSchedReport> observer;
  // In some particular circumstances Platform::Current() will be empty, this is not contralable.
  if (Platform::Current() && Platform::Current()->GetBrowserInterfaceBroker()) {
    Platform::Current()->GetBrowserInterfaceBroker()->GetInterface(
        observer.BindNewPipeAndPassReceiver());
    observer->ReportMemoryUsage(msg);
  }
}
#endif  // BUILDFLAG(ARKWEB_OHOS_MEM_USAGE_REPORT)
}  // namespace blink
