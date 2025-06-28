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

#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/task/single_thread_task_runner.h"
#include "sysprop_render_observer.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

using namespace OHOS::NWeb;
namespace device {

namespace {
class SysPropRenderObserverOhosImpl;
}   // namespace

class SysPropRenderObserverOhos : public SysPropRenderObserver {
 public:
   SysPropRenderObserverOhos();
   SysPropRenderObserverOhos(const SysPropRenderObserverOhos&) = delete;
   SysPropRenderObserverOhos& operator=(const SysPropRenderObserverOhos&) = delete;

   ~SysPropRenderObserverOhos();

   void NotifyClientsFromImpl(std::string value);

   void NotifyDumpClientsFromImpl(std::string value);

 private:
   scoped_refptr<SysPropRenderObserverOhosImpl> impl_;
};

namespace {

// here use an integer to distingush which property key is listened.
constexpr uint8_t DEBUG_TRACE_KEY = 0;
constexpr uint8_t DEBUG_RENDER_DUMP = 1;

class SysPropRenderObserverOhosImpl;

class SysPropTraceEventCallback : public OHOS::NWeb::SystemPropertiesObserver {
 public:
   explicit SysPropTraceEventCallback(SysPropRenderObserverOhosImpl* impl)
       : impl_(impl) {}
   ~SysPropTraceEventCallback() override = default;

   void PropertiesUpdate(const char* value) override;

 private:
   raw_ptr<SysPropRenderObserverOhosImpl> impl_;
};

class SysPropDumpEventCallback : public OHOS::NWeb::SystemPropertiesObserver {
 public:
   explicit SysPropDumpEventCallback(SysPropRenderObserverOhosImpl* impl)
       : impl_(impl) {}
   ~SysPropDumpEventCallback() override = default;

   void PropertiesUpdate(const char* value) override;

 private:
  raw_ptr<SysPropRenderObserverOhosImpl> impl_;
};

class SysPropRenderObserverOhosImpl
    : public base::RefCountedThreadSafe<SysPropRenderObserverOhosImpl> {
 public:
  static scoped_refptr<SysPropRenderObserverOhosImpl> Create(SysPropRenderObserverOhos* owner) {
    auto impl = base::WrapRefCounted(new SysPropRenderObserverOhosImpl(owner));
    return impl;
  }

  SysPropRenderObserverOhosImpl(const SysPropRenderObserverOhosImpl&) = delete;
  SysPropRenderObserverOhosImpl& operator=(const SysPropRenderObserverOhosImpl&) = delete;

  // When this object is created, this class will start to listen the system properties.
  explicit SysPropRenderObserverOhosImpl(SysPropRenderObserverOhos* owner)
      : owner_(owner) {
    trace_callback_ = std::make_unique<SysPropTraceEventCallback>(this);
    dump_callback_ = std::make_unique<SysPropDumpEventCallback>(this);
    auto& sysproprenderClient = OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance();
    //sysproprenderClient.AttachSysPropObserver(OHOS::NWeb::PropertiesKey::PROP_HITRACE_ENABLEFLAGS,
    sysproprenderClient.AttachSysPropObserver(OHOS::NWeb::PropertiesKey::PROP_DEBUG_TRACE,
                                              trace_callback_.get());
    sysproprenderClient.AttachSysPropObserver(OHOS::NWeb::PropertiesKey::PROP_RENDER_DUMP,
                                              dump_callback_.get());
    LOG(INFO) << "SysPropRenderObserverOhosImpl start to listen system parameter.";
    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  }

 private:
  friend class SysPropTraceEventCallback;

  friend class SysPropDumpEventCallback;

  void Propertiesupdate(std::string value);

  void Dumpupdate(std::string value);

  std::unique_ptr<SysPropTraceEventCallback> trace_callback_;

  std::unique_ptr<SysPropDumpEventCallback> dump_callback_;

  raw_ptrSysPropRenderObserverOhos> owner_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
};
}

void SysPropTraceEventCallback::PropertiesUpdate(const char* value) {
  // As the callback all receive const char* but mojom does not support this type, and the value can be
  // modified by others, then convert it to string.
  if (impl_) {
    LOG(DEBUG) << "SysPropTraceEventCallback get properties update from adapter successfully.";
    std::string str(value);
    impl_->Propertiesupdate(str);
  }
}

void SysPropDumpEventCallback::PropertiesUpdate(const char* value) {
  if (impl_) {
    LOG(DEBUG) << "SysPropDumpEventCallback get properties update from adapter successfully.";
    std::string str(value);
    impl_->Dumpupdate(str);
  }
}

SysPropRenderObserverOhos::SysPropRenderObserverOhos() : SysPropRenderObserver(), impl_() {
  impl_ = SysPropRenderObserverOhosImpl::Create(this);
}

SysPropRenderObserverOhos::~SysPropRenderObserverOhos() {}

void SysPropRenderObserverOhos::NotifyClientsFromImpl(std::string value) {
  // Here 0 indicates that receiving a trace property updates and inform the corresponding
  // callback, 0 will tell the mojom client for the type of property, as the customised class
  // alway fail in mojom, then use uint to distingush different type of properties.
  NotifyClients(value, DEBUG_TRACE_KEY);
}

void SysPropRenderObserverOhos::NotifyDumpClientsFromImpl(std::string value) {
  // Here 0 indicates that receiving a dump property updates.
  NotifyClients(value, DEBUG_RENDER_DUMP);
}

std::unique_ptr<SysPropRenderObserver> SysPropRenderObserver::Create() {
  return std::make_unique<SysPropRenderObserverOhos>();
}

void SysPropRenderObserverOhosImpl::Propertiesupdate(std::string value) {
  if (!task_runner_->RunsTasksInCurrentSequence()) {
    task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&SysPropRenderObserverOhosImpl::Propertiesupdate,
                       base::Unretained(this), std::move(value)));
    return;
  }
  owner_->NotifyClientsFromImpl(value);
}

void SysPropRenderObserverOhosImpl::Dumpupdate(std::string value) {
  if (!task_runner_->RunsTasksInCurrentSequence()) {
    task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&SysPropRenderObserverOhosImpl::Dumpupdate,
                       base::Unretained(this), std::move(value)));
    return;
  }
  owner_->NotifyDumpClientsFromImpl(value);
}

}  // namespace