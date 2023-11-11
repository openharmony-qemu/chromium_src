/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "event_reporter.h"
#include "ohos_adapter_helper.h"

using OHOS::NWeb::HiSysEventAdapter;
using OHOS::NWeb::OhosAdapterHelper;
namespace {
// For page load statistics
constexpr char PAGE_LOAD_STATISTICS[] = "PAGE_LOAD_STATISTICS";
constexpr char CURRENT_INSTANCE_ID[] = "INSTANCE_ID";
constexpr char ACCESS_SUM_COUNT[] = "ACCESS_SUM_COUNT";
constexpr char ACCESS_SUCC_COUNT[] = "ACCESS_SUCC_COUNT";
constexpr char ACCESS_FAIL_COUNT[] = "ACCESS_FAIL_COUNT";
constexpr char ACCESS_FAIL_RATIO[] = "ACCESS_FAIL_RATIO";

// For mutiple instance statistics
constexpr char MULTI_INSTANCE_STATISTICS[] = "MULTI_INSTANCE_STATISTICS";
constexpr char CURRENT_INSTANCE_COUNT[] = "INSTANCE_COUNT";
constexpr char INSTANCE_MAX_COUNT[] = "MAX_COUNT";

// For page load error info
constexpr char PAGE_LOAD_ERROR[] = "PAGE_LOAD_ERROR";
constexpr char ERROR_TYPE[] = "ERROR_TYPE";
constexpr char ERROR_CODE[] = "ERROR_CODE";
constexpr char ERROR_DESC[] = "ERROR_DESC";
}  // namespace

void ReportPageLoadStats(int instanceId,
                         int accessSumCount,
                         int accessSuccCount,
                         int accessFailCount) {
  float failRatio = float(accessFailCount) / accessSumCount;
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      PAGE_LOAD_STATISTICS, HiSysEventAdapter::EventType::STATISTIC,
      {CURRENT_INSTANCE_ID, instanceId, ACCESS_SUM_COUNT, accessSumCount,
       ACCESS_SUCC_COUNT, accessSuccCount, ACCESS_FAIL_COUNT, accessFailCount,
       ACCESS_FAIL_RATIO, failRatio});
}

void ReportMultiInstanceStats(int instanceId, int nwebCount, int nwebMaxCount) {
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      MULTI_INSTANCE_STATISTICS, HiSysEventAdapter::EventType::STATISTIC,
      {CURRENT_INSTANCE_ID, instanceId, CURRENT_INSTANCE_COUNT, nwebCount,
       INSTANCE_MAX_COUNT, nwebMaxCount});
}

void ReportPageLoadErrorInfo(int instanceId,
                             const std::string errorType,
                             int errorCode,
                             const std::string errorDesc) {
  std::string error_type = "";
  std::string error_desc = "";
  int error_code = errorCode;
  if (errorType != "") {
    error_type = errorType;
  }
  if (errorDesc != "") {
    error_desc = errorDesc;
  }
  OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
      PAGE_LOAD_ERROR, HiSysEventAdapter::EventType::FAULT,
      {CURRENT_INSTANCE_ID, instanceId, ERROR_TYPE, error_type, ERROR_CODE,
       error_code, ERROR_DESC, error_desc});
}
