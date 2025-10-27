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

#include "services/device/sysprop_render_observer/sysprop_render_observer.h"

#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/trace_event/trace_event.h"
#include "third_party/icu/source/common/unicode/unistr.h"

namespace device {

// This value indicates that the listening system property is PROP_DEBUG_TRACE.
constexpr uint8_t DEBUG_TRACE_KEY = 0;

SysPropRenderObserver::SysPropRenderObserver() {}

SysPropRenderObserver::~SysPropRenderObserver() {
  DCHECK(thread_checker_.CalledOnValidThread());
}

void SysPropRenderObserver::Bind(
    mojo::PendingReceiver<device::mojom::SysPropRenderObserver> receiver) {
  DCHECK(thread_checker_.CalledOnValidThread());
  receivers_.Add(this, std::move(receiver));
}

void SysPropRenderObserver::NotifyClients(std::string value, uint8_t key) {
  DCHECK(thread_checker_.CalledOnValidThread());
  LOG(DEBUG) << "SysPropRenderObserver will notify the client that has been monitored.";

  // here DEBUG_TRACE_KEY indicates the property PROP_DEBUG_TRACE and otherwise indicates PROP_RENDER_DUMP
  if (key == DEBUG_TRACE_KEY) {
    for (auto& client : clients_)
      client->TracePropertiesUpdate(value);
  } else {
    for (auto& client : clients_)
      client->DumpPropertiesUpdate(value);
  }
}

void SysPropRenderObserver::Attach(
    mojo::PendingRemote<device::mojom::SysPropRenderObserverClient> client) {
  DCHECK(thread_checker_.CalledOnValidThread());
  clients_.Add(std::move(client));
}

void SysPropRenderObserver::Detach(
    mojo::PendingRemote<device::mojom::SysPropRenderObserverClient> client) {
  DCHECK(thread_checker_.CalledOnValidThread());
}
}  // namespace device
