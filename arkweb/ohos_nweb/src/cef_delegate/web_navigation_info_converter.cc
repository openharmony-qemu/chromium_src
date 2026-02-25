/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ohos_nweb/src/cef_delegate/web_navigation_info_converter.h"

#include "cef/ohos_cef_ext/include/cef_web_navigation_info.h"

namespace {

using CefWebNavigationInfoKeys::kAttemptType;
using CefWebNavigationInfoKeys::kDnsAddressList;
using CefWebNavigationInfoKeys::kDnsHost;
using CefWebNavigationInfoKeys::kDnsInfo;
using CefWebNavigationInfoKeys::kDnsResult;
using CefWebNavigationInfoKeys::kDnsTransition;
using CefWebNavigationInfoKeys::kRequestAttemptResult;
using CefWebNavigationInfoKeys::kRequestTraceId;
using CefWebNavigationInfoKeys::kSocketAddressList;
using CefWebNavigationInfoKeys::kSocketHost;
using CefWebNavigationInfoKeys::kSocketInfo;
using CefWebNavigationInfoKeys::kSocketResult;
using CefWebNavigationInfoKeys::kSslExpiredDate;
using CefWebNavigationInfoKeys::kSslHost;
using CefWebNavigationInfoKeys::kSslInfo;
using CefWebNavigationInfoKeys::kSslIsFatalCertError;
using CefWebNavigationInfoKeys::kSslIssuer;
using CefWebNavigationInfoKeys::kSslResult;
using CefWebNavigationInfoKeys::kWasFetchedViaProxy;

// Helper: Join CefListValue as comma-separated string in brackets [a,b,c]
std::string JoinCefListWithComma(CefRefPtr<CefListValue> list) {
  if (!list) {
    return "[]";
  }

  size_t size = list->GetSize();
  std::string result;
  for (size_t i = 0; i < size; ++i) {
    auto addr = list->GetString(i);
    if (!addr.empty()) {
      if (!result.empty()) {
        result += ",";
      }
      result += addr;
    }
  }
  return "[" + result + "]";
}

// Helper: Parse DNS info
void ParseDnsInfo(CefRefPtr<CefDictionaryValue> the_dict,
                  WebDnsInfo& dns_info) {
  if (!the_dict) {
    return;
  }

  auto host = the_dict->GetString(kDnsHost);
  if (!host.empty()) {
    dns_info.host = host;
  }
  dns_info.result = the_dict->GetInt(kDnsResult);
  dns_info.dns_transition = the_dict->GetInt(kDnsTransition);

  auto dns_address_list = the_dict->GetList(kDnsAddressList);
  dns_info.address_list = JoinCefListWithComma(dns_address_list);
}

// Helper: Parse SSL info
void ParseSslInfo(CefRefPtr<CefDictionaryValue> the_dict,
                  WebSSLInfo& ssl_info) {
  if (!the_dict) {
    return;
  }

  auto host = the_dict->GetString(kSslHost);
  if (!host.empty()) {
    ssl_info.host = host;
  }
  auto issuer = the_dict->GetString(kSslIssuer);
  if (!issuer.empty()) {
    ssl_info.issuer = issuer;
  }
  auto expired_date = the_dict->GetString(kSslExpiredDate);
  if (!expired_date.empty()) {
    ssl_info.expired_date = expired_date;
  }
  ssl_info.result = the_dict->GetInt(kSslResult);
  ssl_info.is_fatal_error = the_dict->GetBool(kSslIsFatalCertError);
}

// Helper: Parse Socket info
void ParseSocketInfo(CefRefPtr<CefDictionaryValue> the_dict,
                     WebSocketInfo& socket_info) {
  if (!the_dict) {
    return;
  }

  auto host = the_dict->GetString(kSocketHost);
  if (!host.empty()) {
    socket_info.host = host;
  }
  socket_info.result = the_dict->GetInt(kSocketResult);

  auto socket_address_list = the_dict->GetList(kSocketAddressList);
  socket_info.address_list = JoinCefListWithComma(socket_address_list);
}

// Helper: Parse single RequestAttempt
WebRequestAttempt ParseRequestAttempt(CefRefPtr<CefDictionaryValue> dict) {
  WebRequestAttempt attempt;

  auto request_trace_id = dict->GetString(kRequestTraceId);
  if (!request_trace_id.empty()) {
    attempt.request_trace_id = request_trace_id;
  }
  attempt.attempt_type = dict->GetInt(kAttemptType);
  attempt.request_attempt_result = dict->GetInt(kRequestAttemptResult);
  attempt.was_fetched_via_proxy = dict->GetBool(kWasFetchedViaProxy);

  ParseDnsInfo(dict->GetDictionary(kDnsInfo), attempt.dns_info);
  ParseSslInfo(dict->GetDictionary(kSslInfo), attempt.ssl_info);
  ParseSocketInfo(dict->GetDictionary(kSocketInfo), attempt.socket_info);

  return attempt;
}

}  // namespace

namespace OHOS::NWeb {

NWebNavigationInfo ConvertToNWebNavigationInfo(
    CefRefPtr<CefWebNavigationInfo> cef_info) {
  NWebNavigationInfo nweb_info;
  nweb_info.url = cef_info->GetUrl().ToString();
  nweb_info.page_trace_id = cef_info->GetPageTraceId().ToString();
  nweb_info.original_url = cef_info->GetOriginalUrl().ToString();
  nweb_info.connection_type = cef_info->GetConnectionType();
  nweb_info.did_use_https_dns = cef_info->DidUseHttpDns();
  nweb_info.is_https_dns_enabled = cef_info->IsHttpsDnsEnabled();
  nweb_info.did_use_fallback_proxy = cef_info->DidUseFallbackProxy();
  nweb_info.is_fallback_proxy_enabled = cef_info->IsFallbackProxyEnabled();
  nweb_info.is_captive_portal = cef_info->IsCaptivePortal();
  nweb_info.is_auto_reload = cef_info->IsAutoReload();
  nweb_info.has_ignore_certificate_error =
      cef_info->HasIgnoreCertificateError();
  nweb_info.auto_reload_reason = cef_info->GetAutoReloadReason();
  nweb_info.original_error_code = cef_info->GetOriginalErrorCode();
  nweb_info.error_code = cef_info->GetErrorCode();
  nweb_info.hw_code = cef_info->GetHwCode();
  nweb_info.insecure_dns_records = cef_info->GetInsecureDnsRecords().ToString();
  nweb_info.secure_dns_records = cef_info->GetSecureDnsRecords().ToString();
  nweb_info.request_trace_id = cef_info->GetRequestTraceId().ToString();
  nweb_info.time_stamp = cef_info->GetTimeStamp().ToString();
  nweb_info.name_servers = cef_info->GetNameServers().ToString();
  nweb_info.local_ip = cef_info->GetLocalIp().ToString();
  nweb_info.website_policy = cef_info->GetWebsitePolicy();

  // Parse request_attempts from CefListValue
  auto attempts = cef_info->GetRequestAttempts();
  if (attempts) {
    size_t size = attempts->GetSize();
    nweb_info.request_attempts.reserve(size);
    for (size_t i = 0; i < size; ++i) {
      auto attempt_dict = attempts->GetDictionary(i);
      if (!attempt_dict) {
        continue;
      }
      nweb_info.request_attempts.push_back(ParseRequestAttempt(attempt_dict));
    }
  }

  return nweb_info;
}

}  // namespace OHOS::NWeb
