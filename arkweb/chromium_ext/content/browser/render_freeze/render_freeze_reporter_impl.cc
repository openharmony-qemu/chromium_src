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

#include "arkweb/ohos_nweb/src/sysevent/event_reporter.h"
#include "mojo/public/cpp/bindings/self_owned_receiver.h"
#include "render_freeze_reporter_impl.h"

// the param `eventInfo` may be used in the future
void FreezeReporterImpl::ReportFreeze(const std::string& eventInfo) {
  ReportAppfreeze();
}

// static
void FreezeReporterImpl::ProcessPendingReceiver(mojo::PendingReceiver<freeze_reporter::mojom::FreezeReporter> receiver) {
  mojo::MakeSelfOwnedReceiver(std::make_unique<FreezeReporterImpl>(), std::move(receiver));
}