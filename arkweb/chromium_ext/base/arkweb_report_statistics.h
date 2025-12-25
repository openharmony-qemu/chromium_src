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

enum Region {
  REGION_CHINA = 1,
  REGION_OVERSEA = 2,
};

enum Platform {
  PLATFORM_OPERATION_ANALYSIS = 1,
  PLATFORM_BUSINESS_INTELLIGENCE = 2,
};

enum ReportFrequency {
  REPORT_IMMEDIATELY,
  REPORT_DAILY,
};

BASE_EXPORT void SetOnReportStatisticLogCallback(OnReportStatisticLogFunc func);

BASE_EXPORT void SetReportStatisticTaskRunner();

class BASE_EXPORT OperationStatistics {
 public:
  static void Statistics(int region,
                         int platform,
                         std::string event_group,
                         std::string event_id,
                         int data_version,
                         const std::string& content,
                         bool report_under_incognito,
                         bool report_immediately,
                         bool report_anonymously,
                         int report_frequency_is);

  static const char* GROUP_BECE;
  static const char* GROUP_BWCE;

  static const int DEFAULT_DATA_VERSION;
};                        

}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_BLANK_OPT_CONTROLLER_H_