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

#include "nweb_engine_event_logger_cef_delegate.h"
#include "cef/libcef/browser/browser_host_base.h"
#include "nweb_common.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/static/nweb_static_dispatcher.h"
#endif

namespace OHOS::NWeb {

void NWebEngineEventLoggerCefDelegate::ReportEngineEvent(const std::string& module,
    const std::string& resource, const std::string& error_code, const std::string& error_msg) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebStaticDispatcher::OnLoggerReportEvent(module, resource, error_code, error_msg);
#endif
}

}  // namespace OHOS::NWeb
