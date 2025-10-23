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

#include "components/page_load_metrics/browser/page_load_metrics_forward_observer.h"

namespace page_load_metrics {

#if BUILDFLAG(ARKWEB_BFCACHE)
void PageLoadMetricsForwardObserver::
    OnFirstContentfulPaintAfterBackForwardCacheRestoreInPage(
        const mojom::BackForwardCacheTiming& timing,
        size_t index) {}
#endif

}  // namespace page_load_metrics
