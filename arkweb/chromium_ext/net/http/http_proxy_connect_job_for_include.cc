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

namespace {
// HttpProxyConnectJobs will time out after this many seconds.  Note this is in
// addition to the timeout for the transport socket.
#if BUILDFLAG(IS_ANDROID) || BUILDFLAG(IS_IOS)
constexpr base::TimeDelta kHttpProxyConnectJobTunnelTimeout = base::Seconds(10);
#else
constexpr base::TimeDelta kHttpProxyConnectJobTunnelTimeout = base::Seconds(30);
#endif
}

namespace net {

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
void HttpProxyConnectJob::ResetTimerExInBeginConnect() {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          ::switches::kEnableNwebEx)) {
    base::TimeDelta timeout;
    if (common_connect_job_params()->proxy_delegate &&
        common_connect_job_params()->proxy_delegate->IsFallbackProxyServer(
            params_->proxy_chain())) {
      is_fallback_proxy_server_ = true;
      timeout =
          base::Milliseconds(common_connect_job_params()
                                 ->proxy_delegate->GetProxyConnectTimeout());
    } else {
      timeout = AlternateNestedConnectionTimeout(*params_,
                                                 network_quality_estimator());
    }
    ResetTimer(timeout);
  } else {
    ResetTimer(AlternateNestedConnectionTimeout(*params_,
                                                network_quality_estimator()));
  }
}

void HttpProxyConnectJob::ResetTimerExInHttpProxyConnect() {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          ::switches::kEnableNwebEx)) {
    base::TimeDelta timeout;
    if (is_fallback_proxy_server_) {
      timeout = base::Milliseconds(
          common_connect_job_params()->proxy_delegate->GetProxyTunnelTimeout());
      if (!http_auth_controller_) {
        http_auth_controller_ = base::MakeRefCounted<HttpAuthController>(
            HttpAuth::AUTH_PROXY,
            GURL((params_->ssl_params() ? "https://" : "http://") +
                 params_->proxy_server().host_port_pair().ToString()),
            params_->network_anonymization_key(),
            common_connect_job_params()->http_auth_cache,
            common_connect_job_params()->http_auth_handler_factory,
            host_resolver());
      }
    } else {
      timeout = kHttpProxyConnectJobTunnelTimeout;
    }
    ResetTimer(timeout);
  } else {
    ResetTimer(kHttpProxyConnectJobTunnelTimeout);
  }
}
#endif  // BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)

}
