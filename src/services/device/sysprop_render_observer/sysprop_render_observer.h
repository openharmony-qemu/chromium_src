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

#ifndef SERVICES_DEVICE_SYSPROP_RENDER_OBSERVER_SYSPROP_RENDER_OBSERVER_H_
#define SERVICES_DEVICE_SYSPROP_RENDER_OBSERVER_SYSPROP_RENDER_OBSERVER_H_

#include <memory>
#include <string>
#include <string_view>

#include "arkweb/build/features/features.h"
#include "base/threading/thread_checker.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/bindings/remote_set.h"
#include "arkweb/chromium_ext/services/device/public/mojom/sysprop_render_observer.mojom.h"
#include "third_party/icu/source/common/unicode/uversion.h"

template <class T>
class scoped_refptr;

namespace base {
class SequencedTaskRunner;
}

namespace device {

class SysPropRenderObserver : public device::mojom::SysPropRenderObserver {
 public:
  static std::unique_ptr<SysPropRenderObserver> Create(
      scoped_refptr<base::SequencedTaskRunner> file_task_runner);

  static std::unique_ptr<SysPropRenderObserver> Create();

  SysPropRenderObserver(const SysPropRenderObserver&) = delete;
  SysPropRenderObserver& operator=(const SysPropRenderObserver&) = delete;
  ~SysPropRenderObserver() override;

  void Bind(mojo::PendingReceiver<device::mojom::SysPropRenderObserver> receiver);

 protected:
  SysPropRenderObserver();

  void NotifyClients(std::string value, uint8_t key);

  uint8_t property_key;

 private:
  base::ThreadChecker thread_checker_;

  // This method will add the mojom clients, then transfer data to the clients.
  void Attach(mojo::PendingRemote<device::mojom::SysPropRenderObserverClient>
                     client) override;

  // This method is not realised, if there is not client listen system properties,
  // this method can be used to cancel the listening from adapter
  void Detach(mojo::PendingRemote<device::mojom::SysPropRenderObserverClient>
                     client) override;

  mojo::ReceiverSet<device::mojom::SysPropRenderObserver> receivers_;
  mojo::RemoteSet<device::mojom::SysPropRenderObserverClient> clients_;
};
}  // namespace device

#endif  // SERVICES_DEVICE_PUBLIC_MOJOM_SYSPROP_RENDER_OBSERVER_MOJOM_H_
