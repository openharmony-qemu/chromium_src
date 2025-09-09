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

#ifndef ARKWEB_CHROMIUM_EXT_CONTENT_BROWSER_RENDER_FREEZE_REPORTER_IMPL_H_
#define ARKWEB_CHROMIUM_EXT_CONTENT_BROWSER_RENDER_FREEZE_REPORTER_IMPL_H_

#include "arkweb/chromium_ext/content/browser/render_freeze/mojom/freeze_reporter.mojom.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/receiver.h"

class FreezeReporterImpl : public freeze_reporter::mojom::FreezeReporter {
public:
  FreezeReporterImpl() {}
  ~FreezeReporterImpl() override {}
  static void ProcessPendingReceiver(mojo::PendingReceiver<freeze_reporter::mojom::FreezeReporter> receiver);
  void ReportFreeze(const std::string& eventInfo) override;
};
#endif