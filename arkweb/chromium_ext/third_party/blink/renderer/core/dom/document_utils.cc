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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/dom/document_utils.h"

#include "base/trace_event/trace_event.h"

namespace blink {

void DocumentUtils::Trace(Visitor* visitor) const {}

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
void DocumentUtils::DocumentInitializeUtils(const Member<ResourceFetcher>& fetcher_,
                                            const KURL& base_url_)
{
  if (fetcher_) {
    fetcher_->SetMainUrl(base_url_);
  }
}
#endif

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
void DocumentUtils::StartBoostingUtils()
{
  auto type = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
      switches::kProcessType);
  if (type == switches::kRendererProcess) {
    blink::ResSchedReportClient report_client(OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_WEB_GESTURE_ID);
    report_client.ApplySocByIdEx(OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_WEB_GESTURE_ID, true);
  } else {
    OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .CreateSocPerfClientAdapter()
        ->ApplySocPerfConfigByIdEx(OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_WEB_GESTURE_ID, true);
  }
}

void DocumentUtils::SetURLUtils()
{
  if (::base::subtle::TimeTicksNowIgnoringOverride()
              .since_origin()
              .InNanoseconds() -
          base::ohos::TouchObserver::GetInstance().GetLastTouchUpTime() <
      DocumentUtils::MAX_TOUCH_UP_INTERVAL) {
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
        FROM_HERE,
        WTF::BindOnce(&DocumentUtils::StartBoostingUtils),
        base::Milliseconds(DocumentUtils::LOAD_URL_DELAY_TIME));
  }
}
#endif

}