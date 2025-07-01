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

#ifndef BROWSER_PERFORMANCE_DFX_REPORTER_IMPL_H_
#define BROWSER_PERFORMANCE_DFX_REPORTER_IMPL_H_

#include "arkweb/chromium_ext/content/browser/dfx/mojom/dfx_reporting.mojom.h"
#include "base/synchronization/lock.h"
#include "media/cast/cast_config.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "gpu/ipc/common/memory_stats.h"

class DfxReporterImpl : public dfx::mojom::DfxReporter {
public:
  static constexpr uint16_t BYTES_TO_KB = 1024;
  static constexpr std::string PAGE_MEM_LEAK = "PAGE_MEM_LEAK";

  DfxReporterImpl() {}
  ~DfxReporterImpl() override {}

  static void ProcessPendingReceiver(mojo::PendingReceiver<dfx::mojom::DfxReporter>& receiver);

  void ReportMemInfo(const std::string& sysEventInfoJson) override;
  void ReportHiSysEvent(const std::string& eventName, const std::string& sysEventInfoJson) override;
};

#endif // BROWSER_PERFORMANCE_DFX_REPORTER_IMPL_H_