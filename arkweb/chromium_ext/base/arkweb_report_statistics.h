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

#ifndef BASE_ARKWEB_REPORT_STATISTICS_H_
#define BASE_ARKWEB_REPORT_STATISTICS_H_

#include "arkweb/build/features/features.h"
#include "arkweb/ohos_nweb/src/capi/nweb_statistic_callback.h"

#include "base/memory/weak_ptr.h"
#include "base/memory/raw_ptr.h"

#include <string>
#include <utility>

namespace base {
namespace ohos {

    BASE_EXPORT void SetOnReportStatisticLogCallback(OnReportStatisticLogFunc func);

    BASE_EXPORT void ReportStatisticLog(const std::string& content);
    
    BASE_EXPORT void SetReportStatisticTaskRunner();

}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_BLANK_OPT_CONTROLLER_H_