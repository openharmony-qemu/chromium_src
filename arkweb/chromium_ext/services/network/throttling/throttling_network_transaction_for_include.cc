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

namespace network {

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
int ThrottlingNetworkTransaction::RestartWithSecureDnsOnly(
    net::CompletionOnceCallback callback) {
  if (CheckFailed()) {
    return net::ERR_INTERNET_DISCONNECTED;
  }
  if (!interceptor_) {
    return network_transaction_->RestartWithSecureDnsOnly(std::move(callback));
  }

  callback_ = std::move(callback);
  int result = network_transaction_->RestartWithSecureDnsOnly(base::BindOnce(
      &ThrottlingNetworkTransaction::IOCallback, base::Unretained(this), true));
  return Throttle(true, result);
}
#endif

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
int ThrottlingNetworkTransaction::RestartWithFallbackProxy(
    net::CompletionOnceCallback callback) {
  if (CheckFailed()) {
    return net::ERR_INTERNET_DISCONNECTED;
  }
  if (!interceptor_) {
    return network_transaction_->RestartWithFallbackProxy(std::move(callback));
  }

  callback_ = std::move(callback);
  int result = network_transaction_->RestartWithFallbackProxy(base::BindOnce(
      &ThrottlingNetworkTransaction::IOCallback, base::Unretained(this), true));
  return Throttle(true, result);
}

int ThrottlingNetworkTransaction::RestartWithDirect(
    net::CompletionOnceCallback callback) {
  if (CheckFailed()) {
    return net::ERR_INTERNET_DISCONNECTED;
  }
  if (!interceptor_) {
    return network_transaction_->RestartWithDirect(std::move(callback));
  }

  callback_ = std::move(callback);
  int result = network_transaction_->RestartWithDirect(base::BindOnce(
      &ThrottlingNetworkTransaction::IOCallback, base::Unretained(this), true));
  return Throttle(true, result);
}
#endif

}  // namespace network
