/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/net/proxy_resolution/fallback_proxy_utils.h"

#include "base/logging.h"
#include "base/json/json_writer.h"
#include "base/values.h"
#include "arkweb/chromium_ext/base/arkweb_report_statistics.h"
#include "arkweb/chromium_ext/base/ohos/nweb_engine_event_logger.h"
#include "arkweb/chromium_ext/base/ohos/nweb_engine_event_logger_code.h"
#include "arkweb/chromium_ext/url/ohos/log_utils.h"
#include "net/base/schemeful_site.h"
#include "url/origin.h"

namespace {
constexpr char kFallbackProxyUsageEventType[] = "fallback_proxy_usage_report";
constexpr char kUseProxy[] = "use_proxy";
constexpr char kReason[] = "reason";
constexpr char kDomain[] = "domain";
constexpr char kErrorCode[] = "error_code";
constexpr char kPolicy[] = "policy";
constexpr char kHwCode[] = "hw_code";
constexpr char kUseFallbackProxyResultEventType[] = "use_proxy_result";
constexpr char kResult[] = "result";
constexpr char kErrorCodeBefore[] = "error_code_before";
constexpr char kErrorCodeFinal[] = "error_code_final";
constexpr char kTunnelConnectResultEventType[] = "tunnel_connect_result";
constexpr char kHttpResponseCode[] = "http_response_code";
constexpr char kNetErrorCode[] = "net_error_code";
constexpr char kProxyExceptionReportEventType[] = "proxy_exception_report";
constexpr char kProxyTransportConnectResultEventType[] =
    "proxy_transport_connect_result";
constexpr char kConfigEnabled[] = "proxy_config_enable";
constexpr char kHasToken[] = "has_token";
}  // namespace

namespace net {
bool FallbackProxyConfigStatus::proxy_config_enable_ = false;
bool FallbackProxyConfigStatus::has_token_ = false;

void FallbackProxyConfigStatus::SetStatus(bool enable, bool has_token) {
  proxy_config_enable_ = enable;
  has_token_ = has_token;
}

int FallbackProxyConfigStatus::ProxyConfigEnable() {
  return proxy_config_enable_ ? 1 : 0;
}

int FallbackProxyConfigStatus::HasToken() {
  return has_token_ ? 1 : 0;
}

void ReportFallbackProxyUsage(const GURL& url,
                              int can_use_fallback_proxy,
                              int reason,
                              int result,
                              int policy,
                              int hw_code) {
  base::Value::Dict record;
  record.Set(kUseProxy, base::NumberToString(can_use_fallback_proxy));
  if (!can_use_fallback_proxy) {
    record.Set(kReason, base::NumberToString(reason));
  }
  record.Set(kDomain, url.host());
  record.Set(kErrorCode, base::NumberToString(result));
  record.Set(kPolicy, base::NumberToString(policy));
  record.Set(kHwCode, base::NumberToString(hw_code));
  if (reason == (int)ProxyUnusedReason::CLOUD_CONTROL_SWITCH_DISABLED) {
    record.Set(kConfigEnabled, 
      base::NumberToString(FallbackProxyConfigStatus::ProxyConfigEnable()));
    record.Set(kHasToken, 
      base::NumberToString(FallbackProxyConfigStatus::HasToken()));
  }

  auto json = base::WriteJson(record);
  // 经分打点数据上报
  if (json) {
    base::ohos::ReportStatisticLog(json.value());
  }
  std::ostringstream ostr;
  ostr << "use_proxy=" << can_use_fallback_proxy;
  if (!can_use_fallback_proxy) {
    ostr << ", reason=" << reason;
  }
  ostr << ", error_code=" << result << ", policy=" << policy
       << ", hw_code=" << hw_code;
  if (reason == (int)ProxyUnusedReason::CLOUD_CONTROL_SWITCH_DISABLED) {
    ostr << ", proxy_config_enable="
         << FallbackProxyConfigStatus::ProxyConfigEnable()
         << ", has_token=" << FallbackProxyConfigStatus::HasToken();
  }
  // 运维打点数据上报
  base::ohos::ReportEngineEvent(base::ohos::kModuleNet, base::ohos::kDefaultUrl,
                                  base::ohos::kFallbackProxyUsage, ostr.str());
  LOG(INFO) << " event_message: " << ostr.str()
            << ", resource: " << url::LogUtils::ConvertUrlWithMask(url.spec());
}

void ReportUseFallbackProxyResult(const GURL& url,
                                  int error_code_before,
                                  int error_code_final) {
  base::Value::Dict record;
  record.Set(kDomain, url.host());
  int result = (error_code_final == 0) ? 1 : 0;
  record.Set(kResult, base::NumberToString(result));
  record.Set(kErrorCodeBefore, base::NumberToString(error_code_before));
  record.Set(kErrorCodeFinal, base::NumberToString(error_code_final));
  // 经分打点数据上报
   auto json = base::WriteJson(record);
  // 经分打点数据上报
  if (json) {
    base::ohos::ReportStatisticLog(json.value());
  }

  std::ostringstream ostr;
  ostr << "result=" << result << ", error_code_before=" << error_code_before
       << ", error_code_final=" << error_code_final;
  // 运维打点数据上报
  base::ohos::ReportEngineEvent(base::ohos::kModuleNet, base::ohos::kDefaultUrl,
                                  base::ohos::kUseFallbackProxyResult, ostr.str());
  LOG(INFO) << " event_message: " << ostr.str()
            << ", resource: " << url::LogUtils::ConvertUrlWithMask(url.spec());
}

void ReportProxyTunnelConnectResult(const std::string& host,
                                    int response_code,
                                    int error_code) {
  base::Value::Dict record;
  
  int result = (error_code == 0) ? 1 : 0;
  record.Set(kResult, base::NumberToString(result));
  record.Set(kDomain, host);
  record.Set(kHttpResponseCode, base::NumberToString(response_code));
  record.Set(kNetErrorCode, base::NumberToString(error_code));

  auto json = base::WriteJson(record);
  // 经分打点数据上报
  if (json) {
    base::ohos::ReportStatisticLog(json.value());
  }

  std::ostringstream ostr;
  ostr << "result=" << result << ", http_response_code=" << response_code
       << ", net_error_code=" << error_code;
  // 运维打点数据上报
  base::ohos::ReportEngineEvent(base::ohos::kModuleNet, base::ohos::kDefaultUrl,
                                  base::ohos::kProxyTunnelConnectResult, ostr.str());
  LOG(INFO) << " event_message: " << ostr.str()
            << ", resource: " << url::LogUtils::ConvertUrlWithMask(host);

}

void ReportProxyExceptionReason(int reason) {
  base::Value::Dict record;
  record.Set(kReason, base::NumberToString(reason));
  // 经分打点数据上报
  auto json = base::WriteJson(record);
  if (json) {
    base::ohos::ReportStatisticLog(json.value());
  }

  std::ostringstream ostr;
  ostr << "reason=" << reason;
  // 运维打点数据上报
  base::ohos::ReportEngineEvent(base::ohos::kModuleNet, base::ohos::kDefaultUrl,
                                  base::ohos::kProxyExceptionResult, ostr.str());
  LOG(INFO) << " event_message: " << ostr.str();

}

void ReportProxyTransportConnectResult(const std::string& host,
                                       int error_code) {
  base::Value::Dict record;
  record.Set(kDomain, host);
  record.Set(kNetErrorCode, base::NumberToString(error_code));                                      

  // 经分打点数据上报
  auto json = base::WriteJson(record);
  if (json) {
    base::ohos::ReportStatisticLog(json.value());
  }

  std::ostringstream ostr;
  ostr << "net_error_code=" << error_code;
  // 运维打点数据上报
  base::ohos::ReportEngineEvent(base::ohos::kModuleNet, base::ohos::kDefaultUrl,
                                  base::ohos::kProxyTransportConnectResult, ostr.str());
  LOG(INFO) << " event_message: " << ostr.str()
            << ", resource: " << url::LogUtils::ConvertUrlWithMask(host);
}

}  // namespace net
