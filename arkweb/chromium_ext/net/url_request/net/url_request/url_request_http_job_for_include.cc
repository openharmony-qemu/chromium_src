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

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#endif

namespace {
#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
bool IsFallbackProxyIgnoreErrorCode(int result) {
  if (result == net::OK || net::IsCertificateError(result) ||
      result == net::ERR_SSL_CLIENT_AUTH_CERT_NEEDED) {
    return true;
  }

  return false;
}
#endif
}

namespace net {
#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
bool URLRequestHttpJob::MaybeRetryWithFallbackProxy(int result) {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          ::switches::kEnableNwebEx) &&
      !request_->RetryWithFallbackProxy()) {
    bool is_using_fallback_proxy = false;
    if (transaction_ && transaction_->GetResponseInfo() &&
        transaction_->GetResponseInfo()->used_fallback_proxy) {
      is_using_fallback_proxy = true;
    }

    bool is_main_frame = request_->isolation_info().request_type() ==
                         IsolationInfo::RequestType::kMainFrame;
    if (is_using_fallback_proxy) {
      request_->set_fallback_proxy_error_code(result);
      if (result && request_->proxy_delegate()) {
        request_->proxy_delegate()->OnProxyConnectResult(
            transaction_->GetResponseInfo()->proxy_chain, result);
      }

      if (is_main_frame) {
        request_->set_used_fallback_proxy(true);
      }
    }

    // 失败时查看是否可以使用代理重试
    ProxyUnusedReason reason = ProxyUnusedReason::MAX_VALUE;
    if (did_use_fallback_proxy_) {
      if (is_using_fallback_proxy && !IsFallbackProxyIgnoreErrorCode(result)) {
        result = original_net_error_;
      }
    } else if (CanRetryWithFallbackProxy(result, &reason) ||
               (reason == ProxyUnusedReason::NOT_CONNECTION_ERROR &&
                is_main_frame)) {
      original_net_error_ = result;
      int malicious_type = -1;
      int hw_code = -1;
      SBThreatURLPolicy policy =
          GetSafeBrowsingThreatUrlPolicy(result, &malicious_type, &hw_code);
      if (policy == WAIT) {
        LOG(DEBUG) << "Will wait "
                   << request_->proxy_delegate()->GetMaliciousUrlCheckWaitTime()
                   << " to get malicious info, reason " << (int)reason
                   << ", is_main_frame " << is_main_frame << ", url "
                   << url::LogUtils::ConvertUrlWithMask(
                          request()->url().spec());
        base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
            FROM_HERE,
            base::BindOnce(&URLRequestHttpJob::OnStartCompleted,
                           weak_factory_.GetWeakPtr(), result),
            base::Milliseconds(
                request_->proxy_delegate()->GetMaliciousUrlCheckWaitTime()));
        return true;
      }

      bool sb_policy_proceed = (policy == PROCEED);
      if (is_main_frame) {
        if (!sb_policy_proceed) {
          reason =
              ProxyUnusedReason::MALICIOUS_TYPE_OR_HW_CODE_NOT_IN_CLOUD_LIST;
        }
      }
      if (sb_policy_proceed) {
        if (reason != ProxyUnusedReason::NOT_CONNECTION_ERROR) {
          RetryWithFallbackProxy();
          return true;
        } else {
          // 对于reason为ProxyUnusedReason::NOT_CONNECTION_ERROR，
          // 需要NetErrorAutoReloader指定fallback_proxy重试
          request_->set_needs_reload_with_fallback_proxy(true);
          LOG(DEBUG)
              << "This request needs to reload with fallback proxy, url "
              << url::LogUtils::ConvertUrlWithMask(request_->url().spec());
        }
      }
    } else {
      // the request can't retry with fallback proxy
      if (base::CommandLine::ForCurrentProcess()->HasSwitch(
              ::switches::kEnableNwebEx) &&
          result && !is_using_fallback_proxy && is_main_frame) {
      }
    }

    // 如果直接使用代理失败的话，需要使用非代理再重试一遍
    if (is_using_fallback_proxy && !did_use_fallback_proxy_ &&
        !IsFallbackProxyIgnoreErrorCode(result)) {
      RetryWithDirect();
      return true;
    }
  }
  return false;
}

bool URLRequestHttpJob::SupportedHostAndNotUsedSystemProxy(
    ProxyDelegate* proxy_delegate,
    ProxyUnusedReason* unused_reason) {
  DCHECK(proxy_delegate);
  DCHECK(unused_reason);
  if (proxy_delegate->IsFallbackProxyFailedHost(request_->url().host())) {
    *unused_reason = ProxyUnusedReason::HOST_IN_USING_PROXY_FAILED_LIST;
    return false;
  }

  std::string query_block_host;
  IPAddress ip_address;
  bool is_ip = ip_address.AssignFromIPLiteral(request_->url().host());
  if (is_ip) {
    query_block_host = ip_address.ToString();
  } else {
    query_block_host = net::registry_controlled_domains::GetDomainAndRegistry(
        request_->url().host(),
        net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES);
  }
  if (proxy_delegate->IsFallbackProxyBlockHost(query_block_host)) {
    *unused_reason = ProxyUnusedReason::TOP_SITE_IN_BLOCK_LIST;
    return false;
  }

  if (transaction_ && transaction_->GetResponseInfo() &&
      transaction_->GetResponseInfo()->WasFetchedViaProxy()) {
    /* if has used system proxy, then return false */
    *unused_reason = ProxyUnusedReason::HAS_USED_SYSTEM_PROXY;
    return false;
  }
  return true;
}

bool URLRequestHttpJob::CanRetryWithFallbackProxy(
    int result,
    ProxyUnusedReason* unused_reason) {
  DCHECK(unused_reason);
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebEx)) {
    return false;
  }

  if (result == net::OK) {
    return false;
  }

  if (transaction_ && transaction_->GetResponseInfo() &&
      transaction_->GetResponseInfo()->used_fallback_proxy) {
    /* if has used fallback proxy, then return false */
    LOG(DEBUG)
        << "Can't retry with fallback proxy for used fallback proxy, url "
        << url::LogUtils::ConvertUrlWithMask(request_->url().spec());
    return false;
  }

  ProxyDelegate* proxy_delegate = request_->proxy_delegate();
  if (!proxy_delegate) {
    LOG(DEBUG)
        << "Can't retry with fallback proxy for proxy delegate null, url "
        << url::LogUtils::ConvertUrlWithMask(request_->url().spec());
    return false;
  }

  FallbackProxyStatus status = proxy_delegate->GetFallbackProxyStatus();
  if (status != FallbackProxyStatus::NORMAL) {
    if (status == FallbackProxyStatus::UNAVAILABLE) {
      *unused_reason = ProxyUnusedReason::PROXY_SERVER_UNAVAILABLE;
    } else if (status == FallbackProxyStatus::AUTH_FAILED) {
      *unused_reason = ProxyUnusedReason::AUTH_FAILED;
    } else {
      *unused_reason = ProxyUnusedReason::CLOUD_CONTROL_SWITCH_DISABLED;
    }
    return false;
  }

  if (proxy_delegate->IsFallbackProxyRetryErrorCode(result)) {
    net::NetErrorDetails details;
    PopulateNetErrorDetails(&details);
    // 如果stream已经创建成功。证明dns阶段没有发生问题，所以我们不需要重试.
    if (details.stream_created) {
      *unused_reason = ProxyUnusedReason::NOT_CONNECTION_ERROR;
      if (request_->isolation_info().request_type() ==
          IsolationInfo::RequestType::kMainFrame) {
        SupportedHostAndNotUsedSystemProxy(proxy_delegate, unused_reason);
      }
      LOG(DEBUG) << "can't retry with fallback proxy since the stream is "
                    "created, reason "
                 << static_cast<int>(*unused_reason) << ", url "
                 << url::LogUtils::ConvertUrlWithMask(request_->url().spec());
      return false;
    }
  } else {
    *unused_reason = ProxyUnusedReason::ERROR_CODE_NOT_IN_CLOUD_LIST;
    return false;
  }

  if (!SupportedHostAndNotUsedSystemProxy(proxy_delegate, unused_reason)) {
    return false;
  }

  // 对于子frame需要查看其主frame是否通过代理成功加载
  if (request_->isolation_info().request_type() !=
      IsolationInfo::RequestType::kMainFrame) {
    if (request_->isolation_info().top_frame_origin()) {
      if (proxy_delegate->IsFallbackProxySuccessMainFrameHost(
              request_->isolation_info().top_frame_origin()->host())) {
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  }

  return true;
}

void URLRequestHttpJob::RetryWithFallbackProxy() {
  // If the transaction was destroyed, then the job was cancelled.
  if (!transaction_.get()) {
    return;
  }

  did_use_fallback_proxy_ = true;
  response_info_ = nullptr;
  override_response_headers_ = nullptr;  // See https://crbug.com/801237.
  receive_headers_end_ = base::TimeTicks();

  ResetTimer();
  request_info_.secure_dns_only = false;
  request_info_.retry_with_fallback_proxy = true;

  LOG(INFO) << "Will retry with fallback proxy for "
             << url::LogUtils::ConvertUrlWithMask(request_->url().spec());
  int rv = transaction_->RestartWithFallbackProxy(base::BindOnce(
      &URLRequestHttpJob::OnStartCompleted, base::Unretained(this)));
  if (rv == ERR_IO_PENDING) {
    return;
  }

  // The transaction started synchronously, but we need to notify the
  // URLRequest delegate via the message loop.
  base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE, base::BindOnce(&URLRequestHttpJob::OnStartCompleted,
                                weak_factory_.GetWeakPtr(), rv));
}

SBThreatURLPolicy URLRequestHttpJob::GetSafeBrowsingThreatUrlPolicy(
    int result,
    int* malicious_type,
    int* hw_code) {
  DCHECK(malicious_type);
  DCHECK(hw_code);
  if (request()->isolation_info().request_type() !=
      net::IsolationInfo::RequestType::kMainFrame) {
    return PROCEED;
  }

  ProxyDelegate* proxy_delegate = request_->proxy_delegate();
  if (!proxy_delegate) {
    return DENY;
  }

  if (proxy_delegate->GetUrlMaliciousTypeAndHwCode(request()->url_chain(),
                                                   malicious_type, hw_code)) {
    if (proxy_delegate->IsFallbackProxyMaliciousType(*malicious_type) &&
        proxy_delegate->IsFallbackProxyHwCode(*hw_code)) {
      return PROCEED;
    } else {
      return DENY;
    }
  }

  if (wait_for_sb_threat_type_) {
    return DENY;
  }

  wait_for_sb_threat_type_ = true;
  return WAIT;
}

void URLRequestHttpJob::RetryWithDirect() {
  // If the transaction was destroyed, then the job was cancelled.
  if (!transaction_.get()) {
    return;
  }

  did_use_fallback_proxy_ = true;
  response_info_ = nullptr;
  override_response_headers_ = nullptr;  // See https://crbug.com/801237.
  receive_headers_end_ = base::TimeTicks();

  ResetTimer();
  request_info_.secure_dns_only = false;
  request_info_.retry_with_fallback_proxy = false;
  request_info_.load_flags |= LOAD_BYPASS_PROXY;

  LOG(DEBUG) << "Will retry with direct after proxy for "
             << url::LogUtils::ConvertUrlWithMask(request_->url().spec());
  int rv = transaction_->RestartWithDirect(base::BindOnce(
      &URLRequestHttpJob::OnStartCompleted, base::Unretained(this)));
  if (rv == ERR_IO_PENDING) {
    return;
  }

  // The transaction started synchronously, but we need to notify the
  // URLRequest delegate via the message loop.
  base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE, base::BindOnce(&URLRequestHttpJob::OnStartCompleted,
                                weak_factory_.GetWeakPtr(), rv));
}

#endif  // BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)

}  // namespace net
