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

#ifndef DFX_APPFREEZE_MONITOR_RENDER_IMPL_H_
#define DFX_APPFREEZE_MONITOR_RENDER_IMPL_H_

#include <string>
#include "arkweb/chromium_ext/content/browser/dfx/mojom/dfx_reporting.mojom.h"
#include "mojo/public/cpp/bindings/remote.h"

void ReportRenderFreeze();

class AppfreezeMonitorImpl {
public:
  AppfreezeMonitorImpl() = default;
  ~AppfreezeMonitorImpl() = default;
  static std::shared_ptr<AppfreezeMonitorImpl> GetInstance();
  void GetRemoteAndSend(const std::string& args) {
    if (remote_.is_bound()) {
      remote_->ReportHiSysEvent(args, "");
    }
  }
  mojo::PendingReceiver<dfx::mojom::DfxReporter> &GetPendingReceiver() {
    return receiver_;
  }
  bool IsInitialized() { return initialized_; }
  void HasInitialized() { initialized_ = true; }
  bool reported_ = false;
private:
  bool initialized_ = false;
  mojo::Remote<dfx::mojom::DfxReporter> remote_;
  mojo::PendingReceiver<dfx::mojom::DfxReporter> receiver_ =
      remote_.BindNewPipeAndPassReceiver();
};
#endif